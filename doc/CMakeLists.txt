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
# piduino/doc CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

set (PIDUINO_WITH_DOXYGEN_DOC 0 CACHE BOOL "Enable build of the documentation")

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

if("${CMAKE_PROJECT_NAME}" STREQUAL "Project")
  set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

if (PIDUINO_WITH_DOXYGEN_DOC)
  find_package(Doxygen REQUIRED)
  find_program(SED_EXECUTABLE sed)

  if(NOT SED_EXECUTABLE)
    message(FATAL_ERROR "sed is required but was not found. Please install sed.")
  endif()

  set(DOXYFILE_IN ${PROJECT_SOURCE_DIR}/Doxyfile)
  set(DOXYFILE_BUILD ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  # You must set the following variables to point to your project's files
  set(DOXY_README "${PROJECT_SOURCE_DIR}/README.md")
  set(DOXY_INPUT ${DOXY_README} ${PROJECT_SOURCE_DIR}/include/piduino)
  set(DOXY_IMAGE_PATH "${PROJECT_SOURCE_DIR}/doc/images")
  set(DOXY_PROJECT_LOGO "${PROJECT_SOURCE_DIR}/doc/images/PiduinoCommunityLogo.png")
  set(DOXY_EXAMPLE_PATH "${PROJECT_SOURCE_DIR}/examples")

  # replace placeholders in the Doxyfile for local generation
  add_custom_command(
    OUTPUT ${DOXYFILE_BUILD}
    COMMAND ${CMAKE_COMMAND} -E copy ${DOXYFILE_IN} ${DOXYFILE_BUILD}
    COMMAND ${SED_EXECUTABLE} -i 's|^HTML_OUTPUT[[:space:]]*=.*|HTML_OUTPUT = ${CMAKE_CURRENT_BINARY_DIR}/html|' ${DOXYFILE_BUILD}
    COMMAND ${SED_EXECUTABLE} -i 's|^INPUT[[:space:]]*=.*|INPUT = ${DOXY_INPUT}|' ${DOXYFILE_BUILD}
    COMMAND ${SED_EXECUTABLE} -i 's|^IMAGE_PATH[[:space:]]*=.*|IMAGE_PATH = ${DOXY_IMAGE_PATH}|' ${DOXYFILE_BUILD}
    COMMAND ${SED_EXECUTABLE} -i 's|^PROJECT_LOGO[[:space:]]*=.*|PROJECT_LOGO = ${DOXY_PROJECT_LOGO}|' ${DOXYFILE_BUILD}
    COMMAND ${SED_EXECUTABLE} -i 's|^EXAMPLE_PATH[[:space:]]*=.*|EXAMPLE_PATH = ${DOXY_EXAMPLE_PATH}|' ${DOXYFILE_BUILD}
    COMMAND ${SED_EXECUTABLE} -i 's|^USE_MDFILE_AS_MAINPAGE[[:space:]]*=.*|USE_MDFILE_AS_MAINPAGE = ${DOXY_README}|' ${DOXYFILE_BUILD}
    DEPENDS ${DOXYFILE_IN}
    COMMENT "Preparing Doxyfile for local documentation generation"
  )

  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_BUILD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${DOXYFILE_BUILD}
    COMMENT "Generating API HTML documentation with Doxygen"
    VERBATIM
  )
  
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
          DESTINATION ${PIDUINO_INSTALL_DOC_DIR}/api-manual COMPONENT doc)
endif(PIDUINO_WITH_DOXYGEN_DOC)
