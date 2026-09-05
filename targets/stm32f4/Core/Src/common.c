#include "common.h"


size_t board_get_unique_id(uint8_t id[], size_t max_len) {
  (void) max_len;
  volatile uint32_t *stm32_uuid = (volatile uint32_t *) UID_BASE;
  uint32_t *id32 = (uint32_t *) (uintptr_t) id;
  uint8_t const len = 12;

  id32[0] = stm32_uuid[0];
  id32[1] = stm32_uuid[1];
  id32[2] = stm32_uuid[2];

  return len;
}