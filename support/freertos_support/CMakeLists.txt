add_library(freertos_support STATIC 
    freertos.c
    freertos_hook.c
)

target_link_libraries(freertos_support PUBLIC 
    freertos_kernel
    freertos_config
)

target_include_directories(freertos_support PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)