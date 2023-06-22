#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "slice.h"

/** RGBA color collection */
typedef struct color {
    float r;
    float g;
    float b;
    float a;
} color_t;

typedef enum color_parse_err {
    COLOR_SUCCESS = 0,
    COLOR_ERR_INVALID_LENGTH,
    COLOR_ERR_INVALID_HEX_DIGIT
} color_parse_err_t;

/**
 * @brief Parse color from a hexadecimal string into a color structure
 * @param txt A string containing six or eight hexadecimal digits that may optionally be preceded by a '#' character
 * @param problem A pointer to a problem slice of the source `txt` to be set on error, may be NULL
 * @param color A pointer to the color structure to fill data into
 * @return `false` if no error occured in parsing
 */
color_parse_err_t color_parse_hex(slice_t txt, color_t *const color, slice_t *problem);

/** @brief Get a NULL-terimated character string for the given color parsing error */
char const* color_parse_err_str(color_parse_err_t err);
