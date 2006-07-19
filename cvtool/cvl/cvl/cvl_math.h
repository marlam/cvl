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

#include <stddef.h>
#include <stdbool.h>

#include <gsl/gsl_rng.h>

#include "cvl_pixel.h"

inline int cvl_iround(double x);
inline int cvl_iroundf(float x);

inline double cvl_min(double a, double b);
inline double cvl_max(double a, double b);
inline double cvl_min3(double a, double b, double c);
inline double cvl_max3(double a, double b, double c);
inline double cvl_min4(double a, double b, double c, double d);
inline double cvl_max4(double a, double b, double c, double d);
inline int cvl_mini(int a, int b);
inline int cvl_maxi(int a, int b);
inline int cvl_min3i(int a, int b, int c);
inline int cvl_max3i(int a, int b, int c);
inline int cvl_min4i(int a, int b, int c, int d);
inline int cvl_max4i(int a, int b, int c, int d);

int cvl_gcd(int a, int b);
int cvl_lcm(int a, int b);

inline int cvl_reflect(int c, int bound);
inline int cvl_wrap(int c, int bound);
inline int cvl_clamp(int c, int bound);

inline double cvl_deg_to_rad(double x);
inline double cvl_rad_to_deg(double x);

inline double cvl_angle_normalize_1(double a);
inline double cvl_angle_normalize_2(double a);

bool cvl_product_fits_in_int(int a, int b);
bool cvl_product_fits_in_size_t(size_t a, size_t b);

void cvl_sort_array_pixel(cvl_pixel_t *a, size_t a_len);
void cvl_sort_array_int(int *a, size_t a_len);
void cvl_sort_array_float(float *a, size_t a_len);
void cvl_sort_array_double(double *a, size_t a_len);

extern gsl_rng *cvl_rng;
inline void cvl_rng_init(unsigned long int seed);

#endif
