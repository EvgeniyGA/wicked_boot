add_library(lcd_service STATIC 
    include/lcd_printer.h
    src/lcd_driver.h 
    src/lcd_printer.c
    src/lcd_driver.c
)

target_include_directories(lcd_service PUBLIC
    include
)
       
target_link_libraries(lcd_service PRIVATE 
    freertos_kernel
    STM32_Drivers
)