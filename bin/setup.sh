#!/bin/bash

apt-get update
apt-get -y upgrade

apt-get -y install vim nginx build-essential
apt-get -y install postgresql-9.4 clang libboost-all-dev libpqxx-dev
apt-get -y install fail2ban rkhunter
apt-get -y install libcrypto++9 libcrypto++9-dbg libcrypto++-dev

git clone https://github.com/P-H-C/phc-winner-argon2
mv ./phc-winner-argon2/include/argon2.h /usr/include/
cd phc-winner-argon2
make
mv ./libargon2.so /usr/local/lib/
ldconfig
cd ../

git clone https://github.com/miloyip/rapidjson
mv ./rapidjson/include/rapidjson /usr/include/

su postgres

psql -c "CREATE DATABASE webservice OWNER postgres;"

psql -U postgres -d webservice -a -f tables.sql
