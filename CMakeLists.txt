cmake_minimum_required(VERSION 3.1)

project(drivex VERSION 1.0 LANGUAGES CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTING "Build unit tests" ON)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_library(libdrivex STATIC
        drivex/directory_entry.cpp
        drivex/directory_entry.h
        drivex/filesystem.cpp
        drivex/filesystem.h
        drivex/permissions.cpp
        drivex/permissions.h
        drivex/fuse.cpp
        drivex/fuse.h
        drivex/file_type.cpp
        drivex/file_type.h
        drivex/error.cpp
        drivex/error.h
        drivex/file_status.cpp
        drivex/file_status.h
        drivex/error_code.cpp
        drivex/error_code.h)

target_include_directories(libdrivex PUBLIC
        $<BUILD_INTERFACE:${drivex_SOURCE_DIR}>
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>)

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
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)
endif ()

add_executable(hello drivex/test/Hello.cpp)
set_target_properties(hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(hello libdrivex)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${drivex_BINARY_DIR}/drivexConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(TARGETS libdrivex
        EXPORT drivexTargets
        INCLUDES DESTINATION include
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY drivex DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        PATTERN "test/*" EXCLUDE
        PATTERN "test" EXCLUDE
        PATTERN "examples/*" EXCLUDE
        PATTERN "examples" EXCLUDE)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${drivex_SOURCE_DIR}/cmake/drivexConfig.cmake"
        "${drivex_BINARY_DIR}/drivexConfig.cmake"
        INSTALL_DESTINATION share/cmake/drivex
)

install(EXPORT drivexTargets DESTINATION share/cmake/drivex)
install(FILES "${drivex_BINARY_DIR}/drivexConfigVersion.cmake"
        "${drivex_BINARY_DIR}/drivexConfig.cmake"
        DESTINATION share/cmake/drivex)

if (BUILD_TESTING)
    find_package(GTest MODULE REQUIRED)
    add_executable(fuse_test drivex/test/fuse_test.cpp)
    target_link_libraries(fuse_test PRIVATE libdrivex GTest::GTest GTest::Main)
    gtest_discover_tests(fuse_test)
    if (MSVC)
        target_compile_options(fuse_test PRIVATE /W4 /WX /MP)
    else ()
        target_compile_options(fuse_test PRIVATE -Wall -Wextra -pedantic -Werror)
    endif ()
endif ()
