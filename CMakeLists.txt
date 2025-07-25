# Copyright © 2018-2025 Pascal JEAN, All rights reserved.
# This file is part of the Piduino Project.
#
# The Piduino Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The Piduino Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Piduino Project.  If not, see <http://www.gnu.org/licenses/>.

# piduino top-level CMakeLists.txt
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(PIDUINO)
set(PROJECT_DESCRIPTION "C++ library for Pi boards with Arduino API")
set(PROJECT_DESCRIPTION_TEXT "\
  PiDuino is a C ++ library for Pi boards that allows the use of I/O like GPIO,\n\
  I2C, SPI, UART ... with an API as close as possible to the Arduino language.\n\
  The description of Pi cards uses a stored 'Object' model in a database that \n\
  allows to add new models of boards easily.\n\
  Supported SoC models are AllWinner H-Series and Broadcom BCM2708 through 2710."
)

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

set(CMAKE_CXX_STANDARD 11)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dev/cmake" "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_MODULE_PATH})

# Set a default build type if none was specified
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
    STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set (PIDUINO_WITH_ARDUINO 1 CACHE BOOL "Enable Arduino layer")
set (PIDUINO_WITH_ARDUINO 1 CACHE BOOL "Enable Arduino layer")
set (PIDUINO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
set (PIDUINO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
set (PIDUINO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
set (PIDUINO_WITH_STATIC 0 CACHE BOOL "Enable building static libraries.")
set (PIDUINO_DRIVER_BCM2835 1 CACHE BOOL "Build the BCM2835 GPIO/PWM driver")
set (PIDUINO_DRIVER_ALLWINNERH 1 CACHE BOOL "Build the AllWinner H-series GPIO/PWM driver")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Building in Release mode.")
  set(PIDUINO_WITH_DOXYGEN_DOC 1 CACHE BOOL "Enable building Doxygen documentation.")
  set (PIDUINO_UNIT_TESTS 0 CACHE BOOL "Enable building unit tests.")
else()
  message(STATUS "Building in Debug mode.")
  set(PIDUINO_WITH_DOXYGEN_DOC 0 CACHE BOOL "Enable building Doxygen documentation.")
  set (PIDUINO_UNIT_TESTS 1 CACHE BOOL "Enable building unit tests.")
endif()

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
set(INSTALL_ETC_DIR /etc CACHE PATH "Installation directory for configuration")
set(INSTALL_CODELITE_DIR /usr/share/codelite CACHE PATH "Installation directory for codelite templates")

set(DEF_INSTALL_CMAKE_DIR lib)
set(DEF_INSTALL_DOC_DIR share/doc)
set(DEF_INSTALL_DATA_DIR share)

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
set(INSTALL_DOC_DIR ${DEF_INSTALL_DOC_DIR} CACHE PATH
  "Installation directory for documentation")
set(INSTALL_DATA_DIR ${DEF_INSTALL_DATA_DIR} CACHE PATH
  "Installation directory for data")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(CPPDB REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUDEV REQUIRED libudev)

find_package(GZIP)

pkg_check_modules (UNITTESTPP UnitTest++>=2.0.0)

set(PIDUINO_CFLAGS_OTHER ${CMAKE_THREAD_LIBS_INIT} ${LIBUDEV_CFLAGS})
set(PIDUINO_LDFLAGS_OTHER ${CPPDB_LDFLAGS} ${LIBUDEV_LDFLAGS} -lpthread)

include(GetDate)
GetDate(DATE_)

include(GitVersion)
GetGitVersion(PIDUINO_VERSION)
set(PIDUINO_VERSION
  ${PIDUINO_VERSION_MAJOR}.${PIDUINO_VERSION_MINOR}.${PIDUINO_VERSION_PATCH})

include(CheckFunctionExists)
check_function_exists(getprogname HAVE_GETPROGNAME_FUNCTION)
check_function_exists(strlcpy HAVE_STRLCPY)
check_function_exists(strlcat HAVE_STRLCAT)

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE DATA DOC CODELITE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

set(PIDUINO_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PIDUINO_ETC_DIR ${PROJECT_SOURCE_DIR}/etc)
set(PIDUINO_INC_DIR ${PROJECT_SOURCE_DIR}/include)
set(PIDUINO_DATABASE_DIR ${PROJECT_SOURCE_DIR}/database)
set(PIDUINO_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set(PIDUINO_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/piduino/cmake")
set(PIDUINO_INSTALL_DOC_DIR "${INSTALL_DOC_DIR}/piduino")
set(PIDUINO_INSTALL_EXAMPLES_DIR "${PIDUINO_INSTALL_DOC_DIR}/examples")
set(PIDUINO_INSTALL_DATA_DIR "${INSTALL_DATA_DIR}/piduino")
set(PIDUINO_INSTALL_ETC_DIR "${INSTALL_ETC_DIR}")

message(STATUS "Building for ${CMAKE_SYSTEM_PROCESSOR} architecture.")
if((${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm.*") OR (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64"))
  set(PIDUINO_SRC_GPIO_ARCH_DIR ${PIDUINO_SRC_DIR}/arch/arm)
else()
  message(WARNING "${CMAKE_SYSTEM_PROCESSOR} is not a supported architecture !")
endif()

# uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

if(${PIDUINO_WITH_ARDUINO})
  add_definitions(-DPIDUINO_WITH_ARDUINO=1)
  message(STATUS "Build with Arduino layer...")
  set(PIDUINO_WITH_GPIO 1)
endif()
if(${PIDUINO_WITH_GPIO})
  add_definitions(-DPIDUINO_WITH_GPIO=1)
  message(STATUS "Build with GPIO support...")
endif()
if(${PIDUINO_WITH_I2C})
  add_definitions(-DPIDUINO_WITH_I2C=1)
  message(STATUS "Build with I2C bus support...")
endif()
if(${PIDUINO_WITH_SPI})
  add_definitions(-DPIDUINO_WITH_SPI=1)
  message(STATUS "Build with SPI bus support...")
endif()
if(${HAVE_GETPROGNAME_FUNCTION})
  add_definitions(-DHAVE_GETPROGNAME_FUNCTION=1)
endif()

add_definitions(-DPIDUINO_INSTALL_DATA_DIR="${PIDUINO_INSTALL_DATA_DIR}"
  -DPIDUINO_INSTALL_ETC_DIR="${PIDUINO_INSTALL_ETC_DIR}"
  -DPIDUINO_PROJECT_BUILD
  -DPIDUINO_BUILD_YEAR="${DATE_YEAR}")

# Suppress GCC warnings
# See https://gcc.gnu.org/gcc-7/changes.html
# On ARM targets (arm*-*-*), a bug introduced in GCC 5 that affects conformance 
# to the procedure call standard (AAPCS) has been fixed. The bug affects some 
# C++ code where class objects are passed by value to functions and could result 
# in incorrect or inconsistent code being generated. This is an ABI change. 
# If the option -Wpsabi is enabled (on by default) the compiler will emit a 
# diagnostic note for code that might be affected. 
# <<< TODO >>> Fix this problem before compile with GCC 7 !
add_compile_options(-Wno-psabi)

add_subdirectory(dev)
add_subdirectory(lib)
add_subdirectory(utils)
add_subdirectory(doc)

if (UNITTESTPP_FOUND AND PIDUINO_UNIT_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory (tests)
endif()

if(PIDUINO_WITH_DOXYGEN_DOC)
  add_dependencies(piduino-shared doc)
endif()

### Packages generation --------------------------------------------------------
set(CPACK_PACKAGE_VERSION "${PIDUINO_VERSION_MAJOR}.${PIDUINO_VERSION_MINOR}.${PIDUINO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PIDUINO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PIDUINO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PIDUINO_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <epsilonrt@gmail.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING.LESSER")

set(CPACK_GENERATOR "DEB")

if(CPACK_GENERATOR STREQUAL "DEB")

  set(CPACK_DEB_PACKAGE_COMPONENT 1)
  set(CPACK_DEB_COMPONENT_INSTALL 1)
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/epsilonrt/piduino")

  find_program(dpkg_query NAMES dpkg-query)
  find_program(dpkg_cmd NAMES dpkg)

  if(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    if(dpkg_cmd)
      execute_process(COMMAND ${dpkg_cmd} --print-architecture RESULT_VARIABLE ret OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(ret EQUAL 0)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arch}")
      else(ret EQUAL 0)
        message(WARNING "Unable to get debian architecture !")
      endif(ret EQUAL 0)
    endif(dpkg_cmd)
  endif(NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)


  if(dpkg_query)
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libcppdb0 OUTPUT_VARIABLE libcppdb0_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libcppdb0_ver} "-" libcppdb0_ver_dash)
    #message ("libcppdb0_ver_dash=${libcppdb0_ver_dash}")
    #message ("libcppdb0_ver=${libcppdb0_ver}")
    if(libcppdb0_ver_dash)
      string(SUBSTRING ${libcppdb0_ver} 0 ${libcppdb0_ver_dash} libcppdb0_ver)
    endif(libcppdb0_ver_dash)

    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libudev1 OUTPUT_VARIABLE libudev1_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libudev1_ver} "-" libudev1_ver_dash)
    #message ("libudev1_ver_dash=${libudev1_ver_dash}")
    #message ("libudev1_ver=${libudev1_ver}")
    if(libudev1_ver_dash)
      string(SUBSTRING ${libudev1_ver} 0 ${libudev1_ver_dash} libudev1_ver)
    endif(libudev1_ver_dash)

    #message ("libcppdb0_ver=${libcppdb0_ver}")
    #message ("libudev1_ver=${libudev1_ver}")
  endif(dpkg_query)

  message(STATUS "Debian architecture ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

  set(PREINST_FILE "${PROJECT_BINARY_DIR}/preinst")
  set(POSTINST_FILE "${PROJECT_BINARY_DIR}/postinst")
  set(PRERM_FILE "${PROJECT_BINARY_DIR}/prerm")
  set(POSTRM_FILE "${PROJECT_BINARY_DIR}/postrm")
  if(CMAKE_VERSION VERSION_LESS "3.20.0") 
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/preinst.in
      ${PREINST_FILE}
      @ONLY)
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/postinst.in 
      ${POSTINST_FILE} 
      @ONLY)
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/prerm.in 
      ${PRERM_FILE} 
      @ONLY)
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/postrm.in 
      ${POSTRM_FILE} 
      @ONLY)
    execute_process(COMMAND chmod 755 "${PREINST_FILE}")
    execute_process(COMMAND chmod 755 "${POSTINST_FILE}")
    execute_process(COMMAND chmod 755 "${PRERM_FILE}")
    execute_process(COMMAND chmod 755 "${POSTRM_FILE}")
  else()
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/preinst.in
      ${PREINST_FILE}
      FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ @ONLY)
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/postinst.in 
      ${POSTINST_FILE} 
      FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ @ONLY)
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/prerm.in 
      ${PRERM_FILE} 
      FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ @ONLY)
    configure_file(
      ${PROJECT_SOURCE_DIR}/debian/postrm.in 
      ${POSTRM_FILE} 
      FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ @ONLY)
  endif()

  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libpiduino")
  set(CPACK_COMPONENT_LIB_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (library)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the shared libriairie itself.")
  set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libcppdb0,libcppdb-sqlite3-0,libcppdb-mysql0,libcppdb-odbc0,libcppdb-postgresql0,libudev1")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")
  set(CPACK_DEBIAN_LIB_PACKAGE_CONTROL_EXTRA "${PREINST_FILE};${POSTINST_FILE};${PRERM_FILE};${POSTRM_FILE}")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libpiduino-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (development files)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the development files.")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}),pkg-config,git-core,libcppdb-dev,libudev-dev")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${PIDUINO_SRC_DIR}/doc/README-deb.md")

  set(CPACK_DEBIAN_DOC_PACKAGE_NAME "libpiduino-doc")
  set(CPACK_COMPONENT_DOC_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (documentation)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the API documentation.")
  set(CPACK_DEBIAN_DOC_FILE_NAME "lib${PROJECT_NAME}-doc_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DOC_PACKAGE_DEPENDS "${CPACK_DEBIAN_DEV_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_DOC_PACKAGE_SECTION "libdevel")

  set(CPACK_DEBIAN_UTILS_PACKAGE_NAME "piduino-utils")
  set(CPACK_COMPONENT_UTILS_DESCRIPTION "${CPACK_DEBIAN_LIB_PACKAGE_NAME} - ${PROJECT_DESCRIPTION} (utilities)\n${PROJECT_DESCRIPTION_TEXT}\n  This package provides the command-line utilities.")
  set(CPACK_DEBIAN_UTILS_FILE_NAME "${PROJECT_NAME}-utils_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_UTILS_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_UTILS_PACKAGE_SECTION "utils")

endif(CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
