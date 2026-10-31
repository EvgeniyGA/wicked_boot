#include "Driver_Usart.h"
#include "stm32f4xx.h"

extern UART_HandleTypeDef huart1;

int console_putc(char c) {
#ifdef PRINTF_RTT
  int written = SEGGER_RTT_Write(0, (uint8_t*)&c, 1);
  return written;
#elif PRINTF_UART
  HAL_StatusTypeDef status = HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, HAL_MAX_DELAY);
  return 1;
#endif
}

char console_getc(uint32_t timeout) {
    uint8_t cr = -1;
    while((HAL_UART_Receive(&huart1, &cr, 1, 0) != HAL_OK) && timeout){
      timeout--;
    }
    return (char)cr;
}
