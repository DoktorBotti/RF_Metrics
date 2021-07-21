add_library(rf_lib STATIC
        FastSplitList.cpp
        GeneralizedRfAlgo.cpp
        LogDblFact.cpp
        LoggingBackend.cpp
        Matcher.cpp
        MatcherOrTools.cpp
        MciAlgo.cpp
        MsiAlgo.cpp
        PllSplit.cpp
        PllSplitList.cpp
        PllTree.cpp
        RfAlgorithmInterface.cpp
        RfMetricInterface.cpp
        SpiAlgo.cpp
        SplitList.cpp
        StandardRfAlgo.cpp
        helpers/Util.cpp
        helpers/Util.h
        include/FastSplitList.h
        include/GeneralizedRfAlgo.h
        include/HashmapUtil/OneSplitHashingFunctor.hpp
        include/LogDblFact.h
        include/LoggingBackend.h
        include/Matcher.h
        include/MatcherOrTools.h
        include/MciAlgo.h
        include/MsiAlgo.h
        include/PllSplitList.h
        include/PllTree.hpp
        include/RectMatrix.hpp
        include/RfAlgorithmInterface.h
        include/RfMetricInterface.h
        include/SpiAlgo.h
        include/SplitList.h
        include/StandardRfAlgo.h
        include/SymmetricMatrix.hpp
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
        Threads::Threads
        #ortools_include_dir
        #$<BUILD_INTERFACE:${ortools}>

        #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
target_link_libraries(rf_lib PUBLIC
        pll_lib
        #growt_lib
        Boost::log
        rf_io_lib
        ortools
        )

# only generate loads of warnings for our lib - does not propagate to dependencies
target_compile_options(rf_lib PRIVATE -mpopcnt ${COMPILER_WARNING_FLAGS})