cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif ()

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif ()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_EXTENSIONS ON)
# Genera el compile_commands.json para los clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(taller_tp
        VERSION 1.0
        HOMEPAGE_URL "https://github.com/URL-HERE"
        LANGUAGES CXX
)

# cambiar flags con `ccmake build` o `cmake-gui buildq`
option(TALLER_TESTS "Enable / disable tests." ON)
option(TALLER_CLIENT "Enable / disable client program." ON)
option(TALLER_SERVER "Enable / disable server program." ON)
option(TALLER_EDITOR "Enable / disable editor program." ON)
option(TALLER_MAKE_WARNINGS_AS_ERRORS "Enable / disable warnings as errors." ON)

option(OFFLINE "Build without network for testing" OFF)

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Define target: lib common between client, server and editor programs
add_library(taller_common STATIC)

# add the source files that make the lib
# HEY!! TODO XXX: add more subfolders here
add_subdirectory(src/common)


# set compiler warnings to the targets
include(cmake/CompilerWarnings.cmake)
set_project_warnings(taller_common ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

# Say where the headers will be found required by library target
# Make this PUBLIC so any dependant linking lib will inherit this
# location (so our dependants will not have to explicitly request this)
target_include_directories(taller_common PUBLIC ${CMAKE_SOURCE_DIR}/src/)

include(cmake/FetchContent_Declare.cmake)

include(cmake/FindPackage.cmake)

include(cmake/Executables.cmake)

include(cmake/Assets.cmake)
