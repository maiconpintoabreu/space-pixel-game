cmake_minimum_required(VERSION 3.22)
project(space-pixel-game VERSION 0.1.0 LANGUAGES C CXX)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(TESTING "Avoid to breaking trying to use opengl" OFF)
#set(CMAKE_BUILD_TYPE Debug)

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