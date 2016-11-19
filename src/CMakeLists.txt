cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(FeverGraphicsLibrary VERSION 0.0.1 LANGUAGES CXX)

# Find required libraries
find_package(GLEW REQUIRED)
add_library(glew SHARED IMPORTED)
set_property(TARGET glew PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIRS})
set_property(TARGET glew PROPERTY
  IMPORTED_LOCATION ${GLEW_LIBRARIES})

enable_testing()
add_subdirectory(libs/googletest)
add_subdirectory(FeverLibrary)
add_subdirectory(projects)
