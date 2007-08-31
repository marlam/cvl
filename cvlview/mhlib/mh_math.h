/*
 * mh_math.h
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007  Martin Lambers <marlam@marlam.de>
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MH_MATH_H
#define MH_MATH_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#include <math.h>

#include "mh_assert.h"

#ifndef SIZE_MAX
#ifdef __cplusplus
#define SIZE_MAX (static_cast<size_t>(-1))
#else
#define SIZE_MAX ((size_t)-1)
#endif
#endif

/**
 * \param x		The double to be rounded.
 * \return		The rounded value as an integer.
 *
 * Rounds a double.
 */ 
extern inline int mh_iround(double x)
{
    return (int)lround(x);
}

/**
 * \param x		The float to be rounded.
 * \return		The rounded value as an integer.
 *
 * Rounds a float.
 */ 
extern inline int mh_iroundf(float x)
{
    return (int)lroundf(x);
}

/**
 * \param x		An integer.
 * \param y		An integer greater than or equal to zero.
 * \return		x raised to the power of y.
 * 
 * Returns \a x raised to the power of \a y.
 */
extern inline int mh_powi(int x, int y)
{
    mh_assert(y >= 0);

    int result = 1;
    if (x == 2 && y > 0)
    {
	result = x << (y - 1);
    }
    else
    {
	for (int i = 0; i < y; i++)
	    result *= x;
    }
    return result;
}

/**
 * \param x		A size_t.
 * \param y		A size_t.
 * \return		x raised to the power of y.
 * 
 * Returns \a x raised to the power of \a y.
 */
extern inline size_t mh_pows(size_t x, size_t y)
{
    size_t result = 1;
    if (x == 2 && y > 0)
    {
	result = x << (y - 1);
    }
    else
    {
	for (size_t i = 0; i < y; i++)
	    result *= x;
    }
    return result;
}

/**
 * \param a		A double.
 * \param b		A double.
 * \return		The minimum of the given doubles.
 * 
 * Returns the minimum of two doubles.
 */
