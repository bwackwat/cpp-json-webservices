#!/bin/bash
set -e

# This has been tested primarily on CentOS 7, in 2016.
# This was initiated in production August 26th, 2016.

yum makecache fast
yum -y upgrade

# Packages

yum -y install epel-release gcc
yum -y install vim cmake nginx clang fail2ban rkhunter postgresql-server postgresql-contrib
yum -y install libstdc++-static libstdc++ cryptopp cryptopp-devel boost boost-devel libpqxx libpqxx-devel libev postgis

# File Setup

mkdir -p /etc/nginx/log
chown nginx:nginx /etc/nginx/log

mkdir -p /etc/nginx/html
cp -rn ./public/* /etc/nginx/html
mv -n /etc/nginx/nginx.conf /etc/nginx/nginx.conf.orig
cp -n ./bin/nginx.conf /etc/nginx/nginx.conf

mkdir -p /etc/nginx/ssl
#Generate key.
openssl genrsa -des3 -out /etc/nginx/ssl/webservice.key 2048
#Generate certificate signing request.
openssl req -new -key /etc/nginx/ssl/webservice.key -out /etc/nginx/ssl/webservice.csr
cp /etc/nginx/ssl/webservice.key /etc/nginx/ssl/webservice.key.orig
#Remove passphrase from key.
openssl rsa -in /etc/nginx/ssl/webservice.key.orig -out /etc/nginx/ssl/webservice.key
#Generate certificate.
openssl x509 -req -days 365 -in /etc/nginx/ssl/webservice.csr -signkey /etc/nginx/ssl/webservice.key -out /etc/nginx/ssl/webservice.crt

systemctl restart nginx
systemctl enable nginx

setsebool httpd_can_network_connect 1

# Argon2 Password Hashing Setup

if [ -d "./phc-winner-argon2" ]; then
	rm -r ./phc-winner-argon2
fi
git clone https://github.com/P-H-C/phc-winner-argon2.git
mv ./phc-winner-argon2/include/argon2.h /usr/include/
cd phc-winner-argon2
make
mv ./libargon2.so /usr/local/lib/
ldconfig
cd ../

# Rapidjson (header only) Setup

if [ -d "./rapidjson" ]; then
	rm -r ./rapidjson
fi
git clone https://github.com/miloyip/rapidjson.git
cp -rf ./rapidjson/include/rapidjson /usr/include/

# PostgreSQL Configuration

mkdir -p /data
passwd postgres
chown postgres:postgres /data
chmod 666 /root/raw-json-webservices/bin/tables.sql
chown postgres:postgres /root/raw-json-webservices/bin/tables.sql
cp /root/raw-json-webservices/bin/tables.sql /home

#initdb /data/ -E UTF8 --locale=en_US.UTF8
#pg_ctl -D /data -l logfile start
postgresql-setup initdb
su - postgres
psql -c "CREATE DATABASE webservice OWNER postgres;"
su root
psql -U postgres -d webservice -a -f ./bin/tables.sql


#Only change is 
#host    all             all             127.0.0.1/32            ident
#To
#host    all             all             127.0.0.1/32            trust
mv /var/lib/pgsql/data/pg_hba.conf /var/lib/pgsql/data/pg_hba.conf.orig
cp ./bin/pg_hba.conf /var/lib/pgsql/data/pg_hba.conf

systemctl start postgresql
systemctl enable postgresql
