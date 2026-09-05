add_library(config_service STATIC 
    src/config_service.c
)

target_include_directories(config_service PUBLIC 
    include
)

target_link_libraries(config_service PUBLIC 
    freertos_kernel
    fatfs_core
)