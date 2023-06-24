add_library(giraffe_data_link STATIC
  "./giraffe_data_link.cpp"
  "./message_queue.cpp"
)
target_include_directories(giraffe_data_link PUBLIC include)
target_include_directories(giraffe_data_link PRIVATE "./")
