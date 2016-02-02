#!/bin/bash

rm *.hpp.*
rm *.out

clang++ -std=c++11 -g -O0 -Wall -pedantic -lboost_system -lboost_thread -lpqxx -lpq -largon2 -lcryptopp *.cpp -o WebServer &> build.log

cat build.log | more
