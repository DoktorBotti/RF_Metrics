add_executable(commandline_rf console.cpp)
target_link_libraries(commandline_rf PRIVATE
        rf_lib
        ${PLLMODULES_LIBRARIES}
        ${PLL_LIBRARIES}
        ${PLLMODULES_LIBRARIES} )
target_include_directories(commandline_rf PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${LOCALDEPS_DIR}/include>)