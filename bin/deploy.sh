#!/bin/bash

set -e
set -x

cd ../friendly-adventure
git pull

cd ../cpp-json-webservices
git pull

# THIS WONT WORK UNLESS THE USER EXECUTING THE SCRIPT OWNS THE ITEMS (bwackwat)
/bin/cp -r ../friendly-adventure/* /etc/nginx/html/
/bin/cp ./bin/nginx.conf /etc/nginx/

systemctl restart nginx

./bin/build.sh

