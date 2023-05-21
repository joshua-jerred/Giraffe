# This one is messy, it should be cleaned up eventually


# ---- Create Needed Directories and Transfer The Configuration File ----
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR})
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/data_logs)
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/error_logs)
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/telemetry)
file(MAKE_DIRECTORY ${GIRAFFE_BUILD_OUTPUT_DIR}/images)

#configure_file(
#	${GFS_SRC}/${GIRAFFE_CONFIG_FILE_NAME}
#	${GIRAFFE_BUILD_OUTPUT_DIR}/${GIRAFFE_CONFIG_FILE_NAME}
#	COPYONLY
#)
# ------------------------------------------------------------

if(SSTV_ENABLED)
    message(STATUS "SSTV Enabled In Giraffe Flight Software")
    add_compile_definitions(SSTV_ENABLED=1)
else()
    message(STATUS "SSTV Disabled In Giraffe Flight Software")
    add_compile_definitions(SSTV_ENABLED=0)
endif()

# ---------------- Sources --------
include(${GFS_SRC}/sourcelist.CMake)

# ---- Giraffe Flight Software ----
include_directories(
	${GFS_SRC}/
	${GFS_SRC}/configuration
	${GFS_SRC}/modules
	${GFS_SRC}/modules/data
	${GFS_SRC}/utilities
	${GFS_SRC}/extensions
	${GFS_SRC}/interface
	${GFS_SRC}/radios
	${COMMON_SRC}/

	${LIB_SRC}/MWAV/include
)



# -------- GFS Executable ---------
add_executable(giraffe-flight-software
	${GFS_SRC}/main.cpp
	${GFS_SRC}/flight_runner.cpp
	${configuration_sources}
	${gfs_all_modules}
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