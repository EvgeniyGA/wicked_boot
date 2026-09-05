#include "lcd_driver.h"
#include "main.h"//todo

static void LCD_SendNibble(LCD_HandleTypeDef* lcd, uint8_t nibble) {
    HAL_GPIO_WritePin(lcd->D4_Port, lcd->D4_Pin, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd->D5_Port, lcd->D5_Pin, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd->D6_Port, lcd->D6_Pin, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(lcd->D7_Port, lcd->D7_Pin, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    HAL_GPIO_WritePin(lcd->EN_Port, lcd->EN_Pin, GPIO_PIN_SET);
    lcd->delay_ms(1);
    HAL_GPIO_WritePin(lcd->EN_Port, lcd->EN_Pin, GPIO_PIN_RESET);
}

// Отправка байта (в 4-битном режиме отправляется двумя полубайтами)
static void LCD_SendByte(LCD_HandleTypeDef* lcd, uint8_t byte) {
    LCD_SendNibble(lcd, byte >> 4);       // Старший полубайт
    LCD_SendNibble(lcd, byte & 0x0F);     // Младший полубайт
}

void LCD_SendCommand(LCD_HandleTypeDef* lcd, uint8_t command) {
    lcd->delay_ms(1);
    HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, GPIO_PIN_RESET); // RS = 0 (команда)
    lcd->delay_ms(1);
    LCD_SendByte(lcd, command);
    lcd->delay_ms(1);
}

void LCD_SendData(LCD_HandleTypeDef* lcd, uint8_t data) {
    lcd->delay_ms(1);
    HAL_GPIO_WritePin(lcd->RS_Port, lcd->RS_Pin, GPIO_PIN_SET);   // RS = 1 (данные)
    lcd->delay_ms(1);
    LCD_SendByte(lcd, data);
    lcd->delay_ms(1);
}

void LCD_Init(LCD_HandleTypeDef* lcd) {
    lcd->RS_Port = disp_a0_GPIO_Port; 
    lcd->RS_Pin = disp_a0_Pin;
    lcd->EN_Port = disp_e_GPIO_Port; 
    lcd->EN_Pin = disp_e_Pin;
    lcd->D4_Port = disp_d4_GPIO_Port; 
    lcd->D4_Pin = disp_d4_Pin;
    lcd->D5_Port = disp_d5_GPIO_Port; 
    lcd->D5_Pin = disp_d5_Pin;
    lcd->D6_Port = disp_d6_GPIO_Port; 
    lcd->D6_Pin = disp_d6_Pin;
    lcd->D7_Port = disp_d7_GPIO_Port; 
    lcd->D7_Pin = disp_d7_Pin;

    lcd->delay_ms(2000);
    
    LCD_SendNibble(lcd, 0x03);
    lcd->delay_ms(1);
    
    LCD_SendNibble(lcd, 0x03);
    lcd->delay_ms(1);
    
    LCD_SendNibble(lcd, 0x03);
    lcd->delay_ms(1);
    
    LCD_SendNibble(lcd, 0x02);
    lcd->delay_ms(1);
    
    LCD_SendCommand(lcd, 0x2A);
    LCD_SendCommand(lcd, 0x0C);
    LCD_SendCommand(lcd, 0x01);
    LCD_SendCommand(lcd, 0x06);

    LCD_Clear(lcd);
}

void LCD_Clear(LCD_HandleTypeDef* lcd) {
    LCD_SendCommand(lcd, 0x01);
}

// Установка курсора.
// row: 0..3 (строки 1..4)
// col: 0..19 (символы 1..20)
void LCD_SetCursor(LCD_HandleTypeDef* lcd, uint8_t row, uint8_t col) {
    uint8_t address = 0;
    // Адресация DDRAM для 20x4 дисплеев (стандарт HD44780)
    switch (row) {
        case 0: address = 0x00; break;
        case 1: address = 0x40; break;
        case 2: address = 0x14; break;
        case 3: address = 0x54; break;
        default: address = 0x00; break;
    }
    address += col;
    // Бит 7 (0x80) устанавливается в 1 для команды Set DDRAM Address
    LCD_SendCommand(lcd, 0x80 | address);
}

void LCD_SendString(LCD_HandleTypeDef* lcd, const char* str) {
    while (*str) {
        LCD_SendData(lcd, (uint8_t)(*str));
        str++;
    }
}