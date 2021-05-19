add_library(rf_lib STATIC
        GeneralizedRfAlgo.cpp
        LoggingBackend.cpp
        Matcher.cpp
        MciAlgo.cpp
        MsiAlgo.cpp
        PllSplits.cpp
        PllTree.cpp
        RfMetricInterface.cpp
        StandardRfAlgo.cpp
        helpers/Util.cpp
        helpers/Util.h
        include/GeneralizedRfAlgo.h
        include/HashingFunctor.hpp
        include/HashmapKey.hpp
        include/LoggingBackend.h
        include/Matcher.h
        include/MciAlgo.h
        include/MsiAlgo.h
        include/PllSplits.hpp
        include/PllTree.hpp
        include/RfAlgorithmInterface.h
        include/RfMetricInterface.h
        include/StandardRfAlgo.h
        include/SymmetricMatrix.hpp
        SpiAlgo.cpp
        include/SpiAlgo.h
        )

set_target_properties(rf_lib PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        )

target_include_directories(rf_lib
        PUBLIC # our includes
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_BINARY_DIR}/misc/include
        SYSTEM PRIVATE # includes with (hopefully) no warnings and exported includes for other targets
        #ortools_include_dir
        $<BUILD_INTERFACE:${ortools}>
        #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
target_link_libraries(rf_lib PUBLIC
        pll_lib
        growt_lib
        Boost::log
        rf_io_lib
        ortools
        )

# only generate loads of warnings for our lib - does not propagate to dependencies
target_compile_options(rf_lib PRIVATE ${COMPILER_WARNING_FLAGS})