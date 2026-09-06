cmake_minimum_required(VERSION 3.22)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

file(READ "${CMAKE_SOURCE_DIR}/VERSION" FW_VERSION_RAW)
string(STRIP "${FW_VERSION_RAW}" FW_VERSION)

project(wicked_boot VERSION ${FW_VERSION} LANGUAGES C CXX ASM)

if(FOR_QEMU)
    add_compile_definitions(MSC_USE_SRAM)
    add_compile_definitions(PRINTF_UART)
    add_compile_definitions(FOR_QEMU)
    set(MAIN_TIME_RESOLUTION 1 CACHE STRING "buffer size")
    set(DISKIO_DRIVER_SRC "drivers/sram_diskio.c")
else()
    add_compile_definitions(MSC_USE_FLASH)
    add_compile_definitions(PRINTF_RTT)
    set(MAIN_TIME_RESOLUTION 32 CACHE STRING "buffer size")
    set(DISKIO_DRIVER_SRC "drivers/flash_diskio.c")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

if(NOT DEFINED TARGET)
    message(FATAL_ERROR "TARGET not specified: ${TARGET}")#    set(TARGET "stm32f4" CACHE STRING "Target hardware platform")
endif()

set(MAIN_EXECUTABLE "wicked_boot")
add_executable(${MAIN_EXECUTABLE})

target_compile_definitions(${MAIN_EXECUTABLE} PRIVATE
    MAIN_TIME_RESOLUTION=${MAIN_TIME_RESOLUTION}
)

include(cmake/version.cmake)
generate_version_header()


#set(MCUBOOT_USE_MBED_TLS OFF CACHE BOOL "" FORCE)
#set(MCUBOOT_USE_TINYCRYPT ON CACHE BOOL "" FORCE)
#set(MCUBOOT_SIGN_EC256 ON CACHE BOOL "" FORCE)
#set(MCUBOOT_BOOT_MAX_ALIGN 8 CACHE STRING "" FORCE)
#set(MCUBOOT_FLASH_ALIGN 4 CACHE STRING "" FORCE)
#set(MCUBOOT_LOG_LEVEL 4  CACHE STRING "" FORCE)

# 1. ЖЕСТКО задаем переменные для внутреннего CMake-скрипта MCUboot
#set(MCUBOOT_USE_MBED_TLS OFF CACHE BOOL "" FORCE)
#set(MCUBOOT_USE_TINYCRYPT ON CACHE BOOL "" FORCE)
#set(MCUBOOT_SIGN_EC256 ON CACHE BOOL "" FORCE)

# 2. ГЛОБАЛЬНО задаем дефайны для компилятора (применится ко ВСЕМ .c файлам проекта)
#add_compile_definitions(
#    MCUBOOT_USE_TINYCRYPT=1
#    MCUBOOT_USE_MBED_TLS=0
#    MCUBOOT_SIGN_EC256=1
#    MCUBOOT_BOOT_MAX_ALIGN=8
#    MCUBOOT_FLASH_ALIGN=4
#    MCUBOOT_LOG_LEVEL=4
#)



# 3. Добавляем bootutil
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/mcuboot/boot/bootutil bootutil_build)


# 5. Линкуем математику TinyCrypt к bootutil
#target_link_libraries(bootutil PUBLIC tinycrypt)

# 6. Линкуем bootutil к твоему support-таргету (чтобы он видел заголовки)


add_subdirectory(targets)
add_subdirectory(modules)
add_subdirectory(support)
add_subdirectory(app)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/mcuboot/ext/tinycrypt tinycrypt_build)
#target_include_directories(bootutil PUBLIC 
#    ${CMAKE_CURRENT_SOURCE_DIR}/support/mcuboot_support/include
#    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/tinycrypt

#    ${CMAKE_CURRENT_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/include
#)







#target_compile_definitions(bootutil PUBLIC
#    MCUBOOT_USE_TINYCRYPT=1
#    MCUBOOT_USE_MBED_TLS=0
#    MCUBOOT_SIGN_EC256=1
#    MCUBOOT_BOOT_MAX_ALIGN=8
#    MCUBOOT_FLASH_ALIGN=4
#    MCUBOOT_LOG_LEVEL=4
#)


target_link_libraries(${MAIN_EXECUTABLE} 
    freertos_kernel
    freertos_support
    freertos_cli_core
    cli_service
    fatfs_core

    segger_rtt         
    segger_sysview 
    lcd_service

    config_service
    status_service
    tinyusb_support
    usb_service
    diskio_drivers
    bsp_drivers
    app_version
    
    bootutil
    mcuboot_support
)

find_program(CMAKE_OBJCOPY NAMES arm-none-eabi-objcopy REQUIRED)

add_custom_command(TARGET ${MAIN_EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${MAIN_EXECUTABLE}> ${MAIN_EXECUTABLE}.hex
    BYPRODUCTS ${MAIN_EXECUTABLE}.hex
    COMMENT "Generating Intel HEX file: ${MAIN_EXECUTABLE}.hex"
)
add_custom_command(TARGET ${MAIN_EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${MAIN_EXECUTABLE}> ${MAIN_EXECUTABLE}.bin
    BYPRODUCTS ${MAIN_EXECUTABLE}.bin
    COMMENT "Generating binary file: ${MAIN_EXECUTABLE}.bin"
)
