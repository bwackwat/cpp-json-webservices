#!/bin/bash

set -e
set -x

cd ../friendly-adventure
git pull

python ../cpp-json-webservices/bin/templater.py

cd ../cpp-json-webservices
git pull

# THIS WONT WORK UNLESS THE USER EXECUTING THE SCRIPT OWNS THE ITEMS (bwackwat)
/bin/cp -r ../friendly-adventure/* /etc/nginx/html/
/bin/cp ./bin/nginx.conf /etc/nginx/

systemctl restart nginx

if [ "$1" == "--nobuild" ]; then
    echo "Skipping build step (--nobuild)."
else
    echo "Doing build step (--nobuild missing)"
    ./bin/build.sh
fi

