cmake_minimum_required(VERSION 2.8)

project(drivex LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_USE_STATIC_LIBS ON) 
find_package(Boost REQUIRED filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${CMAKE_BINARY_DIR}/lib)

if (NOT TARGET gtest)
    add_subdirectory(external/googletest)
endif()

add_library(libdrivex STATIC
        drivex/DirectoryEntry.cpp drivex/DirectoryEntry.h
        drivex/FileSystem.cpp drivex/FileSystem.h
        drivex/Fuse.cpp drivex/Fuse.h)
set_target_properties(libdrivex PROPERTIES
        OUTPUT_NAME drivex
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


target_compile_features(libdrivex PUBLIC cxx_return_type_deduction)
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_compile_options(libdrivex PUBLIC -Wall -Werror -Wextra
            -D_FILE_OFFSET_BITS=64 -Bstatic -static-libgcc -static-libstdc++)
endif()

if (WIN32)
    add_subdirectory(external/dokany)
    add_dependencies(libdrivex dokany)
endif ()

add_executable(hello drivex/examples/Hello.cpp)
set_target_properties(hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
    target_link_libraries(libdrivex ${Boost_LIBRARIES} )
    target_link_libraries(hello libdrivex ${Boost_FILESYSTEM_LIBRARY} dokanfuse1)
else ()
    target_link_libraries(libdrivex pthread ${Boost_LIBRARIES} fuse)
    target_link_libraries(hello libdrivex)
endif ()

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

enable_testing()

add_executable(TestFuse drivex/test/TestFuse.cpp)
add_dependencies(TestFuse googletest)

target_link_libraries(TestFuse gtest libdrivex)

set_target_properties(TestFuse PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestFuse
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestFuse)
