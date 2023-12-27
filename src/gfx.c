#include "gfx.h"
#include "slice.h"

/** 
 * @brief Get the value of a single ASCII hexadecimal character 
 * @return A value between 0 and 16 (inclusive), or 255 if the digit is invalid
 */
static inline uint8_t hex_digit(char ch) {
    if(ch >= '0' && ch <= '9') { return ch - '0'; }
    else if(ch >= 'A' && ch <= 'F') { return ch - 'A' + 10; }
    else if(ch >= 'a' && ch <= 'f') { return ch - 'a' + 10; }
    else { return 0; }
}

static color_parse_err_t hex_color_part(slice_t txt, float *v, slice_t *problem) {
    uint8_t digit = hex_digit(txt.ptr[1]);
    if(digit > 16) { *problem = txt; return COLOR_ERR_INVALID_HEX_DIGIT; }

    uint8_t color_value = digit;
    digit = hex_digit(txt.ptr[0]);
    if(digit > 16) { *problem = txt; return COLOR_ERR_INVALID_HEX_DIGIT; }
    color_value |= digit << 4;

    *v = (float)color_value / 255.f;

    return COLOR_SUCCESS;
}

color_parse_err_t color_parse_hex(slice_t txt, color_t *const color, slice_t *problem) {
    if(txt.len == 0) { return COLOR_ERR_INVALID_LENGTH; }
    
    if(txt.ptr[0] == '#') {
        txt = slice_sub_rest(txt, 1);
    }

    if(txt.len != 6 && txt.len != 8) { return COLOR_ERR_INVALID_LENGTH; }

    color_parse_err_t rc;
    if((rc = hex_color_part(txt, &color->r, problem)) != COLOR_SUCCESS) { return rc; }
    txt = slice_sub_rest(txt, 2);
    if((rc = hex_color_part(txt, &color->g, problem)) != COLOR_SUCCESS) { return rc; }
    txt = slice_sub_rest(txt, 2);
    if((rc = hex_color_part(txt, &color->b, problem)) != COLOR_SUCCESS) { return rc; }
    
    if(txt.len > 2) {
        txt = slice_sub_rest(txt, 2);
        if((rc = hex_color_part(txt, &color->a, problem)) != COLOR_SUCCESS) { return rc; }
    } else {
        color->a = 1.f;
    }

    return COLOR_SUCCESS;
}