extern inline double mh_min(double a, double b)
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
extern inline double mh_max(double a, double b)
{
    return fmax(a, b);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \param c		A double.
 * \return		The minimum of the given doubles.
 * 
 * Returns the minimum of three doubles.
 */
extern inline double mh_min3(double a, double b, double c)
{
    return mh_min(mh_min(a, b), c);
}

/**
 * \param a		A double.
 * \param b		A double.
 * \param c		A double.
 * \return		The maximum of the given doubles.
 * 
 * Returns the maximum of three doubles.
 */
extern inline double mh_max3(double a, double b, double c)
{
    return mh_max(mh_max(a, b), c);
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
extern inline double mh_min4(double a, double b, double c, double d)
{
    return mh_min(mh_min(a, b), mh_min(c, d));
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
extern inline double mh_max4(double a, double b, double c, double d)
{
    return mh_max(mh_max(a, b), mh_max(c, d));
}

/**
 * \param a		A float.
 * \param b		A float.
 * \return		The minimum of the given floats.
 * 
 * Returns the minimum of two floats.
 */
extern inline float mh_minf(float a, float b)
{
    return fminf(a, b);
}

/**
 * \param a		A float.
 * \param b		A float.
 * \return		The maximum of the given floats.
 * 
 * Returns the maximum of two floats.
 */
extern inline float mh_maxf(float a, float b)
{
    return fmaxf(a, b);
}

/**
 * \param a		A float.
 * \param b		A float.
 * \param c		A float.
 * \return		The minimum of the given floats.
 * 
 * Returns the minimum of three floats.
 */
extern inline float mh_min3f(float a, float b, float c)
{
    return mh_minf(mh_minf(a, b), c);
}

/**
 * \param a		A float.
 * \param b		A float.
 * \param c		A float.
 * \return		The maximum of the given floats.
 * 
 * Returns the maximum of three floats.
 */
extern inline float mh_max3f(float a, float b, float c)
{
    return mh_maxf(mh_maxf(a, b), c);
}

/**
 * \param a		A float.
 * \param b		A float.
 * \param c		A float.
 * \param d		A float.
 * \return		The minimum of the given floats.
 * 
 * Returns the minimum of four floats.
 */
extern inline float mh_min4f(float a, float b, float c, float d)
{
    return mh_minf(mh_minf(a, b), mh_minf(c, d));
}

/**
 * \param a		A float.
 * \param b		A float.
 * \param c		A float.
 * \param d		A float.
 * \return		The maximum of the given floats.
 * 
 * Returns the maximum of four floats.
 */
extern inline float mh_max4f(float a, float b, float c, float d)
{
    return mh_maxf(mh_maxf(a, b), mh_maxf(c, d));
}

/**
 * \param a		A int.
 * \param b		A int.
 * \return		The minimum of the given ints.
 * 
 * Returns the minimum of two ints.
 */
extern inline int mh_mini(int a, int b)
{
    return a < b ? a : b;
}

/**
 * \param a		A int.
 * \param b		A int.
 * \return		The maximum of the given ints.
 * 
 * Returns the maximum of two ints.
 */
extern inline int mh_maxi(int a, int b)
{
    return a > b ? a : b;
}

/**
 * \param a		A int.
 * \param b		A int.
 * \param c		A int.
 * \return		The minimum of the given ints.
 * 
 * Returns the minimum of three ints.
 */
extern inline int mh_min3i(int a, int b, int c)
{
    return mh_mini(mh_mini(a, b), c);
}

/**
 * \param a		A int.
 * \param b		A int.
 * \param c		A int.
 * \return		The maximum of the given ints.
 * 
 * Returns the maximum of three ints.
 */
extern inline int mh_max3i(int a, int b, int c)
{
    return mh_maxi(mh_maxi(a, b), c);
}

/**
 * \param a		A int.
 * \param b		A int.
 * \param c		A int.
 * \param d		A int.
 * \return		The minimum of the given ints.
 * 
 * Returns the minimum of four ints.
 */
extern inline int mh_min4i(int a, int b, int c, int d)
{
    return mh_mini(mh_mini(a, b), mh_mini(c, d));
}

/**
 * \param a		A int.
 * \param b		A int.
 * \param c		A int.
 * \param d		A int.
 * \return		The maximum of the given ints.
 * 
 * Returns the maximum of four ints.
 */
extern inline int mh_max4i(int a, int b, int c, int d)
{
    return mh_maxi(mh_maxi(a, b), mh_maxi(c, d));
}


/**
 * \param a		A int.
 * \param b		A int.
 * \return		The minimum of the given size_ts.
 * 
 * Returns the minimum of two size_ts.
 */
extern inline size_t mh_mins(size_t a, size_t b)
{
    return a < b ? a : b;
}

/**
 * \param a		A size_t.
 * \param b		A size_t.
 * \return		The maximum of the given size_ts.
 * 
 * Returns the maximum of two size_ts.
 */
extern inline size_t mh_maxs(size_t a, size_t b)
{
    return a > b ? a : b;
}

/**
 * \param a		A size_t.
 * \param b		A size_t.
 * \param c		A size_t.
 * \return		The minimum of the given size_ts.
 * 
 * Returns the minimum of three size_ts.
 */
extern inline size_t mh_min3s(size_t a, size_t b, size_t c)
{
    return mh_mins(mh_mins(a, b), c);
}

/**
 * \param a		A size_t.
 * \param b		A size_t.
 * \param c		A size_t.
 * \return		The maximum of the given size_ts.
 * 
 * Returns the maximum of three size_ts.
 */
extern inline size_t mh_max3s(size_t a, size_t b, size_t c)
{
    return mh_maxs(mh_maxs(a, b), c);
}

/**
 * \param a		A size_t.
 * \param b		A size_t.
 * \param c		A size_t.
 * \param d		A size_t.
 * \return		The minimum of the given size_ts.
 * 
 * Returns the minimum of four size_ts.
 */
extern inline size_t mh_min4s(size_t a, size_t b, size_t c, size_t d)
{
    return mh_mins(mh_mins(a, b), mh_mins(c, d));
}

/**
 * \param a		A size_t.
 * \param b		A size_t.
 * \param c		A size_t.
 * \param d		A size_t.
 * \return		The maximum of the given size_ts.
 * 
 * Returns the maximum of four size_ts.
 */
extern inline size_t mh_max4s(size_t a, size_t b, size_t c, size_t d)
{
    return mh_maxs(mh_maxs(a, b), mh_maxs(c, d));
}

/**
 * \param x		A double.
 * \param min		Lower bound.
 * \param max		Upper bound.
 *
 * Returns \a x clamped to [\a min, \a max].
 */
extern inline double mh_clamp(double x, double min, double max)
{
    return mh_min(max, mh_max(min, x));
}

/**
 * \param x		A float.
 * \param min		Lower bound.
 * \param max		Upper bound.
 *
 * Returns \a x clamped to [\a min, \a max].
 */
extern inline float mh_clampf(float x, float min, float max)
{
    return mh_minf(max, mh_maxf(min, x));
}

/**
 * \param x		An integer.
 * \param min		Lower bound.
 * \param max		Upper bound.
 *
 * Returns \a x clamped to [\a min, \a max].
 */
extern inline int mh_clampi(int x, int min, int max)
{
    return mh_mini(max, mh_maxi(min, x));
}

/**
 * \param x		A size_t.
 * \param min		Lower bound.
 * \param max		Upper bound.
 *
 * Returns \a x clamped to [\a min, \a max].
 */
extern inline size_t mh_clamps(size_t x, size_t min, size_t max)
{
    return mh_mins(max, mh_maxs(min, x));
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
extern inline int mh_coord_reflect(int c, int bound)
{
    mh_assert(bound > 0);

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
extern inline int mh_coord_wrap(int c, int bound)
{
    mh_assert(bound > 0);

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
extern inline int mh_coord_clamp(int c, int bound)
{
    mh_assert(bound > 0);

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
 * \param a		A number.
 * \param b		A number.
 * \return 		The greatest common divisor of a and b.
 *
 * Returns the greatest common divisor of a and b.
 */
int mh_gcd(int a, int b);

/**
 * \param a		A number.
 * \param b		A number.
 * \return 		The least common multiple of a and b.
 *
 * Returns the least common multiple of a and b.
 */
int mh_lcm(int a, int b);


/**
 * \param a		The first factor.
 * \param b		The second factor.
 * \return		Whether the product fits into an int.
 *
 * Checks if the product of two ints fits into an int.
 * Used to prevent arithmetic overflows.
 */ 
extern inline bool mh_pfiti(int a, int b)
{
    /* Adapted from the comp.lang.c FAQ, see http://c-faq.com/misc/sd26.html */
    if (a < 0)
	a = -a;
    if (b < 0)
	b = -b; 
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
extern inline bool mh_pfits(size_t a, size_t b)
{
    return (b == 0 || !(SIZE_MAX / b < a));
}


/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of doubles.
 * This is just a convenient wrapper around qsort.
 */
void mh_sort_array_double(double *a, size_t a_len);

/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of floats.
 * This is just a convenient wrapper around qsort.
 */
void mh_sort_array_float(float *a, size_t a_len);

/**
 * \param a		The array.
 * \param a_len		The length of the array.
 *
 * Sorts an array of ints.
 * This is just a convenient wrapper around qsort.
 */
void mh_sort_array_int(int *a, size_t a_len);

/**
 * \param k		Length parameter. The length is 2k+1.
 * \param s		Standard deviation.
 * \param mask		An array with at least 2k+1 elements.
 * \param weight_sum	Storage space for the weight sum or NULL.
 *
 * Generates a 1D Gauss mask with standard deviation \a s in the (2k+1) first
 * elements of \a mask. The sum of weights in the mask is stored in \a
 * weight_sum if \a weight_sum is not NULL.
 */
void mh_gauss_mask(int k, float s, float *mask, float *weight_sum);

/**
 * \param n		Number of intervalls.
 * \param X		n+1 sample points.
 * \param Y		n+1 sample values.
 * \param K		Storage space for n+1 second derivatives.
 *
 * Computes a cubic spline for the \a n+1 samples given by \a X and \a Y. This is
 * done by computing the \a n+1 second derivates (curvature values) at the \a n+1
 * sample points \a X and storing them in \a K. The spline can be evaluated with
 * mh_cspline_eval() using these values. The sample points \a X must be sorted
 * in ascending order.
 */
void mh_cspline_prep(int n, const float *X, const float *Y, float *K);

/**
 * \param n		Number of intervalls.
 * \param X		n+1 sample points.
 * \param Y		n+1 sample values.
 * \param K		n+1 second derivatives.
 * \param x		Evaluation point.
 * \return		The evaluation result.
 *
 * Evaluates a cubic spline given by the \a n+1 samples \a X (sorted in
 * ascending order), \a Y and the curvature values \a K that were previously
 * computed by mh_cspline_prep(). The point \a x at which the spline is 
 * evaluated must be inside the sample points (\a X[0] <= \a x <= \a X[n]).
 */
float mh_cspline_eval(int n, const float *X, const float *Y, const float *K, float x);

/**
 * \param a		An array.
 * \param s		The size of one array element.
 * \param n		The number of array elements.
 *
 * Shuffles the array elements using the rand(3) random number generator.
 * The caller may want to seed this RNG using srand(3) before calling this
 * function.
 */
void mh_shuffle(void *a, size_t s, size_t n);

/**
 * \param x		The angle in degrees.
 * \return		The angle in radians.
 *
 * Converts degrees to radians.
 */
extern inline float mh_deg_to_rad(float x)
{
    return (x * ((float)M_PI / 180.0f));
}

/**
 * \param x		The angle in radians.
 * \return		The angle in degrees.
 *
 * Converts radians to degrees.
 */
extern inline float mh_rad_to_deg(float x)
{
    return (x * (180.0f / (float)M_PI));
}

/**
 * \param x		The angle.
 * \return		The angle normalized to [0,2PI].
 * 
 * Normalizes an angle to [0,2PI].
 */
extern inline float mh_angle_0_to_2pi(float x)
{
    x = x / (2.0f * (float)M_PI);
    x -= floorf(x);
    return (x * 2.0f * (float)M_PI);
}

/**
 * \param x		The angle.
 * \return		The angle normalized to [-PI,+PI].
 * 
 * Normalizes an angle to [-PI,+PI].
 */
extern inline float mh_angle_mpi_to_ppi(float x)
{
    x = (x + (float)M_PI) / (2.0f * (float)M_PI);
    x -= floor(x);
    return (x * 2.0f * (float)M_PI - (float)M_PI);
}

/**
 * \param x		An integer.
 * \return		Whether the integer is a power of two.
 *
 * Returns true if \a x is a power of two, and false otherwise.
 */
extern inline bool mh_is_power_of_two(int x)
{
    return (x > 0 && (x & (x - 1)) == 0);
}

/**
 * \param x		An integer.
 * \return		The next power of two.
 *
 * Returns the next power of two that is greater than or equal to \a x.
 */
extern inline int mh_next_power_of_two(int x)
{
    int p = 1;
    while (p < x)
	p *= 2;
    return p;
}

/**
 * \param x		A positive integer.
 * \return		The logarithm of x with base 2.
 *
 * Returns the logarithm of x with base 2, rounded towards zero.
 */
extern inline int mh_log2(int x)
{
    mh_assert(x > 0);

    int log2 = 0;
    while (x > 1)
    {
	x /= 2;
	log2++;
    }
    return log2;
}

#ifdef __cplusplus
}
#endif

#endif
