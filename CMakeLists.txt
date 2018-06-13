cmake_minimum_required(VERSION 2.8)

project(filex LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(Boost_USE_STATIC_LIBS ON) 
find_package(Boost REQUIRED filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${Boost_LIBRARY_DIRS})
link_directories(${CMAKE_BINARY_DIR}/lib)
add_subdirectory(external/gtest)

add_library(libfilex STATIC
    filex/DirectoryEntry.cpp  filex/DirectoryEntry.h
    filex/FileSystem.cpp      filex/FileSystem.h
    filex/Fuse.cpp            filex/Fuse.h)
set_target_properties(libfilex PROPERTIES
        OUTPUT_NAME filex
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


target_compile_features(libfilex PUBLIC cxx_return_type_deduction)
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_compile_options(libfilex PUBLIC -Wall -Werror -Wextra
            -D_FILE_OFFSET_BITS=64 -Bstatic -static-libgcc -static-libstdc++)
endif()

if (WIN32)
    add_subdirectory(external/dokany)
    add_dependencies(libfilex dokany)
endif ()

add_executable(hello filex/examples/Hello.cpp)
set_target_properties(hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
    target_link_libraries(libfilex ${Boost_LIBRARIES} )
    target_link_libraries(hello libfilex ${Boost_FILESYSTEM_LIBRARY} dokanfuse1)
else ()
    target_link_libraries(libfilex pthread ${Boost_LIBRARIES} fuse)
    target_link_libraries(hello libfilex)
endif ()

target_include_directories(libfilex PUBLIC "${filex_SOURCE_DIR}")

install(TARGETS libfilex 
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY filex DESTINATION include 
        FILES_MATCHING PATTERN "*.h" 
        PATTERN "test/*" EXCLUDE
        PATTERN "test" EXCLUDE
        PATTERN "examples/*" EXCLUDE
        PATTERN "examples" EXCLUDE)

enable_testing()

add_executable(TestFuse filex/test/TestFuse.cpp)
add_dependencies(TestFuse googletest)

if ((${CMAKE_BUILD_TYPE} STREQUAL Debug) 
    AND (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC))
	target_link_libraries(TestFuse gtestd libfilex)
else()
	target_link_libraries(TestFuse gtest libfilex)
endif()

set_target_properties(TestFuse PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_CURRENT_BINARY_DIR}/test)
add_test(NAME TestFuse
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test/TestFuse)
