# This one is messy, it should be cleaned up eventually


# ---- Create Needed Directories and Transfer The Configuration File ----
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR})
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/data_logs)
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/error_logs)
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/telemetry)
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/images)

configure_file(
	${GFS_SRC}/${GIRAFFE_CONFIG_FILE_NAME}
	${GIRAFFE_BUILD_OUTPUT_DIR}/${GIRAFFE_CONFIG_FILE_NAME}
	COPYONLY
)
# ------------------------------------------------------------

if(SSTV_ENABLED)
    message(STATUS "SSTV Enabled In Giraffe Flight Software")
    add_compile_definitions(SSTV_ENABLED=1)
else()
    message(STATUS "SSTV Disabled In Giraffe Flight Software")
    add_compile_definitions(SSTV_ENABLED=0)
endif()

# ---- Giraffe Flight Software ----
include_directories(
	${GFS_SRC}/
	${GFS_SRC}/modules
	${GFS_SRC}/modules/configuration
	${GFS_SRC}/modules/data
	${GFS_SRC}/utilities
	${GFS_SRC}/extensions
	${GFS_SRC}/interface
	${GFS_SRC}/radios
	${COMMON_SRC}/

	${LIB_SRC}/MWAV/include
)

# --------- Module Sources --------
include(${GFS_SRC}/sourcelist.CMake)

# -------- GFS Executable ---------
add_executable(giraffe-flight-software
	${GFS_SRC}/main.cpp
	${GFS_SRC}/flight_runner.cpp
	${gfs_all_modules}
#
#	${GFS_SRC}/modules/module.cpp
#	${GFS_SRC}/modules/configuration/module_configuration.cpp
#	${GFS_SRC}/modules/module-data.cpp
#	${GFS_SRC}/modules/module-extensions.cpp
#	${GFS_SRC}/modules/module-console.cpp
#	${GFS_SRC}/modules/module-telemetry.cpp
#	${GFS_SRC}/modules/server/module-server.cpp
#
#	${GFS_SRC}/utilities/timer.cpp
#	${GFS_SRC}/utilities/data-stream.cpp
#	${GFS_SRC}/utilities/socket.cpp
#	${GFS_SRC}/utilities/ubx.cpp
#
#	${GFS_SRC}/interface/i2c.cpp
#	${GFS_SRC}/interface/one-wire.cpp
#	${GFS_SRC}/interface/serial.cpp
#	${GFS_SRC}/interface/gpio.cpp
#
#	${GFS_SRC}/extensions/extension.cpp
#	${GFS_SRC}/extensions/simulated-extensions.cpp
#	${GFS_SRC}/extensions/bmp180.cpp
#	${GFS_SRC}/extensions/ds18b20.cpp
#	${GFS_SRC}/extensions/samm8q.cpp
#	${GFS_SRC}/extensions/bme280.cpp
#	${GFS_SRC}/extensions/sysinfo.cpp
#	${GFS_SRC}/extensions/max17049.cpp
#	${GFS_SRC}/extensions/mcp3021.cpp
#	${GFS_SRC}/extensions/rpicam.cpp
#	
#	${GFS_SRC}/radios/radio.cpp
#	${GFS_SRC}/radios/dra-sa-radio.cpp
)
set_target_properties(giraffe-flight-software PROPERTIES OUTPUT_NAME gfs)
set_target_properties(
	giraffe-flight-software PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}
)
#target_link_libraries(giraffe-flight-software PRIVATE MWAVlib)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(giraffe-flight-software PRIVATE Threads::Threads)

#target_link_libraries(giraffe-flight-software PUBLIC i2c)
target_link_libraries(giraffe-flight-software PRIVATE nlohmann_json::nlohmann_json)