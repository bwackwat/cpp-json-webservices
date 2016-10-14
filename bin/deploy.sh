#!/bin/bash

set -e
set -x

cd ../friendly-adventure
git pull

python ../cpp-json-webservices/bin/templater.py

cd ../cpp-json-webservices
git pull

cd ../ignorant-json
git pull

/bin/cp ../ignorant-json/json.* ./src/

# THIS WONT WORK UNLESS THE USER EXECUTING THE SCRIPT OWNS THE ITEMS (bwackwat)
/bin/cp -r ../friendly-adventure/* /etc/nginx/html/

if [ "$@" == "--production" ]; then
	sed "/#DEVELOPMENT$/d" ./bin/nginx.conf > /etc/nginx/nginx.conf
else
	sed "/#PRODUCTION$/d" ./bin/nginx.conf > /etc/nginx/nginx.conf
fi

systemctl restart nginx

if [ "$@" == "--nobuild" ]; then
    echo "Skipping build step (--nobuild)."
else
    echo "Doing build step (--nobuild missing)"
    ./bin/build.sh
fi

