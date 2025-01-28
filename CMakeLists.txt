cmake_minimum_required(VERSION 3.22)
project(space-pixel-game VERSION 0.1.0 LANGUAGES C CXX)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set options for build types
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

# Dependencies
# set(RAYLIB_VERSION 5.5) # Change this to the version you want to use
set(RAYLIB_VERSION "master")
include(FetchContent)
# check if RAYLIB_VERSION is a number
if(RAYLIB_VERSION MATCHES "^[0-9.]+$")
    find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED

    if (NOT raylib_FOUND) # If there's none, fetch and build raylib
        FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
        FetchContent_GetProperties(raylib)
        if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
            set(FETCHCONTENT_QUIET NO)
            FetchContent_MakeAvailable(raylib)
            set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        endif()

    endif()
else()
    # download the latest version of raylib
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG ${RAYLIB_VERSION} 
    )
    FetchContent_GetProperties(raylib)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
endif()    

add_subdirectory(src)

set(GOOGLETEST_VERSION 1.15.2)

# --- Add Google Test using FetchContent ---
if (NOT googletest_FOUND) # If there's none, fetch and build googletest
    FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/google/googletest/archive/refs/tags/v${GOOGLETEST_VERSION}.tar.gz
    )
endif()
# Check if we are in Debug mode before making Google Test available
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    find_package(googletest ${GOOGLETEST_VERSION} QUIET) # QUIET or REQUIRED
    FetchContent_GetProperties(googletest)
    if (NOT googletest_POPULATED) # Have we downloaded googletest yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(googletest)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()   

    add_subdirectory(tests)
    enable_testing() # Enable CTest only when tests are included
endif()