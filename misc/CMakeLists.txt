
if(NOT "${CMAKE_CURRENT_BINARY_DIR}" MATCHES ".*softwipe.*")
# only defining the softwipe target if path does not contain "softwipe" to prevent accidental recursive builds
    add_custom_target(rf_softwipe
    DEPENDS
        rf_lib
    POST_BUILD
    COMMENT
        "This command runs the softwipe evaluation tool on ALL targets. It stores the analysis results in ROOT_proj/misc"
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
        #${CMAKE_COMMAND} -E touch ./genFile
        /bin/bash -c "softwipe -CM ${CMAKE_CURRENT_SOURCE_DIR}/../ -e executeFile -x ${CMAKE_CURRENT_SOURCE_DIR}/../libs,${CMAKE_CURRENT_SOURCE_DIR}/../utils,${CMAKE_CURRENT_SOURCE_DIR}"
)

endif()
