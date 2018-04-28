# Copyright © 2018 Pascal JEAN, All rights reserved.
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
cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project (PIDUINO)
set (PROJECT_DESCRIPTION "Library and tools for embedded development")
set (PROJECT_TOOLS_PATHS ${PROJECT_SOURCE_DIR}/dev/bin ${PROJECT_SOURCE_DIR}/dev/rpi)

string(TOLOWER ${CMAKE_PROJECT_NAME} PROJECT_NAME)

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dev/cmake" "${CMAKE_CURRENT_BINARY_DIR}" ${CMAKE_MODULE_PATH})

set (PIDUINO_WITH_STATIC 0 CACHE BOOL "Enable building static libraries.")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set (PIDUINO_CFLAGS_OTHER ${CMAKE_THREAD_LIBS_INIT})

#find_package(PkgConfig REQUIRED)
find_package(CppDb REQUIRED)

#pkg_check_modules (SQLITE3 REQUIRED sqlite3)
set (PIDUINO_LDFLAGS_OTHER  ${CPPDB_LDFLAGS})
#list(APPEND PIDUINO_CFLAGS_OTHER ${SQLITE3_CFLAGS})

include (GitVersion)
include (PiBoardInfo)

GetGitVersion(PIDUINO_VERSION)
set(PIDUINO_VERSION
  ${PIDUINO_VERSION_MAJOR}.${PIDUINO_VERSION_MINOR}.${PIDUINO_VERSION_PATCH})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")

set(DEF_INSTALL_CMAKE_DIR lib)
set(DEF_INSTALL_DOC_DIR share/doc)
set(DEF_INSTALL_DATA_DIR share)

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
set(INSTALL_DOC_DIR ${DEF_INSTALL_DOC_DIR} CACHE PATH
  "Installation directory for documentation")
set(INSTALL_DATA_DIR ${DEF_INSTALL_DATA_DIR} CACHE PATH
  "Installation directory for data")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE DATA DOC)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

