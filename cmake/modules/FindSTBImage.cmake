# Find STBI
# Find the STBI includes and library
#
#  STBI_INCLUDE_DIRS - where to find stb image header file.
#  STBI_FOUND        - True if STBI found.
#
# Modified from https://github.com/tapio/tomato/blob/master/cmake/FindSOIL.cmake

IF (STBI_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(STBI_FIND_QUIETLY TRUE)
ENDIF (STBI_INCLUDE_DIRS)

FIND_PATH(STBI_INCLUDE_DIR stb_image.h PATH_SUFFIXES include PATHS ${STBI_BASE_DIR})

MARK_AS_ADVANCED( STBI_INCLUDE_DIR )

# Per-recommendation
SET(STBI_INCLUDE_DIRS "${STBI_INCLUDE_DIR}")

# handle the QUIETLY and REQUIRED arguments and set STBI_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(stb DEFAULT_MSG STBI_INCLUDE_DIRS)
