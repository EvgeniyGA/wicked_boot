#ifdef MSC_USE_FLASH
/* Includes ------------------------------------------------------------------*/
#include <flash_diskio.h>
#include <string.h>
#include "ff_gen_drv.h"
#include "diskio_config.h"
#include "W25Qxx.h"
#ifdef STM32F746xx
#include "stm32f7xx.h"
#elif defined STM32F407xx
#include "stm32f4xx.h"
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;
/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize (BYTE pdrv);
DSTATUS USER_status (BYTE pdrv);
DRESULT USER_ioctl (BYTE pdrv, BYTE cmd, void *buff);

Diskio_drvTypeDef  USER_Driver =
{
  USER_initialize,
  USER_status,
  USER_read,
  USER_write,
  USER_ioctl,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize (
	BYTE pdrv           /* Physical drive nmuber to identify the drive */
)
{
    Stat = STA_NOINIT;
    Stat = 0;
    return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status (
	BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    Stat = STA_NOINIT;
    if (pdrv == 0){
		return RES_OK;
	}
    return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read (
	BYTE pdrv,      /* Physical drive nmuber to identify the drive */
	BYTE *buff,     /* Data buffer to store read data */
	DWORD sector,   /* Sector address in LBA */
	UINT count      /* Number of sectors to read */
)
{
	W25Q_ReadBytes(sector*4096, buff, count*4096);
    return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_write (
	BYTE pdrv,          /* Physical drive nmuber to identify the drive */
	const BYTE *buff,   /* Data to be written */
	DWORD sector,       /* Sector address in LBA */
	UINT count          /* Number of sectors to write */
)
{
	for(uint32_t b = 0; b < count; b++)
	{
		W25Q_EraseSector(sector + b);
		for(uint32_t i = 0; i < DISK_BLOCK_SIZE; i += 256){
			W25Q_WriteBytes((sector + b) * DISK_BLOCK_SIZE + i, (uint8_t *)&buff[b*DISK_BLOCK_SIZE + i], 256);
		}
	}
	if (sector%100 == 0){
	  //printf("WRITE: ADDR=%lu LEN=%u\r\n", blk_addr, blk_len);
	}
    return RES_OK;
}

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
DRESULT USER_ioctl (
	BYTE pdrv,      /* Physical drive nmuber (0..) */
	BYTE cmd,       /* Control code */
	void *buff      /* Buffer to send/receive control data */
)
{
    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *(uint32_t*)buff = DISK_BLOCK_NUM;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(uint16_t*)buff = DISK_BLOCK_SIZE;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *(uint32_t*)buff = 1;
            return RES_OK;
        case CTRL_TRIM:
            return RES_OK;
    }
    return RES_PARERR;
}
#endif


