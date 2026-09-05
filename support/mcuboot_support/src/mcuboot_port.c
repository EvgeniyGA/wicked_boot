#include "mcuboot_config/mcuboot_config.h"

void example_assert_handler(const char *file, int line) {
    (void)file;
    (void)line;
    while (1) {
        // Hang
    }
}