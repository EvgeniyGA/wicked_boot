#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"
#include <string.h>
#include <stddef.h>

#include "bootutil_priv.h"
#include "bootutil/bootutil_log.h"
#include "Driver_Flash.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

extern ARM_DRIVER_FLASH Driver_FLASH0;

static const struct flash_area flash_map[] = {
    {
        .fa_id = FLASH_AREA_BOOTLOADER,
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off = 0x00000000,
        .fa_size = 0x00020000,
        .fa_driver = &Driver_FLASH0
    },
    {
        .fa_id = FLASH_AREA_IMAGE_PRIMARY(0),
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off = 0x00040000,
        .fa_size = 0x00040000,
        .fa_driver = &Driver_FLASH0
    },
    {
        .fa_id = FLASH_AREA_IMAGE_SECONDARY(0),
        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH,
        .fa_off = 0x00080000,
        .fa_size = 0x00040000,
        .fa_driver = &Driver_FLASH0
    }
};

static const int flash_map_entry_num = ARRAY_SIZE(flash_map);

/*
 * Check the target address in the flash_area_xxx operation.
 */
static bool is_range_valid(const struct flash_area *area,
                           uint32_t off,
                           uint32_t len)
{
    uint32_t size;

    if (!area) {
        return false;
    }

    if (!boot_u32_safe_add(&size, off, len)) {
        return false;
    }

    if (area->fa_size < size) {
        return false;
    }

    return true;
}

/*
 * `open` a flash area.  The `area` in this case is not the individual
 * sectors, but describes the particular flash area in question.
 */
int flash_area_open(uint8_t id, const struct flash_area **area)
{
    int i;

    BOOT_LOG_DBG("area %d", id);

    for (i = 0; i < flash_map_entry_num; i++) {
        if (id == flash_map[i].fa_id) {
            break;
        }
    }
    if (i == flash_map_entry_num) {
        return -1;
    }

    *area = &flash_map[i];
    return 0;
}

void flash_area_close(const struct flash_area *area)
{
    /* Nothing to do. */
}

int flash_area_read(const struct flash_area *area, uint32_t off, void *dst,
                    uint32_t len)
{
    BOOT_LOG_DBG("read area=%d, off=%#x, len=%#x", area->fa_id, off, len);

    if (!is_range_valid(area, off, len)) {
        return -1;
    }

    return DRV_FLASH_AREA(area)->ReadData(area->fa_off + off, dst, len);
}

int flash_area_write(const struct flash_area *area, uint32_t off,
                     const void *src, uint32_t len)
{
    BOOT_LOG_DBG("write area=%d, off=%#x, len=%#x", area->fa_id, off, len);

    if (!is_range_valid(area, off, len)) {
        return -1;
    }

    return DRV_FLASH_AREA(area)->ProgramData(area->fa_off + off, src, len);
}

int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len)
{
    ARM_FLASH_INFO *flash_info;
    uint32_t deleted_len = 0;
    int32_t rc = 0;

    BOOT_LOG_DBG("erase area=%d, off=%#x, len=%#x", area->fa_id, off, len);

    if (!is_range_valid(area, off, len)) {
        return -1;
    }

    flash_info = DRV_FLASH_AREA(area)->GetInfo();

    if (flash_info->sector_info == NULL) {
        /* Uniform sector layout */
        while (deleted_len < len) {
            rc = DRV_FLASH_AREA(area)->EraseSector(area->fa_off + off);
            if (rc != 0) {
                break;
            }
            deleted_len += flash_info->sector_size;
            off         += flash_info->sector_size;
        }
    } else {
        /* Inhomogeneous sector layout, explicitly defined
         * Currently not supported.
         */
    }

    return rc;
}

uint32_t flash_area_align(const struct flash_area *area)
{
    ARM_FLASH_INFO *flash_info;

    flash_info = DRV_FLASH_AREA(area)->GetInfo();
    return flash_info->program_unit;
}
