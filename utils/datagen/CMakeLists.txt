add_library(datagen RndTreeGenerator.cpp RndTreeGenerator.h)

target_link_libraries(datagen ${PLLMODULES_LIBRARIES})
target_include_directories(datagen
        PRIVATE ${LOCALDEPS_DIR}/include
        PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})
