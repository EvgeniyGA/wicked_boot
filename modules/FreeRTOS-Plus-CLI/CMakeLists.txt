add_library(freertos_cli_core STATIC 
    FreeRTOS_CLI.c
)

target_include_directories(freertos_cli_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(freertos_cli_core PUBLIC 
    freertos_kernel
)
