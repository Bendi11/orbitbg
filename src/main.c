#include <X11/X.h>
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <X11/Xlib.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mmath.h"
#include "planet.h"
#include "render.h"

#define ERR_BUF_LEN (1024)

int error_handler(Display *dsp, XErrorEvent *ev) {
    static char errbuf[ERR_BUF_LEN];
    XGetErrorText(dsp, ev->error_code, errbuf, ERR_BUF_LEN);
    fputs("X11 error: ", stderr);
    fputs(errbuf, stderr);

    XCloseDisplay(dsp);
    return -1;
}

int main(int argc, char *argv[]) {
    time_t timer = time(NULL);
    double time = julian_time(timer);

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
    
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_scale(cr, 0.3, 0.3);
    cairo_matrix_t x_reflection_matrix;
    cairo_get_matrix(cr, &x_reflection_matrix);
    x_reflection_matrix.yy *= -1.0;
    cairo_set_matrix(cr, &x_reflection_matrix);

    orbit_params_t 
        mercury = orbit_params_time(&MERCURY, time),
        venus = orbit_params_time(&VENUS, time),
        earth = orbit_params_time(&EARTH, time),
        mars = orbit_params_time(&MARS, time);
    
    cairo_save(cr);
        planet_draw_orbit(cr, mercury);
        planet_draw_orbit(cr, venus);
        planet_draw_orbit(cr, earth);
        planet_draw_orbit(cr, mars);

        cairo_set_source_rgba(cr, 0.071, 0.071, 0.078, 1.);
        cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
        cairo_paint(cr);
    cairo_restore(cr);

    cairo_save(cr);
    
    cairo_set_source_rgb(cr, 1, 0.957, 0.918);
    cairo_arc(cr, 0., 0., 0.00465047e+1 * 4., 0., M_PI * 2.);
    cairo_fill(cr);

    cairo_restore(cr);
        planet_render(cr, &MERCURY, mercury);
        planet_render(cr, &VENUS, venus);
        planet_render(cr, &EARTH, earth);
        planet_render(cr, &MARS, mars);
    cairo_surface_flush(surface);

    XSetWindowBackgroundPixmap(dsp, win, pxmap);
    XClearWindow(dsp, win);

    XFlush(dsp);
    XSync(dsp, False);

    XKillClient(dsp, AllTemporary);
    XSetCloseDownMode(dsp, RetainTemporary);
}

