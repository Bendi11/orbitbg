#pragma once

#include <stddef.h>

/**
 * A slice of characters with a pointer to the beginning of the string and a length
 */
typedef struct slice {
    char *ptr;
    size_t len;
} slice_t;

slice_t slice_new(const char *str);
