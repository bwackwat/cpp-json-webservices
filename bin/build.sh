#!/bin/bash

echo "|                                                  |"
echo "|                                                  |"
echo "|                                                  |"

rm build.log
rm webservice

clang++ -std=c++11 -g -O3 -Wall -pedantic\
 -lboost_system -lboost_thread -lpqxx -lpq -largon2 -lcryptopp\
 src/*.cpp -o bin/webservice &> build.log

cat build.log

rm run.log

killall webservice
bin/webservice > run.log 2>&1 &

tail -f run.log

