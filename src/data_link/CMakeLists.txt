add_library(giraffe_data_link STATIC
  "./giraffe_data_link.cpp"
  "./message_queue.cpp"
)
target_include_directories(giraffe_data_link PUBLIC include)
target_include_directories(giraffe_data_link PRIVATE "./")
target_link_libraries(giraffe_data_link giraffe_diagnostics)

add_executable(gdl_terminal
  "./gdl_terminal.cpp"
)
target_link_libraries(gdl_terminal giraffe_data_link)