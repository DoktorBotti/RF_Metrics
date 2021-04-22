add_library(rf_lib STATIC
        PllTree.cpp
        PllTree.hpp
        PllSplits.cpp
        PllSplits.hpp
)

set_target_properties(rf_lib PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_link_libraries(rf_lib ${PLLMODULES_LIBRARIES})
target_include_directories(rf_lib PRIVATE ${LOCALDEPS_DIR}/include)
