/*
 * cvl_vector.c
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
 * \file cvl_vector.h
 * \brief Vectors.
 *
 * Functions to work with various vector types.
 */

#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
extern int errno;

#include <cairo.h>

#include "cvl/cvl_field.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_cairo.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_vector.h"


/**
 * \param dst		The destination vector.
 * \param src		The source vector.
 *
 * Copies a vector from \a src to \a dst.
 */
inline void cvl_vector2i_copy(int *dst, const int *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

/**
 * \param v		The vector.
 *
 * Zeroes a vector.
 */
inline void cvl_vector2i_zero(int *v)
{
    v[0] = 0;
    v[1] = 0;
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Adds two vectors.
 */
inline void cvl_vector2i_add(int *result, const int *a, const int *b)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Subracts two vectors.
 */
inline void cvl_vector2i_sub(int *result, const int *a, const int *b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
}

/**
 * \param v		The vector.
 * \param lambda	The scalar.
 *
 * Scales a vector.
 */
inline void cvl_vector2i_scale(int *v, float lambda)
{
    v[0] = cvl_iround(v[0] * lambda);
    v[1] = cvl_iround(v[1] * lambda);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The result.
 *
 * Multiplies two vectors.
 */
inline int cvl_vector2i_mul(const int *a, const int *b)
{
    return (a[0] * b[0] + a[1] * b[1]);
}

/**
 * \param v		The vector.
 * \return		The norm.
 *
 * Computes the euclidean norm of a vector.
 */
inline float cvl_vector2i_norm_euc(const int *v)
{
    return sqrtf(cvl_vector2i_mul(v, v));
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the geodesic distance between two vectors.
 */
inline float cvl_vector2i_dist_arc(const int *a, const int *b)
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

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the euclidean distance between two vectors.
 */
inline float cvl_vector2i_dist_euc(const int *a, const int *b)
{
    int d[2];
    cvl_vector2i_sub(d, a, b);
    return cvl_vector2i_norm_euc(d);
}

/**
 * \param field		The vector field.
 * \param sample_x	Visualize every sample_x-th vector in horizontal direction.
 * \param sample_y	Visualize every sample_y-th vector in vertical direction.
 * \param dist_x	Horizontal needle distance.
 * \param dist_y	Vertical needle distance.
 * \param factor	Factor for the needle length.
 * \return		The needle diagram.
 *
 * Every \a sample_x -th vector in horizontal direction and every \a sample_y -th 
 * vector in vertical direction will be represented by a needle. The needles 
 * will have a distance of \a dist_x pixels in horizontal and \a dist_y pixels in
 * vertical direction. The needle length is the length of the vector after it
 * was scaled with \a factor.
 */
cvl_frame_t *cvl_vector2i_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor)
{
    cvl_assert(field != NULL);
    cvl_assert(sample_x > 0);
    cvl_assert(sample_y > 0);
    cvl_assert(dist_x > 0);
    cvl_assert(dist_y > 0);
    
    int width = cvl_field_width(field) / sample_x * dist_x;
    int height = cvl_field_height(field) / sample_y * dist_y;
    cvl_frame_t *vis = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
    cvl_pixel_type_t original_pixel_type;
    cairo_t *cr;
    
    cvl_frame_fill_rect(vis, 0, 0, width, height, cvl_pixel_gray(0xff));
    cvl_cairo_start(vis, &cr, &original_pixel_type);
    cairo_set_line_width(cr, 1.0);
    for (int y = 0; y < cvl_field_height(field); y += sample_y)
    {
	for (int x = 0; x < cvl_field_width(field); x += sample_x)
	{
	    const int *v = cvl_field_get(field, x, y);
	    int needle_pos_x = x / sample_x * dist_x;
	    int needle_pos_y = y / sample_y * dist_y;
	    int delta_x = cvl_iroundf((float)(v[0]) * factor);
	    int delta_y = cvl_iroundf((float)(v[1]) * factor);
	    cairo_arc(cr, needle_pos_x + 0.5, needle_pos_y + 0.5, 1.0, 0.0, 2.0 * M_PI);
	    cairo_close_path(cr);
	    cairo_set_source_rgb(cr, 0.63, 0.63, 0.63);
	    cairo_stroke(cr);
	    cairo_move_to(cr, needle_pos_x + 0.5, needle_pos_y + 0.5);
	    cairo_rel_line_to(cr, delta_x + 0.5, delta_y + 0.5);
	    cairo_close_path(cr);
	    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	    cairo_stroke(cr);
	}
    }
    cvl_cairo_stop(vis, cr, original_pixel_type);
    return vis;
}

/**
 * \param dst		The destination vector.
 * \param src		The source vector.
 *
 * Copies a vector from \a src to \a dst.
 */
inline void cvl_vector2_copy(float *dst, const float *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

/**
 * \param v		The vector.
 *
 * Zeroes a vector.
 */
inline void cvl_vector2_zero(float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param epsilon	The tolerance.
 *
 * Tests if two vectors are equal, with a given tolerance.
 */
inline bool cvl_vector2_equal(const float *a, const float *b, float epsilon)
{
    return (fabsf(a[0] - b[0]) < epsilon && fabs(a[1] - b[1]) < epsilon);
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Adds two vectors.
 */
inline void cvl_vector2_add(float *result, const float *a, const float *b)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Subracts two vectors.
 */
inline void cvl_vector2_sub(float *result, const float *a, const float *b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
}

/**
 * \param v		The vector.
 * \param lambda	The scalar.
 *
 * Scales a vector.
 */
inline void cvl_vector2_scale(float *v, float lambda)
{
    v[0] *= lambda;
    v[1] *= lambda;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The result.
 *
 * Multiplies two vectors.
 */
inline float cvl_vector2_mul(const float *a, const float *b)
{
    return (a[0] * b[0] + a[1] * b[1]);
}

/**
 * \param v		The vector.
 * \return		The norm.
 *
 * Computes the euclidean norm of a vector.
 */
inline float cvl_vector2_norm_euc(const float *v)
{
    return sqrtf(cvl_vector2_mul(v, v));
}

/**
 * \param v		The vector.
 * 
 * Scales a vector to length 1.
 */
inline void cvl_vector2_normalize(float *v)
{
    cvl_vector2_scale(v, 1.0f / cvl_vector2_norm_euc(v));
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the geodesic distance between two vectors.
 */
inline float cvl_vector2_dist_arc(const float *a, const float *b)
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

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the euclidean distance between two vectors.
 */
inline float cvl_vector2_dist_euc(const float *a, const float *b)
{
    float d[2];
    cvl_vector2_sub(d, a, b);
    return cvl_vector2_norm_euc(d);
}

/**
 * \param field		The vector field.
 * \param sample_x	Visualize every sample_x -th vector in horizontal direction.
 * \param sample_y	Visualize every sample_y -th vector in vertical direction.
 * \param dist_x	Horizontal needle distance.
 * \param dist_y	Vertical needle distance.
 * \param factor	Factor for the needle length.
 * \return		The needle diagram.
 *
 * Every \a sample_x -th vector in horizontal direction and every \a sample_y -th 
 * vector in vertical direction will be represented by a needle. The needles 
 * will have a distance of \a dist_x pixels in horizontal and \a dist_y pixels in
 * vertical direction. The needle length is the length of the vector after it
 * was scaled with \a factor.
 */
cvl_frame_t *cvl_vector2_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor)
{
    cvl_assert(field != NULL);
    cvl_assert(sample_x > 0);
    cvl_assert(sample_y > 0);
    cvl_assert(dist_x > 0);
    cvl_assert(dist_y > 0);
    
    int width = cvl_field_width(field) / sample_x * dist_x;
    int height = cvl_field_height(field) / sample_y * dist_y;
    cvl_frame_t *vis = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
    cvl_pixel_type_t original_pixel_type;
    cairo_t *cr;
    
    cvl_frame_fill_rect(vis, 0, 0, width, height, cvl_pixel_gray(0xff));
    cvl_cairo_start(vis, &cr, &original_pixel_type);
    cairo_set_line_width(cr, 1.0);
    for (int y = 0; y < cvl_field_height(field); y += sample_y)
    {
	for (int x = 0; x < cvl_field_width(field); x += sample_x)
	{
	    const float *v = cvl_field_get(field, x, y);
	    int needle_pos_x = x / sample_x * dist_x;
	    int needle_pos_y = y / sample_y * dist_y;
	    int delta_x = cvl_iround(v[0] * factor);
	    int delta_y = cvl_iround(v[1] * factor);
	    cairo_arc(cr, needle_pos_x + 0.5, needle_pos_y + 0.5, 1.0, 0.0, 2.0 * M_PI);
	    cairo_close_path(cr);
	    cairo_set_source_rgb(cr, 0.63, 0.63, 0.63);
	    cairo_stroke(cr);
	    cairo_move_to(cr, needle_pos_x + 0.5, needle_pos_y + 0.5);
	    cairo_rel_line_to(cr, delta_x + 0.5, delta_y + 0.5);
	    cairo_close_path(cr);
	    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	    cairo_stroke(cr);
	}
    }
    cvl_cairo_stop(vis, cr, original_pixel_type);
    return vis;
}

/**
 * \param dst		The destination vector.
 * \param src		The source vector.
 *
 * Copies a vector from \a src to \a dst.
 */
inline void cvl_vector3_copy(float *dst, const float *src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

/**
 * \param v		The vector.
 *
 * Zeroes a vector.
 */
inline void cvl_vector3_zero(float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param epsilon	The tolerance.
 *
 * Tests if two vectors are equal, with a given tolerance.
 */
inline bool cvl_vector3_equal(const float *a, const float *b, float epsilon)
{
    return (fabsf(a[0] - b[0]) < epsilon && fabsf(a[1] - b[1]) < epsilon && fabsf(a[2] - b[2]) < epsilon);
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Adds two vectors.
 */
inline void cvl_vector3_add(float *result, const float *a, const float *b)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Subracts two vectors.
 */
inline void cvl_vector3_sub(float *result, const float *a, const float *b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
}

/**
 * \param v		The vector.
 * \param lambda	The scalar.
 *
 * Scales a vector.
 */
inline void cvl_vector3_scale(float *v, float lambda)
{
    v[0] *= lambda;
    v[1] *= lambda;
    v[2] *= lambda;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The result.
 *
 * Multiplies two vectors.
 */
inline float cvl_vector3_mul(const float *a, const float *b)
{
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

/**
 * \param result	The result.
 * \param a		A vector.
 * \param b		A vector.
 *
 * Computes the cross product of two vectors.
 */
inline void cvl_vector3_crossproduct(float *result, const float *a, const float *b)
{
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

/**
 * \param v		The vector.
 * \return		The norm.
 *
 * Computes the euclidean norm of a vector.
 */
inline float cvl_vector3_norm_euc(const float *v)
{
    return sqrtf(cvl_vector3_mul(v, v));
}

/**
 * \param v		The vector.
 * 
 * Scales a vector to length 1.
 */
inline void cvl_vector3_normalize(float *v)
{
    cvl_vector3_scale(v, 1.0f / cvl_vector3_norm_euc(v));
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the geodesic distance between two vectors.
 */
inline float cvl_vector3_dist_arc(const float *a, const float *b)
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

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the euclidean distance between two vectors.
 */
inline float cvl_vector3_dist_euc(const float *a, const float *b)
{
    float d[3];
    cvl_vector3_sub(d, a, b);
    return cvl_vector3_norm_euc(d);
}

/**
 * \param field		The vector field.
 * \param sample_x	Visualize every sample_x -th vector in horizontal direction.
 * \param sample_y	Visualize every sample_y -th vector in vertical direction.
 * \param dist_x	Horizontal needle distance.
 * \param dist_y	Vertical needle distance.
 * \param factor	Factor for the needle length.
 * \return		The needle diagram.
 *
 * Every \a sample_x -th vector in horizontal direction and every \a sample_y -th 
 * vector in vertical direction will be represented by a needle. The needles 
 * will have a distance of \a dist_x pixels in horizontal and \a dist_y pixels in
 * vertical direction. The needle length is the length of the vector after it
 * was scaled with \a factor.
 */
cvl_frame_t *cvl_vector3_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor)
{
    cvl_assert(field != NULL);
    cvl_assert(sample_x > 0);
    cvl_assert(sample_y > 0);
    cvl_assert(dist_x > 0);
    cvl_assert(dist_y > 0);
    
    int width = cvl_field_width(field) / sample_x * dist_x;
    int height = cvl_field_height(field) / sample_y * dist_y;
    cvl_frame_t *vis = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
    cvl_pixel_type_t original_pixel_type;
    cairo_t *cr;
    
    cvl_frame_fill_rect(vis, 0, 0, width, height, cvl_pixel_gray(0xff));
    cvl_cairo_start(vis, &cr, &original_pixel_type);
    cairo_set_line_width(cr, 1.0);
    for (int y = 0; y < cvl_field_height(field); y += sample_y)
    {
	for (int x = 0; x < cvl_field_width(field); x += sample_x)
	{
	    const float *v = cvl_field_get(field, x, y);
	    int needle_pos_x = x / sample_x * dist_x;
	    int needle_pos_y = y / sample_y * dist_y;
	    float n = cvl_vector3_norm_euc(v);
	    float alpha = (isfinite(v[2] / n) ? (M_PI_2 - acos(v[2] / cvl_vector3_norm_euc(v))) : 0.0);
	    int delta_x = cvl_iround(v[0] * factor * cos(alpha));
	    int delta_y = cvl_iround(v[1] * factor * sin(alpha));
	    cairo_arc(cr, needle_pos_x + 0.5, needle_pos_y + 0.5, 1.0, 0.0, 2.0 * M_PI);
	    cairo_close_path(cr);
	    cairo_set_source_rgb(cr, 0.63, 0.63, 0.63);
	    cairo_stroke(cr);
	    cairo_move_to(cr, needle_pos_x + 0.5, needle_pos_y + 0.5);
	    cairo_rel_line_to(cr, delta_x + 0.5, delta_y + 0.5);
	    cairo_close_path(cr);
	    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	    cairo_stroke(cr);
	}
    }
    cvl_cairo_stop(vis, cr, original_pixel_type);
    return vis;
}
