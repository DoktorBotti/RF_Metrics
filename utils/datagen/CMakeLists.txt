add_executable(datagen main.cpp)

target_link_libraries(datagen ${PLLMODULES_LIBRARIES})
target_include_directories(datagen PRIVATE ${LOCALDEPS_DIR}/include)
