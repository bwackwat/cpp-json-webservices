#!/bin/bash

rm *.hpp.*
rm *.out

clang++ -std=c++11 -g -O0 -Wall -pedantic -lboost_system -lboost_thread *.cpp -o WebServer
