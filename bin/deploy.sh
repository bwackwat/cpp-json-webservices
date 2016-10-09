#!/bin/bash

set -e
set -x

cd ../friendly-adventure
git pull

cd ../cpp-json-webservices
git pull

/bin/cp -r ../friendly-adventure/* /etc/nginx/html/
/bin/cp ./bin/nginx.conf /etc/nginx/

systemctl restart nginx

./bin/build.sh

