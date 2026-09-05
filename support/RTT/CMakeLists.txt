add_library(segger_rtt INTERFACE)

#target_include_directories(${PROJECT_NAME} PUBLIC
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/RTT/RTT
#)

#target_sources(${PROJECT_NAME} PRIVATE
##    ${CMAKE_CURRENT_SOURCE_DIR}/../..//modules/RTT/Syscalls/SEGGER_RTT_Syscalls_GCC.c
#    ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/RTT/RTT/SEGGER_RTT.c
#    ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/RTT/RTT/SEGGER_RTT_printf.c
#)

target_include_directories(segger_rtt INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}                    
    ${CMAKE_SOURCE_DIR}/modules/RTT/RTT           
)

target_sources(segger_rtt INTERFACE
    ${CMAKE_SOURCE_DIR}/modules/RTT/RTT/SEGGER_RTT.c
#    ${CMAKE_SOURCE_DIR}/modules/RTT/RTT/SEGGER_RTT_ASM_ARMv7M.S
    ${CMAKE_SOURCE_DIR}/modules/RTT/RTT/SEGGER_RTT_printf.c
)

target_compile_definitions(segger_rtt INTERFACE
    USE_SEGGER_RTT
)

#target_compile_definitions(${PROJECT_NAME} PRIVATE USE_SEGGER_RTT)