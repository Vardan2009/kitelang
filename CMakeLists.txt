# CMakeLists.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

cmake_minimum_required(VERSION 3.8)

project("kitelang")

# Set the source and destination directories
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/stdinclude")
set(DEST_DIR "${CMAKE_BINARY_DIR}/kitelang/stdinclude")

file(MAKE_DIRECTORY "${DEST_DIR}")

file(GLOB CONFIG_FILES "${SOURCE_DIR}/*")

foreach(CONFIG_FILE ${CONFIG_FILES})
    get_filename_component(FILE_NAME ${CONFIG_FILE} NAME)

    configure_file(
        "${CONFIG_FILE}"
        "${DEST_DIR}/${FILE_NAME}"
        COPYONLY 
    )
endforeach()

add_subdirectory("kitelang")