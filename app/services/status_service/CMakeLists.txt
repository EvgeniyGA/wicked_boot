add_library(status_service STATIC 
    src/status_service.c
)

target_include_directories(status_service PUBLIC 
    include
)

target_link_libraries(status_service PUBLIC 
    freertos_kernel
)