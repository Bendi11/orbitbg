#include <X11/X.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_BUF_LEN (1024)

int error_handler(Display *dsp, XErrorEvent *ev) {
    static char errbuf[ERR_BUF_LEN];
    XGetErrorText(dsp, ev->error_code, errbuf, ERR_BUF_LEN);
    fputs("X11 error: ", stderr);
    fputs(errbuf, stderr);

    XCloseDisplay(dsp);
    return -1;
}


typedef struct orbit_params {
    double semi_major_axis;
    double eccentricity;
    double long_periapsis;
    double sidereral_period;
    double mean_anomaly_epoch;
} orbit_params_t;


static const double J2000 = 2451545.0;
static const double time = 2460065.945463;

static const orbit_params_t MERCURY = {
    .semi_major_axis = 57.909,
    .eccentricity = 0.2056,
    .long_periapsis = 1.3518922267586,
    .sidereral_period = 87.969,
    .mean_anomaly_epoch = 3.050765719,
};

static const orbit_params_t VENUS = {
    .semi_major_axis = 108.210,
    .eccentricity = 0.0068,
    .long_periapsis = 1.3383305132,
    .sidereral_period = 224.701,
    .mean_anomaly_epoch = 0.8746717546,
};


static const orbit_params_t EARTH = {
    .semi_major_axis = 149.5978875,
    .eccentricity = 0.016710219,
    .long_periapsis = -0.196535243881,
    .sidereral_period = 365.256,
    .mean_anomaly_epoch = 6.2590474036,
};

static const orbit_params_t MARS = {
    .semi_major_axis = 227.956,
    .eccentricity = 0.0935,
    .long_periapsis = -0.41743821348097,
    .sidereral_period = 686.980,
    .mean_anomaly_epoch = 0.3388033144,
};


static const orbit_params_t MERCURY_RATES = {
    .semi_major_axis = 0.00005535121,
    .eccentricity = 0.00001906,
    .long_periapsis = 0.0027820570966,
};

static const orbit_params_t VENUS_RATES = {
    .semi_major_axis = 0.00058343169,
    .eccentricity = -0.00004107,
    .long_periapsis = 0.00268329,
};

static const orbit_params_t EARTH_RATES = {
    .semi_major_axis = 0.00084074003,
    .eccentricity = -0.00004392,
    .long_periapsis = 0.005642189402893,
};

static const orbit_params_t MARS_RATES = {
    .semi_major_axis = 0.00276307267,
    .eccentricity = 0.00007882,
    .long_periapsis = 0.007756433087666,
};

void draw_planet(cairo_t *cr, orbit_params_t orbit, double time) {
    double centuries = (time - J2000) / 36525.;
    double mean_ang = ((2 * M_PI) / (orbit.sidereral_period));
    double mean_anomaly = orbit.mean_anomaly_epoch + mean_ang * (time - J2000);
    //mean_anomaly = mean_anomaly - floor(mean_anomaly / M_PI)* M_PI;

    /*double eccentric_anomaly = mean_anomaly - orbit.eccentricity * sin(mean_anomaly);

    for(unsigned i = 0; i < 10; ++i) {
        double d_mean_anomaly = mean_anomaly - (eccentric_anomaly - orbit.eccentricity * sin(eccentric_anomaly));
        eccentric_anomaly += d_mean_anomaly / (1 - orbit.eccentricity * cos(eccentric_anomaly));
    }*/
    
    double eccentric_anomaly=mean_anomaly;
    for (unsigned q=0;q<20;q++) eccentric_anomaly=mean_anomaly+orbit.eccentricity*sin(eccentric_anomaly);

    double true_anomaly = 2. * atan(sqrt((1.0+orbit.eccentricity)/(1.0-orbit.eccentricity)) * tan(eccentric_anomaly / 2.));

    /*double true_anomaly = acos(
        (cos(eccentric_anomaly) - orbit.eccentricity) /
        (1 - orbit.eccentricity * cos(eccentric_anomaly))
    );*/

    double radius = orbit.semi_major_axis * ((1 - orbit.eccentricity * orbit.eccentricity) / (1 + orbit.eccentricity * cos(true_anomaly)));

    printf(
        "n: %f\nt: %f\nTech: %f\nM0: %f\nM: %f\ne: %f\nE: %f\nv: %f\nd: %f\n",
        mean_ang, time, J2000, orbit.mean_anomaly_epoch, mean_anomaly, orbit.eccentricity, eccentric_anomaly, true_anomaly, radius);
    
    printf(
        "ANGLE: %f\n=====DONE=====\n",
        -true_anomaly
    );
    cairo_save(cr);

    cairo_translate(cr, radius * cos(true_anomaly), -radius * sin(true_anomaly));
    cairo_arc(cr, 0., 0., 5., 0., M_PI * 2.);
    cairo_fill(cr);

    cairo_restore(cr);
}

