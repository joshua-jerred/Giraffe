set(AUTO_GENERATED_INPUT_FILES
    ${CMAKE_SOURCE_DIR}/project/generate_source_files.sh
    ${CMAKE_SOURCE_DIR}/project/generators/ids_gen.py
    ${CMAKE_SOURCE_DIR}/project/generators/config_gen.py
    ${CMAKE_SOURCE_DIR}/project/metadata/gfs_data.jsonc
    ${CMAKE_SOURCE_DIR}/project/metadata/diagnostic_ids.json
    ${CMAKE_SOURCE_DIR}/project/metadata/gfs_commands.json
    ${CMAKE_SOURCE_DIR}/project/metadata/gfs_configuration.json)
set(AUTO_GENERATED_OUTPUT_FILES
    ${CMAKE_SOURCE_DIR}/src/common/data_ids.hpp
    ${CMAKE_SOURCE_DIR}/src/common/data_ids_string_map.cpp
    ${CMAKE_SOURCE_DIR}/src/common/giraffe_diagnostic_ids.hpp
    ${CMAKE_SOURCE_DIR}/src/common/diagnostic_id_strings.cpp
    ${CMAKE_SOURCE_DIR}/src/common/command_ids.hpp
    ${CMAKE_SOURCE_DIR}/src/common/command_id_strings.cpp
    ${CMAKE_SOURCE_DIR}/src/flight_system/configuration/configuration_enums.hpp
    ${CMAKE_SOURCE_DIR}/src/flight_system/configuration/configuration.cpp
    ${CMAKE_SOURCE_DIR}/src/flight_system/configuration/configuration.hpp)

# TODO: Dependency problems, does not build when it should.
add_custom_target(
  AutoGeneratedFiles ALL
  DEPENDS ${AUTO_GENERATED_INPUT_FILES}
  COMMENT "GENERATING AUTO-GENERATED SOURCE FILES")
add_custom_command(
  OUTPUT ${AUTO_GENERATED_OUTPUT_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  DEPENDS ${AUTO_GENERATED_INPUT_FILES}
  COMMENT "GENERATING AUTO-GENERATED SOURCE FILES"
  COMMAND ${CMAKE_SOURCE_DIR}/project/generate_source_files.sh)
