# cmake/version.cmake

function(generate_version_header)
    file(READ "${CMAKE_SOURCE_DIR}/VERSION" FW_VERSION_RAW)
    string(STRIP "${FW_VERSION_RAW}" FW_VERSION)
    
    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ "${FW_VERSION}")
    set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_2})
    set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_3})
    set(PROJECT_VERSION "${FW_VERSION}")
    
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    if(NOT GIT_HASH)
        set(GIT_HASH "unknown")
    endif()
    
    string(TIMESTAMP BUILD_DATE "%Y-%m-%d" UTC)
    string(TIMESTAMP BUILD_TIME "%H:%M:%S" UTC)
    
    configure_file(
        ${CMAKE_SOURCE_DIR}/version.h.in
        ${CMAKE_BINARY_DIR}/generated/version.h
        @ONLY
    )
    
    set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
    set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
    set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)
    set(PROJECT_VERSION ${PROJECT_VERSION} PARENT_SCOPE)
    set(GIT_HASH ${GIT_HASH} PARENT_SCOPE)
    set(BUILD_DATE ${BUILD_DATE} PARENT_SCOPE)
    set(BUILD_TIME ${BUILD_TIME} PARENT_SCOPE)

    add_library(app_version INTERFACE)
    target_include_directories(app_version INTERFACE ${CMAKE_BINARY_DIR}/generated)
    
endfunction()