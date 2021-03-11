cmake_minimum_required(VERSION 3.12)
project(inforf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(LOCALDEPS_DIR ${PROJECT_BINARY_DIR}/localdeps)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(libs)
add_subdirectory(src)
add_subdirectory(test)
