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
} orbit_params_t;

static const orbit_params_t MERCURY = {
    .semi_major_axis = 57.909,
    .eccentricity = 0.2056,
    .long_periapsis = 1.3518700794 * 2.,
};

static const orbit_params_t VENUS = {
    .semi_major_axis = 108.210,
    .eccentricity = 0.0068,
    .long_periapsis = 1.3383305132,
};

static const orbit_params_t EARTH = {
    .semi_major_axis = 149.5978875,
    .eccentricity = 0.016710219,
    .long_periapsis = -0.196535243881,
};

static const orbit_params_t MARS = {
    .semi_major_axis = 227.956,
    .eccentricity = 0.0935,
    .long_periapsis = 0.8653087613296,
};

void draw_orbit(cairo_t *cr, orbit_params_t orbit) {
    cairo_save(cr);

    double semi_minor_axis = orbit.semi_major_axis * sqrt(1. - orbit.eccentricity * orbit.eccentricity);
    double focus = (orbit.semi_major_axis * orbit.eccentricity);
    
    
    cairo_translate(cr, -focus * sin(orbit.long_periapsis), -focus * cos(orbit.long_periapsis));
    cairo_rotate(cr, -orbit.long_periapsis);
    cairo_scale(cr, semi_minor_axis, orbit.semi_major_axis);

    cairo_arc(cr, 0., 0., 1., 0., M_PI * 2.);

    cairo_restore(cr);
    
    cairo_save(cr);

    cairo_set_line_width(cr, 2.);
    cairo_stroke(cr);

    cairo_restore(cr);
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
    
    {
        cairo_save(cr);
        cairo_scale(cr, 0.002, 0.002);
        cairo_set_source_rgba(cr, 1., 1., 1., 0.24);
        draw_orbit(cr, MERCURY);
        draw_orbit(cr, VENUS);
        draw_orbit(cr, EARTH);
        draw_orbit(cr, MARS);
        cairo_restore(cr);
    }
    

    cairo_set_source_rgb(cr, 0.922, 0.514, 0.314);
    cairo_translate(cr, 0.3, 0.);
    cairo_arc(cr, 0., 0., 0.02, 0., M_PI * 2.);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 0., 0., 0., 0.5);
    cairo_move_to(cr, 0.001, 0.);
    cairo_arc_negative(cr, 0.001, 0., 0.020001, M_PI_2, M_PI_2 * 3.);
    cairo_fill(cr);

    cairo_restore(cr);

    cairo_surface_flush(surface);

    XSetWindowBackgroundPixmap(dsp, win, pxmap);
    XClearWindow(dsp, win);

    XFlush(dsp);
    XSync(dsp, False);

    XKillClient(dsp, AllTemporary);
    XSetCloseDownMode(dsp, RetainTemporary);
}

