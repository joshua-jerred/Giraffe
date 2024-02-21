add_library(gfs_configuration INTERFACE)
target_sources(gfs_configuration INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sections/cfg_extensions.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sections/cfg_adcmappings.cpp
)
target_include_directories(gfs_configuration INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/sections
)