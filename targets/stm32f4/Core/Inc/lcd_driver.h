#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

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