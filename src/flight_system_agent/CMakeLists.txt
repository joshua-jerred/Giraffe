add_executable(flight_system_agent
  "./main.cpp"
)
target_link_libraries(flight_system_agent
  giraffe_common
  BoosterSeat
  # giraffe_debug_assert
  # SignalEasel
)
target_include_directories(flight_system_agent PRIVATE
  ${THIRD_PARTY_SRC}/nlohmann_json/single_include
)