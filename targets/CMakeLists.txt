#------------------------------------------------------------
#------------- cubeide --------------------------------------
#------------------------------------------------------------
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/cmake/stm32cubemx
    ${CMAKE_BINARY_DIR}/stm32cubemx_${TARGET}
)


#------------------------------------------------------------
#----------------------- bsp drivers ---------------------
#------------------------------------------------------------
if(${TARGET} STREQUAL "stm32f4")
set(BSP_SOURCES
    ${TARGET}/Core/Src/Driver_GPIO.c
    ${TARGET}/Core/Src/W25Qxx.c
    ${TARGET}/Core/Src/Driver_Flash.c
    ${TARGET}/Core/Src/Driver_Usart.c
    ${TARGET}/Core/Src/boot_hal.c
    ${TARGET}/Core/Src/common.c
    ${TARGET}/Core/Src/lcd_driver.c
)
else()
set(BSP_SOURCES
    ${TARGET}/Core/Src/W25Qxx.c
# todo
)
endif()

add_library(bsp_drivers STATIC 
    ${BSP_SOURCES}
)

target_link_libraries(bsp_drivers PUBLIC 
    stm32cubemx
    bootutil
    segger_rtt
)

target_include_directories(bsp_drivers PUBLIC 
    ${TARGET}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/targets/${TARGET}/Core/Inc
)
