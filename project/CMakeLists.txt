add_custom_command(
  COMMAND  ${CMAKE_SOURCE_DIR}/project/generate_source_files.sh
  DEPENDS
    ${CMAKE_SOURCE_DIR}/project/generate_source_files.sh
    ${CMAKE_SOURCE_DIR}/project/generators/ids_gen.py
    ${CMAKE_SOURCE_DIR}/project/generators/config_gen.py
    ${CMAKE_SOURCE_DIR}/project/metadata/gfs_data.jsonc
    ${CMAKE_SOURCE_DIR}/project/metadata/diagnostic_ids.jsonc
    ${CMAKE_SOURCE_DIR}/project/metadata/gfs_commands.jsonc
    ${CMAKE_SOURCE_DIR}/project/metadata/gfs_configuration.json
  OUTPUT
    ${CMAKE_SOURCE_DIR}/src/common/data_ids.hpp
    ${CMAKE_SOURCE_DIR}/src/common/data_ids_string_map.cpp
    ${CMAKE_SOURCE_DIR}/src/common/giraffe_diagnostic_ids.hpp
    ${CMAKE_SOURCE_DIR}/src/common/diagnostic_id_strings.cpp
    ${CMAKE_SOURCE_DIR}/src/common/command_ids.hpp
    ${CMAKE_SOURCE_DIR}/src/common/command_id_strings.cpp
    ${CMAKE_SOURCE_DIR}/src/flight_system/configuration/configuration_enums.hpp
    ${CMAKE_SOURCE_DIR}/src/flight_system/configuration/configuration.cpp
    ${CMAKE_SOURCE_DIR}/src/flight_system/configuration/configuration.hpp
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "GENERATING AUTO-GENERATED SOURCE FILES"
)
