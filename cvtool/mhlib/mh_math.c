/*
 * mh_math.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
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

/** 
 * \file mh_math.h
 * \brief Math related functions.
 *
 * Miscellaneous math related functions.
 */

#include "config.h"

#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mh_assert.h"
#include "mh_alloc.h"
#include "mh_math.h"


int mh_gcd(int a, int b)
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

int mh_lcm(int a, int b)
{
    int gcd = mh_gcd(a,b);
    return (gcd == 0 ? 0 : (a / gcd) * b);
}


/* Helper for mh_sort_array_double */
static int mh_compare_doubles(double *x1, double *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

void mh_sort_array_double(double *a, size_t a_len)
{
    qsort(a, a_len, sizeof(double), (int (*)(const void *, const void *))mh_compare_doubles);
}

/* Helper for mh_sort_array_float */
static int mh_compare_floats(float *x1, float *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

void mh_sort_array_float(float *a, size_t a_len)
{
    qsort(a, a_len, sizeof(float), (int (*)(const void *, const void *))mh_compare_floats);
}

/* Helper for mh_sort_array_int */
static int mh_compare_ints(int *x1, int *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

void mh_sort_array_int(int *a, size_t a_len)
{
    qsort(a, a_len, sizeof(int), (int (*)(const void *, const void *))mh_compare_ints);
}


void mh_gauss_mask(int k, float s, float *mask, float *weight_sum)
{
    mh_assert(k >= 0 && k < INT_MAX / 2 - 1);
    mh_assert(s >= 0.0f);
    mh_assert(mask != NULL);

    float *gauss = mh_alloc(mh_alloc_mul(k + 1, sizeof(float)));

    float gauss_sum = 0.0f;
    for (int i = 0; i <= k; i++)
    {
	gauss[i] = expf(- (float)(i * i) / (2.0f * s * s)) / (sqrtf(2.0f * (float)M_PI) * s);
	gauss_sum += gauss[i];
    }

    for (int i = 0; i <= k; i++)
	mask[i] = gauss[k - i];
    for (int i = k + 1; i < 2 * k + 1; i++)
	mask[i] = gauss[i - k];
    if (weight_sum)
	*weight_sum = 2.0f * gauss_sum - gauss[0];

    free(gauss);
}


void mh_cspline_prep(int n, const float *X, const float *Y, float *K)
{
    mh_assert(n > 0);
    mh_assert(X != NULL);
    mh_assert(Y != NULL);
    mh_assert(K != NULL);
    mh_assert(mh_pfits(n, sizeof(float)));

    float *h = mh_alloc(mh_alloc_mul(n + 1, sizeof(float)));
    float *e = mh_alloc(mh_alloc_mul(n + 1, sizeof(float)));
    float *u = mh_alloc(mh_alloc_mul(n + 1, sizeof(float)));
    float *r = mh_alloc(mh_alloc_mul(n + 1, sizeof(float)));

    for (int i = 0; i < n; i++)
    {
	h[i] = X[i + 1] - X[i];
	e[i] = (6.0f / h[i]) * (Y[i + 1] - Y[i]);
    }
    u[1] = 2.0f * (h[0] + h[1]);
    r[1] = e[1] - e[0];
    for (int i = 2; i < n; i++)
    {
	u[i] = 2.0f * (h[i] + h[i - 1]) - (h[i - 1] * h[i - 1] / u[i - 1]);
	r[i] = (e[i] - e[i - 1]) - (r[i - 1] * h[i - 1] / u[i - 1]);
    }
    K[n] = 0.0f;
    for (int i = n - 1; i > 0; i--)
    {
	K[i] = (r[i] - h[i] * K[i + 1]) / u[i];
    }
    K[0] = 0.0f;

    free(h);
    free(e);
    free(u);
    free(r);
}

float mh_cspline_eval(int n, const float *X, const float *Y, const float *K, float x)
{
    mh_assert(n > 0);
    mh_assert(X != NULL);
    mh_assert(Y != NULL);
    mh_assert(K != NULL);
    mh_assert(x >= X[0]);
    mh_assert(x <= X[n]);

    int r = 0;
    int s = n - 1;
    int i = -1;
    for (;;)
    {
	i = (r + s) / 2;
	if (x < X[i])
	    s = i - 1;
	else if (x > X[i + 1])
	    r = i + 1;
	else 
	    break;
    }

    float h = X[i + 1] - X[i];
    float a = (K[i + 1] - K[i]) / (6.0f * h);
    float b = K[i] / 2.0f;
    float c = (Y[i + 1] - Y[i]) / h - (h / 6.0f) * (2.0f * K[i] + K[i + 1]);
    float d = Y[i];
    float xd = x - X[i];
    return a * xd * xd * xd + b * xd * xd + c * xd + d;
}


void mh_shuffle(void *a, size_t s, size_t n)
{
    mh_assert(a != NULL);
    mh_assert(s > 0);

    char *ac = a;
    char tmp[s];
    for (size_t i = n - 1; i > 0; i--)
    {
     	size_t j = rand() % (i + 1);
	memcpy(tmp, ac + i * s, s);
	memcpy(ac + i * s, ac + j * s, s);
	memcpy(ac + j * s, tmp, s);
    }
}
