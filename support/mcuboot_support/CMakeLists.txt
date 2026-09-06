add_library(mbedtls_asn1 STATIC
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/src/asn1parse.c
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/src/platform_util.c
#    ${CMAKE_CURRENT_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/src/oid.c
)
target_include_directories(mbedtls_asn1 PUBLIC
    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/include
)


# Путь к твоему сабмодулю tinycrypt
set(TINYCRYPT_DIR ${CMAKE_SOURCE_DIR}/modules/tinycrypt)

# 1. Создаем статическую библиотеку из исходников
# Для базовой валидации подписи (SHA256) достаточно sha256.c и utils.c
add_library(tinycrypt STATIC
    ${TINYCRYPT_DIR}/lib/source/sha256.c
    ${TINYCRYPT_DIR}/lib/source/utils.c
    ${TINYCRYPT_DIR}/lib/source/ecc.c
    # Если позже захочешь шифрование образов, добавишь сюда:
    ${TINYCRYPT_DIR}/lib/source/ecc_dsa.c
    # ${TINYCRYPT_DIR}/lib/source/aes_encrypt.c
    # ${TINYCRYPT_DIR}/lib/source/cbc_mode.c
)

# 2. МАГИЯ ИНКЛУДОВ: указываем папку lib/include как PUBLIC
# Благодаря этому #include <tinycrypt/sha256.h> сработает идеально,
# и этот путь автоматически пробросится всем, кто будет линковать tinycrypt.
target_include_directories(tinycrypt PUBLIC 
    ${TINYCRYPT_DIR}/lib/include
)

# 3. Линкуем tinycrypt к bootutil (или к mcuboot_support)
# Теперь bootutil "увидит" заголовки tinycrypt и сможет скомпилироваться
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
#    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/tinycrypt
#    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/include
)

target_link_libraries(mcuboot_support PUBLIC 
    STM32_Drivers
    bootutil
)

target_include_directories(bootutil PUBLIC 
    ${CMAKE_SOURCE_DIR}/support/mcuboot_support/include
#    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/tinycrypt
#    ${CMAKE_SOURCE_DIR}/modules/mcuboot/ext/mbedtls-asn1/include
)