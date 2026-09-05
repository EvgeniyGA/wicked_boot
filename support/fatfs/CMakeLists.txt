add_library(fatfs_core STATIC)

set(FATFS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/modules/fatfs/source)

target_sources(fatfs_core PRIVATE
    ${FATFS_SOURCE_DIR}/ff.c
    ${FATFS_SOURCE_DIR}/ff_gen_drv.c
    ${FATFS_SOURCE_DIR}/diskio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fatfs.c
)

target_include_directories(fatfs_core PUBLIC
    ${FATFS_SOURCE_DIR}  
    ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(fatfs_core PUBLIC freertos_kernel STM32_Drivers diskio_drivers)
