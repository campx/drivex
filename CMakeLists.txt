cmake_minimum_required(VERSION 2.8)

project(fusex LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Werror")
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

include_directories(${fusex_SOURCE_DIR}/include)
add_library(libfusex src/FileSystem.cpp src/Fuse.cpp)
set_target_properties(libfusex PROPERTIES OUTPUT_NAME fusex)
target_link_libraries(libfusex pthread ${FUSE_LIBRARIES})

add_executable(hello examples/Hello.cpp)
target_link_libraries(hello libfusex ${FUSE_LIBRARIES} stdc++fs)

target_include_directories(libfusex INTERFACE
                           "${fusex_SOURCE_DIR}/include")

enable_testing()
add_executable(TestFuse test/TestFuse.cpp)
