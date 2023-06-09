#include <X11/X.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xlib.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERR_BUF_LEN (1024)

int error_handler(Display *dsp, XErrorEvent *ev) {
    static char errbuf[ERR_BUF_LEN];
    XGetErrorText(dsp, ev->error_code, errbuf, ERR_BUF_LEN);
    fputs("X11 error: ", stderr);
    fputs(errbuf, stderr);

    XCloseDisplay(dsp);
    return -1;
}

enum {
    P_MERCURY,
    P_VENUS,
    P_EARTH,
    P_MARS
};

typedef uint8_t planet_kind_t;

typedef struct orbit_params {
    planet_kind_t planet;
    double semi_major_axis;
    double eccentricity;
    double inclination;
    double mean_long;
    double long_perhelion;
    double long_ascending;
} orbit_params_t;


static const double J2000 = 2451545.0;

static const orbit_params_t MERCURY = {P_MERCURY, 0.38709927,     0.20563593,     7.00497902,     252.25032350,    77.45779628,     48.33076593};
static const orbit_params_t VENUS =   {P_VENUS,   0.72333566,     0.00677672,     3.39467605,     181.97909950,   131.60246718,     76.67984255};
static const orbit_params_t EARTH =   {P_EARTH,   1.00000261,     0.01671123,    -0.00001531,     100.46457166,   102.93768193,     0.0};
static const orbit_params_t MARS =    {P_MARS,    1.52371034,     0.09339410,     1.84969142,     -4.55343205,   -23.94362959,     49.55953891};

static const orbit_params_t MERCURY_R = {P_MERCURY, 0.00000037,      0.00001906,     -0.00594749,   149472.67411175,      0.16047689,     -0.12534081};
static const orbit_params_t VENUS_R =   {P_VENUS,   0.00000390,     -0.00004107,     -0.00078890,    58517.81538729,      0.00268329,     -0.27769418};
static const orbit_params_t EARTH_R =   {P_EARTH,   0.00000562,     -0.00004392,     -0.01294668,    35999.37244981,      0.32327364,      0.0};
static const orbit_params_t MARS_R =    {P_MARS,    0.00001847,      0.00007882,     -0.00813131,    19140.30268499,      0.44441088,     -0.29257343};

#define DEGRAD (M_PI / 180.)
#define RADEG (180. / M_PI)

double cosd(double angle){
    double angleradians = angle * DEGRAD;
    return cos(angleradians);
}

double acosd(double v) { return acos(v) * RADEG; }

double sind(double angle){
    double angleradians = angle * DEGRAD;
    return sin(angleradians);
}

double tand(double angle) {
    return sin(angle * DEGRAD);
}

double atand(double angle) {
    return atan(angle) * RADEG;
}


orbit_params_t apply_time(orbit_params_t orbit, orbit_params_t rates, double time) {
    double centuries = (time - J2000) / 36525.;
    return (orbit_params_t){
        .planet = orbit.planet,
        .semi_major_axis = orbit.semi_major_axis + rates.semi_major_axis * centuries,
        .eccentricity = orbit.eccentricity + rates.eccentricity * centuries,
        .long_perhelion = orbit.long_perhelion + rates.long_perhelion * centuries,
        .long_ascending = orbit.long_ascending + rates.long_ascending * centuries,
        .mean_long = orbit.mean_long + rates.mean_long * centuries,
        .inclination = orbit.inclination + rates.inclination * centuries,
    };
}

