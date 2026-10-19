#ifndef DRIVER_USART_H_
#define DRIVER_USART_H_

#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif

#define CONSOLE_GETCH_TIMEOUT   0x1FFFFFUL

int console_putc(char c);
char console_getc(uint32_t timeout);

#ifdef  __cplusplus
}
#endif

#endif /* DRIVER_USART_H_ */