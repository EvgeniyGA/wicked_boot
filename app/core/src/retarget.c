#include <stdio.h>
#include "app.h"
#include "SEGGER_RTT.h"
#ifdef STM32F746xx
#include "stm32f7xx.h"
#elif defined STM32F407xx
#include "stm32f4xx.h"
#endif

extern UART_HandleTypeDef huart1;

int _write(int file, char *ptr, int len) {
    (void)file;
#ifdef PRINTF_RTT
    int written = SEGGER_RTT_Write(0, ptr, len);
    return written;
#elif PRINTF_UART
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    if (status == HAL_OK) {
		return len; // Возвращаем количество успешно отправленных байт
	}
#endif
    return -1;
}
