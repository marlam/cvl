/*
 * cvl_vector.h
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

#ifndef CVL_VECTOR_H
#define CVL_VECTOR_H

#include <stdbool.h>
#include <stdint.h>

#include "cvl_frame.h"

void cvl_vector2i_copy(int *dst, const int *src);
extern inline void cvl_vector2i_copy(int *dst, const int *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

void cvl_vector2i_zero(int *v);
extern inline void cvl_vector2i_zero(int *v)
{
    v[0] = 0;
    v[1] = 0;
}

void cvl_vector2i_add(int *result, const int *a, const int *b);
extern inline void cvl_vector2i_add(int *result, const int *a, const int *b)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

void cvl_vector2i_sub(int *result, const int *a, const int *b);
extern inline void cvl_vector2i_sub(int *result, const int *a, const int *b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
}

void cvl_vector2i_scale(int *v, float lambda);
extern inline void cvl_vector2i_scale(int *v, float lambda)
{
    v[0] = cvl_iround(v[0] * lambda);
    v[1] = cvl_iround(v[1] * lambda);
}

int cvl_vector2i_mul(const int *a, const int *b);
extern inline int cvl_vector2i_mul(const int *a, const int *b)
{
    return (a[0] * b[0] + a[1] * b[1]);
}

float cvl_vector2i_norm_euc(const int *v);
extern inline float cvl_vector2i_norm_euc(const int *v)
{
    return sqrtf(cvl_vector2i_mul(v, v));
}

float cvl_vector2i_dist_arc(const int *a, const int *b);
extern inline float cvl_vector2i_dist_arc(const int *a, const int *b)
{
    float x = (float)cvl_vector2i_mul(a, b) / (cvl_vector2i_norm_euc(a) * cvl_vector2i_norm_euc(b));
    if (x < -1.0f)
    {
	x = -1.0f;
    }
    else if (x > 1.0f)
    {
	x = 1.0f;
    }
    return acosf(x);
}

float cvl_vector2i_dist_euc(const int *a, const int *b);
extern inline float cvl_vector2i_dist_euc(const int *a, const int *b)
{
    int d[2];
    cvl_vector2i_sub(d, a, b);
    return cvl_vector2i_norm_euc(d);
}

cvl_frame_t *cvl_vector2i_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

void cvl_vector2_copy(float *dst, const float *src);
extern inline void cvl_vector2_copy(float *dst, const float *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

void cvl_vector2_zero(float *v);
extern inline void cvl_vector2_zero(float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
}

bool cvl_vector2_equal(const float *a, const float *b, float epsilon);
extern inline bool cvl_vector2_equal(const float *a, const float *b, float epsilon)
{
    return (fabsf(a[0] - b[0]) < epsilon && fabs(a[1] - b[1]) < epsilon);
}

void cvl_vector2_add(float *result, const float *a, const float *b);
extern inline void cvl_vector2_add(float *result, const float *a, const float *b)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

void cvl_vector2_sub(float *result, const float *a, const float *b);
extern inline void cvl_vector2_sub(float *result, const float *a, const float *b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
}

void cvl_vector2_scale(float *v, float lambda);
extern inline void cvl_vector2_scale(float *v, float lambda)
{
    v[0] *= lambda;
    v[1] *= lambda;
}

float cvl_vector2_mul(const float *a, const float *b);
extern inline float cvl_vector2_mul(const float *a, const float *b)
{
    return (a[0] * b[0] + a[1] * b[1]);
}

float cvl_vector2_norm_euc(const float *v);
extern inline float cvl_vector2_norm_euc(const float *v)
{
    return sqrtf(cvl_vector2_mul(v, v));
}

void cvl_vector2_normalize(float *v);
extern inline void cvl_vector2_normalize(float *v)
{
    cvl_vector2_scale(v, 1.0f / cvl_vector2_norm_euc(v));
}

float cvl_vector2_dist_arc(const float *a, const float *b);
extern inline float cvl_vector2_dist_arc(const float *a, const float *b)
{
    float x = cvl_vector2_mul(a, b) / (cvl_vector2_norm_euc(a) * cvl_vector2_norm_euc(b));
    if (x < -1.0f)
    {
	x = -1.0f;
    }
    else if (x > 1.0f)
    {
	x = 1.0f;
    }
    return acosf(x);
}

float cvl_vector2_dist_euc(const float *a, const float *b);
extern inline float cvl_vector2_dist_euc(const float *a, const float *b)
{
    float d[2];
    cvl_vector2_sub(d, a, b);
    return cvl_vector2_norm_euc(d);
}

cvl_frame_t *cvl_vector2_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

void cvl_vector3_copy(float *dst, const float *src);
extern inline void cvl_vector3_copy(float *dst, const float *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

void cvl_vector3_zero(float *v);
extern inline void cvl_vector3_zero(float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
}

bool cvl_vector3_equal(const float *a, const float *b, float epsilon);
extern inline bool cvl_vector3_equal(const float *a, const float *b, float epsilon)
{
    return (fabsf(a[0] - b[0]) < epsilon && fabsf(a[1] - b[1]) < epsilon && fabsf(a[2] - b[2]) < epsilon);
}

void cvl_vector3_add(float *result, const float *a, const float *b);
extern inline void cvl_vector3_add(float *result, const float *a, const float *b)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
}

void cvl_vector3_sub(float *result, const float *a, const float *b);
extern inline void cvl_vector3_sub(float *result, const float *a, const float *b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
}

void cvl_vector3_scale(float *v, float lambda);
extern inline void cvl_vector3_scale(float *v, float lambda)
{
    v[0] *= lambda;
    v[1] *= lambda;
    v[2] *= lambda;
}

float cvl_vector3_mul(const float *a, const float *b);
extern inline float cvl_vector3_mul(const float *a, const float *b)
{
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void cvl_vector3_crossproduct(float *result, const float *a, const float *b);
extern inline void cvl_vector3_crossproduct(float *result, const float *a, const float *b)
{
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

float cvl_vector3_norm_euc(const float *v);
extern inline float cvl_vector3_norm_euc(const float *v)
{
    return sqrtf(cvl_vector3_mul(v, v));
}

void cvl_vector3_normalize(float *v);
extern inline void cvl_vector3_normalize(float *v)
{
    cvl_vector3_scale(v, 1.0f / cvl_vector3_norm_euc(v));
}

float cvl_vector3_dist_arc(const float *a, const float *b);
extern inline float cvl_vector3_dist_arc(const float *a, const float *b)
{
    float x = cvl_vector3_mul(a, b) / (cvl_vector3_norm_euc(a) * cvl_vector3_norm_euc(b));
    if (x < -1.0f)
    {
	x = -1.0f;
    }
    else if (x > 1.0f)
    {
	x = 1.0f;
    }
    return acosf(x);
}

float cvl_vector3_dist_euc(const float *a, const float *b);
extern inline float cvl_vector3_dist_euc(const float *a, const float *b)
{
    float d[3];
    cvl_vector3_sub(d, a, b);
    return cvl_vector3_norm_euc(d);
}

cvl_frame_t *cvl_vector3_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

#endif
