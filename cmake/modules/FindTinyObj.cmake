# Find TinyObj header
#
#  TINYOBJ_INCLUDE_DIRS - where to find stb image header file.
#  TINYOBJ_FOUND        - True if STB found.
#
#  TINYOBJ_BASE_DIR - Provide the possible location of the tiny obj header file
#
# Modified from https://github.com/tapio/tomato/blob/master/cmake/FindSOIL.cmake

IF (TINYOBJ_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(TINYOBJ_FIND_QUIETLY TRUE)
ENDIF (TINYOBJ_INCLUDE_DIRS)

FIND_PATH(TINYOBJ_INCLUDE_DIR tiny_obj_loader.h PATH_SUFFIXES include PATHS ${TINYOBJ_BASE_DIR})

MARK_AS_ADVANCED( TINYOBJ_INCLUDE_DIR )

# Per-recommendation
SET(TINYOBJ_INCLUDE_DIRS "${TINYOBJ_INCLUDE_DIR}")

# handle the QUIETLY and REQUIRED arguments and set TINYOBJ_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(tinyobj DEFAULT_MSG TINYOBJ_INCLUDE_DIRS)
