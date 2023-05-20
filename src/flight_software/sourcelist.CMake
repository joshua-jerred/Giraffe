set(configuration_module_sources
	${GFS_SRC}/modules/configuration/configuration_module.cpp
	${GFS_SRC}/modules/configuration/configuration.cpp
	${GFS_SRC}/modules/configuration/configuration_json.cpp
	${GFS_SRC}/modules/configuration/validators.cpp
	${GFS_SRC}/modules/configuration/file.cpp
)

set(data_module_sources
	${GFS_SRC}/modules/data/streams.cpp
	${GFS_SRC}/modules/data/data_module.cpp
)

set(module_sources
	${GFS_SRC}/modules/module.cpp
)

set(gfs_all_modules
	${configuration_module_sources}
	${data_module_sources}
	${module_sources}
)