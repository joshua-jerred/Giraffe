set(configuration_sources
	${GFS_SRC}/configuration/configuration.cpp
	${GFS_SRC}/configuration/configuration_json.cpp
	${GFS_SRC}/configuration/validators.cpp
)

set(console_module_sources
	${GFS_SRC}/modules/console/console_module.cpp
	${GFS_SRC}/modules/console/curses_utilities.cpp
)

set(data_module_sources
	${GFS_SRC}/modules/data/streams.cpp
	${GFS_SRC}/modules/data/data_module.cpp
)

set(server_module_sources
	${GFS_SRC}/modules/server/socket.cpp
	${GFS_SRC}/modules/server/server_module.cpp
	${GFS_SRC}/modules/server/request_handler.cpp
	${COMMON_SRC}/protocol.cpp
)

set(system_module_sources
	${GFS_SRC}/modules/system/system_module.cpp
)

set(module_sources
	${GFS_SRC}/modules/module.cpp
)

set(gfs_all_modules
	${system_module_sources}
	${console_module_sources}
	${data_module_sources}
	${server_module_sources}
	${module_sources}
)