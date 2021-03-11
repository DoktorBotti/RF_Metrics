add_executable(inforf-example
    main.cpp)

target_link_libraries(inforf-example 
    inforf 
    ${PLLMODULES_LIBRARIES}
    ${PLL_LIBRARIES}
    ${PLLMODULES_LIBRARIES}
)
target_include_directories(inforf-example PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${LOCALDEPS_DIR}/include>)
