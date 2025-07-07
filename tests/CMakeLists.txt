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

# Enable AddressSanitizer
set(ASAN_FLAGS)
list(APPEND ASAN_FLAGS -fsanitize=address)
list(APPEND ASAN_FLAGS -fno-omit-frame-pointer)
list(APPEND ASAN_FLAGS -g)  # Optionnel pour de meilleurs backtraces

set(ASAN_LINK_FLAGS -fsanitize=address)

function(add_test_incl_memcheck test)
  message(STATUS " add test ${test}")
  # create an executable, which instantiates a runner from UnitTest++
  add_executable(${test} ${test}/main.cpp)
  add_dependencies(${test} piduino-shared)

  # depending on the framework, you need to link to it
  target_link_libraries(${test} ${LINK_OPTIONS} ${UNITTESTPP_LIBRARIES})
  target_compile_options(${test} PUBLIC ${COMPILE_DEFINITIONS} ${UNITTESTPP_CFLAGS} ${ASAN_FLAGS})
  target_link_options(${test} PUBLIC ${ASAN_LINK_FLAGS})

  # now register the executable with CTest
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test}/run_as_root")

    add_test(NAME ${test} COMMAND ${test})
    # AddressSanitizer is enabled by compiler flags, so just run the test
    add_test(NAME ${test}_asan
      COMMAND ${test}
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    set_tests_properties(${test}_asan PROPERTIES
      LABELS "asan"
      TIMEOUT 60
    )
  else()

    message(STATUS " test ${test} requires root privileges, it will run with sudo")
    add_test(NAME ${test} COMMAND ${SUDO_EXECUTABLE} $<TARGET_FILE:${test}>
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    add_test(NAME ${test}_asan
      COMMAND ${SUDO_EXECUTABLE} $<TARGET_FILE:${test}>
      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    set_tests_properties(${test}_asan PROPERTIES
      LABELS "asan"
      TIMEOUT 60
    )
  endif()
endfunction()

add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND} -V)

foreach(test ${TESTS})
  add_test_incl_memcheck(${test})
endforeach()

add_dependencies(build_and_test piduino-shared)

