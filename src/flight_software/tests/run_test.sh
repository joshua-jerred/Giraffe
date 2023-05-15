#!/bin/bash
test_path="./build/tests/flight_software/$TEST_ID"
test_name="$TEST_NAME"
echo "Running test $test_name in $test_path"
cd "./build/tests/flight_software/$TEST_ID" && ./$TEST_NAME