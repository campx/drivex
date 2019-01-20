cmake_minimum_required(VERSION 3.1)

include("3rdparty/gate/cmake/HunterGate.cmake")
HunterGate(
        URL "https://github.com/ruslo/hunter/archive/v0.23.68.tar.gz"
        SHA1 "d7365dfaf3db5c0939f3c62141a2ec1eac660962"
)

project(drivex)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(Boost_USE_STATIC_LIBS ON)
hunter_add_package(Boost COMPONENTS system filesystem)
find_package(Boost CONFIG REQUIRED system filesystem)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

add_library(libdrivex STATIC
        drivex/DirectoryEntry.cpp drivex/DirectoryEntry.h
        drivex/FileSystem.cpp drivex/FileSystem.h
        drivex/Permissions.cpp drivex/Permissions.h
        drivex/Fuse.cpp drivex/Fuse.h 
		drivex/FileType.cpp drivex/FileType.h 
		drivex/Error.cpp drivex/Error.h 
		drivex/FileStatus.cpp drivex/FileStatus.h 
		drivex/ErrorCode.cpp drivex/ErrorCode.h)
set_target_properties(libdrivex PROPERTIES
        OUTPUT_NAME drivex
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


target_compile_features(libdrivex PUBLIC cxx_return_type_deduction)
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_compile_options(libdrivex PRIVATE -Wall -Werror -Wextra
            -D_FILE_OFFSET_BITS=64 -Bstatic)
endif ()

if (WIN32)
    add_subdirectory(external/dokany)
    add_dependencies(libdrivex dokany)
endif ()

add_executable(hello drivex/examples/Hello.cpp)
set_target_properties(hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
    target_link_libraries(libdrivex ${Boost_LIBRARIES})
    target_link_libraries(hello libdrivex ${Boost_FILESYSTEM_LIBRARY} dokanfuse1)
else ()
    target_link_libraries(libdrivex PUBLIC pthread ${Boost_LIBRARIES} fuse)
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
add_subdirectory(drivex/test)
