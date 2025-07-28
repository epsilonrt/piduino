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
# piduino/dev CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

if("${CMAKE_PROJECT_NAME}" STREQUAL "Project")
  set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()


file(GLOB dev_cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake)

install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/git-version 
  DESTINATION "${INSTALL_BIN_DIR}" 
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ 
    WORLD_EXECUTE WORLD_READ
  COMPONENT dev
  )

install (FILES ${dev_cmake} 
  DESTINATION "${PIDUINO_INSTALL_CMAKE_DIR}" 
  PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  COMPONENT dev
  )

install (DIRECTORY codelite/executable-piduino 
  DESTINATION "${INSTALL_CODELITE_DIR}/templates/projects" 
  DIRECTORY_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
  COMPONENT dev
  FILES_MATCHING 
  PATTERN "*" 
  PATTERN "Debug" EXCLUDE
  PATTERN "Release" EXCLUDE
  PATTERN "*.mk" EXCLUDE
  PATTERN "*.txt" EXCLUDE
  PATTERN ".clang" EXCLUDE
  PATTERN ".codelite" EXCLUDE)

# Examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/
  DESTINATION ${PIDUINO_INSTALL_EXAMPLES_DIR}
  COMPONENT dev
  FILES_MATCHING 
  PATTERN "*.c" 
  PATTERN "*.cpp" 
  PATTERN "*.h" 
  PATTERN "*.project"  
  PATTERN "*.workspace"
  PATTERN "Debug" EXCLUDE
  PATTERN "Release" EXCLUDE
  PATTERN "*.mk" EXCLUDE
  PATTERN "*.txt" EXCLUDE
  PATTERN ".clang" EXCLUDE
  PATTERN ".codelite" EXCLUDE
  PATTERN "cmake-build*" EXCLUDE)