void draw_orbit(cairo_t *cr, orbit_params_t orbit) {
    cairo_save(cr);

    double semi_minor_axis = orbit.semi_major_axis * sqrt(1. - orbit.eccentricity * orbit.eccentricity);
    double focus = (orbit.semi_major_axis * orbit.eccentricity);
    
    cairo_translate(cr, focus * sin(orbit.long_periapsis - M_PI_2), -focus * cos(orbit.long_periapsis - M_PI_2));
    cairo_rotate(cr, orbit.long_periapsis);
    cairo_scale(cr, semi_minor_axis, orbit.semi_major_axis);

    cairo_arc(cr, 0., 0., 1., 0., M_PI * 2.);

    cairo_restore(cr);
    
    cairo_save(cr);

    cairo_set_line_width(cr, 2.);
    cairo_stroke(cr);

    cairo_restore(cr);
}

orbit_params_t apply_time(orbit_params_t orbit, orbit_params_t rates, double time) {
    double centuries = time / 36525.;
    return (orbit_params_t){
        .semi_major_axis = orbit.semi_major_axis + rates.semi_major_axis * centuries,
        .eccentricity = orbit.eccentricity + rates.eccentricity * centuries,
        .long_periapsis = orbit.long_periapsis + rates.long_periapsis * centuries,
        .mean_anomaly_epoch = orbit.mean_anomaly_epoch,
        .sidereral_period = orbit.sidereral_period,
    };
}

int main(int argc, char *argv[]) {
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
    cairo_set_source_rgb(cr, 0.102, 0.118, 0.1412);
    cairo_paint(cr);
    

    cairo_set_source_rgb(cr, 1, 0.957, 0.918);
    cairo_translate(cr, 0.5 * ratio, 0.5);
    cairo_arc(cr, 0., 0., 0.045, 0., M_PI * 2.);
    cairo_fill(cr);
    
    double t = time - J2000;
    orbit_params_t mercury = apply_time(MERCURY, MERCURY_RATES, t);
    orbit_params_t venus = apply_time(VENUS, VENUS_RATES, t);
    orbit_params_t earth = apply_time(EARTH, EARTH_RATES, t);
    orbit_params_t mars = apply_time(MARS, MARS_RATES, t);
    
    {
        cairo_save(cr);
        cairo_scale(cr, 0.002, 0.002);
        cairo_set_source_rgba(cr, 1., 1., 1., 0.24);
        draw_planet(cr, mercury, time);
        draw_orbit(cr, mercury);
        draw_planet(cr, venus, time);
        draw_orbit(cr, venus);
        draw_planet(cr, earth, time);
        draw_orbit(cr, earth);
        draw_planet(cr, mars, time);
        draw_orbit(cr, mars);
        cairo_restore(cr);
    }
    

    cairo_restore(cr);

    cairo_surface_flush(surface);

    XSetWindowBackgroundPixmap(dsp, win, pxmap);
    XClearWindow(dsp, win);

    XFlush(dsp);
    XSync(dsp, False);

    XKillClient(dsp, AllTemporary);
    XSetCloseDownMode(dsp, RetainTemporary);
}

