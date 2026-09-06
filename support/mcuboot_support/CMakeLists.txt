add_library(mbedtls_asn1 STATIC
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/src/asn1parse.c
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/src/platform_util.c
)

target_include_directories(mbedtls_asn1 PUBLIC
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/include
)

add_library(tinycrypt STATIC
    ${CMAKE_SOURCE_DIR}/modules/tinycrypt/lib/source/sha256.c
    ${CMAKE_SOURCE_DIR}/modules/tinycrypt/lib/source/utils.c
    ${CMAKE_SOURCE_DIR}/modules/tinycrypt/lib/source/ecc.c
    ${CMAKE_SOURCE_DIR}/modules/tinycrypt/lib/source/ecc_dsa.c
)

target_include_directories(tinycrypt PUBLIC 
    ${CMAKE_SOURCE_DIR}/modules/tinycrypt/lib/include
)

target_link_libraries(bootutil PUBLIC 
    tinycrypt
    mbedtls_asn1
)

add_library(mcuboot_support STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flash_map.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcuboot_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ecdsa_pub_key.c
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/boot/mbed/app_enc_keys.c
)

target_include_directories(mcuboot_support PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(mcuboot_support PUBLIC 
    STM32_Drivers
    bootutil
)

target_include_directories(bootutil PUBLIC 
    ${CMAKE_SOURCE_DIR}/support/mcuboot_support/include
)