################################# Fever ########################################
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(app VERSION 0.0.1 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
  src/app.cpp
  )

target_link_libraries(${PROJECT_NAME}
  PRIVATE Fever
  PRIVATE sdl2
  PRIVATE stb
  PRIVATE glm
  PRIVATE tinyobj
  )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

target_compile_features(${PROJECT_NAME} PUBLIC
  cxx_aggregate_default_initializers
  )
