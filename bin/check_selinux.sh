#!/bin/bash

SELINUX="getenforce"
if [ $(getenforce) == "Disabled" ]; then
	echo "no"
else
	echo "yes"
fi
