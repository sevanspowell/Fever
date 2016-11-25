# Find STB
# Find the STB includes and library
#
#  STB_INCLUDE_DIRS - where to find stb image header file.
#  STB_FOUND        - True if STB found.
#
# Modified from https://github.com/tapio/tomato/blob/master/cmake/FindSOIL.cmake

IF (STB_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(STB_FIND_QUIETLY TRUE)
ENDIF (STB_INCLUDE_DIRS)

FIND_PATH(STB_INCLUDE_DIR stb_image.h PATH_SUFFIXES include PATHS ${STB_BASE_DIR})

MARK_AS_ADVANCED( STB_INCLUDE_DIR )

# Per-recommendation
SET(STB_INCLUDE_DIRS "${STB_INCLUDE_DIR}")

# handle the QUIETLY and REQUIRED arguments and set STB_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(stb DEFAULT_MSG STB_INCLUDE_DIRS)
