cmake_minimum_required(VERSION 3.14)

project(
	rpicam-apps
	VERSION 1.0
	DESCRIPTION "rpicam-apps with cmake"
)

set(CMAKE_CXX_STANDARD 17)

add_executable(rpi_still
  apps/rpicam_still.cpp
)
target_include_directories(rpi_still PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/core
)