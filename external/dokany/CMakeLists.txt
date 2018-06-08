cmake_minimum_required(VERSION 2.8)
project(dokany NONE)
include(ExternalProject)

set(PLATFORM Win32)

ExternalProject_Add(
        dokany
        URL https://github.com/dokan-dev/dokany/releases/download/v1.0.5/dokan-1.0.5.1000.zip
        INSTALL_DIR ${CMAKE_BINARY_DIR}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        BUILD_IN_SOURCE 1
        BUILD_COMMAND ""
        INSTALL_COMMAND xcopy /y /i Win32\\Msys2\\lib\\* ..\\..\\..\\..\\lib &&
        xcopy /y /i Win32\\Msys2\\include\\* ..\\..\\..\\..\\include
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
)
