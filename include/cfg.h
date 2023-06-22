#pragma once

#include "gfx.h"

typedef struct planet_cfg {
    color_t color;
} planet_cfg_t;

/** Rendering configuration values */
typedef struct cfg {
    double sun_scale;
    color_t sun_color;
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
    /** Primary color that the sun emits according to its spectral class */
    .sun_color = (color_t){1., 0.957, 0.918, 1.},
    .planet_scale = 1E+3,
    .background_color = (color_t){0.071, 0.071, 0.078, 1.},
    .orbit_line_color = (color_t){1., 1., 1., 0.14},
    .orbit_line_width = 0.005,
    .mercury = {
        .color = (color_t){0.608, 0.596, 0.631, 1.},
    },
    .venus = {
        .color = (color_t){0.784, 0.616, 0.384, 1.},
    },
    .earth = {
        .color = (color_t){0.231, 0.541, 0.769, 1.},
    },
    .mars = {
        .color = (color_t){0.678, 0.384, 0.259, 1.},
    }
};
