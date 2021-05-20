cmake_minimum_required(VERSION 3.8)
project(inforf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(LOCALDEPS_DIR ${PROJECT_BINARY_DIR}/localdeps)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
enable_testing()

add_subdirectory(libs)
# need to set the module path in common top cmake file. Makes modules Catch available for automatic test recognition
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/libs/catch2/contrib "${CMAKE_SOURCE_DIR}/cmake")

# search for gmp library
find_package(GMP REQUIRED)

# find and configure boost in system
SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.76.0 COMPONENTS log REQUIRED)

# find or-tools (google, linear programming library)
# if the below find_package fails, you are probably not building in docker and need to manually point to your or-tools binary.
# e.g.: cmake -DORTOOLS_ROOT:STRING="/my/path/to/the/unzipped/file" ..
if(NOT DEFINED ORTOOLS_ROOT)
    SET(ORTOOLS_ROOT /home/user/or-tools_Ubuntu-20.04-64bit_v9.0.9048)
endif()
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${ORTOOLS_ROOT})
find_package(ortools REQUIRED)

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
#adds automatic softwipe execution
add_subdirectory(misc)
