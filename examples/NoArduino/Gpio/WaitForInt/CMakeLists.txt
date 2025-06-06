cmake_minimum_required(VERSION 3.0)

# 
project(GpioWaitForInt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(PkgConfig REQUIRED) 
pkg_check_modules(PIDUINO REQUIRED IMPORTED_TARGET piduino)

file(GLOB SOURCES *.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC PkgConfig::PIDUINO)

set(PROGRAM_PERMISSIONS_BY_ROOT
    OWNER_WRITE OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE SETUID)
install(TARGETS ${PROJECT_NAME} DESTINATION bin PERMISSIONS ${PROGRAM_PERMISSIONS_BY_ROOT})
