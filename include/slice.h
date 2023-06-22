#pragma once

#include <stddef.h>

/**
 * A slice of characters with a pointer to the beginning of the string and a length
 */
typedef struct slice {
    char *ptr;
    size_t len;
} slice_t;

/** @brief Create a new slice from a NULL-terminated string */
slice_t slice_from_str(const char *str);

static inline slice_t slice_sub(const slice_t slice, size_t begin, size_t len) { return (slice_t){ .ptr = slice.ptr + begin, .len = len }; }
static inline slice_t slice_sub_rest(const slice_t slice, size_t begin) { return (slice_t){ .ptr = slice.ptr + begin, .len = slice.len - begin }; }
