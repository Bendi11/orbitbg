#pragma once

#include "gfx.h"
#include "mmath.h"

/** Theming data for a planet */
typedef struct planet_cfg {
    /** Color palette in descending brightness order */
    color_t palette[4];
} planet_cfg_t;

/** Rendering configuration values */
typedef struct cfg {
    double sun_scale;
    color_t sun_color;
    point_t sun_pos;
    double planet_scale;
    color_t background_color;
    color_t orbit_line_color;
    double orbit_line_width;
    planet_cfg_t mercury, venus, earth, mars;
} cfg_t;

static const double SUN_SCALE = 40.;
static const double PLANET_SCALE = 1E+3;

/// Global configuration for rendering
extern struct cfg CONFIG;


/// Get the default configuration for planet color and size
struct cfg default_config();

