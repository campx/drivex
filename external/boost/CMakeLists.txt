cmake_minimum_required(VERSION 2.8)
project(boost NONE)
include(ExternalProject)

if (UNIX)
    set(BOOTSTRAP bootstrap.sh)
elseif (WIN32)
    set(BOOTSTRAP bootstrap.bat)
    if (MINGW)
        set(TOOLSET gcc)
        set(ADDRESS_MODEL address-model=32)
    endif ()
endif ()
string(TOLOWER ${CMAKE_BUILD_TYPE} VARIANT)

ExternalProject_Add (
    boost
    GIT_REPOSITORY    https://github.com/boostorg/boost
    GIT_TAG           master
        GIT_SUBMODULES tools/build tools/boostdep libs/filesystem
    INSTALL_DIR       ${CMAKE_BINARY_DIR}
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}
    BUILD_IN_SOURCE   1
    CONFIGURE_COMMAND python <SOURCE_DIR>/tools/boostdep/depinst/depinst.py filesystem
        BUILD_COMMAND <SOURCE_DIR>/${BOOTSTRAP} ${TOOLSET}
    INSTALL_COMMAND   <SOURCE_DIR>/b2 install --prefix=<INSTALL_DIR>
        --with-filesystem --layout=system variant=${VARIANT} ${ADDRESS_MODEL}
    UPDATE_COMMAND    ""
)
