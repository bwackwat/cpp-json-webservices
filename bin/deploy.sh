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

cd ../cpp-json-webservices

/bin/cp ../ignorant-json/json.* ./src/

# THIS WONT WORK UNLESS THE USER EXECUTING THIS SCRIPT OWNS THE ITEMS (bwackwat)
/bin/cp -r ../friendly-adventure/* /etc/nginx/html/

if [[ "$@" == "--production" ]]; then
	# Remove development lines.
	sed "/#DEVELOPMENT$/d" ./bin/nginx.conf > /etc/nginx/nginx.conf
else
	# Remove production lines.
	sed "/#PRODUCTION$/d" ./bin/nginx.conf > /etc/nginx/nginx.conf
fi

systemctl restart nginx

if [[ "$@" == "--nobuild" ]]; then
    echo "Skipping build step (--nobuild)."
else
    echo "Doing build step (--nobuild missing)"
    ./bin/build.sh
fi

