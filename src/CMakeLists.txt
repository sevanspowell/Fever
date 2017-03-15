cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(FeverGraphicsLibrary VERSION 0.0.1 LANGUAGES C CXX)

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
set_property(TARGET sdl2 PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR})
set_property(TARGET sdl2 PROPERTY
  IMPORTED_LOCATION ${SDL2_LIBRARY})

######### STB Image ########### 
set(STB_BASE_DIR "libs/stb_image")
find_package(STB REQUIRED)
add_library(stb INTERFACE)
set_property(TARGET stb PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${STB_INCLUDE_DIRS})

######### Metal ########## 
if (APPLE)
  find_library(METAL_LIBRARY Metal)
  find_library(QUARTZCORE_FRAMEWORK QuartzCore)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation )
  find_library(COCOA_LIBRARY Cocoa)
endif (APPLE)

enable_testing()
add_subdirectory(libs/googletest EXCLUDE_FROM_ALL)
add_subdirectory(FeverLibrary)
add_subdirectory(projects/app)
