# Collect source files from the src directory
file(GLOB SRC_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB SRC_HEADERS "${CMAKE_SOURCE_DIR}/src/include/*.h")

# Create a static library from src files
add_library(space-pixel-lib STATIC ${SRC_SOURCES} ${SRC_HEADERS})
target_link_libraries(space-pixel-lib PUBLIC raylib)

# Set include directories for the library
target_include_directories(space-pixel-lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
)

# Only build tests in Debug mode
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    # Collect test source files
    file(GLOB TEST_SOURCES "*.cpp")

    # Create the test executable (pseudo-target)
    add_executable(space-pixel-tests ${TEST_SOURCES})

    # Link the test executable against the game library and Google Test
    target_link_libraries(space-pixel-tests
        PRIVATE
            space-pixel-lib
            GTest::gtest
            GTest::gtest_main
            pthread
    )

    # Discover and add tests
    enable_testing()
    include(GoogleTest)

    # Perform test discovery using the created target
    gtest_discover_tests(space-pixel-tests)
endif()

target_compile_definitions(space-pixel-lib PRIVATE TESTING)

# Installation (Optional)
install(TARGETS space-pixel-lib DESTINATION lib)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/include/ DESTINATION include)