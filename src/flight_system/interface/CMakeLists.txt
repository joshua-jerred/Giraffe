add_library(gfs_interface INTERFACE)
target_sources(gfs_interface INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/i2c_interface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/one_wire_interface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ubx/ubx_protocol.cpp
)
target_include_directories(gfs_interface INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/ubx
)
target_link_libraries(gfs_interface INTERFACE
	i2c
)