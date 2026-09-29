#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

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

// Структура для хранения настроек пинов
typedef struct {
    GPIO_TypeDef* RS_Port;
    uint16_t      RS_Pin;
    GPIO_TypeDef* EN_Port;
    uint16_t      EN_Pin;
    GPIO_TypeDef* D4_Port;
    uint16_t      D4_Pin;
    GPIO_TypeDef* D5_Port;
    uint16_t      D5_Pin;
    GPIO_TypeDef* D6_Port;
    uint16_t      D6_Pin;
    GPIO_TypeDef* D7_Port;
    uint16_t      D7_Pin;
    void (*delay_ms)(uint32_t);
} LCD_HandleTypeDef;

void LCD_Init(LCD_HandleTypeDef* lcd);
void LCD_SendCommand(LCD_HandleTypeDef* lcd, uint8_t command);
void LCD_SendData(LCD_HandleTypeDef* lcd, uint8_t data);
void LCD_SendString(LCD_HandleTypeDef* lcd, const char* str);
void LCD_SetCursor(LCD_HandleTypeDef* lcd, uint8_t row, uint8_t col);
void LCD_Clear(LCD_HandleTypeDef* lcd);

#endif