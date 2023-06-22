#pragma once

#include <math.h>

#define DEGRAD (M_PI / 180.)
#define RADEG (180. / M_PI)


/** A simple x, y pair structure */
typedef struct point {
    double x;
    double y;
} point_t;

static inline double cosd(double angle)  { return cos(angle * DEGRAD);  }
static inline double acosd(double v)     { return acos(v) * RADEG;      }
static inline double sind(double angle)  { return sin(angle * DEGRAD);  }
static inline double tand(double angle)  { return sin(angle * DEGRAD); }
static inline double atand(double angle) { return atan(angle) * RADEG; }
