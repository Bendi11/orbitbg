#pragma once

#include "cfg.h"
#include "planet.h"
#include <cairo.h>

/** Translate the cairo context to the given planet's orbital position and render a shaded circle for the planet */
void planet_draw(cairo_t *cr, planet_t const *planet, planet_cfg_t const *rendering, orbit_params_t orbit);

/** Render the given planet as a shaded circle via the given cairo context */
void planet_draw_circle(cairo_t *cr, planet_t const *planet, planet_cfg_t const *rendering);

/** @brief Translate the given cairo context to the planet's orbital position in au, rotating the positive Y axis to face the sun */
void planet_transform_to_orbit_pos(cairo_t *cr, orbit_params_t orbit);

/** @brief Render an ellipse representing the given orbit onto the given cairo context */
void planet_draw_orbit(cairo_t *cr, orbit_params_t orbit);
