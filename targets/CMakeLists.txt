#------------------------------------------------------------
#------------- cubeide --------------------------------------
#------------------------------------------------------------
add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/cmake/stm32cubemx
    ${CMAKE_BINARY_DIR}/stm32cubemx_${TARGET}
)
#------------------------------------------------------------
#------------- freertos platform specific config ------------
#------------------------------------------------------------
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/support/freertos_support
)

target_link_libraries(freertos_config INTERFACE segger_sysview)

if(${TARGET} STREQUAL "stm32f4")
    set( FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "FreeRTOS port" FORCE)
elseif(${TARGET} STREQUAL "stm32f7")
    set( FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "FreeRTOS port" FORCE)
else()
    message(FATAL_ERROR "Неизвестный TARGET: '${TARGET}'. Добавьте настройку FREERTOS_PORT для этой платформы.")
endif()

set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)

#------------------------------------------------------------
#----------------------- bsp drivers ---------------------
#------------------------------------------------------------
set(BSP_SOURCES
    ${TARGET}/Core/Src/gpio_driver.c
    ${TARGET}/Core/Src/W25Qxx.c
    ${TARGET}/Core/Src/common.c
)

add_library(bsp_drivers STATIC 
    ${BSP_SOURCES}
)

target_link_libraries(bsp_drivers PUBLIC 
    stm32cubemx
)

target_include_directories(bsp_drivers PUBLIC 
    ${TARGET}/Core/Inc
)
