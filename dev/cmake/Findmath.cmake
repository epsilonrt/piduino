# Try to find real time libraries
# Once done, this will define
#
# MATH_FOUND - system has math library
# MATH_LIBRARIES - math libraries directory

if(MATH_LIBRARIES)
  set(MATH_FIND_QUIETLY TRUE)
endif(MATH_LIBRARIES)
 
find_library(MATH_LIBRARY m)
set(MATH_LIBRARIES ${MATH_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set
# MATH_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(math DEFAULT_MSG MATH_LIBRARY)

if(MATH_FOUND)
  set(MATH_LDFLAGS -lm)
  #message("MATH_LDFLAGS=${MATH_LDFLAGS}")
endif(MATH_FOUND)

mark_as_advanced(MATH_LIBRARY)
