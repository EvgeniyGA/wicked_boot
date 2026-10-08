cmake_minimum_required(VERSION 3.22)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/shell/include
)

target_sources(${PROJECT_NAME} PRIVATE
    core/src/app.c
    core/src/shell_commands.c
    core/src/retarget.c
    core/src/wrapper.c
    core/src/version_check.c
    shell/src/shell.c
)
