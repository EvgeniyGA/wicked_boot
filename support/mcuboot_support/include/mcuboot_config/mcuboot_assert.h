#ifndef _MCUBOOT_ASSERT_H_
#define _MCUBOOT_ASSERT_H_
//#include "hal/logging.h"

extern void example_assert_handler(const char *file, int line);

#define assert(exp) \
  do {                                                  \
    if (!(exp)) {                                       \
      example_assert_handler(__FILE__, __LINE__);       \
    }                                                   \
  } while (0)

  #endif