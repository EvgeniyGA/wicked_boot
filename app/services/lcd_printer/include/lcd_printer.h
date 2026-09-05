#ifndef _LCD_PRINTER_INIT
#define _LCD_PRINTER_INIT

#include <stdint.h>

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

__attribute__((format(printf, 3, 4)))
uint8_t lcd_print(uint8_t line, uint8_t offset, const char* format, ...);
void lcd_printer_init(void);

#endif