#include "cfg.h"
#include "gfx.h"
#include "slice.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cfg CONFIG = {};

static color_t hex(char const *);

struct cfg default_config() {
    struct cfg conf = {
        .sun_scale = 40.,
        /** Primary color that the sun emits according to its spectral class */
        .sun_color = (color_t){1., 0.957, 0.918, 1.},
        .sun_pos = (point_t){0.5, 0.52},
        .planet_scale = 1E+3,
        .background_color = (color_t){0.071, 0.071, 0.078, 1.},
        .orbit_line_color = (color_t){1., 1., 1., 0.14},
        .orbit_line_width = 0.005,
    };

    const color_t night[4] = {
        hex("#242424"),
        hex("#1B1B1C"),
        hex("#121214"),
        hex("#09090B"),
    },
    mercury[] = {
        hex("#9b98a1"),
        hex("#8F8B96"),
        hex("#898492"),
        hex("#837C8E"),
    },
    mars[] = {
        hex("#AD6242"),
        hex("#964632"),
        hex("#8A382A"),
        hex("#7E2A22"),
    },
    venus[] = {
        hex("#C49656"),
        hex("#BF8E4A"),
        hex("#B48349"),
        hex("#A87748"),
    },
    earth[] = {
        hex("#3793B2"),
        hex("#3689B3"),
        hex("#2F71A1"),
        hex("#23577D"),
    },
    sol[] = {
        hex("#fff4ea"),
        hex("#FFEDDB"),
        hex("#FFE8D1"),
        hex("#FFE4C7"),
    };

    conf.sun_color = sol[0];
    conf.orbit_line_color = night[1];
    conf.background_color = night[2];
    memcpy(&conf.mercury.palette[0], mercury, 4 * sizeof(color_t));
    memcpy(&conf.venus.palette[0],   venus,   4 * sizeof(color_t));
    memcpy(&conf.earth.palette[0],   earth,   4 * sizeof(color_t));
    memcpy(&conf.mars.palette[0] ,   mars,    4 * sizeof(color_t));

    return conf;
}


/** Parse a hex color from a constant string, only to be used with hex strings known to be valid */
static color_t hex(const char *str) {
    color_t color;
    slice_t problem;
    if(color_parse_hex((slice_t){.ptr = (char*)str, .len = strlen(str)}, &color, &problem) != COLOR_SUCCESS) {
        fprintf(stderr, "Failed to parse color %s: at part %S", str, problem.ptr, problem.len);
        exit(-1);
    }

    return color;
}
