# =*========GIRAFFE========*=
# A Unified Flight Command and Control System
# https://github.com/joshua-jerred/Giraffe
# https://giraffe.joshuajer.red/
# =*=======================*=
#
# @file   package.cmake
# @see    giraffe_application_structure.md
#
# =*=======================*=
# @author     Joshua Jerred (https://joshuajer.red)
# @date       2024-12-17
# @copyright  2024 (license to be defined)

# --- CONFIGURATION --- #
# set(GIRAFFE_INSTALL_DIR "/bin/giraffe" CACHE STRING "Giraffe Install Directory - Ground Station")
# --------------------- #
# --- 'giraffe' target: Builds all primary Giraffe targets --- #

# add_custom_target(giraffe_ground_station
#   COMMENT "Package Giraffe Ground Station"
#   COMMAND ${CMAKE_COMMAND} -E copy
#                 ${CMAKE_SOURCE_DIR}/src/ground_station
#                 ${CMAKE_CURRENT_BINARY_DIR}/ground_station
#   # COMMAND bash "${CMAKE_SOURCE_DIR}/project/scripts/package_ground_station.sh"
# )


add_custom_target(giraffe_core
COMMENT "Build all primary Giraffe targets"
DEPENDS
  giraffe_flight_system
  giraffe_daemon
  giraffe_data_link_server
  # giraffe_ground_station
  COMMAND echo ""
)

set(GIRAFFE_PACKAGE_DIR_NAME "giraffe_package")
set(GIRAFFE_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/${GIRAFFE_PACKAGE_DIR_NAME}")
add_custom_target(giraffe_package
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin"
  DEPENDS giraffe_core

  # Clean up the package directory
  COMMAND rm -rf "${GIRAFFE_PACKAGE_DIR}"
  COMMAND rm -rf "${CMAKE_CURRENT_BINARY_DIR}/bin/giraffe*.tar.gz"

  # Create the directory with the software manifest
  COMMAND ${CMAKE_COMMAND} -E make_directory "${GIRAFFE_PACKAGE_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/src/software_manifest.json"
    "${GIRAFFE_PACKAGE_DIR}/software_manifest.json"

  # Ground Station
  COMMAND ${CMAKE_COMMAND} -E make_directory
    "${GIRAFFE_PACKAGE_DIR}/ground_station"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/src/ground_station/backend"
  "${GIRAFFE_PACKAGE_DIR}/ground_station/backend"
  COMMAND rm -rf "${GIRAFFE_PACKAGE_DIR}/ground_station/backend/node_modules"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  "${CMAKE_SOURCE_DIR}/src/ground_station/frontend"
  "${GIRAFFE_PACKAGE_DIR}/ground_station/frontend"
  COMMAND rm -rf "${GIRAFFE_PACKAGE_DIR}/ground_station/frontend/node_modules"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_BINARY_DIR}/bin/gdl_server"
    "${GIRAFFE_PACKAGE_DIR}/ground_station/gdl_server"

  # Flight System
  COMMAND ${CMAKE_COMMAND} -E make_directory
    "${GIRAFFE_PACKAGE_DIR}/flight_system"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_BINARY_DIR}/bin/gfs"
    "${GIRAFFE_PACKAGE_DIR}/flight_system/gfs"

  # Static Files
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/project/package/static_content/install-ground-station.sh"
    "${GIRAFFE_PACKAGE_DIR}/install-ground-station.sh"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/project/package/static_content/install-flight-system.sh"
    "${GIRAFFE_PACKAGE_DIR}/install-flight-system.sh"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/project/package/static_content/README.md"
    "${GIRAFFE_PACKAGE_DIR}/README.md"

  # Tar it all up
  COMMAND tar -czf
    "${CMAKE_CURRENT_BINARY_DIR}/bin/giraffe-${CMAKE_SYSTEM_PROCESSOR}-${GIRAFFE_SEMANTIC_VERSION}.tar.gz"
    "${GIRAFFE_PACKAGE_DIR_NAME}"
)

add_custom_target(giraffe
COMMENT "[recommended] Build full Giraffe system"
DEPENDS
  giraffe_core
  giraffe_package
  COMMAND echo ""
)

# --- 'install' target: Installs the 'giraffe' target components --- #
# message(STATUS "Giraffe Install Directory: ${GIRAFFE_INSTALL_DIR}")

# install(DIRECTORY DESTINATION ${GIRAFFE_INSTALL_DIR})

# install(PROGRAMS
#   "${CMAKE_SOURCE_DIR}/project/scripts/install.sh"
# )