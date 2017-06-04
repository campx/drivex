cmake_minimum_required(VERSION 2.8)

project(cpp-fuse LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #create compilation database for clang

if (APPLE)
	# find fuse
else()
	find_package(PkgConfig REQUIRED)
	pkg_search_module(FUSE REQUIRED fuse)
	if( FUSE_FOUND )
		include_directories(${FUSE_INCLUDE_DIRS})
		message(STATUS "Using FUSE ${FUSE_VERSION}")
	endif()
endif()

include_directories(${cpp-fuse_SOURCE_DIR}/include)
add_library(libcpp-fuse src/Fuse.cpp)
set_target_properties(libcpp-fuse PROPERTIES OUTPUT_NAME cpp-fuse)
target_link_libraries(libcpp-fuse pthread ${FUSE_LIBRARIES})

add_executable(hello examples/Hello.cpp)
target_link_libraries(hello libcpp-fuse ${FUSE_LIBRARIES} stdc++fs)

enable_testing()
add_executable(TestFuse test/TestFuse.cpp)
