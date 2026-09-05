#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "version_check.h"

bool is_hash_invalid(const char* hash) {
    if (hash == NULL) {
        return true;
    }

    size_t len = strlen(hash);
    if (len < 4) {
        return true;
    }

/*    for (size_t i = 0; i < len; i++) {
        char c = hash[i];
        if (!isxdigit((unsigned char)c) && c != '-') {
            return true;
        }
    }*/
    return false;
}