add_library(gfs_extensions INTERFACE)
target_sources(gfs_extensions INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/extension_base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/extension_base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/adc_configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/bme280.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ds18b20.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/samm8q.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mcp3021.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rgb_status_led.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/bmi088.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/pi_camera.cpp
)
target_include_directories(gfs_extensions INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)