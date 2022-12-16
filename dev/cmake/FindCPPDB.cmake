# Find the CppDb includes and library
#
# This module defines
#  CPPDB_INCLUDE_DIRS - include directories for cppdb
#  CPPDB_LIBRARY_DIRS - where to find libraries
#  CPPDB_LIBRARIES    - libraries to link against

find_path(CPPDB_INCLUDE_DIRS cppdb/frontend.h)

find_library(CPPDB_LIBRARIES cppdb)

if(CPPDB_INCLUDE_DIRS)
    if(CPPDB_LIBRARIES)
      get_filename_component(CPPDB_LIBRARY_DIRS ${CPPDB_LIBRARIES} PATH)
      set(CPPDB_FOUND TRUE)
      set(CPPDB_LIBRARIES cppdb ${CMAKE_DL_LIBS})
      set(CPPDB_LDFLAGS -lcppdb -l${CMAKE_DL_LIBS})
    endif(CPPDB_LIBRARIES)
endif(CPPDB_INCLUDE_DIRS)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CPPDB DEFAULT_MSG CPPDB_LIBRARIES CPPDB_INCLUDE_DIRS)

#message(STATUS "CPPDB_INCLUDE_DIRS=${CPPDB_INCLUDE_DIRS}")
#message(STATUS "CPPDB_LIBRARIES=${CPPDB_LIBRARIES}")
#message(STATUS "CPPDB_LIBRARY_DIRS=${CPPDB_LIBRARY_DIRS}")
