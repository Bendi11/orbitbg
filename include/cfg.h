#pragma once

#include "gfx.h"

typedef struct planet_cfg {
    color_t color;
} planet_cfg_t;

/** Rendering configuration values */
typedef struct cfg {
    double sun_scale;
    double planet_scale;
    color_t background_color;
    color_t orbit_line_color;
    double orbit_line_width;
    planet_cfg_t mercury, venus, earth, mars;
} cfg_t;

static const double SUN_SCALE = 40.;
static const double PLANET_SCALE = 1E+3;

extern struct cfg CONFIG;
static const struct cfg DEFAULT_CONFIG = {
    .sun_scale = 40.,
    .planet_scale = 1E+3,
    .background_color = (color_t){0.071, 0.071, 0.078, 1.},
    .orbit_line_color = (color_t){1., 1., 1., 0.14},
    .orbit_line_width = 0.005,
};
