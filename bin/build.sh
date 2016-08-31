#!/bin/bash

echo "|                                                  |"
echo "|                                                  |"
echo "|                                                  |"

rm build.log
rm webservice

clang++ -std=c++11 -g -O3 -Wall -pedantic\
 -lboost_system -lboost_thread -lpqxx -lpq -largon2 -lcryptopp\
 src/*.cpp -o webservice &> build.log

less build.log

rm run.log

killall webservice
./webservice > run.log 2>&1 &

tail -f run.log