void cairo_planet_circle(cairo_t *cr, planet_kind_t planet) {
    double radius;

    switch(planet) {
        case P_EARTH: {
            cairo_set_source_rgba(cr, 0.463, 0.714, 0.769, 1.);
            radius = 4.26352e-2;
        } break;

        case P_MERCURY: {
            cairo_set_source_rgba(cr, 0.608, 0.596, 0.631, 1.);
            radius = 1.63104e-2;
        } break;

        case P_VENUS: {
            cairo_set_source_rgba(cr, 0.784, 0.616, 0.384, 1.);
            radius = 4.045378435e-2;
        } break;

        case P_MARS: {
            cairo_set_source_rgba(cr, 0.949, 0.471, 0.263, 1.);
            radius = 2.26608e-2;
        } break;

        default: {
            cairo_set_source_rgba(cr, 1., 1., 1., 1.);
            radius = 0.03;
        } break;
    }


    //cairo_arc(cr, 0., 0., radius, 0., M_PI * 2.);

    cairo_set_source_rgba(cr, 0., 0., 0., 1.);
    double a1 = 20. * DEGRAD;
    double a2 = 160. * DEGRAD;
        
    double y = radius * sin(a1);
    double x = radius * cos(a1);
    
    cairo_save(cr);

        cairo_set_line_width(cr, 0.03);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
        cairo_arc_negative(cr, 0., 0., radius, a1, a2);
        //cairo_stroke(cr);

        cairo_save(cr);
            cairo_scale(cr, 1., 0.401);
            cairo_arc_negative(cr, 0., y / 0.4, fabs(x), 0., M_PI);
        cairo_restore(cr);

        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
        cairo_close_path(cr);

    cairo_restore(cr);
    //cairo_close_path(cr);
    
    cairo_fill_preserve(cr);
    cairo_set_line_width(cr, 0.003);
    cairo_stroke(cr);
}

void draw_planet(cairo_t *cr, orbit_params_t o_orbit, orbit_params_t rates, double time) {
    cairo_save(cr);

    orbit_params_t orbit = apply_time(o_orbit, rates, time);
    
    double arg_perhelion = orbit.long_perhelion - orbit.long_ascending;
    double mean_anomaly = orbit.mean_long - orbit.long_perhelion;
    mean_anomaly = fmod(mean_anomaly, 360.f);
    
    double estar = RADEG * orbit.eccentricity;
    double eccentric_anomaly = mean_anomaly - estar * sind(mean_anomaly);
    for(unsigned i = 0; i < 50; ++i) {
        double dm = mean_anomaly - (eccentric_anomaly - estar * sind(eccentric_anomaly));
        eccentric_anomaly += dm / (1. - orbit.eccentricity * cosd(eccentric_anomaly));
    }

    double x = orbit.semi_major_axis * (cosd(eccentric_anomaly) - orbit.eccentricity);
    double y = ((orbit.semi_major_axis * sqrt(1. - orbit.eccentricity * orbit.eccentricity))) * sind(eccentric_anomaly);

    double c_w = cosd(orbit.long_perhelion);
    double s_w = sind(orbit.long_perhelion);
    double c_o = cosd(orbit.long_ascending);
    double s_o = sind(orbit.long_ascending);
    double c_i = cosd(orbit.inclination);
    double s_i = sind(orbit.inclination);

    double x_ecl = 
        (c_w*c_o - s_w*s_o*c_i) * x +
        (-s_w*c_o- c_w*s_o*c_i) * y;

    double y_ecl =
        (c_w*s_o+s_w*c_o*c_i) * x +
        (-s_w*s_o + c_w*c_o*c_i) * y;

    double z_ecl = (s_w * s_i) * x + (c_w * s_i) * y;

    double x_eq = x_ecl;
    double y_eq = cosd(23.43928) * y_ecl - sind(23.43928) * z_ecl;
    
    double ang = acosd((cosd(eccentric_anomaly) - orbit.eccentricity) / (1. - orbit.eccentricity * cosd(eccentric_anomaly)));
    double r = orbit.semi_major_axis * ((1. - orbit.eccentricity * orbit.eccentricity) / (1. + orbit.eccentricity * cosd(ang)));

    double focus = (orbit.semi_major_axis * orbit.eccentricity);
    
    cairo_save(cr);

    cairo_rotate(cr, -(orbit.long_ascending) * DEGRAD);
    cairo_translate(cr, x_ecl, y_ecl);
    cairo_rotate(cr, (orbit.long_ascending) * DEGRAD);
    cairo_planet_circle(cr, orbit.planet);

    cairo_restore(cr);
}

static double julian_date(time_t time) {
    double days = (double)time / 86400.;
    static const double unix_epoch_julian = 2440588.;
    return unix_epoch_julian + days;
}

