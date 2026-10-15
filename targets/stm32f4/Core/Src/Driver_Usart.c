#include "Driver_Usart.h"
#include "stm32f4xx.h"

extern UART_HandleTypeDef huart1;

int console_putc(char c) {
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, HAL_MAX_DELAY);
  return 1;
}

char console_getc(void) {
    uint8_t cr = -1;
    uint32_t timeout = 0x2FFFFF;
    while((timeout--) && (HAL_UART_Receive(&huart1, &cr, 1, 0) != HAL_OK));
    return (char)cr;
}