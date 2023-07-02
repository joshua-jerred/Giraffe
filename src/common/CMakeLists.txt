add_library(giraffe_diagnostics STATIC
  diagnostic/giraffe_exception.cpp
  diagnostic/giraffe_debug_assert.cpp
  diagnostic/diagnostic_id_strings.cpp
)
target_include_directories(giraffe_diagnostics PUBLIC ./diagnostic)