cmake_minimum_required(VERSION 3.5)
project(inforf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(LOCALDEPS_DIR ${PROJECT_BINARY_DIR}/localdeps)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
enable_testing()

add_subdirectory(libs)
# need to set the module path in common top cmake file. Makes modules Catch available for automatic test recognition
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/libs/catch2/contrib)

add_subdirectory(src)
add_subdirectory(utils)

add_subdirectory(test)
