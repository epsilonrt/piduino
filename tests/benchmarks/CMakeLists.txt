# Copyright © 2018-2025 Pascal JEAN, All rights reserved.
# This file is part of the piduino Project.
#
# The piduino Project is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The piduino Project is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the piduino Project.  If not, see <http://www.gnu.org/licenses/>.

# piduino tests/benchmarks CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

include(SubDirList)
SUBDIRLIST(BENCHMARKS ${CMAKE_CURRENT_SOURCE_DIR})

find_program(SUDO_EXECUTABLE NAMES sudo)

if(NOT SUDO_EXECUTABLE)
  message(FATAL_ERROR "sudo executable not found, please install sudo")
endif()

if(PIDUINO_LIB_DIR)
  link_directories(${PIDUINO_LIB_DIR})
  add_definitions(${PIDUINO_CFLAGS_OTHER})
  include_directories(BEFORE ${PIDUINO_INC_DIR} ${PIDUINO_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR})
  list(APPEND LINK_OPTIONS piduino Threads::Threads ${PIDUINO_LDFLAGS_OTHER})
else()
  find_package(piduino CONFIG REQUIRED)
  link_directories(${PIDUINO_LIBRARY_DIRS})
  add_definitions(${PIDUINO_CFLAGS})
  include_directories(BEFORE ${PIDUINO_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR})
  list(APPEND LINK_OPTIONS ${PIDUINO_LIBRARIES})
endif()


function(add_benchmark benchmark)
  message(STATUS "Add benchmark ${benchmark}")
  # create an executable, which instantiates a runner from UnitTest++
  add_executable(${benchmark} ${benchmark}/main.cpp)
  add_dependencies(${benchmark} piduino-shared)

  # depending on the framework, you need to link to it
  target_link_libraries(${benchmark} ${LINK_OPTIONS})
  target_compile_options(${benchmark} PUBLIC ${COMPILE_DEFINITIONS})

  # now register the executable with CTest
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${benchmark}/run_as_root")

    add_test(NAME ${benchmark} COMMAND ${benchmark})
  else()

    message(STATUS "  benchmark ${benchmark} requires root privileges, it will run with sudo")
    add_test(NAME ${benchmark} COMMAND ${SUDO_EXECUTABLE} $<TARGET_FILE:${benchmark}>
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endif()
endfunction()

add_custom_target(build_and_benchmarks ${CMAKE_CTEST_COMMAND} -V)

foreach(benchmark ${BENCHMARKS})
  add_benchmark(${benchmark})
endforeach()

add_dependencies(build_and_benchmarks piduino-shared)

