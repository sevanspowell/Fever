################################# Fever ########################################
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(FeverLibrary VERSION 0.0.1 LANGUAGES C CXX)

add_library(Fever
  src/Sample.c
  )

target_include_directories(Fever
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  src
  )

set_property(TARGET Fever PROPERTY C_STANDARD 90)

# target_compile_features(Fever
#   )

target_link_libraries(Fever
  glew
  )

install(TARGETS Fever EXPORT FeverConfig
  ARCHIVE DESTINATION lib 
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  )
install(DIRECTORY include/ DESTINATION include)

# Make project importable from the install directory
install(EXPORT FeverConfig DESTINATION share/Fever/cmake)

# Make project importable from the build directory
export(TARGETS Fever FILE FeverConfig.cmake)

################################# Unit tests ###################################
add_executable(FeverTest
  test/test.cpp
  )

target_link_libraries(FeverTest
  Fever
  gtest
  )

add_test(FeverTest FeverTest)
