#!/usr/bin/env bash

set -e

export PRINT_OUTPUT="false"

function id_gen() {
  export ID_GEN_IN_FILE_PATH=$1
  export ID_GEN_OUT_FILE_PATH=$2
  export STRING_MAP_OUT_FILE_PATH=$3
  python3 ./project/generators/ids_gen.py
}

echo "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"
echo "-- --    generating automated source files      -- --"

export REVERSE_MAP="false"
id_gen \
  ./project/metadata/gfs_data.jsonc \
  ./src/common/data_ids.hpp \
  ./src/common/data_ids_string_map.cpp

id_gen \
  ./project/metadata/diagnostic_ids.json \
  ./src/common/giraffe_diagnostic_ids.hpp \
  ./src/common/diagnostic_id_strings.cpp

export REVERSE_MAP="true"
id_gen \
  ./project/metadata/gfs_commands.json \
  ./src/common/command_ids.hpp \
  ./src/common/command_ids_strings.cpp
export REVERSE_MAP="false"

export CONFIG_GEN_IN_FILE_PATH=./project/metadata/gfs_configuration.json
export CONFIG_GEN_OUT_FILE_PATH=./src/flight_system/configuration
python3 ./project/generators/config_gen.py

echo "Generating bit test enums"
python3 ./project/generators/bit_test_gen.py \
  ./project/metadata/gfs_bit_test.json \
  ./src/flight_system/flight_runner/bit_test_enums.hpp

echo "-- -- -- -- -- --     DONE    -- -- -- -- -- -- -- --"
echo "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --"