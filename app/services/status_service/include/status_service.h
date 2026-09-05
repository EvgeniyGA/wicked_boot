#include <stdint.h>

typedef void(*status_heartbit_t)(void);

uint8_t status_service_init(status_heartbit_t);