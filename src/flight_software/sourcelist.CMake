set(configuration_sources
	${GFS_SRC}/configuration/configuration.cpp
	${GFS_SRC}/configuration/configuration_json.cpp
	${GFS_SRC}/configuration/validators.cpp
)

set(data_module_sources
	${GFS_SRC}/modules/data/streams.cpp
	${GFS_SRC}/modules/data/data_module.cpp
)

set(module_sources
	${GFS_SRC}/modules/module.cpp
)

set(gfs_all_modules
	${data_module_sources}
	${module_sources}
)