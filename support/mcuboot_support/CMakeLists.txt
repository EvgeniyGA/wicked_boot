add_library(mcuboot_support STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flash_map.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(mcuboot_support PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#target_link_libraries(mcuboot_support PUBLIC 
#    bootutil
#)
