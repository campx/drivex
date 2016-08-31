cmake_minimum_required(VERSION 2.8)

project(truss-fuse LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

add_executable(truss-fuse src/main.cpp)

find_package(PkgConfig REQUIRED)
pkg_search_module(FUSE REQUIRED fuse)
if( FUSE_FOUND )
    include_directories(${FUSE_INCLUDE_DIRS})
    target_link_libraries(truss-fuse ${FUSE_LIBRARIES})
    message(STATUS "Using FUSE ${FUSE_VERSION}")
endif()

target_link_libraries(truss-fuse pthread)
