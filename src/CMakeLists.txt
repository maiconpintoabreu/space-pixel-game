file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include include/physics)

# Detect the compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Do not use /W4 for now, as it is too strict and generates too many warnings
    # target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX) 
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  #Intel Compiler
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")    
    # Since WASM is used, ALLOW_MEMORY_GROWTH has no extra overheads
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s WASM=1 -s ASYNCIFY -s ALLOW_MEMORY_GROWTH=1 --shell-file ${CMAKE_SOURCE_DIR}/src/minshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    #DEPENDS ${PROJECT_NAME}
#else()
    #add_custom_command(
    #    TARGET ${PROJECT_NAME} POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    #)
    #DEPENDS ${PROJECT_NAME}
endif()

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} raylib)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")

    # Preload files
    #file(GLOB ASSETS "${CMAKE_SOURCE_DIR}/src/resources/*") # Gather all files in resources
    #foreach(ASSET ${ASSETS})
    #    get_filename_component(ASSET_NAME ${ASSET} NAME)
    #    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" ASSET_PATH ${ASSET})
    #    message(STATUS "Preloading: ${ASSET_PATH} as ${CMAKE_SOURCE_DIR}/src/resources/${ASSET_NAME}")
    #    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${ASSET_PATH}@resources/${ASSET_NAME}")
    #endforeach()
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()