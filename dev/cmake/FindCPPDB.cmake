#ckwg +4
# Copyright 2010 by Kitware, Inc. All Rights Reserved. Please refer to
# KITWARE_LICENSE.TXT for licensing information, or contact General Counsel,
# Kitware, Inc., 28 Corporate Drive, Clifton Park, NY 12065.

# Locate the system installed CppDB
# The following variables will be set:
#
# CPPDB_FOUND   - Set to true if CPPDB can be found
# CPPDB_INCLUDE_DIR - The path to the CPPDB header files
# CPPDB_LIBRARY     - The full path to the CPPDB library
# CPPDB_LIB_DIR     - The full path containing the CPPDB library

if( CPPDB_DIR )
  find_package( CPPDB NO_MODULE )
elseif( NOT CPPDB_FOUND )

  message(STATUS "Searching for cppdb/frontend.h")
  find_path( CPPDB_INCLUDE_DIR
    NAMES frontend.h
    PATHS ${CPPDB_INCLUDE_DIR}
    PATH_SUFFIXES cppdb )
  message(STATUS "Searching for frontend.h - ${CPPDB_INCLUDE_DIR}")

  message(STATUS "Searching for libcppdb")
  find_library( CPPDB_LIBRARY cppdb PATHS ${CPPDB_LIB_DIR})
  message(STATUS "Searching for libcppdb - ${CPPDB_LIBRARY}")


  include( FindPackageHandleStandardArgs )
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    CPPDB
    DEFAULT_MSG
    CPPDB_INCLUDE_DIR
    CPPDB_LIBRARY )

  if( CPPDB_FOUND )
    get_filename_component(CPPDB_LIB_DIR ${CPPDB_LIBRARY} PATH)
  endif( CPPDB_FOUND )

endif()
