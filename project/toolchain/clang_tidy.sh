#!/bin/bash

FILE_LIST=(
  # "./src/flight_system/main.cpp"
  # "./src/flight_system/flight_runner.cpp"
  "./src/flight_system/extensions/bme280.cpp"
)

clang-tidy -config-file=.clang-tidy $(echo ${FILE_LIST[*]}) -p=build