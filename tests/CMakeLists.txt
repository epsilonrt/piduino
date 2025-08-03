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

# piduino tests CMakeLists.txt
cmake_minimum_required(VERSION 3.0)

include(SubDirList)
SUBDIRLIST(TESTS ${CMAKE_CURRENT_SOURCE_DIR})

set(CTEST_OUTPUT_ON_FAILURE OFF)
find_program(SUDO_EXECUTABLE NAMES sudo)
find_program(VALGRIND_EXECUTABLE NAMES valgrind)

if(NOT SUDO_EXECUTABLE)
  message(FATAL_ERROR "sudo executable not found, please install sudo")
endif()

# valgrind memcheck is buggy on armv7l and armhf architectures
# unable to run tests with valgrind on these architectures
# MESSAGE("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
if(VALGRIND_EXECUTABLE AND CMAKE_SYSTEM_PROCESSOR AND NOT CMAKE_HOST_SYSTEM_PROCESSOR  MATCHES "armv7l|armhf")
  message(STATUS "valgrind executable found: ${VALGRIND_EXECUTABLE}")
  set (MEMORY_TESTS ON)
else()
  message(STATUS "valgrind executable not found or not supported on this architecture, memory tests will not be run")
  set (MEMORY_TESTS OFF)
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

# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()

function(add_test_incl_memcheck test)
  message(STATUS "Add test ${test}")
  # create an executable, which instantiates a runner from UnitTest++
  add_executable(${test} ${test}/main.cpp)
  add_dependencies(${test} piduino-shared)

  # depending on the framework, you need to link to it
  target_link_libraries(${test} ${LINK_OPTIONS} ${UNITTESTPP_LIBRARIES})
  target_compile_options(${test} PUBLIC ${COMPILE_DEFINITIONS} ${UNITTESTPP_CFLAGS})

  if(test STREQUAL "test5-gpiopwm")
    set(MEMORY_TESTS OFF)
  endif()

  # now register the executable with CTest
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test}/run_as_root")

    add_test(NAME ${test} COMMAND ${test})
    if(MEMORY_TESTS)
      message(STATUS "  test ${test} will be run with valgrind")
      # add a memory check test using valgrind
      # this is useful to detect memory leaks and other issues
      # see https://valgrind.org/docs/manual/manual.html
      # --error-exitcode=1: exit with error code 1 if there are errors
      # --tool=memcheck: use the memcheck tool
      # --leak-check=full: perform a full leak check
      # --errors-for-leak-kinds=definite: report definite leaks
      # --show-leak-kinds=definite: show only definite leaks
      # $<TARGET_FILE:${test}>: the executable to run
      # WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}: set the working directory for the test
      # this is useful if the test needs to access files in the current directory
      # or if the test is run from a different directory
      # --track-origins=yes: track the origins of uninitialized values
      # --suppressions=${CMAKE_CURRENT_SOURCE_DIR}/valgrind.supp: suppress known issues
      # --gen-suppressions=all: generate suppressions for all issues
      # --error-exitcode=1: exit with error code 1 if there are errors
      # --log-file=valgrind.log: write the log to a file
      # --trace-children=yes: trace child processes
      # --track-fds=yes: track file descriptors
      # --track-fds=yes: track file descriptors
      # --show-leak-kinds=definite: show only definite leaks
      # --track-origins=yes: track the origins of uninitialized values
      add_test(NAME ${test}_memchecked
        COMMAND timeout 30s ${VALGRIND_EXECUTABLE}  # Timeout after 30 seconds
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        --max-stackframe=2000000  # stack size limit
        --main-stacksize=16000000 # main stack size limit
        $<TARGET_FILE:${test}>
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
      set_tests_properties(${test}_memchecked PROPERTIES
        LABELS "memcheck"
        TIMEOUT 30  # Timeout CMake également
      )
    endif()
  else()

    message(STATUS "  test ${test} requires root privileges, it will run with sudo")
    add_test(NAME ${test} COMMAND ${SUDO_EXECUTABLE} $<TARGET_FILE:${test}>
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    if(MEMORY_TESTS)
      message(STATUS "  test ${test} will be run with valgrind")
      add_test(NAME ${test}_memchecked
        COMMAND timeout 30s ${SUDO_EXECUTABLE} ${VALGRIND_EXECUTABLE}  # Timeout after 30 seconds
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        --max-stackframe=2000000  # stack size limit
        --main-stacksize=16000000 # main stack size limit
        $<TARGET_FILE:${test}>
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
      set_tests_properties(${test}_memchecked PROPERTIES
        LABELS "memcheck"
        TIMEOUT 30
      )
    endif()
  endif()
endfunction()

add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND} -V)

list(REMOVE_ITEM TESTS "benchmarks")

foreach(test ${TESTS})
  add_test_incl_memcheck(${test})
endforeach()

add_dependencies(build_and_test piduino-shared)
add_subdirectory(benchmarks)

