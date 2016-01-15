#!/bin/bash

rm *.hpp.*
rm *.out

clang++ -std=c++14 -Wall -pedantic -lboost_system -lboost_thread *.hpp *.cpp
