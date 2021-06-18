
if(NOT "${CMAKE_CURRENT_BINARY_DIR}" MATCHES ".*softwipe.*")
# only defining the softwipe target if path does not contain "softwipe" to prevent accidental recursive builds
    add_custom_target(rf_softwipe
    DEPENDS
        rf_lib
    COMMENT
        "This command runs the softwipe evaluation tool on ALL targets. It stores the analysis results in ROOT_proj/misc"
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
        #${CMAKE_COMMAND} -E touch ./genFile
        /bin/bash -c "softwipe -CM ${CMAKE_SOURCE_DIR} -e executeFile -x ${CMAKE_SOURCE_DIR}/libs,${CMAKE_SOURCE_DIR}/utils,${CMAKE_CURRENT_SOURCE_DIR},${CMAKE_SOURCE_DIR}/src/commandline/IO/json,${PROJECT_BINARY_DIR} -a ASSERT,CHECK,REQUIRE"
)

endif()

# create target which spills out a header containing the current commit id
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
    FIND_PACKAGE(Git)
    IF(GIT_FOUND)
        EXECUTE_PROCESS(
                COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE "gitCommit_BUILD_VERSION"
                ERROR_QUIET
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        MESSAGE(STATUS "Git version: ${gitCommit_BUILD_VERSION}" )
    ELSE(GIT_FOUND)
        SET(gitCommit_BUILD_VERSION 0)
    ENDIF(GIT_FOUND)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gitVersionHeader.h.in  ${CMAKE_CURRENT_BINARY_DIR}/include/gitVersionHeader.h @ONLY)

add_executable(min_example ${CMAKE_CURRENT_SOURCE_DIR}/sample_main.cpp)
target_link_libraries(min_example growt_lib)