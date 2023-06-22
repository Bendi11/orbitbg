#pragma once

#include "gfx.h"
#include "mmath.h"
#include <time.h>

/** Parameters used to calculate a planet's orbit */
typedef struct orbit_params {
    // Length of the semi-major axis in au
    double semi_major_axis_au;
    // Eccentricity of the planet's orbit (unitless)
    double eccentricity;
    // Inclination of the orbit in radians
    double inclination_rad;
    // Mean longitude in degrees
    double mean_long_deg;
    // Longitude of the periapsis in degrees
    double long_perhelion_deg;
    // Longitude of the ascending node in degrees
    double long_ascending_deg;
} orbit_params_t;

/** Primary container with all data to calculate position and render a planet */
typedef struct planet {
    // color of the planet
    color_t color;
    // Radius of the planet in km
    double radius_km;
    // Base orbital parameters
    orbit_params_t base;
    // Rate of change per century since J2000 of orbital parameters
    orbit_params_t rates;
} planet_t;

/** @brief Calculate the julian timestamp for the given POSIX timestamp */
double julian_time(time_t time);

orbit_params_t orbit_params_time(planet_t const* planet, double julian_time);

/** @brief Calculate the given planet's position in the plane of the ecliptic in au */
point_t orbit_position(orbit_params_t const params);
