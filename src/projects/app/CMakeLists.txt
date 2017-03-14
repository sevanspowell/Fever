################################# Fever ########################################
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(FeverLibrary VERSION 0.0.1 LANGUAGES C CXX)

set_property(TARGET Fever PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Fever PROPERTY CXX_STANDARD 11)

add_executable(app
  src/app.cpp
  )

target_link_libraries(app
  PRIVATE Fever
  PRIVATE sdl2
  PRIVATE stb
  )

target_compile_features(app PUBLIC
  cxx_aggregate_default_initializers
  )
