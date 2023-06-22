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

/** @brief Julian J2000 timestamp */
static const double J2000 = 2451545.0;
static const double DAYS_PER_CENTURY = 36525.0;
static const double KM_TO_AU = 6.6845871226706E-9;

static const planet_t MERCURY = {
    .color = (color_t){0.608, 0.596, 0.631, 1.},
    .radius_km = 2439.7,
    .base =  {0.38709927,     0.20563593,     7.00497902,     252.25032350,    77.45779628,     48.33076593},
    .rates = {0.00000037,     0.00001906,     -0.00594749,    149472.67411175, 0.16047689,      -0.12534081}, 
};

static const planet_t VENUS = {
    .color = (color_t){0.784, 0.616, 0.384, 1.},
    .radius_km = 6051.8,
    .base =  {0.72333566,     0.00677672,      3.39467605,     181.97909950,        131.60246718,     76.67984255},
    .rates = {0.00000390,     -0.00004107,     -0.00078890,    58517.81538729,      0.00268329,       -0.27769418},
};

static const planet_t EARTH = {
    .color = (color_t){0.231, 0.541, 0.769, 1.},
    .radius_km = 6371,
    .base =  {1.00000261,     0.01671123,      -0.00001531,    100.46457166,        102.93768193,     0.0},
    .rates = {0.00000562,     -0.00004392,     -0.01294668,    35999.37244981,      0.32327364,       0.0},
};

static const planet_t MARS = {
    .color = (color_t){0.678, 0.384, 0.259, 1.},
    .radius_km = 3389.5,
    .base =  {1.52371034,     0.09339410,      1.84969142,     -4.55343205,         -23.94362959,     49.55953891},
    .rates = {0.00001847,     0.00007882,      -0.00813131,    19140.30268499,      0.44441088,       -0.29257343},
};
