/*
 * wrapper.c
 *
 *  Created on: Jul 25, 2026
 *      Author: evgeny
 */
#include <stdio.h>
#include <stdarg.h>
#ifdef STM32F746xx
#include "stm32f7xx.h"
#elif defined STM32F407xx
#include "stm32f4xx.h"
#endif

int __real_printf(const char *fmt, ...);

int __wrap_printf(const char *fmt, ...) {
  unsigned int t = HAL_GetTick();
  va_list args;
  int count0, count1;
  count0 = __real_printf(">>%d.%03d: ", t/1000, t%1000);
  va_start(args, fmt);
  count1 = vprintf(fmt, args);
  va_end(args);
  return count0+count1;
}

__weak HAL_StatusTypeDef __wrap_HAL_RCC_OscConfig(const RCC_OscInitTypeDef  *RCC_OscInitStruct){
  return HAL_OK;
}

HAL_StatusTypeDef __wrap_HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency){
  return HAL_OK;
}

void __wrap_MX_USB_OTG_FS_PCD_Init(void){
  return;
}
