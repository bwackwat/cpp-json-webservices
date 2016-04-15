#!/bin/bash

# This has been tested primarily on Debian 8, in 2016.

apt-get update
apt-get -y upgrade

# Dependencies

apt-get -y install vim cmake nginx build-essential
apt-get -y install postgresql-9.4 clang libboost-all-dev libpqxx-dev
apt-get -y install fail2ban rkhunter
apt-get -y install libcrypto++9 libcrypto++9-dbg libcrypto++-dev
apt-get -y install redis-server redis-tools
apt-get -y install libhiredis-dev libev-dev

# Argon2 Password Hashing Setup

git clone https://github.com/P-H-C/phc-winner-argon2.git
mv ./phc-winner-argon2/include/argon2.h /usr/include/
cd phc-winner-argon2
make
mv ./libargon2.so /usr/local/lib/
ldconfig
cd ../

# Rapidjson (header only) Setup

git clone https://github.com/miloyip/rapidjson.git
mv ./rapidjson/include/rapidjson /usr/include/

# Redox, Redis C++ Client Setup

git clone https://github.com/hmartiro/redox.git
cd redox
mkdir build
cd build
cmake ../
make install
mv libredox.so* /usr/local/lib
ldconfig
cd ../../

# CUSTOMIZE ./raw-json-webservices/bin/redis.commands AS CONFIGURATION INIT
# USE "redis-cli < ./raw-json-webservices/bin/redis.commands" TO INIT THAT CONFIGURATION

su postgres

psql -c "CREATE DATABASE webservice OWNER postgres;"

psql -U postgres -d webservice -a -f tables.sql