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
# utils/gpio CMakeLists.txt

cmake_minimum_required(VERSION 2.8.11)

add_custom_target(setuid
  COMMAND piduino-setuid  ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  COMMENT "Setting set-uid root bit for executable")

set(PROGRAM_PERMISSIONS_DEFAULT
    OWNER_WRITE OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE)

if (PIDUINO_WITH_GPIO)
  add_subdirectory (gpio)
endif()
