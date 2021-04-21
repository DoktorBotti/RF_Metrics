
add_executable(inforf-example
    main.cpp first_tests.cpp)

target_link_libraries(inforf-example PUBLIC
        inforf
        ${PLLMODULES_LIBRARIES}
        ${PLL_LIBRARIES}
        ${PLLMODULES_LIBRARIES}
    PRIVATE
        Catch2::Catch2
        )
include(CTest)
include(Catch)
catch_discover_tests(inforf-example)
target_include_directories(inforf-example PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${LOCALDEPS_DIR}/include>)
