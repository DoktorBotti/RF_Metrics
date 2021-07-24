include_directories(AFTER ${LOCALDEPS_DIR}/include)

set(PLLMOD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/src")
set(LIBPLL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/pll-modules/libs/libpll/src")

file(COPY 
    ${PLLMOD_SRC}/pllmod_common.h
    ${PLLMOD_SRC}/msa/pll_msa.h
    ${PLLMOD_SRC}/tree/pll_tree.h
    ${PLLMOD_SRC}/util/pllmod_util.h
    ${LIBPLL_SRC}/pll.h
    DESTINATION ${LOCALDEPS_DIR}/include/libpll)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LOCALDEPS_DIR}/lib)

add_subdirectory(pll-modules)

# defining target lib for libpll
add_library(pll_lib INTERFACE)
target_link_libraries(pll_lib INTERFACE ${PLLMODULES_LIBRARIES})
target_include_directories(pll_lib INTERFACE
        ${LOCALDEPS_DIR}/include
        )

if(ENABLE_TESTS)
    add_subdirectory(catch2)
endif()
