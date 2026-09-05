/**
  ******************************************************************************
  * @file    sram_diskio_config.h
  * @author  MCD Application Team
  * @brief   Template for the sram_diskio_config.h. This file should be copied and
             under project.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the st_license.txt
  * file in the root directory of this software component.
  * If no st_license.txt file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef SRAM_DISKIO_CONFIG_H
#define SRAM_DISKIO_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

//#include "stm32f4xx_hal.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Block size */
#ifdef MSC_USE_SRAM
	#define DISK_BLOCK_SIZE            	512
	#define DISK_BLOCK_NUM				      128
#elif defined(MSC_USE_FLASH)
	#define DISK_BLOCK_SIZE            	4096
	#define DISK_BLOCK_NUM				      256
#endif
#define TOTAL_DISK_SIZE				(DISK_BLOCK_SIZE*DISK_BLOCK_NUM)
#define SRAM_DISK_SIZE				TOTAL_DISK_SIZE


#ifdef __cplusplus
}
#endif

#endif /* SRAM_DISKIO_CONFIG_H */
