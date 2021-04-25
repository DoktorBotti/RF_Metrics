cmake_minimum_required(VERSION 3.8)
project(inforf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(LOCALDEPS_DIR ${PROJECT_BINARY_DIR}/localdeps)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
enable_testing()

add_subdirectory(libs)
# need to set the module path in common top cmake file. Makes modules Catch available for automatic test recognition
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/libs/catch2/contrib)

# define compiler warning flags which then can be set for specific targets
set(COMPILER_WARNING_FLAGS $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wunused-parameter
        -Wredundant-decls -Wreturn-type -Wswitch-default
        -Wunused-value -Wimplicit -Wimplicit-function-declaration -Wimplicit-int
        -Wimport -Wunused -Wunused-function -Wunused-label -Wno-int-to-pointer-cast
        -Wbad-function-cast -Wmissing-declarations -Wmissing-prototypes -Wnested-externs
        -Wold-style-definition -Wstrict-prototypes -Wpointer-sign -Wextra -Wredundant-decls
        -Wunused -Wunused-function -Wunused-parameter -Wunused-value -Wunused-variable -Wformat
        -Wformat-nonliteral -Wparentheses -Wsequence-point -Wuninitialized -Wundef -Wbad-function-cast>
        $<$<CXX_COMPILER_ID:MSVC>:
        /W4>)

add_subdirectory(src/commandline)
add_subdirectory(src/rf)
add_subdirectory(utils)

add_subdirectory(test)
