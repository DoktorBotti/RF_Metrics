add_subdirectory(IO)

add_executable(commandline_rf console.cpp)
target_link_libraries(commandline_rf rf_lib)
target_include_directories(commandline_rf
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include

        SYSTEM INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
target_compile_options(commandline_rf PRIVATE ${COMPILER_WARNING_FLAGS})
