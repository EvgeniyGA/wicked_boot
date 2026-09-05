add_library(usb_service STATIC 
    src/usb_service.c
)

target_include_directories(usb_service PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(usb_service PUBLIC 
    freertos_kernel
    freertos_cli_core
    tinyusb_support
    bsp_drivers
)