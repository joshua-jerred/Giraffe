add_library(giraffe_data_link STATIC
  "./giraffe_data_link.cpp"
  "./message_queue.cpp"
)
target_include_directories(giraffe_data_link PUBLIC include)
target_include_directories(giraffe_data_link PRIVATE "./")
target_link_libraries(giraffe_data_link 
  giraffe_common
  BoosterSeat
)

add_executable(gdl_terminal
  "./gdl_terminal.cpp"
)
target_link_libraries(gdl_terminal giraffe_data_link)

add_library(ax25_state_machine INTERFACE
"./ax25/state_machine/management_data_link.cpp"
"./ax25/state_machine/segmenter.cpp"
"./ax25/state_machine/reassembler.cpp"
"./ax25/state_machine/data_link.cpp"
"./ax25/state_machine/link_multiplexer.cpp"
"./ax25/state_machine/duplex_physical_layer.cpp"
"./ax25/state_machine/simplex_physical_layer.cpp"
)
target_include_directories(ax25_state_machine INTERFACE 
  "./ax25/state_machine/"
  "./ax25/"
)
target_link_libraries(ax25_state_machine INTERFACE
  giraffe_common
  BoosterSeat
)