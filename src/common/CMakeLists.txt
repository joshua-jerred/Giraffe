# A generic 
add_library(giraffe_debug_assert STATIC
  giraffe_debug_assert.cpp
  giraffe_assert.hpp
)
target_include_directories(giraffe_debug_assert PUBLIC ./)

add_library(giraffe_diagnostics STATIC
  diagnostic/giraffe_diagnostics.cpp
  diagnostic/diagnostic_id_strings.cpp
)
target_include_directories(giraffe_diagnostics PUBLIC ./diagnostic)