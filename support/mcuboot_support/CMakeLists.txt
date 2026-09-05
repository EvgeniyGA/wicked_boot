# Путь к твоему сабмодулю tinycrypt
set(TINYCRYPT_DIR ${CMAKE_SOURCE_DIR}/modules/tinycrypt)

# 1. Создаем статическую библиотеку из исходников
# Для базовой валидации подписи (SHA256) достаточно sha256.c и utils.c
add_library(tinycrypt STATIC
    ${TINYCRYPT_DIR}/lib/source/sha256.c
    ${TINYCRYPT_DIR}/lib/source/utils.c
    # Если позже захочешь шифрование образов, добавишь сюда:
    # ${TINYCRYPT_DIR}/lib/source/aes_decrypt.c
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
target_link_libraries(bootutil PUBLIC tinycrypt)


add_library(mcuboot_support STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flash_map.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mcuboot_port.c
)

target_include_directories(mcuboot_support PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/tinycrypt/lib/include
)

target_link_libraries(mcuboot_support PUBLIC 
    STM32_Drivers
)
