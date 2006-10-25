/*
 * cvl_math.h
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

#ifndef CVL_MATH_H
#define CVL_MATH_H

#include <stdbool.h>
#include <math.h>

#include <gsl/gsl_rng.h>

#include "cvl_assert.h"


bool cvl_product_fits_in_int(int a, int b);
bool cvl_product_fits_in_size_t(size_t a, size_t b);

extern inline int cvl_iround(double x)
{
    return (int)lround(x);
}

extern inline int cvl_iroundf(float x)
{
    return (int)lroundf(x);
}

extern inline double cvl_min(double a, double b)
{
    return fmin(a, b);
}

extern inline double cvl_max(double a, double b)
{
    return fmax(a, b);
}

extern inline int cvl_mini(int a, int b)
{
    return (a < b ? a : b);
}

extern inline int cvl_maxi(int a, int b)
{
    return (a > b ? a : b);
}

extern inline double cvl_min3(double a, double b, double c)
{
    return cvl_min(cvl_min(a, b), c);
}

extern inline double cvl_max3(double a, double b, double c)
{
    return cvl_max(cvl_max(a, b), c);
}

extern inline int cvl_min3i(int a, int b, int c)
{
    return cvl_mini(cvl_mini(a, b), c);
}

extern inline int cvl_max3i(int a, int b, int c)
{
    return cvl_maxi(cvl_maxi(a, b), c);
}

extern inline double cvl_min4(double a, double b, double c, double d)
{
    return cvl_min(cvl_min(a, b), cvl_min(c, d));
}

extern inline double cvl_max4(double a, double b, double c, double d)
{
    return cvl_max(cvl_max(a, b), cvl_max(c, d));
}

extern inline int cvl_min4i(int a, int b, int c, int d)
{
    return cvl_mini(cvl_mini(a, b), cvl_mini(c, d));
}

extern inline int cvl_max4i(int a, int b, int c, int d)
{
    return cvl_maxi(cvl_maxi(a, b), cvl_maxi(c, d));
}

int cvl_gcd(int a, int b);
int cvl_lcm(int a, int b);

extern inline int cvl_reflect(int c, int bound)
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

extern inline int cvl_wrap(int c, int bound)
{
    cvl_assert(bound > 0);

    int w = c % bound;
    if (c < 0 && w != 0)
    {
  	w += bound;
    }
    return w;
}

extern inline int cvl_clamp(int c, int bound)
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

extern inline double cvl_deg_to_rad(double x)
{
    return (x * (M_PI / 180.0));
}

extern inline double cvl_rad_to_deg(double x)
{
    return (x * (180.0 / M_PI));
}

extern inline double cvl_angle_normalize_1(double a)
{
    a = a / (2.0 * M_PI);
    a -= floor(a);
    return (a * 2.0 * M_PI);
}

extern inline double cvl_angle_normalize_2(double a)
{
    a = (a + M_PI) / (2.0 * M_PI);
    a -= floor(a);
    return (a * 2.0 * M_PI - M_PI);
}

extern gsl_rng *cvl_rng;
void cvl_rng_init(unsigned long int seed);

#endif
