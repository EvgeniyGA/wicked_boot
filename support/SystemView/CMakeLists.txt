add_library(segger_sysview INTERFACE)

target_include_directories(segger_sysview INTERFACE
    ${CMAKE_SOURCE_DIR}/modules/SystemView/SYSVIEW
    ${CMAKE_SOURCE_DIR}/modules/SystemView/Config
    ${CMAKE_SOURCE_DIR}/modules/SystemView/SEGGER
    ${CMAKE_SOURCE_DIR}/modules/SystemView/Sample/FreeRTOSV11
)

target_sources(segger_sysview INTERFACE
    ${CMAKE_SOURCE_DIR}/modules/RTT/RTT/SEGGER_RTT_ASM_ARMv7M.S 
    ${CMAKE_SOURCE_DIR}/modules/SystemView/SYSVIEW/SEGGER_SYSVIEW.c
    ${CMAKE_SOURCE_DIR}/modules/SystemView/Sample/FreeRTOSV11/SEGGER_SYSVIEW_FreeRTOS.c
    ${CMAKE_SOURCE_DIR}/modules/SystemView/Sample/FreeRTOSV11/Config/Cortex-M/SEGGER_SYSVIEW_Config_FreeRTOS.c
)

target_link_libraries(segger_sysview INTERFACE segger_rtt)