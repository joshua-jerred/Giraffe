add_executable(gfs_simulator
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/sim_environmental.cpp
)
target_link_libraries(gfs_simulator
  BoosterSeat
)