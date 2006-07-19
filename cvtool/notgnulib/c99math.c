#include "config.h"

#include <math.h>


#ifndef HAVE_LROUND
long int lround(double x)
{
    return (long int)round(x);
}
#endif

#ifndef HAVE_FMIN
double fmin(double x, double y)
{
    return x < y ? x : y;
}
#endif

#ifndef HAVE_FMAX
double fmax(double x, double y)
{
    return x > y ? x : y;
}
#endif
