add_library(mcuboot_config INTERFACE)
target_include_directories(mcuboot_config SYSTEM INTERFACE
    include
)


add_library(mcuboot_support STATIC 
    src/flash_map.c
    include
)

target_include_directories(mcuboot_support PUBLIC 
    include
    include/mcuboot_config
    include/sysflash
    include/flash_map_backend
)

target_link_libraries(mcuboot_support PUBLIC 
    bootutil
    mcuboot_config
)

