
add_executable(catch2_tests
        main.cpp
        first_tests.cpp
        PllSplitsTest.cpp
        RFTest.cpp
        testIO.cpp
        MciTest.cpp
        OrToolsTest.cpp
        ${CMAKE_SOURCE_DIR}/src/rf/helpers/Util.h
        OrToolsTest.cpp
        GeneralizedMetricsTest.cpp
        SymmetryDebug.cpp
        cacherTest.cpp
        )

target_link_libraries(catch2_tests PUBLIC
        rf_lib
        PRIVATE
        Catch2::Catch2
        rf_io_lib
        datagen
        )
include(CTest)
include(Catch)
catch_discover_tests(catch2_tests)
target_include_directories(catch2_tests PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${LOCALDEPS_DIR}/include>
        ${CMAKE_BINARY_DIR}/misc/include
        rf_io_lib
        )
target_compile_options(catch2_tests PRIVATE ${COMPILER_WARNING_FLAGS})