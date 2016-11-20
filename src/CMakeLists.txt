cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(FeverGraphicsLibrary VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake/modules")

# Find required libraries
######### GLEW ########## 
find_package(GLEW REQUIRED)
add_library(glew SHARED IMPORTED)
set_property(TARGET glew PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIRS})
set_property(TARGET glew PROPERTY
  IMPORTED_LOCATION ${GLEW_LIBRARIES})

######### SDL2 ########## 
find_package(SDL2 REQUIRED)
add_library(sdl2 SHARED IMPORTED)
# message("SDL2 INCLUDE DIR: " ${SDL2_INCLUDE_DIR})
# message("SDL2 LIBRARY: " ${SDL2_LIBRARY})
# set(SDL2_LIBRARY "/usr/local/lib/libSDL2.dylib")
message("SDL2 LIBRARY: " ${SDL2_LIBRARY})
set_property(TARGET sdl2 PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR})
set_property(TARGET sdl2 PROPERTY
  IMPORTED_LOCATION ${SDL2_LIBRARY})

enable_testing()
add_subdirectory(libs/googletest)
add_subdirectory(FeverLibrary)
add_subdirectory(projects)
