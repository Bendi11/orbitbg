#pragma once

#include "gfx.h"

/** Parameters used to calculate a planet's orbit */
typedef struct orbit_params {
    // Length of the semi-major axis in au
    double semi_major_axis;
    // Eccentricity of the planet's orbit (unitless)
    double eccentricity;
    // Inclination of the orbit in radians
    double inclination;
    // Mean longitude in degrees
    double mean_long;
    // Longitude of the periapsis in degrees
    double long_perhelion;
    // Longitude of the ascending node in degrees
    double long_ascending;
} orbit_params_t;

/** Primary container with all data to calculate position and render a planet */
typedef struct planet {
    // color of the planet in 
    color_t color;
    // Radius in km
    double radius;
    // Base orbital parameters
    orbit_params_t base;
    // Rate of change per century since J2000 of orbital parameters
    orbit_params_t rates;
} planet_t;
