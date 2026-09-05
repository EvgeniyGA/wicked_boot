/*
 * internal_flash_diskio.h
 *
 *  Created on: Jul 12, 2026
 *      Author: evgeny
 */

#ifndef INC_INTERNAL_FLASH_DISKIO_H_
#define INC_INTERNAL_FLASH_DISKIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "ff_gen_drv.h"

extern Diskio_drvTypeDef  USER_Driver;
DRESULT USER_read(BYTE lun, BYTE *buff, LBA_t sector, UINT count);
DRESULT USER_write(BYTE lun, const BYTE *buff, LBA_t sector, UINT count);

#ifdef __cplusplus
}
#endif

#endif /* INC_INTERNAL_FLASH_DISKIO_H_ */