void draw_orbit(cairo_t *cr, orbit_params_t orbit) {
    cairo_save(cr);

    double semi_minor_axis = orbit.semi_major_axis * sqrt(1. - orbit.eccentricity * orbit.eccentricity);
    double focus = (orbit.semi_major_axis * orbit.eccentricity);

    cairo_rotate(cr, orbit.long_perhelion * DEGRAD);
    cairo_translate(cr, -focus, 0.);
    
    cairo_set_line_width(cr, 0.005);
    cairo_move_to(cr, 0., 0.);
    cairo_line_to(cr, semi_minor_axis, 0.);

    cairo_scale(cr, orbit.semi_major_axis, semi_minor_axis);

    cairo_arc(cr, 0., 0., 1., 0., M_PI * 2. - 0.05);

    cairo_restore(cr);
    
    cairo_save(cr);
    
    cairo_set_source_rgba(cr, 1., 1., 1., 0.14);
    cairo_set_line_width(cr, .005);
    cairo_stroke(cr);

    cairo_restore(cr);
}

int main(int argc, char *argv[]) {
    time_t timer = time(NULL);
    double time = julian_date(timer);

    Display *dsp = XOpenDisplay(NULL);
    if(dsp == NULL) {
        fputs("Failed to open X11 display", stderr);
        exit(-1);
    }
    
    Screen *screen = XDefaultScreenOfDisplay(dsp);
    Window win = XRootWindowOfScreen(screen);
    
    XWindowAttributes attribs;
    XGetWindowAttributes(dsp, win, &attribs);
    
    int depth = XDefaultDepthOfScreen(screen);
    Drawable pxmap = XCreatePixmap(dsp, win, attribs.width, attribs.height, depth);
    XMapWindow(dsp, win);
    
    Visual *display = XDefaultVisualOfScreen(screen);
    cairo_surface_t *surface = cairo_xlib_surface_create(dsp, pxmap, display, attribs.width, attribs.height);
    cairo_xlib_surface_set_size(surface, attribs.width, attribs.height);
    
    double ratio = (double)attribs.width / (double)attribs.height;

    cairo_t *cr = cairo_create(surface);
    
    cairo_set_antialias(cr, CAIRO_ANTIALIAS_BEST);
    cairo_scale(cr, attribs.height, attribs.height);
    
    cairo_save(cr);

    cairo_translate(cr, 0.5 * ratio, 0.52);
    
    orbit_params_t mercury = apply_time(MERCURY, MERCURY_R, time);
    orbit_params_t venus = apply_time(VENUS, VENUS_R, time);
    orbit_params_t earth = apply_time(EARTH, EARTH_R, time);
    orbit_params_t mars = apply_time(MARS, MARS_R, time);
    
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_scale(cr, 0.3, 0.3);
    cairo_matrix_t x_reflection_matrix;
    cairo_get_matrix(cr, &x_reflection_matrix);
    x_reflection_matrix.yy *= -1.0;
    cairo_set_matrix(cr, &x_reflection_matrix);

    
    cairo_save(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    
    draw_orbit(cr, mercury);
    draw_orbit(cr, venus);
    draw_orbit(cr, earth);
    draw_orbit(cr, mars);

    cairo_set_source_rgba(cr, 0.071, 0.071, 0.078, 1.);
    cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
    cairo_paint(cr);

    cairo_restore(cr);

    cairo_set_source_rgb(cr, 1, 0.957, 0.918);
    double rad = 0.00465047e+1 * 4.;
    double a1 = 20. * DEGRAD;
    double a2 = 160. * DEGRAD;
        
    double y = rad * sin(a1);
    double x = rad * cos(a1);
    
    cairo_save(cr);

    cairo_set_line_width(cr, 0.03);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
    cairo_arc_negative(cr, 0., 0., rad, a1, a2);
    cairo_stroke(cr);

    cairo_save(cr);
    cairo_scale(cr, 1., 0.401);


    cairo_arc_negative(cr, 0., y / 0.4, fabs(x), 0., M_PI);
    cairo_restore(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_stroke(cr);

    cairo_restore(cr);


    draw_planet(cr, MERCURY, MERCURY_R, time);
    draw_planet(cr, VENUS, VENUS_R, time);
    draw_planet(cr, EARTH, EARTH_R, time);
    draw_planet(cr, MARS, MARS_R, time);

    
    cairo_surface_flush(surface);

    XSetWindowBackgroundPixmap(dsp, win, pxmap);
    XClearWindow(dsp, win);

    XFlush(dsp);
    XSync(dsp, False);

    XKillClient(dsp, AllTemporary);
    XSetCloseDownMode(dsp, RetainTemporary);
}

