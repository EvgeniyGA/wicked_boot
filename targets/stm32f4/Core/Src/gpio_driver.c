#include "gpio_driver.h"

int8_t gpio_channel_change_state(uint8_t channel, uint8_t new_state){
    uint8_t ret = 0;
    switch(channel){
        case GPIO_CHANNEL_2a: HAL_GPIO_WritePin (en_led2a_GPIO_Port, en_led2a_Pin, 
            (new_state == GPIO_CHANNEL_OFF) ? GPIO_PIN_RESET : GPIO_PIN_SET); ret = 0; break;
        default: ret = -1; break;
    }
    return ret;
}