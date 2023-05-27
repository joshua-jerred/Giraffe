set(common_sources
	${COMMON_SRC}/protocol.cpp
)

set(common_includes
	${COMMON_SRC}/
)

set(gfs_sources
	# main
	${GFS_SRC}/flight_runner.cpp

	# configuration 
	${GFS_SRC}/configuration/configuration.cpp

	# module base
	${GFS_SRC}/modules/module.cpp

	# data module
	${GFS_SRC}/modules/data/data_module.cpp

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
)

set(gfs_include_dirs
	${GFS_SRC}/
	${GFS_SRC}/utilities
	${GFS_SRC}/configuration
	${GFS_SRC}/modules
	${GFS_SRC}/modules/data
	${GFS_SRC}/modules/console
	${GFS_SRC}/modules/server
	${GFS_SRC}/modules/system
)