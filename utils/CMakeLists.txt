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
# piduino/utils CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

# set packaging dir
if(NOT CPACK_PACKAGE_DIRECTORY)
  set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)
endif()

add_subdirectory (pido)
add_subdirectory (pinfo)
