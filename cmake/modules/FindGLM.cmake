# Find GLM
# Find the GLM includes and library
#
#  GLM_INCLUDE_DIRS - where to find glm image header file.
#  GLM_FOUND        - True if GLM found.
#
# Modified from https://github.com/tapio/tomato/blob/master/cmake/FindSOIL.cmake

IF (GLM_INCLUDE_DIRS)
  # Already in cache, be silent
  SET(GLM_FIND_QUIETLY TRUE)
ENDIF (GLM_INCLUDE_DIRS)

FIND_PATH(GLM_INCLUDE_DIR glm/glm.hpp PATH_SUFFIXES include PATHS ${GLM_BASE_DIR})

MARK_AS_ADVANCED( GLM_INCLUDE_DIR )

# Per-recommendation
SET(GLM_INCLUDE_DIRS "${GLM_INCLUDE_DIR}")

# handle the QUIETLY and REQUIRED arguments and set GLM_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(glm DEFAULT_MSG GLM_INCLUDE_DIRS)
