add_library(rf_lib STATIC
        include/PllSplits.hpp
        include/PllTree.hpp
        include/RfMetricInterface.hpp

        PllTree.cpp
        PllSplits.cpp
        RfMetricInterface.cpp
        )

set_target_properties(rf_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        )

target_link_libraries(rf_lib PUBLIC
        pll_lib
        growt_lib

        )
target_include_directories(rf_lib
        PRIVATE # our includes
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        SYSTEM INTERFACE # includes with (hopefully) no warnings and exported includes for other targets

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        pll_lib
        growt_lib
        )
# only generate loads of warnings for our lib - does not propagate to dependencies
target_compile_options(rf_lib PRIVATE ${COMPILER_WARNING_FLAGS})