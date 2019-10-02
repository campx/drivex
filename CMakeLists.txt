cmake_minimum_required(VERSION 3.1)

project(drivex)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTING "Build unit tests" ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_library(libdrivex STATIC
        drivex/DirectoryEntry.cpp
        drivex/DirectoryEntry.h
        drivex/FileSystem.cpp
        drivex/FileSystem.h
        drivex/Permissions.cpp
        drivex/Permissions.h
        drivex/Fuse.cpp
        drivex/Fuse.h
        drivex/FileType.cpp
        drivex/FileType.h
        drivex/Error.cpp
        drivex/Error.h
        drivex/FileStatus.cpp
        drivex/FileStatus.h
        drivex/ErrorCode.cpp
        drivex/ErrorCode.h)

set_target_properties(libdrivex PROPERTIES
        OUTPUT_NAME drivex
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(libdrivex PUBLIC ${Boost_FILESYSTEM_LIBRARY})

if (NOT MSVC)
    target_compile_options(libdrivex PRIVATE -Wall -Werror -Wextra)
    target_compile_options(libdrivex PUBLIC -D_FILE_OFFSET_BITS=64 -Bstatic)
    target_link_libraries(libdrivex PUBLIC fuse pthread)
endif ()

if (WIN32)
    add_subdirectory(thirdparty/dokany)
    add_dependencies(libdrivex dokany)
    target_link_libraries(libdrivex PUBLIC dokanfuse1)
    target_include_directories(libdrivex PUBLIC
            ${CMAKE_CURRENT_BINARY_DIR}/include)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)
endif ()

add_executable(hello drivex/test/Hello.cpp)
set_target_properties(hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(hello libdrivex)
target_include_directories(libdrivex PUBLIC "${drivex_SOURCE_DIR}")

install(TARGETS libdrivex
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY drivex DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "test/*" EXCLUDE
        PATTERN "test" EXCLUDE
        PATTERN "examples/*" EXCLUDE
        PATTERN "examples" EXCLUDE)

if (BUILD_TESTING)
    find_package(GTest MODULE REQUIRED)
    add_executable(TestFuse drivex/test/TestFuse.cpp)
    target_link_libraries(TestFuse PRIVATE libdrivex GTest::GTest GTest::Main)
    gtest_discover_tests(TestFuse)
    if (MSVC)
        target_compile_options(TestFuse PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(TestFuse PRIVATE -Wall -Wextra -pedantic -Werror)
    endif ()
endif ()
