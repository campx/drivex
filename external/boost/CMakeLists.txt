cmake_minimum_required(VERSION 2.8)
project(boost NONE)
include(ExternalProject)

ExternalProject_Add (
    boost
    GIT_REPOSITORY    https://github.com/boostorg/boost
    GIT_TAG           master
    GIT_SUBMODULES    tools/build libs/config tools/boostdep libs/filesystem
    INSTALL_DIR       ${CMAKE_BINARY_DIR}
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}
    BUILD_IN_SOURCE   1
    CONFIGURE_COMMAND python <SOURCE_DIR>/tools/boostdep/depinst/depinst.py filesystem
    BUILD_COMMAND     <SOURCE_DIR>/bootstrap.sh --prefix=<INSTALL_DIR>
    INSTALL_COMMAND   <SOURCE_DIR>/b2 install --prefix=<INSTALL_DIR>
    UPDATE_COMMAND    ""
)