set (PIDUINO_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set (PIDUINO_INC_DIR ${PROJECT_SOURCE_DIR}/include)
set (PIDUINO_DATABASE_DIR ${PROJECT_SOURCE_DIR}/sql)
set (PIDUINO_LIB_DIR ${PROJECT_BINARY_DIR}/lib)
set (PIDUINO_INSTALL_CMAKE_DIR "${INSTALL_CMAKE_DIR}/piduino/cmake")
set (PIDUINO_INSTALL_DOC_DIR "${INSTALL_DOC_DIR}/piduino")
set (PIDUINO_INSTALL_EXAMPLES_DIR "${PIDUINO_INSTALL_DOC_DIR}/examples")
set (PIDUINO_INSTALL_DATA_DIR "${INSTALL_DATA_DIR}/piduino")

if (NOT PIBOARD_ID)
  message (STATUS "Check the target platform, you can use PIBOARD_ID to force the target...")
  # Détection du type de carte sur le système hôte, doit être forcé en cross-compile
  GetPiBoardInfo()
endif (NOT PIBOARD_ID)

if (NOT PIBOARD_FOUND)
  # Linux Generic
  message(STATUS "BOARD not defined !")
  if (CMAKE_HOST_UNIX)
    set (PIBOARD_ID BOARD_GENERIC_LINUX)
    set (PIBOARD_NAME "Linux")
  else (CMAKE_HOST_UNIX)
    message (FATAL_ERROR "This system is not supported !")
  endif (CMAKE_HOST_UNIX)
endif (NOT PIBOARD_FOUND)

add_definitions (-DPIBOARD_ID=${PIBOARD_ID} -DPIDUINO_DBPATH="${PIDUINO_INSTALL_DATA_DIR}/piduino.db")

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

if ("${PIBOARD_ID}" STREQUAL "BOARD_RASPBERRYPI")
  # Raspberry Pi ---------------------------------------------------------------
  set (PIDUINO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
  set (PIDUINO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
  set (PIDUINO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
  set (PIDUINO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (PIDUINO_SRC_ARCH_DIR ${PIDUINO_SRC_DIR}/arch/arm/broadcom)
  set (PIDUINO_SRC_BOARD_DIR ${PIDUINO_SRC_ARCH_DIR}/raspberrypi)

elseif (("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_NEO") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_AIR") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_NEO2") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_NEOPLUS2") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_M1") OR 
        ("${PIBOARD_ID}" STREQUAL "BOARD_NANOPI_M1PLUS"))
  # NanoPi Neo / Neo Air / M1  -------------------------------------------------
  set (PIDUINO_WITH_GPIO 1 CACHE BOOL "Enable GPIO")
  set (PIDUINO_WITH_I2C 1 CACHE BOOL "Enable I2C bus")
  set (PIDUINO_WITH_SPI 1 CACHE BOOL "Enable SPI bus")
  set (PIDUINO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (PIDUINO_SRC_ARCH_DIR ${PIDUINO_SRC_DIR}/arch/arm/allwinner)
  set (PIDUINO_SRC_BOARD_DIR ${PIDUINO_SRC_ARCH_DIR}/nanopi)

else ()
  # Linux Generic  -------------------------------------------------------------
  # BOARD_GENERIC_LINUX
  set (PIDUINO_WITH_SERIAL 1 CACHE BOOL "Enable Serial Port")
  set (PIDUINO_SRC_ARCH_DIR ${PIDUINO_SRC_DIR}/arch/generic)
  set (PIDUINO_WITH_GPIO 0)
  set (PIDUINO_WITH_I2C 0)
  set (PIDUINO_WITH_SPI 0)
endif ()

message (STATUS "Build for ${PIBOARD_NAME} target, ${PIBOARD_ID} defined...")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_subdirectory (dev)
add_subdirectory (lib)
add_subdirectory (utils)
add_subdirectory (doc)

if (PIDUINO_WITH_DOXYGEN_DOC)
  add_dependencies(piduino-shared doc)
endif()

### Packages generation --------------------------------------------------------
set(CPACK_PACKAGE_VERSION "${PIDUINO_VERSION_MAJOR}.${PIDUINO_VERSION_MINOR}-${PIDUINO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PIDUINO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PIDUINO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PIDUINO_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT "Pascal JEAN <epsilonrt@gmail.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING.LESSER")

set(CPACK_GENERATOR "DEB")


if (CPACK_GENERATOR STREQUAL "DEB")

  set(CPACK_DEB_PACKAGE_COMPONENT 1)
  set(CPACK_DEB_COMPONENT_INSTALL 1)
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/epsilonrt/piduino")

  find_program(dpkg_query NAMES dpkg-query)
  find_program(dpkg_cmd NAMES dpkg)

  if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    if (dpkg_cmd)
      execute_process(COMMAND ${dpkg_cmd} --print-architecture RESULT_VARIABLE ret OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
      if (ret EQUAL 0)
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${arch}")
      else (ret EQUAL 0)
        message(WARNING "Unable to get debian architecture !")
      endif (ret EQUAL 0)
    endif (dpkg_cmd)
  endif (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)


  if (dpkg_query)
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libc6 OUTPUT_VARIABLE libc6_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libc6_ver} "-" libc6_ver_dash)
    #message ("libc6_ver_dash=${libc6_ver_dash}")
    #message ("libc6_ver=${libc6_ver}")
    if (libc6_ver_dash)
      string(SUBSTRING ${libc6_ver} 0 ${libc6_ver_dash} libc6_ver)
    endif (libc6_ver_dash)
    
    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libgcc1 OUTPUT_VARIABLE libgcc1_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libgcc1_ver} "-" libgcc1_ver_dash)
    #message ("libgcc1_ver_dash=${libgcc1_ver_dash}")
    #message ("libgcc1_ver=${libgcc1_ver}")
    if (libgcc1_ver_dash)
      string(SUBSTRING ${libgcc1_ver} 0 ${libgcc1_ver_dash} libgcc1_ver)
    endif (libgcc1_ver_dash)

    execute_process(COMMAND ${dpkg_query} --showformat=\${Version} --show libsqlite3-0 OUTPUT_VARIABLE libsqlite3_ver OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(FIND ${libsqlite3_ver} "-" libsqlite3_ver_dash)
    #message ("libsqlite3_ver_dash=${libsqlite3_ver_dash}")
    #message ("libsqlite3_ver=${libsqlite3_ver}")
    if (libsqlite3_ver_dash)
      string(SUBSTRING ${libsqlite3_ver} 0 ${libsqlite3_ver_dash} libsqlite3_ver)
    endif (libsqlite3_ver_dash)

    #message ("libc6_ver=${libc6_ver}")
    #message ("libgcc1_ver=${libgcc1_ver}")
    #message ("libsqlite3_ver=${libsqlite3_ver}")
  endif (dpkg_query)
  
  message(STATUS "Debian architecture ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

  set(CPACK_DEBIAN_LIB_PACKAGE_NAME "libpiduino")
  set(CPACK_COMPONENT_LIB_DESCRIPTION "${PROJECT_DESCRIPTION} (library)")
  set(CPACK_DEBIAN_LIB_FILE_NAME "lib${PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>= ${libc6_ver}), libgcc1 (>= ${libgcc1_ver}), libsqlite3 (>= ${libsqlite3_ver})")
  set(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")

  set(CPACK_DEBIAN_DEV_PACKAGE_NAME "libpiduino-dev")
  set(CPACK_COMPONENT_DEV_DESCRIPTION "${PROJECT_DESCRIPTION} (development files)")
  set(CPACK_DEBIAN_DEV_FILE_NAME "lib${PROJECT_NAME}-dev_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION}), cmake, bash, git-core, pkg-config${libgps_dev_depends}")
  set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
  #set(CPACK_PACKAGE_DESCRIPTION_FILE "${PIDUINO_SRC_DIR}/doc/README-deb.md")

  set(CPACK_DEBIAN_DOC_PACKAGE_NAME "libpiduino-doc")
  set(CPACK_COMPONENT_DOC_DESCRIPTION "${PROJECT_DESCRIPTION} (documentation)")
  set(CPACK_DEBIAN_DOC_FILE_NAME "lib${PROJECT_NAME}-doc_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_DOC_PACKAGE_DEPENDS "${CPACK_DEBIAN_DEV_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_DOC_PACKAGE_SECTION "libdevel")
  
  set(CPACK_DEBIAN_UTILS_PACKAGE_NAME "piduino-utils")
  set(CPACK_COMPONENT_UTILS_DESCRIPTION "${PROJECT_DESCRIPTION} (utilities)")
  set(CPACK_DEBIAN_UTILS_FILE_NAME "${PROJECT_NAME}-utils_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.deb")
  set(CPACK_DEBIAN_UTILS_PACKAGE_DEPENDS "${CPACK_DEBIAN_LIB_PACKAGE_NAME} (= ${CPACK_PACKAGE_VERSION})")
  set(CPACK_DEBIAN_UTILS_PACKAGE_SECTION "utils")
  
endif (CPACK_GENERATOR STREQUAL "DEB")

include(CPack)
