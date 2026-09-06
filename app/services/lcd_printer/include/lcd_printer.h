#ifndef _LCD_PRINTER_INIT
#define _LCD_PRINTER_INIT

#include <stdint.h>

#define LCD_PRINTER_STACK_SIZE      configMINIMAL_STACK_SIZE * 2
#define LCD_MAX_LEN             (20)
#define LCD_PRINTER_BUF_LEN     (10)

typedef enum{
    LCD_PRINTER_LINE1 = 0,
    LCD_PRINTER_LINE2,
    LCD_PRINTER_LINE3,
    LCD_PRINTER_LINE4,
    LCD_PRINTER_LINES
}lcd_printer_lines_e;

typedef enum{
    LCD_PRINTER_OFFSET_ZERO = 0,
    LCD_PRINTER_OFFSET_HALF = 10 //check
}lcd_printer_offset_e;

typedef struct{
    char data[LCD_MAX_LEN];
    uint8_t data_len;
    uint8_t line;
    uint8_t offset;
}lcd_printer_msg_t;

__attribute__((format(printf, 3, 4)))
uint8_t lcd_print(uint8_t line, uint8_t offset, const char* format, ...);
void lcd_printer_init(void);

#endif