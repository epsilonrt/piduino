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

# libpiduino CMakeLists.txt
cmake_minimum_required(VERSION 2.8.11)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB src_piduino ${PIDUINO_SRC_DIR}/*.c ${PIDUINO_SRC_DIR}/*.cpp)
file(GLOB src_arch  ${PIDUINO_SRC_ARCH_DIR}/*.c ${PIDUINO_SRC_ARCH_DIR}/*.cpp)
file(GLOB src_board  ${PIDUINO_SRC_BOARD_DIR}/*.c ${PIDUINO_SRC_BOARD_DIR}/*.cpp)

set (hdr_piduino 
  ${PIDUINO_INC_DIR}/piduino/clock.h
  ${PIDUINO_INC_DIR}/piduino/scheduler.h
  ${PIDUINO_INC_DIR}/piduino/iomap.h
  ${PIDUINO_INC_DIR}/piduino/board.h
  ${PIDUINO_INC_DIR}/piduino/soc.h
  ${PIDUINO_INC_DIR}/piduino/manufacturer.h
  ${PIDUINO_INC_DIR}/piduino/database.h
  ${PIDUINO_INC_DIR}/piduino/configfile.h
  ${PIDUINO_INC_DIR}/piduino/system.h
)

set (hdr_gpio 
  ${PIDUINO_INC_DIR}/piduino/gpio.h
  ${PIDUINO_INC_DIR}/piduino/gpiopin.h
  ${PIDUINO_INC_DIR}/piduino/gpioconnector.h
  ${PIDUINO_INC_DIR}/piduino/arduino.h
)

set (hdr_gpio_root 
  ${PIDUINO_INC_DIR}/Arduino.h
)

set (hdr_i2c
#  ${PIDUINO_INC_DIR}/piduino/i2c.h
)

set (hdr_spi
#  ${PIDUINO_INC_DIR}/piduino/spi.h
)

set (hdr_serial
#  ${PIDUINO_INC_DIR}/piduino/serial.h
)


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_piduino} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if (PIDUINO_WITH_GPIO)
  file(GLOB src_gpio ${PIDUINO_SRC_DIR}/gpio/*.c ${PIDUINO_SRC_DIR}/gpio/*.cpp)
  list(APPEND hdr_public ${hdr_gpio})
  list(APPEND hdr_root ${hdr_gpio_root})
endif (PIDUINO_WITH_GPIO)

if (PIDUINO_WITH_I2C)
  file(GLOB src_i2c ${PIDUINO_SRC_DIR}/i2c/*.c ${PIDUINO_SRC_DIR}/i2c/*.cpp)
  list(APPEND hdr_public ${hdr_i2c})
endif (PIDUINO_WITH_I2C)

if (PIDUINO_WITH_SPI)
  file(GLOB src_spi ${PIDUINO_SRC_DIR}/spi/*.c ${PIDUINO_SRC_DIR}/spi/*.cpp)
  list(APPEND hdr_public ${hdr_spi})
endif (PIDUINO_WITH_SPI)

if (PIDUINO_WITH_SERIAL)
  file(GLOB src_serial ${PIDUINO_SRC_DIR}/serial/*.c ${PIDUINO_SRC_DIR}/serial/*.cpp)
  list(APPEND hdr_public ${hdr_serial})
endif (PIDUINO_WITH_SERIAL)

#message ("hdr_public=${hdr_public}")

set (libsrc ${src_piduino} ${src_arch}  ${src_board} 
  ${src_gpio} ${src_i2c} ${src_spi} ${src_serial}
  )

include_directories(BEFORE
  ${PIDUINO_INC_DIR} 
  ${PIDUINO_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

add_library(piduino-shared SHARED ${libsrc})
target_compile_definitions(piduino-shared PUBLIC LOG_ASSERT PIDUINO_WITH_CONFIG_H)
set_target_properties(piduino-shared PROPERTIES 
  OUTPUT_NAME piduino 
  CLEAN_DIRECT_OUTPUT 1 
  C_STANDARD 99 
  VERSION ${PIDUINO_VERSION} 
  SOVERSION ${PIDUINO_VERSION_SHORT}
  PUBLIC_HEADER "${hdr_public}"
  )

if(PIDUINO_WITH_STATIC)
  add_library(piduino-static STATIC ${libsrc})
  target_compile_definitions(piduino-static PUBLIC LOG_ASSERT PIDUINO_WITH_CONFIG_H)
  set_target_properties(piduino-static PROPERTIES
    OUTPUT_NAME piduino 
    CLEAN_DIRECT_OUTPUT 1 
    C_STANDARD 99 
    VERSION ${PIDUINO_VERSION} 
  )
endif(PIDUINO_WITH_STATIC)

# CMake Package ----------------------------------------------------------------
# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
if (PIDUINO_WITH_STATIC)
  set (exported_targets piduino-shared piduino-static)
else(PIDUINO_WITH_STATIC)
  set (exported_targets piduino-shared)
endif(PIDUINO_WITH_STATIC)

export(TARGETS ${exported_targets}
  FILE "${PROJECT_BINARY_DIR}/piduino.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Piduino)

# Create the piduino-config.cmake and piduino-config-version files
file(RELATIVE_PATH REL_LIB_DIR "${PIDUINO_INSTALL_CMAKE_DIR}" 
  "${INSTALL_LIB_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${PIDUINO_INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PIDUINO_INC_DIR}" "${PIDUINO_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/piduino")
configure_file(piduino-config.cmake.in
  "${PROJECT_BINARY_DIR}/piduino-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${PIDUINO_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIB_DIRS "\${PIDUINO_CMAKE_DIR}/${REL_LIB_DIR}")
configure_file(piduino-config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/piduino-config.cmake" @ONLY)

# ... for both
configure_file(piduino-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/piduino-config-version.cmake" @ONLY)

# Install the piduino-config.cmake and piduino-config-version.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/piduino-config.cmake"
  "${PROJECT_BINARY_DIR}/piduino-config-version.cmake"
  DESTINATION "${PIDUINO_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT piduino DESTINATION
  "${PIDUINO_INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the piduino boards database
install(FILES
  "${PIDUINO_DATABASE_DIR}/piduino.db"
  DESTINATION "${PIDUINO_INSTALL_DATA_DIR}" COMPONENT lib)

install (TARGETS piduino-shared
  # IMPORTANT: Add the piduino library to the "export-set"
  EXPORT piduino
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/piduino" COMPONENT dev
  )

install(FILES
  "${hdr_root}" DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT dev)

if(PIDUINO_WITH_STATIC)
  install (TARGETS piduino-static 
    EXPORT piduino
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  )
endif(PIDUINO_WITH_STATIC)


## pkg-config ------------------------------------------------------------------
foreach(item ${PIDUINO_CFLAGS_OTHER})
  string(APPEND PIDUINO_CFLAGS_PKGCONFIG "${item} ")
endforeach(item PIDUINO_CFLAGS_OTHER)

foreach(item ${PIDUINO_LDFLAGS_OTHER})
  string(APPEND PIDUINO_LDFLAGS_PKGCONFIG "${item} ")
endforeach(item PIDUINO_LDFLAGS_OTHER)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/piduino.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/piduino.pc @ONLY)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/piduino.pc 
  DESTINATION "${INSTALL_LIB_DIR}/pkgconfig"
  COMPONENT dev)
