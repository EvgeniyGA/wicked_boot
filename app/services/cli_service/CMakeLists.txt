add_library(cli_service STATIC
    src/cli_service.c
    src/cli_commands.c 
    src/cli_commands_fs.c
    src/cli_commands_app.c
)

target_include_directories(cli_service PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(cli_service PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cli_service PUBLIC 
    freertos_cli_core
    fatfs_core
)