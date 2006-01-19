#ifndef NOTGNULIB_C99MATH_H
#define NOTGNULIB_C99MATH_H

#include "config.h"

#include <math.h>

#ifndef HAVE_LROUND
long int lround(double x);
#endif
#ifndef HAVE_FMIN
double fmin(double x, double y);
#endif
#ifndef HAVE_FMAX
double fmax(double x, double y);
#endif

#endif
