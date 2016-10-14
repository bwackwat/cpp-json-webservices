#!/bin/bash

# Exit on error.
set -e
# Echo commands.
set -x

# This has been tested primarily on CentOS 7, in 2016.
# This was initiated in production October 8th, 2016.

yum makecache fast
yum -y upgrade

# Packages

yum -y install epel-release
yum -y install cmake nginx clang fail2ban rkhunter certbot
yum -y install postgresql-server postgresql-contrib postgresql-libs
yum -y install libstdc++-static libstdc++ cryptopp cryptopp-devel boost boost-devel libpqxx libpqxx-devel libev postgis

# File Setup

mkdir -p /etc/nginx/log
chown nginx:nginx /etc/nginx/log

if [ ! -d "../friendly-adventure" ]; then
        git clone https://github.com/bwackwat/friendly-adventure ../friendly-adventure
fi
mkdir -p /etc/nginx/html
cp -rn ../friendly-adventure/* /etc/nginx/html
cp -n /etc/nginx/nginx.conf /etc/nginx/nginx.conf.orig
/bin/cp -f ./bin/nginx.conf /etc/nginx/nginx.conf

# Argon2 Password Hashing Setup

if [ ! -d "./phc-winner-argon2" ]; then
        git clone https://github.com/P-H-C/phc-winner-argon2.git
fi
cp -n ./phc-winner-argon2/include/argon2.h /usr/include/
cd phc-winner-argon2
make
cp -n ./libargon2.so /usr/local/lib/
cp /usr/local/lib/libargon2.so /lib64/
cp /lib64/libargon2.so /lib64/libargon2.so.0
ldconfig
cd ../

# ignorant-json Setup

if [ ! -d "../ignorant-json" ]; then
        git clone https://github.com/bwackwat/ignorant-json ../ignorant-json
fi
/bin/cp ../ignorant-json/json.* ./src/

# PostgreSQL Configuration

chmod 666 bin/tables.sql
chown postgres:postgres bin/tables.sql

export PGDATA=/data
if [ ! -d "/data" ]; then
        mkdir -p /data
        postgresql-setup initdb
fi
#initdb /data/ -E UTF8 --locale=en_US.UTF8
#pg_ctl -D /data -l logfile start

#Only change is 
#host    all             all             127.0.0.1/32            ident
#To
#host    all             all             127.0.0.1/32            trust
mv -n /var/lib/pgsql/data/pg_hba.conf /var/lib/pgsql/data/pg_hba.conf.orig
cp -f ./bin/pg_hba.conf /var/lib/pgsql/data/pg_hba.conf

systemctl start postgresql
systemctl enable postgresql

mkdir -p /etc/nginx/ssl

echo -n "Enter password for SSL key:" 
read -s password
#Generate key.
openssl genrsa -des3 -passout pass:$password -out /etc/nginx/ssl/webservice.key 2048
#Generate certificate signing request.
openssl req -new -passin pass:$password -key /etc/nginx/ssl/webservice.key -out /etc/nginx/ssl/webservice.csr
cp /etc/nginx/ssl/webservice.key /etc/nginx/ssl/webservice.key.orig
#Remove passphrase from key.
openssl rsa -in /etc/nginx/ssl/webservice.key.orig -passin pass:$password -out /etc/nginx/ssl/webservice.key
#Generate certificate.
openssl x509 -req -days 365 -passin pass:$password -in /etc/nginx/ssl/webservice.csr -signkey /etc/nginx/ssl/webservice.key -out /etc/nginx/ssl/webservice.crt

openssl dhparam -out /etc/nginx/ssl/dhparam.pem 2048

#certbot certonly

if [ $(getenforce) == "Disabled" ]; then
	echo "SELinux is disabled on this current install."
else
	echo "SELinux is enforcing on this current install. Setting httpd_can_network_connect to 1."
	setsebool httpd_can_network_connect 1
fi

systemctl restart nginx
systemctl enable nginx
psql -U postgres -c "CREATE DATABASE webservice OWNER postgres;"
psql -U postgres -d webservice -a -f ./bin/tables.sql
