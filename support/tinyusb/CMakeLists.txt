add_library(tinyusb_support STATIC 
    usb_descriptors.c
    msc_disk.c
    ${CMAKE_SOURCE_DIR}/modules/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c
    ${CMAKE_SOURCE_DIR}/modules/tinyusb/src/portable/synopsys/dwc2/dwc2_common.c
)

target_include_directories(tinyusb_support PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/modules/tinyusb/hw
)

tinyusb_target_add(tinyusb_support)

target_link_libraries(tinyusb_support PUBLIC 
    freertos_kernel
    diskio_drivers
    STM32_Drivers
)