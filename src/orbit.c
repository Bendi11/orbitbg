#include "planet.h"
#include "consts.h"
#include "mmath.h"

static const double SECONDS_PER_DAY = 86400.;
static const double UNIX_EPOCH_JULIAN = 2440588.;

double julian_time(time_t time) {
    double days = (double)time / SECONDS_PER_DAY;
    return UNIX_EPOCH_JULIAN + days;
}

orbit_params_t orbit_params_time(planet_t const *planet, double time_julian) {
    double centuries = (time_julian - J2000) / DAYS_PER_CENTURY;
    return (orbit_params_t){
        .semi_major_axis_au = planet->base.semi_major_axis_au + planet->rates.semi_major_axis_au * centuries,
        .eccentricity = planet->base.eccentricity + planet->rates.eccentricity * centuries,
        .long_perhelion_deg = planet->base.long_perhelion_deg + planet->rates.long_perhelion_deg * centuries,
        .long_ascending_deg = planet->base.long_ascending_deg + planet->rates.long_ascending_deg * centuries,
        .mean_long_deg = planet->base.mean_long_deg + planet->rates.mean_long_deg * centuries,
        .inclination_rad = planet->base.inclination_rad + planet->rates.inclination_rad * centuries,
    };
}

point_t orbit_position(orbit_params_t const orbit) {
    double mean_anomaly = orbit.mean_long_deg - orbit.long_perhelion_deg;
    mean_anomaly = fmod(mean_anomaly, 360.f);
    
    double estar = RADEG * orbit.eccentricity;
    double eccentric_anomaly = mean_anomaly - estar * sind(mean_anomaly);
    for(unsigned i = 0; i < 50; ++i) {
        double dm = mean_anomaly - (eccentric_anomaly - estar * sind(eccentric_anomaly));
        eccentric_anomaly += dm / (1. - orbit.eccentricity * cosd(eccentric_anomaly));
    }

    double x = orbit.semi_major_axis_au * (cosd(eccentric_anomaly) - orbit.eccentricity);
    double y = ((orbit.semi_major_axis_au * sqrt(1. - orbit.eccentricity * orbit.eccentricity))) * sind(eccentric_anomaly);

    double c_w = cosd(orbit.long_perhelion_deg);
    double s_w = sind(orbit.long_perhelion_deg);
    double c_o = cosd(orbit.long_ascending_deg);
    double s_o = sind(orbit.long_ascending_deg);
    double c_i = cosd(orbit.inclination_rad);
    double s_i = sind(orbit.inclination_rad);

    double x_ecl = 
        (c_w*c_o - s_w*s_o*c_i) * x +
        (-s_w*c_o- c_w*s_o*c_i) * y;

    double y_ecl =
        (c_w*s_o+s_w*c_o*c_i) * x +
        (-s_w*s_o + c_w*c_o*c_i) * y;

    double z_ecl = (s_w * s_i) * x + (c_w * s_i) * y;

    return (point_t) {
        .x = cosd(-orbit.long_ascending_deg) * x_ecl - sind(-orbit.long_ascending_deg) * y_ecl,
        .y = sind(-orbit.long_ascending_deg) * x_ecl + cosd(-orbit.long_ascending_deg) * y_ecl,
    };
}
