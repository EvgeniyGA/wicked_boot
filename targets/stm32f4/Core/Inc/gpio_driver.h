#ifndef _GPIO_DRIVER_H
#define _GPIO_DRIVER_H

#include "main.h"
#include <stdio.h>

typedef enum {
    GPIO_CHANNEL_OFF = 0,
    GPIO_CHANNEL_ON
}channel_state_e;

typedef enum {
    GPIO_CHANNEL_1a = 0,
    GPIO_CHANNEL_1b,
    GPIO_CHANNEL_2a,
    GPIO_CHANNEL_2b, 
    GPIO_CHANNEL_3a,
    GPIO_CHANNEL_3b,
    GPIO_CHANNEL_COUNT
}channel_number_e;

int8_t gpio_channel_change_state(uint8_t channel, uint8_t new_state);

#endif