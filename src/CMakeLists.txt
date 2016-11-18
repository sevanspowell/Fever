cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(FeverGraphicsLibrary VERSION 0.0.1 LANGUAGES CXX)

# Find required libraries

enable_testing()
add_subdirectory(FeverLibrary)
add_subdirectory(app)
