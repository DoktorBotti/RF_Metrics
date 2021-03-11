add_library(inforf
    PllTree.cpp
    PllTree.hpp
    PllSplits.cpp
    PllSplits.hpp
)

set_target_properties(inforf PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_link_libraries(inforf ${PLLMODULES_LIBRARIES})
target_include_directories(inforf PRIVATE ${LOCALDEPS_DIR}/include)
