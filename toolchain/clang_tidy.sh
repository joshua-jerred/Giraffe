#!/bin/bash

FILE_LIST=(
  "./src/flight_system/main.cpp"
)

clang-tidy -config-file=.clang-tidy $(echo ${FILE_LIST[*]}) -p=build