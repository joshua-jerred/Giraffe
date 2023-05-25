# This one is messy, it should be cleaned up eventually


# ---- Create Needed Directories and Transfer The Configuration File ----
file(MAKE_DIRECTORY ${GFS_BIN})
file(MAKE_DIRECTORY ${GFS_BIN}/data_logs)
file(MAKE_DIRECTORY ${GFS_BIN}/error_logs)
file(MAKE_DIRECTORY ${GFS_BIN}/telemetry)
file(MAKE_DIRECTORY ${GFS_BIN}/images)

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

# --- File Generation ---
# configuration
execute_process(
	COMMAND python3 ${GFS_SRC}/generators/config_gen.py 
	"${COMMON_SRC}/metadata/gfs_configuration.json"  # in json as arg
	"${GFS_SRC}/configuration" # out dir
	WORKING_DIRECTORY ${GFS_SRC}/
)

# data_ids.hpp
# execute_process(
# 	COMMAND python3 ${GFS_SRC}/generators/ids_gen.py 
# 	"${COMMON_SRC}/metadata/gfs_data.json"  # in json as arg
# 	"${GFS_SRC}/modules/data/data_ids.hpp" # out hpp as arg
# 	"data"
# 	WORKING_DIRECTORY ${GFS_SRC}/
# )

# log_ids.hpp
execute_process(
	COMMAND python3 ${GFS_SRC}/generators/ids_gen.py 
	"${COMMON_SRC}/metadata/gfs_log.json"  # in json as arg
	"${GFS_SRC}/modules/data/log_ids.hpp" # out hpp as arg
	"log"
	WORKING_DIRECTORY ${GFS_SRC}/
)

# ---- Giraffe Flight Software ----
include_directories(
	${gfs_include_dirs}

	${COMMON_SRC}/

	${LIB_SRC}/MWAV/include
)

# -------- GFS Executable ---------
add_executable(giraffe-flight-software
	${GFS_SRC}/main.cpp
	${gfs_sources}
)
set_target_properties(giraffe-flight-software PROPERTIES OUTPUT_NAME gfs)
set_target_properties(
	giraffe-flight-software PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GFS_BIN}
)
#target_link_libraries(giraffe-flight-software PRIVATE MWAVlib)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(giraffe-flight-software PRIVATE Threads::Threads)

#target_link_libraries(giraffe-flight-software PUBLIC i2c)
target_link_libraries(giraffe-flight-software PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(giraffe-flight-software PRIVATE ${CURSES_LIBRARIES})