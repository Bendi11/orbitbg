#pragma once

#include "slice.h"


typedef struct color {
    float r;
    float g;
    float b;
    float a;
} color_t;

void color_parse_hex(const slice_t txt);
