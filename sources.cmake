
# --- DIRECTORIES --- #
set(COMMON_SRC ${CMAKE_SOURCE_DIR}/src/common)
set(COMMON_BIN ${CMAKE_BINARY_DIR}/common)
set(COMMON_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/common)
set(COMMON_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/common)

set(GFS_SRC  ${CMAKE_SOURCE_DIR}/src/flight_system)
set(GFS_BIN ${CMAKE_BINARY_DIR}/bin/flight_system)
set(GFS_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/flight_system)
set(GFS_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/flight_system)

set(GDL_SRC ${CMAKE_SOURCE_DIR}/src/data_link)
set(GDL_BIN ${CMAKE_BINARY_DIR}/bin/data_link)
set(GDL_TEST_SRC ${CMAKE_SOURCE_DIR}/tests/data_link)
set(GDL_TEST_BIN ${CMAKE_BINARY_DIR}/bin/tests/data_link)

set(LIB_SRC ${CMAKE_SOURCE_DIR}/lib)
# ------------------- #

set(common_sources
	${COMMON_SRC}/protocol/protocol.cpp
)

set(common_include_dirs
	${COMMON_SRC}
	"${COMMON_SRC}/protocol"
	"${COMMON_SRC}/command"
)

set(gfs_sources
	# common
	${common_sources}

	# configuration 
	${GFS_SRC}/configuration/configuration.cpp
	${GFS_SRC}/configuration/sections/cfg_extensions.cpp

	# module base
	${GFS_SRC}/modules/module.cpp

	# data module
	${GFS_SRC}/modules/data/data_module.cpp
	${GFS_SRC}/modules/data/data_log.cpp
	${GFS_SRC}/modules/data/influxdb.cpp
	${GFS_SRC}/modules/data/data_formatting.cpp
	${GFS_SRC}/modules/data/positional.cpp
	${GFS_SRC}/modules/data/shared_data/to_json.cpp
	${GFS_SRC}/modules/data/shared_data/error_frame.cpp

	# console module
	${GFS_SRC}/modules/console/console_module.cpp
	${GFS_SRC}/modules/console/curses_utilities.cpp
	${GFS_SRC}/modules/console/console_pages.cpp

	# server module
	${GFS_SRC}/modules/server/socket.cpp
	${GFS_SRC}/modules/server/server_module.cpp
	${GFS_SRC}/modules/server/request_handler.cpp

	# system module
	${GFS_SRC}/modules/system/system_module.cpp

	# extensions module and extensions
	${GFS_SRC}/modules/extension/extension_module.cpp
	${GFS_SRC}/extensions/extension.cpp
)

set(gfs_include_dirs
	${common_include_dirs}
	${GFS_SRC}/
	${GFS_SRC}/utilities
	${GFS_SRC}/configuration
	${GFS_SRC}/command

	# modules
	${GFS_SRC}/modules
	${GFS_SRC}/modules/data
	${GFS_SRC}/modules/data/shared_data
	${GFS_SRC}/modules/console
	${GFS_SRC}/modules/server
	${GFS_SRC}/modules/system
	${GFS_SRC}/modules/extension

	# extensions
	${GFS_SRC}/extensions
)