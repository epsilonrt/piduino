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
cmake_minimum_required(VERSION 3.0)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

WriteGitVersionFile(${CMAKE_CURRENT_BINARY_DIR}/version.h)
  
file(GLOB src_piduino  ${PIDUINO_SRC_DIR}/*.cpp)
list(REMOVE_ITEM src_piduino ${PIDUINO_SRC_DIR}/piduino.cpp)

set (hdr_piduino 
  ${PIDUINO_INC_DIR}/piduino/board.h
  ${PIDUINO_INC_DIR}/piduino/clock.h
  ${PIDUINO_INC_DIR}/piduino/configfile.h
  ${PIDUINO_INC_DIR}/piduino/converter.h
  ${PIDUINO_INC_DIR}/piduino/database.h
  ${PIDUINO_INC_DIR}/piduino/fifo.h
  ${PIDUINO_INC_DIR}/piduino/filedevice.h
  ${PIDUINO_INC_DIR}/piduino/fileno.h
  ${PIDUINO_INC_DIR}/piduino/filestream.h
  ${PIDUINO_INC_DIR}/piduino/flags.h
  ${PIDUINO_INC_DIR}/piduino/global.h
  ${PIDUINO_INC_DIR}/piduino/gpio2.h
  ${PIDUINO_INC_DIR}/piduino/iodevice.h
  ${PIDUINO_INC_DIR}/piduino/iomap.h
  ${PIDUINO_INC_DIR}/piduino/linearbuffer.h
  ${PIDUINO_INC_DIR}/piduino/manufacturer.h
  ${PIDUINO_INC_DIR}/piduino/memory.h
  ${PIDUINO_INC_DIR}/piduino/popl.h
  ${PIDUINO_INC_DIR}/piduino/pwm.h
  ${PIDUINO_INC_DIR}/piduino/ringbuffer.h
  ${PIDUINO_INC_DIR}/piduino/scheduler.h
  ${PIDUINO_INC_DIR}/piduino/soc.h
  ${PIDUINO_INC_DIR}/piduino/socpwm.h
  ${PIDUINO_INC_DIR}/piduino/string.h
  ${PIDUINO_INC_DIR}/piduino/syslog.h
  ${PIDUINO_INC_DIR}/piduino/system.h
  ${PIDUINO_INC_DIR}/piduino/terminal.h
  ${PIDUINO_INC_DIR}/piduino/terminalnotifier.h
  ${PIDUINO_INC_DIR}/piduino/threadsafebuffer.h
)

set (hdr_gpio 
  ${PIDUINO_INC_DIR}/piduino/gpio.h
  ${PIDUINO_INC_DIR}/piduino/gpioconnector.h
  ${PIDUINO_INC_DIR}/piduino/gpiopin.h
  ${PIDUINO_INC_DIR}/piduino/gpiopwm.h
)

set (hdr_arduino 
  ${PIDUINO_INC_DIR}/piduino/arduino/Piduino.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Arduino.h
  ${PIDUINO_INC_DIR}/piduino/arduino/WString.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Print.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Printable.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Stream.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Terminal.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Console.h
  ${PIDUINO_INC_DIR}/piduino/arduino/WCharacter.h
  ${PIDUINO_INC_DIR}/piduino/arduino/binary.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Client.h
  ${PIDUINO_INC_DIR}/piduino/arduino/IPAddress.h
  ${PIDUINO_INC_DIR}/piduino/arduino/Server.h
  ${PIDUINO_INC_DIR}/piduino/arduino/pgmspace.h
)

set (hdr_arduino_i2c 
  ${PIDUINO_INC_DIR}/piduino/arduino/Wire.h
)

set (hdr_arduino_spi 
  ${PIDUINO_INC_DIR}/piduino/arduino/SPI.h
)

set (hdr_arduino_serial 
  ${PIDUINO_INC_DIR}/piduino/arduino/HardwareSerial.h
)

set (hdr_i2c
  ${PIDUINO_INC_DIR}/piduino/i2cdev.h
)

set (hdr_spi
  ${PIDUINO_INC_DIR}/piduino/spidev.h
)

set (hdr_serial
  ${PIDUINO_INC_DIR}/piduino/serialport.h
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
                ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

set(hdr_public ${hdr_piduino} ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if (PIDUINO_WITH_ARDUINO)
  file(GLOB src_arduino ${PIDUINO_SRC_DIR}/arduino/*.cpp ${PIDUINO_SRC_DIR}/arduino/compat/*.c)
  list(APPEND hdr_public ${hdr_arduino})
  file(GLOB src_arduino_serial ${PIDUINO_SRC_DIR}/arduino/serial/*.cpp)
  list(APPEND hdr_public ${hdr_arduino_serial})
  if (PIDUINO_WITH_I2C)
    file(GLOB src_arduino_i2c ${PIDUINO_SRC_DIR}/arduino/i2c/*.cpp)
    list(APPEND hdr_public ${hdr_arduino_i2c})
  endif (PIDUINO_WITH_I2C)
  if (PIDUINO_WITH_SPI)
    file(GLOB src_arduino_spi ${PIDUINO_SRC_DIR}/arduino/spi/*.cpp)
    list(APPEND hdr_public ${hdr_arduino_spi})
  endif (PIDUINO_WITH_SPI)
endif (PIDUINO_WITH_ARDUINO)

if (PIDUINO_WITH_GPIO)
  file(GLOB src_gpio ${PIDUINO_SRC_DIR}/gpio/*.cpp)
if (PIDUINO_DRIVER_BCM2835)
  file(GLOB_RECURSE src_gpio_bcm2835 ${PIDUINO_SRC_GPIO_ARCH_DIR}/broadcom/*.cpp)
endif (PIDUINO_DRIVER_BCM2835)
if (PIDUINO_DRIVER_ALLWINNERH)
  file(GLOB_RECURSE src_gpio_allwinnerh ${PIDUINO_SRC_GPIO_ARCH_DIR}/allwinner/*.cpp)
endif (PIDUINO_DRIVER_ALLWINNERH)
  list(APPEND hdr_public ${hdr_gpio})
endif (PIDUINO_WITH_GPIO)

if (PIDUINO_WITH_I2C)
  file(GLOB src_i2c ${PIDUINO_SRC_DIR}/i2c/*.cpp)
  list(APPEND hdr_public ${hdr_i2c})
endif (PIDUINO_WITH_I2C)

if (PIDUINO_WITH_SPI)
  file(GLOB src_spi ${PIDUINO_SRC_DIR}/spi/*.cpp)
  list(APPEND hdr_public ${hdr_spi})
endif (PIDUINO_WITH_SPI)

file(GLOB src_serial ${PIDUINO_SRC_DIR}/serial/*.cpp)
list(APPEND hdr_public ${hdr_serial})

#message ("hdr_public=${hdr_public}")

set (libsrc ${PIDUINO_SRC_DIR}/piduino.cpp  
  ${src_piduino} ${src_gpio} ${src_gpio_bcm2835} ${src_gpio_allwinnerh} 
  ${src_i2c} ${src_spi} ${src_serial}
  ${src_arduino} ${src_arduino_i2c} ${src_arduino_spi} ${src_arduino_serial}
  ${src_libc_compat}
  )

# this is the "object library" target: compiles the sources only once
add_library(objlib OBJECT ${libsrc})

target_include_directories(objlib BEFORE PRIVATE
  ${PIDUINO_INC_DIR} 
  ${PIDUINO_INC_DIR}/piduino/arduino 
  ${PIDUINO_SRC_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_compile_definitions(objlib PUBLIC LOG_ASSERT PIDUINO_WITH_CONFIG_H)

# shared libraries need PIC
set_property(TARGET objlib PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(piduino-shared SHARED $<TARGET_OBJECTS:objlib>)
set_target_properties(piduino-shared PROPERTIES 
  OUTPUT_NAME piduino 
  CLEAN_DIRECT_OUTPUT 1 
  VERSION ${PIDUINO_VERSION} 
  SOVERSION ${PIDUINO_VERSION_MAJOR}
  PUBLIC_HEADER "${hdr_public}"
  )

if(PIDUINO_WITH_STATIC)
  add_library(piduino-static STATIC $<TARGET_OBJECTS:objlib>)
  set_target_properties(piduino-static PROPERTIES
    OUTPUT_NAME piduino 
    CLEAN_DIRECT_OUTPUT 1 
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
set(CONF_INCLUDE_DIRS "${PIDUINO_INC_DIR}" "${PIDUINO_INC_DIR}/piduino/arduino" "${PIDUINO_SRC_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIB_DIRS "${PROJECT_BINARY_DIR}/lib/piduino")
configure_file(piduino-config.cmake.in
  "${PROJECT_BINARY_DIR}/piduino-config.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${PIDUINO_CMAKE_DIR}/${REL_INCLUDE_DIR}" "\${PIDUINO_CMAKE_DIR}/${REL_INCLUDE_DIR}/piduino/arduino")
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

# Install configuration file
configure_file("${PIDUINO_ETC_DIR}/piduino.conf.in" 
  "${PROJECT_BINARY_DIR}/piduino.conf" @ONLY)
install(FILES
  "${PROJECT_BINARY_DIR}/piduino.conf"
  DESTINATION "${PIDUINO_INSTALL_ETC_DIR}" COMPONENT lib)

# Install the piduino boards database
install(FILES
  "${PIDUINO_DATABASE_DIR}/piduino.db"
  DESTINATION "${PIDUINO_INSTALL_DATA_DIR}" COMPONENT lib)

# Install the piduino licence files
install(FILES
  "${PROJECT_SOURCE_DIR}/COPYING" "${PROJECT_SOURCE_DIR}/COPYING.LESSER"
  DESTINATION "${PIDUINO_INSTALL_DATA_DIR}" COMPONENT lib)

# Install the 3rdparty's project licence files
install(DIRECTORY
  "${PROJECT_SOURCE_DIR}/3rdparty"
  DESTINATION "${PIDUINO_INSTALL_DATA_DIR}" COMPONENT lib)

install(FILES
  "${PROJECT_SOURCE_DIR}/README.md"
  DESTINATION "${PIDUINO_INSTALL_DOC_DIR}" COMPONENT lib)

install (TARGETS piduino-shared
  # IMPORTANT: Add the piduino library to the "export-set"
  EXPORT piduino
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/piduino" COMPONENT dev
  )

install(DIRECTORY
  "${PIDUINO_INC_DIR}/piduino/arduino" DESTINATION "${INSTALL_INCLUDE_DIR}/piduino" COMPONENT dev)

if(PIDUINO_WITH_STATIC)
  install (TARGETS piduino-static 
    EXPORT piduino
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev
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
