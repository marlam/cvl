/*
 * cvl_math.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** 
 * \file cvl_math.h
 * \brief Math related functions.
 *
 * Miscellaneous math related functions: Rounding, minimum and maximum, angles,
 * sorting, random numbers, and so on.
 */

#include "config.h"

#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>

#include <gsl/gsl_rng.h>

#include "c99math.h"

#include "cvl/cvl_math.h"
#include "cvl/cvl_msg.h"


/**
 * \param x		The double to be rounded.
 * \return		The rounded value as an integer.
 *
 * Rounds a double.
 */ 
inline int cvl_iround(double x)
{
    return (int)lround(x);
}

/**
 * \param x		The float to be rounded.
 * \return		The rounded value as an integer.
 *
 * Rounds a float.
 */ 
inline int cvl_iroundf(float x)
{
    return (int)lroundf(x);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \return		The minimum of the given doubles.
 * 
 * Returns the minimum of two doubles.
 */
inline double cvl_min(double a, double b)
{
    return fmin(a, b);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \return		The maximum of the given doubles.
 *
 * Returns the maximum of two doubles.
 */
inline double cvl_max(double a, double b)
{
    return fmax(a, b);
}

/**
 * \param a		An int.
 * \param b		An int.
 * \return		The minimum of the given ints.
 * 
 * Returns the minimum of two ints.
 */
inline int cvl_mini(int a, int b)
{
    return (a < b ? a : b);
}

/**
 * \param a		An int.
 * \param b		An int.
 * \return		The maximum of the given ints.
 * 
 * Returns the maximum of two ints.
 */
inline int cvl_maxi(int a, int b)
{
    return (a > b ? a : b);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \param c		A double.
 * \return		The minimum of the given doubles.
 * 
 * Returns the minimum of three doubles.
 */
inline double cvl_min3(double a, double b, double c)
{
    return cvl_min(cvl_min(a, b), c);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \param c		A double.
 * \return		The maximum of the given doubles.
 * 
 * Returns the maximum of three doubles.
 */
inline double cvl_max3(double a, double b, double c)
{
    return cvl_max(cvl_max(a, b), c);
}

/**
 * \param a		An int.
 * \param b		An int.
 * \param c		An int.
 * \return		The minimum of the given ints.
 * 
 * Returns the minimum of three ints.
 */
inline int cvl_min3i(int a, int b, int c)
{
    return cvl_mini(cvl_mini(a, b), c);
}

/**
 * \param a		An int.
 * \param b		An int.
 * \param c		An int.
 * \return		The maximum of the given ints.
 * 
 * Returns the maximum of three ints.
 */
inline int cvl_max3i(int a, int b, int c)
{
    return cvl_maxi(cvl_maxi(a, b), c);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \param c		A double.
 * \param d		A double.
 * \return		The minimum of the given doubles.
 * 
 * Returns the minimum of four doubles.
 */
inline double cvl_min4(double a, double b, double c, double d)
{
    return cvl_min(cvl_min(a, b), cvl_min(c, d));
}

/**
 * \param a		A double.
 * \param b		A double.
 * \param c		A double.
 * \param d		A double.
 * \return		The maximum of the given doubles.
 * 
 * Returns the maximum of four doubles.
 */
inline double cvl_max4(double a, double b, double c, double d)
{
    return cvl_max(cvl_max(a, b), cvl_max(c, d));
}

/**
 * \param a		An int.
 * \param b		An int.
 * \param c		An int.
 * \param d		An int.
 * \return		The minimum of the given ints.
 * 
 * Returns the minimum of four ints.
 */
inline int cvl_min4i(int a, int b, int c, int d)
{
    return cvl_mini(cvl_mini(a, b), cvl_mini(c, d));
}

/**
 * \param a		An int.
 * \param b		An int.
 * \param c		An int.
 * \param d		An int.
 * \return		The maximum of the given ints.
 * 
 * Returns the maximum of four ints.
 */
inline int cvl_max4i(int a, int b, int c, int d)
{
    return cvl_maxi(cvl_maxi(a, b), cvl_maxi(c, d));
}

/**
 * \param a		A number.
 * \param b		A number.
 * \return 		The greatest common divisor of a and b.
 *
 * Returns the greatest common divisor of a and b.
 */
int cvl_gcd(int a, int b)
{
    int t;
    
    while (b != 0) 
    {
	t = b;
	b = a % b;
	a = t;
    }
    return a;
}

/**
 * \param a		A number.
 * \param b		A number.
 * \return 		The least common multiple of a and b.
 *
 * Returns the least common multiple of a and b.
 */
int cvl_lcm(int a, int b)
{
    int gcd = cvl_gcd(a,b);
    return (gcd == 0 ? 0 : (a / gcd) * b);
}

/**
 * \param c		The coordinate that should be reflected if necessary.
 * \param bound		The upper bound of the coordinate + 1 (in most cases width or height).
 * \return 		The coordinate with reflective indexing applied.
 *
 * Returns the coordinate with reflective indexing applied. Every value of \a c
 * can be mapped into [0,bound-1] in this way. \a bound must be greater than
 * zero.
 */
inline int cvl_reflect(int c, int bound)
{
    cvl_assert(bound > 0);

    int r;

    if (c < 0)
    {
	c = - (c + 1);
    }
    r = c % bound;
    if ((c / bound) % 2 == 1)
    {
	r = (bound - 1) - r;
    }
    return r;
}

/**
 * \param c		The coordinate that should be wrapped if necessary.
 * \param bound		The upper bound of the coordinate + 1 (in most cases width or height).
 * \return 		The coordinate with wrapped indexing applied.
 *
 * Returns the coordinate with wrapped indexing applied. Every value of \a c
 * can be mapped into [0,bound-1] in this way. \a bound must be greater than
 * zero.
 */
inline int cvl_wrap(int c, int bound)
{
    cvl_assert(bound > 0);

    int w = c % bound;
    if (c < 0 && w != 0)
    {
  	w += bound;
    }
    return w;
}

/**
 * \param c		The coordinate that should be clamped if necessary.
 * \param bound		The upper bound of the coordinate + 1 (in most cases width or height).
 * \return 		The coordinate with clamped indexing applied.
 *
 * Returns the coordinate with clamped indexing applied. Every value of \a c
 * can be mapped into [0,bound-1] in this way. \a bound must be greater than
 * zero.
 */
inline int cvl_clamp(int c, int bound)
{
    cvl_assert(bound > 0);

    if (c < 0)
    {
	c = 0;
    }
    else if (c >= bound)
    {
	c = bound - 1;
    }
    return c;
}


/**
 * \param x		Degrees.
 * \return		Radians.
 * 
 * Converts degrees to radians.
 */
inline double cvl_deg_to_rad(double x)
{
    return (x * (M_PI / 180.0));
}

/**
 * \param x		Radians.
 * \return		Degrees.
 * 
 * Converts radians to degrees.
 */
inline double cvl_rad_to_deg(double x)
{
    return (x * (180.0 / M_PI));
}

/**
 * \param a		An angle.
 * \return		The angle normalized to [0,2PI].
 * 
 * Normalizes an angle.
 */
inline double cvl_angle_normalize_1(double a)
{
    a = a / (2.0 * M_PI);
    a -= floor(a);
    return (a * 2.0 * M_PI);
}

/**
 * \param a		An angle.
 * \return		The angle normalized to [-PI,+PI].
 * 
 * Normalizes an angle.
 */
inline double cvl_angle_normalize_2(double a)
{
    a = (a + M_PI) / (2.0 * M_PI);
    a -= floor(a);
    return (a * 2.0 * M_PI - M_PI);
}

/* Helper for cvl_sort_array_pixel */
static int cvl_compare_pixels(cvl_pixel_t *p1, cvl_pixel_t *p2)
{
    return (*p1 < *p2 ? -1 : (*p1 > *p2 ? +1 : 0));
}

/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of pixels.
 * This is just a convenient wrapper around qsort.
 */
void cvl_sort_array_pixel(cvl_pixel_t *a, size_t a_len)
{
    qsort(a, a_len, sizeof(cvl_pixel_t), (int (*)(const void *, const void *))cvl_compare_pixels);
}

/* Helper for cvl_sort_array_int */
static int cvl_compare_ints(int *x1, int *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of ints.
 * This is just a convenient wrapper around qsort.
 */

void cvl_sort_array_int(int *a, size_t a_len)
{
    qsort(a, a_len, sizeof(int), (int (*)(const void *, const void *))cvl_compare_ints);
}


/* Helper for cvl_sort_array_double */
static int cvl_compare_floats(float *x1, float *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of floats.
 * This is just a convenient wrapper around qsort.
 */
void cvl_sort_array_float(float *a, size_t a_len)
{
    qsort(a, a_len, sizeof(float), (int (*)(const void *, const void *))cvl_compare_floats);
}


/* Helper for cvl_sort_array_double */
static int cvl_compare_doubles(double *x1, double *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of doubles.
 * This is just a convenient wrapper around qsort.
 */
void cvl_sort_array_double(double *a, size_t a_len)
{
    qsort(a, a_len, sizeof(double), (int (*)(const void *, const void *))cvl_compare_doubles);
}


/**
 * \param a		The first factor.
 * \param b		The second factor.
 * \return		Whether the product fits into an int.
 *
 * Checks if the product of two ints fits into an int.
 * Used to prevent arithmetic overflows.
 */ 
bool cvl_product_fits_in_int(int a, int b)
{
    /* Adapted from the comp.lang.c FAQ, see http://c-faq.com/misc/sd26.html */
    if (a < 0)
    {
	a = -a;
    }
    if (b < 0)
    { 
	b = -b; 
    }
    return (b == 0 || !(INT_MAX / b < a));
}

/**
 * \param a		The first factor.
 * \param b		The second factor.
 * \return		Whether the product fits into an int.
 *
 * Checks if the product of two size_t's into a size_t. 
 * Used to prevent arithmetic overflows.
 */ 
bool cvl_product_fits_in_size_t(size_t a, size_t b)
{
    return (b == 0 || !(SIZE_MAX / b < a));
}


/* The random number generator used throughout CVL */
gsl_rng *cvl_rng = NULL;

/**
 * \param seed		The seed value.
 * 
 * Seeds the GSL random number generator (after initializing it if necessary).
 * The special seed 0 disabled seeding of the RNG; this function then just makes
 * sure that the RNG is initialized. This initialization seeds the RNG with a
 * default value which can be changed by setting the environment variable
 * GSL_RNG_SEED. See http://www.gnu.org/software/gsl/manual/gsl-ref_17.html#SEC268.
 */
inline void cvl_rng_init(unsigned long int seed)
{
    if (!cvl_rng)
    {
	gsl_rng_env_setup();
	cvl_rng = gsl_rng_alloc(gsl_rng_default);
	cvl_msg_dbg("initializing GSL random number generator %s", 
		gsl_rng_name(cvl_rng));
	if (seed == 0)
	{
	    seed = gsl_rng_default_seed;
	}
    }
    if (seed != 0)
    {
	gsl_rng_set(cvl_rng, seed);
	cvl_msg_dbg("seeding GSL random number generator %s with seed %lu", 
		gsl_rng_name(cvl_rng), gsl_rng_default_seed);
    }
}
