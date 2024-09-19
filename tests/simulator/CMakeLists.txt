add_library(gfs_simulator INTERFACE)
target_sources(gfs_simulator INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/gfs_simulator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sim_environmental.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sim_physics.cpp
)
target_link_libraries(gfs_simulator INTERFACE
  BoosterSeat
  fmt::fmt
)
target_include_directories(gfs_simulator INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(gfs_simulator_standalone
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)
target_link_libraries(gfs_simulator_standalone PRIVATE
  gfs_simulator
)