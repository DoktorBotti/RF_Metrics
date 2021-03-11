add_library(splitmaker
    PllTree.cpp
    PllTree.hpp
)

set_target_properties(splitmaker PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_link_libraries(splitmaker ${PLLMODULES_LIBRARIES})
target_include_directories(splitmaker PRIVATE ${LOCALDEPS_DIR}/include)
