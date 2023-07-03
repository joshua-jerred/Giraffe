# This one is messy, it should be cleaned up eventually

file(MAKE_DIRECTORY ${GFS_BIN})

if(SSTV_ENABLED)
    message(STATUS "SSTV Enabled In Giraffe Flight System")
    add_compile_definitions(SSTV_ENABLED=1)
else()
    message(STATUS "SSTV Disabled In Giraffe Flight System")
    add_compile_definitions(SSTV_ENABLED=0)
endif()

set(EXE_NAME giraffe_flight_system)

# -------- Interface Libs ---------

# # # Flight System Configuration
add_library(gfs_configuration INTERFACE)
target_sources(gfs_configuration INTERFACE
	${GFS_SRC}/configuration/configuration.cpp
	${GFS_SRC}/configuration/sections/cfg_extensions.cpp
)
target_include_directories(gfs_configuration INTERFACE
  ./configuration
	./configuration/sections
)

# # # Hardware Interface
add_library(gfs_interface INTERFACE)
target_sources(gfs_interface INTERFACE
	${GFS_SRC}/interface/i2c_interface.cpp
	${GFS_SRC}/interface/one_wire_interface.cpp
)
target_include_directories(gfs_interface INTERFACE
  ./interface
)
target_link_libraries(gfs_interface INTERFACE
	i2c
)

# # # Extensions
add_library(gfs_extensions INTERFACE)
target_sources(gfs_extensions INTERFACE
	${GFS_SRC}/extensions/extension.cpp
	${GFS_SRC}/extensions/bme280.cpp
	${GFS_SRC}/extensions/ds18b20.cpp
)
target_include_directories(gfs_extensions INTERFACE
	${GFS_SRC}/extensions
)

# # # Module Base
add_library(gfs_module_base INTERFACE)
target_sources(gfs_module_base INTERFACE
	${GFS_SRC}/modules/module.cpp
)
target_include_directories(gfs_module_base INTERFACE
  ${GFS_SRC}/modules
)

# # # Data Module
add_library(gfs_data_module INTERFACE)
target_sources(gfs_data_module INTERFACE
	${GFS_SRC}/modules/data/data_module.cpp
	${GFS_SRC}/modules/data/data_log.cpp
	${GFS_SRC}/modules/data/influxdb.cpp
	${GFS_SRC}/modules/data/data_formatting.cpp
	${GFS_SRC}/modules/data/positional.cpp
	${GFS_SRC}/modules/data/to_string.cpp
	${GFS_SRC}/modules/data/data_ids_string_map.cpp
	${GFS_SRC}/modules/data/shared_data/to_json.cpp
	${GFS_SRC}/modules/data/shared_data/error_frame.cpp
)
target_include_directories(gfs_data_module INTERFACE
  ${GFS_SRC}/modules/data
	${GFS_SRC}/modules/data/shared_data
)

# -------- GFS Executable ---------
add_executable( # see sources.cmake for source files
	${EXE_NAME}
		# main
		${GFS_SRC}/main.cpp
		${GFS_SRC}/flight_runner.cpp
	${gfs_sources}
)

target_include_directories(
	${EXE_NAME} PRIVATE
	${gfs_include_dirs}
)

# -------- Properties ---------
set_target_properties(
	${EXE_NAME} PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GFS_BIN}
	OUTPUT_NAME gfs
)

# -------- First Party Libraries ---------
target_link_libraries(giraffe_flight_system PRIVATE
	# internal
	giraffe_common
	gfs_interface
	gfs_data_module

	# external
	BoosterSeat
)

# -------- Third Party Libraries ---------
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(
	giraffe_flight_system PRIVATE 
	i2c
	Threads::Threads
	nlohmann_json::nlohmann_json
	${CURSES_LIBRARIES}
)