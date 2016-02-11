#!/bin/bash

echo "|                                                  |"
echo "|                                                  |"
echo "|                                                  |"

clang++ -std=c++11 -g -O0 -Wall -pedantic\
 -lboost_system -lboost_thread -lpqxx -lpq -largon2 -lcryptopp\
 -lredox -lev -lhiredis\
 *.cpp -o WebService &> build.log

cat build.log | more