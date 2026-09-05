
#ifndef _CONFIG_SERVICE_H
#define _CONFIG_SERVICE_H

#include <stdint.h>

uint8_t config_service_init(void);

uint8_t config_read(char* name, uint8_t* data, uint16_t datalen);

#endif