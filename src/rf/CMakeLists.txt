add_library(rf_lib STATIC
        include/PllSplits.hpp
        include/PllTree.hpp
        include/RfMetricInterface.h
        PllTree.cpp
        PllSplits.cpp
        RfMetricInterface.cpp
        LoggingBackend.cpp
        include/LoggingBackend.h
        include/RfAlgorithmInterface.h
        StandardRfAlgo.cpp
        include/StandardRfAlgo.h
        include/SymmetricMatrix.hpp
        helpers/Util.h
        helpers/Util.cpp
        include/HashmapKey.hpp
        include/HashingFunctor.hpp
        TrivialSequentialRFAlgo.cpp
        include/TrivialSequentialRFAlgo.h
        )

set_target_properties(rf_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        )

target_link_libraries(rf_lib PUBLIC
        pll_lib
        growt_lib
        Boost::log
        )
target_include_directories(rf_lib
        PUBLIC # our includes
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_BINARY_DIR}/misc/include
        #SYSTEM INTERFACE # includes with (hopefully) no warnings and exported includes for other targets

        #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
# only generate loads of warnings for our lib - does not propagate to dependencies
target_compile_options(rf_lib PRIVATE ${COMPILER_WARNING_FLAGS})