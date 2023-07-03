add_library(giraffe_common INTERFACE)
target_sources(giraffe_common INTERFACE
  diagnostic/giraffe_exception.cpp
  # diagnostic/giraffe_debug_assert.cpp
  diagnostic/diagnostic_id_strings.cpp
)
target_include_directories(giraffe_common INTERFACE
  ./
  ./diagnostic
  ./command
)

add_library(giraffe_debug_assert INTERFACE)
target_sources(giraffe_debug_assert INTERFACE
  diagnostic/giraffe_debug_assert.cpp
)