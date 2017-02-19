cmake_minimum_required(VERSION 3.0)
project(taihen-parser)

# Set default locations
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if (EXISTS ${CMAKE_SOURCE_DIR}/taihen-parser)
    add_subdirectory(taihen-parser)
endif(EXISTS ${CMAKE_SOURCE_DIR}/taihen-parser)

add_subdirectory(taihen-parser-alt)
add_subdirectory(taihen-parser-bench)

