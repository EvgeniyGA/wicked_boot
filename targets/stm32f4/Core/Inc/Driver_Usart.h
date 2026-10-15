#ifndef DRIVER_USART_H_
#define DRIVER_USART_H_

#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif

int console_putc(char c);
char console_getc(void);

#ifdef  __cplusplus
}
#endif

#endif /* DRIVER_USART_H_ */