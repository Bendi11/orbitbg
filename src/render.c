#include <cairo.h>

#include "planet.h"
#include "render.h"

void planet_render(cairo_t *cr, const planet_t *planet, orbit_params_t orbit) {
    cairo_save(cr);
        translate_to_planet_orbit(cr, orbit);
        cairo_planet_circle(cr, planet);
    cairo_restore(cr);
}

void cairo_planet_circle(cairo_t *cr, planet_t const *planet) {
    double radius = planet->radius_km * KM_TO_AU * 1E+3;
    cairo_set_source_rgba(cr, planet->color.r, planet->color.g, planet->color.b, planet->color.a);

    cairo_save(cr);
        cairo_arc(cr, 0., 0., radius, 0., M_PI * 2.);
        cairo_fill(cr);
        cairo_set_source_rgba(cr, 0., 0., 0., 0.5);
        cairo_arc(cr, 0., 0., radius, M_PI, M_PI * 2.);    
        cairo_fill(cr);
    cairo_restore(cr);
    
    cairo_save(cr);
        cairo_arc(cr, 0., 0., radius, 0., M_PI);
        cairo_fill(cr);
        cairo_scale(cr, 1., 0.5);
        cairo_arc_negative(cr, 0., 0., radius, 0., M_PI);
        cairo_fill(cr);
    cairo_restore(cr);
}

void translate_to_planet_orbit(cairo_t *cr, orbit_params_t orbit) {
    point_t pos = orbit_position(orbit);
    double angle = M_PI - atan2(pos.y, pos.x);
    
    cairo_translate(cr, pos.x, pos.y);
    cairo_rotate(cr, -(angle + M_PI_2));
}

void planet_draw_orbit(cairo_t *cr, orbit_params_t orbit) {
    double semi_minor_axis = orbit.semi_major_axis_au * sqrt(1. - orbit.eccentricity * orbit.eccentricity);
    double focus = (orbit.semi_major_axis_au * orbit.eccentricity);
    

    cairo_save(cr);
        cairo_rotate(cr, orbit.long_perhelion_deg * DEGRAD);
        cairo_translate(cr, -focus, 0.);
        
        cairo_set_line_width(cr, 0.005);
        cairo_move_to(cr, 0., 0.);
        cairo_line_to(cr, semi_minor_axis, 0.);

        cairo_scale(cr, orbit.semi_major_axis_au, semi_minor_axis);
        cairo_arc(cr, 0., 0., 1., 0., M_PI * 2. - 0.05);
    cairo_restore(cr);
    
    cairo_save(cr);
        cairo_set_source_rgba(cr, 1., 1., 1., 0.14);
        cairo_set_line_width(cr, .005);
        cairo_stroke(cr);
    cairo_restore(cr);
}
