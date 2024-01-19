add_library(giraffe_data_link_core STATIC
  "./giraffe_data_link.cpp"
  "./message_queue.cpp"
  "./network_layer.cpp"
  "./physical_layer.cpp"
  "./transport_layer.cpp"
)
target_include_directories(giraffe_data_link_core PUBLIC include)
target_include_directories(giraffe_data_link_core PRIVATE "./")
target_link_libraries(giraffe_data_link_core
  giraffe_common
  BoosterSeat
  SignalEasel
)

add_executable(gdl_server
  "server/main.cpp"
)
target_link_libraries(gdl_server 
  giraffe_data_link_core
  SQLiteCpp
  sqlite3
)
target_include_directories(gdl_server PRIVATE "./server")

add_executable(gdl_terminal
  "./gdl_terminal.cpp"
)
target_link_libraries(gdl_terminal 
  giraffe_data_link_core
)