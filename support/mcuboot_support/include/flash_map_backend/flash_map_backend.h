/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 * Copyright (c) 2020 Arm Limited.
 * Copyright (c) 2023 STMicroelectronics.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/mcu-tools/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 */

#ifndef __FLASH_MAP_BACKEND_H__
#define __FLASH_MAP_BACKEND_H__

#include <stdint.h>
#include <stddef.h>

struct flash_area {
    uint8_t  fa_id;         /** The slot/scratch identification */
    uint8_t  fa_device_id;  /** The device id (usually there's only one) */
    uint16_t pad16;
    uint32_t fa_off;        /** The flash offset from the beginning */
    uint32_t fa_size;       /** The size of this sector */
};

struct flash_sector {
    uint32_t fs_off;
    uint32_t fs_size;
};

int flash_area_open(uint8_t id, const struct flash_area **area_outp);
void flash_area_close(const struct flash_area *fa);
int flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len);
int flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len);
int flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len);
size_t flash_area_align(const struct flash_area *area);
uint8_t flash_area_erased_val(const struct flash_area *area);
int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors);
int flash_area_id_from_multi_image_slot(int image_index, int slot);
int flash_area_id_from_image_slot(int slot);

#endif /* __FLASH_MAP_BACKEND_H__ */
