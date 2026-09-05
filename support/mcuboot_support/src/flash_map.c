#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stddef.h>





// -----------------------------------------------------------------------------
// 4. Геттеры для нового API MCUboot
// -----------------------------------------------------------------------------
uint8_t flash_area_get_id(const struct flash_area *fa) {
    return fa->fa_id;
}

uint8_t flash_area_get_device_id(const struct flash_area *fa) {
    return fa->fa_device_id;
}

uint32_t flash_area_get_off(const struct flash_area *fa) {
    return fa->fa_off;
}

uint32_t flash_area_get_size(const struct flash_area *fa) {
    return fa->fa_size;
}

uint32_t flash_sector_get_off(const struct flash_sector *fs) {
    return fs->fs_off;
}

uint32_t flash_sector_get_size(const struct flash_sector *fs) {
    return fs->fs_size;
}
// -----------------------------------------------------------------------------
// 1. Определение областей флеш-памяти (строго по границам секторов STM32F4!)
// Разметка для 1 МБ STM32F407:
// Сектора 0-3: по 16 КБ (итого 64 КБ под Bootloader)
// Сектор 4: 64 КБ (можно использовать под данные/настройки)
// Сектор 5: 128 КБ (Primary Slot)
// Сектор 6: 128 КБ (Secondary Slot)
// -----------------------------------------------------------------------------
static const struct flash_area flash_areas[] = {
    // Bootloader: Сектора 0-3 (4 * 16КБ = 64КБ)
    {
        .fa_id = FLASH_AREA_BOOTLOADER,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off = 0x08000000,
        .fa_size = 0x00010000 // 64 KB
    },
    // Primary Slot: Сектор 5 (128КБ)
    {
        .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off = 0x08020000,
        .fa_size = 0x00020000 // 128 KB
    },
    // Secondary Slot: Сектор 6 (128КБ)
    {
        .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off = 0x08040000,
        .fa_size = 0x00020000 // 128 KB
    }
};

// -----------------------------------------------------------------------------
// 2. Вспомогательные функции для работы с аппаратными секторами STM32F4
// -----------------------------------------------------------------------------
static uint32_t get_sector_number(uint32_t address) {
    if (address < 0x08010000) {
        return (address - 0x08000000) / 0x4000; // Сектора 0-3 по 16 КБ
    } else if (address < 0x08020000) {
        return 4; // Сектор 4, 64 КБ
    } else if (address < 0x08100000) {
        return 5 + (address - 0x08020000) / 0x20000; // Сектора 5-11 по 128 КБ
    }
    return 0xFF; // Ошибка (адрес вне диапазона)
}

static uint32_t get_sector_size(uint32_t sector_num) {
    if (sector_num <= 3) return 0x4000;  // 16 KB
    if (sector_num == 4) return 0x10000; // 64 KB
    return 0x20000;                      // 128 KB (сектора 5-11)
}

// -----------------------------------------------------------------------------
// 3. Реализация API MCUboot
// -----------------------------------------------------------------------------

int flash_area_open(uint8_t id, const struct flash_area **area_outp) {
    for (size_t i = 0; i < sizeof(flash_areas) / sizeof(flash_areas[0]); i++) {
        if (flash_areas[i].fa_id == id) {
            *area_outp = &flash_areas[i];
            return 0;
        }
    }
    return -1; // Область не найдена
}

void flash_area_close(const struct flash_area *fa) {
    // Для внутренней флеш-памяти STM32 закрывать ничего не нужно
    (void)fa;
}

int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len) {
    // Внутренняя флеш STM32 отображена в память (memory-mapped), просто копируем
    if (off + len > fa->fa_size) {
        return -1; // Выход за границы области
    }
    memcpy(dst, (const void *)(fa->fa_off + off), len);
    return 0;
}

int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len) {
    if (off + len > fa->fa_size) {
        return -1;
    }

    HAL_StatusTypeDef status = HAL_OK;
    uint32_t address = fa->fa_off + off;
    const uint8_t *data = (const uint8_t *)src;

    HAL_FLASH_Unlock();

    // STM32F4 программируется по словам (4 байта / 32 бита).
    // MCUboot гарантирует, что len кратно MCUBOOT_FLASH_ALIGN (мы зададим 4).
    for (uint32_t i = 0; i < len; i += 4) {
        uint32_t data_to_write = *((const uint32_t *)(data + i));
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data_to_write);
        if (status != HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }
        address += 4;
    }

    HAL_FLASH_Lock();
    return 0;
}

int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len) {
    if (off + len > fa->fa_size) {
        return -1;
    }

    uint32_t current_addr = fa->fa_off + off;
    uint32_t remaining_len = len;

    HAL_FLASH_Unlock();

    while (remaining_len > 0) {
        uint32_t sector = get_sector_number(current_addr);
        uint32_t sector_size = get_sector_size(sector);
        
        uint32_t erase_len = (remaining_len < sector_size) ? remaining_len : sector_size;

        FLASH_EraseInitTypeDef erase_init = {
            .TypeErase = FLASH_TYPEERASE_SECTORS,
            .Banks = FLASH_BANK_1,
            .Sector = sector,
            .NbSectors = 1, // Стираем по одному сектору за раз
            .VoltageRange = FLASH_VOLTAGE_RANGE_3 // 2.7V - 3.6V (стандарт для 3.3V)
        };

        uint32_t sector_error = 0;
        if (HAL_FLASHEx_Erase(&erase_init, &sector_error) != HAL_OK) {
            HAL_FLASH_Lock();
            return -1;
        }

        current_addr += erase_len;
        remaining_len -= erase_len;
    }

    HAL_FLASH_Lock();
    return 0;
}

size_t flash_area_align(const struct flash_area *area) {
    (void)area;
    return 4; // Для STM32F4 минимальный шаг записи - 4 байта (WORD)
}

uint8_t flash_area_erased_val(const struct flash_area *area) {
    (void)area;
    return 0xFF;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors) {
    const struct flash_area *fa = NULL;
    if (flash_area_open(fa_id, &fa) != 0) {
        return -1;
    }

    uint32_t current_addr = fa->fa_off;
    uint32_t remaining_size = fa->fa_size;
    uint32_t sector_count = 0;

    while (remaining_size > 0 && sector_count < *count) {
        uint32_t sector_num = get_sector_number(current_addr);
        uint32_t sector_size = get_sector_size(sector_num);
        
        uint32_t current_sector_size = (remaining_size < sector_size) ? remaining_size : sector_size;

        sectors[sector_count].fs_off = current_addr - fa->fa_off;
        sectors[sector_count].fs_size = current_sector_size;

        current_addr += current_sector_size;
        remaining_size -= current_sector_size;
        sector_count++;
    }

    *count = sector_count;
    return 0;
}

int flash_area_id_from_multi_image_slot(int image_index, int slot) {
    if (image_index != 0) {
        return -1; // Поддерживаем только image 0
    }
    return flash_area_id_from_image_slot(slot);
}

int flash_area_id_from_image_slot(int slot) {
    switch (slot) {
        case 0: return FLASH_AREA_IMAGE_PRIMARY(0);
        case 1: return FLASH_AREA_IMAGE_SECONDARY(0);
        default: return -1;
    }
}