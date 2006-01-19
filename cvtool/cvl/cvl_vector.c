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

#include "cvl/cvl_field.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_draw.h"
#include "cvl/cvl_vector.h"


/**
 * \typedef cvl_vector2i_t
 * A vector with two integer components.
 */

/**
 * \typedef cvl_vector2_t
 * A vector with two floating point components.
 */

/**
 * \typedef cvl_vector3_t
 * A vector with three floating point components.
 */


/**
 * \param s		String buffer.
 * \param s_size	The size of \a s.
 * \param v		The vector.
 * \return		Success or error.
 *
 * Converts \a v into a string. May fail if \a s is too short.
 */
bool cvl_vector2i_to_string(char *s, size_t s_size, cvl_vector2i_t v)
{
    return (snprintf(s, s_size, "%d %d", v[0], v[1]) < (int)s_size);
}
    
/**
 * \param s		String.
 * \param v		The vector.
 * \return		Success or error.
 *
 * Converts the string \a s into a vector \a v. May fail if \a s is not a valid
 * representation of the given vector type.
 */
bool cvl_vector2i_from_string(const char *s, cvl_vector2i_t v)
{
    long val;
    char *p;

    for (int i = 0; i < 2; i++)
    {
	errno = 0;
	val = strtol(s, &p, 10);
	if (s == p || (*p != ' ' && *p != '\0') || errno == ERANGE
		|| val < INT_MIN || val > INT_MAX)
	{
	    return false;
	}
	v[i] = val;
	s = p;
    }
    return true;
}

/**
 * \param dst		The destination vector.
 * \param src		The source vector.
 *
 * Copies a vector from \a src to \a dst.
 */
inline void cvl_vector2i_copy(cvl_vector2i_t dst, const cvl_vector2i_t src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

/**
 * \param v		The vector.
 *
 * Zeroes a vector.
 */
inline void cvl_vector2i_zero(cvl_vector2i_t v)
{
    v[0] = 0;
    v[1] = 0;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Adds two vectors.
 */
inline void cvl_vector2i_add(const cvl_vector2i_t a, const cvl_vector2i_t b, cvl_vector2i_t result)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Subracts two vectors.
 */
inline void cvl_vector2i_sub(const cvl_vector2i_t a, const cvl_vector2i_t b, cvl_vector2i_t result)
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
inline void cvl_vector2i_scale(cvl_vector2i_t v, double lambda)
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
inline int cvl_vector2i_mul(const cvl_vector2i_t a, const cvl_vector2i_t b)
{
    return (a[0] * b[0] + a[1] * b[1]);
}

/**
 * \param v		The vector.
 * \return		The norm.
 *
 * Computes the euclidean norm of a vector.
 */
inline double cvl_vector2i_norm_euc(const cvl_vector2i_t v)
{
    return sqrt(cvl_vector2i_mul(v, v));
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the geodesic distance between two vectors.
 */
inline double cvl_vector2i_dist_arc(const cvl_vector2i_t a, const cvl_vector2i_t b)
{
    double x = (double)cvl_vector2i_mul(a, b) / (cvl_vector2i_norm_euc(a) * cvl_vector2i_norm_euc(b));
    if (x < -1.0)
    {
	x = -1.0;
    }
    else if (x > 1.0)
    {
	x = 1.0;
    }
    return acos(x);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the euclidean distance between two vectors.
 */
inline double cvl_vector2i_dist_euc(const cvl_vector2i_t a, const cvl_vector2i_t b)
{
    cvl_vector2i_t d;
    cvl_vector2i_sub(a, b, d);
    return cvl_vector2i_norm_euc(d);
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
cvl_frame_t *cvl_vector2i_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor)
{
    int width = cvl_field_width(field) / sample_x * dist_x;
    int height = cvl_field_height(field) / sample_y * dist_y;
    cvl_frame_t *vis = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
    cvl_frame_fill_rect(vis, 0, 0, width, height, cvl_pixel_gray(0xff));

    for (int y = 0; y < cvl_field_height(field); y += sample_y)
    {
	for (int x = 0; x < cvl_field_width(field); x += sample_x)
	{
	    cvl_vector2i_t v;
	    int needle_pos_x = x / sample_x * dist_x;
	    int needle_pos_y = y / sample_y * dist_y;
	    cvl_field_get(field, x, y, &v);
	    int delta_x = cvl_iround((double)(v[0]) * factor);
	    int delta_y = cvl_iround((double)(v[1]) * factor);
	    cvl_draw_circle(vis, cvl_pixel_gray(160), needle_pos_x, needle_pos_y, 1);
	    cvl_draw_line(vis, cvl_pixel_gray(0), needle_pos_x, needle_pos_y,
		    needle_pos_x + delta_x, needle_pos_y + delta_y);
	}
    }
    return vis;
}

/**
 * \param s		String buffer.
 * \param s_size	The size of \a s.
 * \param v		The vector.
 * \return		Success or error.
 *
 * Converts \a v into a string. May fail if \a s is too short.
 */
bool cvl_vector2_to_string(char *s, size_t s_size, cvl_vector2_t v)
{
    return (snprintf(s, s_size, "%.20g %.20g", v[0], v[1]) < (int)s_size);
}
    
/**
 * \param s		String.
 * \param v		The vector.
 * \return		Success or error.
 *
 * Converts the string \a s into a vector \a v. May fail if \a s is not a valid
 * representation of the given vector type.
 */
bool cvl_vector2_from_string(const char *s, cvl_vector2_t v)
{
    char *p;

    for (int i = 0; i < 2; i++)
    {
	errno = 0;
	v[i] = strtod(s, &p);
	if (s == p || (*p != ' ' && *p != '\0') || errno == ERANGE)
	{
	    return false;
	}
	s = p;
    }
    return true;
}

/**
 * \param dst		The destination vector.
 * \param src		The source vector.
 *
 * Copies a vector from \a src to \a dst.
 */
inline void cvl_vector2_copy(cvl_vector2_t dst, const cvl_vector2_t src)
{
    dst[0] = src[0];
    dst[1] = src[1];
}

/**
 * \param v		The vector.
 *
 * Zeroes a vector.
 */
inline void cvl_vector2_zero(cvl_vector2_t v)
{
    v[0] = 0.0;
    v[1] = 0.0;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param epsilon	The tolerance.
 *
 * Tests if two vectors are equal, with a given tolerance.
 */
inline bool cvl_vector2_equal(const cvl_vector2_t a, const cvl_vector2_t b, double epsilon)
{
    return (fabs(a[0] - b[0]) < epsilon && fabs(a[1] - b[1]) < epsilon);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Adds two vectors.
 */
inline void cvl_vector2_add(const cvl_vector2_t a, const cvl_vector2_t b, cvl_vector2_t result)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Subracts two vectors.
 */
inline void cvl_vector2_sub(const cvl_vector2_t a, const cvl_vector2_t b, cvl_vector2_t result)
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
inline void cvl_vector2_scale(cvl_vector2_t v, double lambda)
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
inline double cvl_vector2_mul(const cvl_vector2_t a, const cvl_vector2_t b)
{
    return (a[0] * b[0] + a[1] * b[1]);
}

/**
 * \param v		The vector.
 * \return		The norm.
 *
 * Computes the euclidean norm of a vector.
 */
inline double cvl_vector2_norm_euc(const cvl_vector2_t v)
{
    return sqrt(cvl_vector2_mul(v, v));
}

/**
 * \param v		The vector.
 * 
 * Scales a vector to length 1.
 */
inline void cvl_vector2_normalize(cvl_vector2_t v)
{
    cvl_vector2_scale(v, 1.0 / cvl_vector2_norm_euc(v));
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the geodesic distance between two vectors.
 */
inline double cvl_vector2_dist_arc(const cvl_vector2_t a, const cvl_vector2_t b)
{
    double x = cvl_vector2_mul(a, b) / (cvl_vector2_norm_euc(a) * cvl_vector2_norm_euc(b));
    if (x < -1.0)
    {
	x = -1.0;
    }
    else if (x > 1.0)
    {
	x = 1.0;
    }
    return acos(x);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the euclidean distance between two vectors.
 */
inline double cvl_vector2_dist_euc(const cvl_vector2_t a, const cvl_vector2_t b)
{
    cvl_vector2_t d;
    cvl_vector2_sub(a, b, d);
    return cvl_vector2_norm_euc(d);
}

/**
 * \param vectors	The vectors.
 * \param n		The number of vectors.
 * \param result	The result.
 *
 * Computes an average of the given vectors, using the given method.
 */
void cvl_vector2_avg_bary(const cvl_vector2_t *vectors, int n, cvl_vector2_t result)
{
    double lensum = 0.0;
    cvl_vector2_zero(result);
    for (int i = 0; i < n; i++)
    {
	cvl_vector2_t tmp;
	double len;		
	cvl_vector2_copy(tmp, vectors[i]);
	len = cvl_vector2_norm_euc(tmp);
	if (len > 0.001)
	{
	    cvl_vector2_scale(tmp, 1.0 / len);
	    cvl_vector2_add(result, tmp, result);
	    lensum += len;
	}
    }
    cvl_vector2_normalize(result);
    cvl_vector2_scale(result, lensum / (double)n);
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
	int sample_x, int sample_y, int dist_x, int dist_y, double factor)
{
    int width = cvl_field_width(field) / sample_x * dist_x;
    int height = cvl_field_height(field) / sample_y * dist_y;
    cvl_frame_t *vis = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
    cvl_frame_fill_rect(vis, 0, 0, width, height, cvl_pixel_gray(0xff));

    for (int y = 0; y < cvl_field_height(field); y += sample_y)
    {
	for (int x = 0; x < cvl_field_width(field); x += sample_x)
	{
	    cvl_vector2_t v;
	    int needle_pos_x = x / sample_x * dist_x;
	    int needle_pos_y = y / sample_y * dist_y;
	    cvl_field_get(field, x, y, &v);
	    int delta_x = cvl_iround(v[0] * factor);
	    int delta_y = cvl_iround(v[1] * factor);
	    cvl_draw_circle(vis, cvl_pixel_gray(160), needle_pos_x, needle_pos_y, 1);
	    cvl_draw_line(vis, cvl_pixel_gray(0), needle_pos_x, needle_pos_y,
		    needle_pos_x + delta_x, needle_pos_y + delta_y);
	}
    }
    return vis;
}

/**
 * \param s		String buffer.
 * \param s_size	The size of \a s.
 * \param v		The vector.
 * \return		Success or error.
 *
 * Converts \a v into a string. May fail if \a s is too short.
 */
bool cvl_vector3_to_string(char *s, size_t s_size, cvl_vector3_t v)
{
    return (snprintf(s, s_size, "%.20g %.20g %.20g", v[0], v[1], v[2]) < (int)s_size);
}
    
/**
 * \param s		String.
 * \param v		The vector.
 * \return		Success or error.
 *
 * Converts the string \a s into a vector \a v. May fail if \a s is not a valid
 * representation of the given vector type.
 */
bool cvl_vector3_from_string(const char *s, cvl_vector3_t v)
{
    char *p;

    for (int i = 0; i < 3; i++)
    {
	errno = 0;
	v[i] = strtod(s, &p);
	if (s == p || (*p != ' ' && *p != '\0') || errno == ERANGE)
	{
	    return false;
	}
	s = p;
    }
    return true;
}

/**
 * \param dst		The destination vector.
 * \param src		The source vector.
 *
 * Copies a vector from \a src to \a dst.
 */
inline void cvl_vector3_copy(cvl_vector3_t dst, const cvl_vector3_t src)
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
inline void cvl_vector3_zero(cvl_vector3_t v)
{
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param epsilon	The tolerance.
 *
 * Tests if two vectors are equal, with a given tolerance.
 */
inline bool cvl_vector3_equal(const cvl_vector3_t a, const cvl_vector3_t b, double epsilon)
{
    return (fabs(a[0] - b[0]) < epsilon && fabs(a[1] - b[1]) < epsilon && fabs(a[2] - b[2]) < epsilon);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Adds two vectors.
 */
inline void cvl_vector3_add(const cvl_vector3_t a, const cvl_vector3_t b, cvl_vector3_t result)
{
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Subracts two vectors.
 */
inline void cvl_vector3_sub(const cvl_vector3_t a, const cvl_vector3_t b, cvl_vector3_t result)
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
inline void cvl_vector3_scale(cvl_vector3_t v, double lambda)
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
inline double cvl_vector3_mul(const cvl_vector3_t a, const cvl_vector3_t b)
{
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \param result	The result.
 *
 * Computes the cross product of two vectors.
 */
inline void cvl_vector3_crossproduct(const cvl_vector3_t a, const cvl_vector3_t b, cvl_vector3_t result)
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
inline double cvl_vector3_norm_euc(const cvl_vector3_t v)
{
    return sqrt(cvl_vector3_mul(v, v));
}

/**
 * \param v		The vector.
 * 
 * Scales a vector to length 1.
 */
inline void cvl_vector3_normalize(cvl_vector3_t v)
{
    cvl_vector3_scale(v, 1.0 / cvl_vector3_norm_euc(v));
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the geodesic distance between two vectors.
 */
inline double cvl_vector3_dist_arc(const cvl_vector3_t a, const cvl_vector3_t b)
{
    double x = cvl_vector3_mul(a, b) / (cvl_vector3_norm_euc(a) * cvl_vector3_norm_euc(b));
    if (x < -1.0)
    {
	x = -1.0;
    }
    else if (x > 1.0)
    {
	x = 1.0;
    }
    return acos(x);
}

/**
 * \param a		A vector.
 * \param b		A vector.
 * \return		The distance.
 *
 * Computes the euclidean distance between two vectors.
 */
inline double cvl_vector3_dist_euc(const cvl_vector3_t a, const cvl_vector3_t b)
{
    cvl_vector3_t d;
    cvl_vector3_sub(a, b, d);
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
	int sample_x, int sample_y, int dist_x, int dist_y, double factor)
{
    int width = cvl_field_width(field) / sample_x * dist_x;
    int height = cvl_field_height(field) / sample_y * dist_y;
    cvl_frame_t *vis = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
    cvl_frame_fill_rect(vis, 0, 0, width, height, cvl_pixel_gray(0xff));

    for (int y = 0; y < cvl_field_height(field); y += sample_y)
    {
	for (int x = 0; x < cvl_field_width(field); x += sample_x)
	{
	    cvl_vector3_t v;
	    int needle_pos_x = x / sample_x * dist_x;
	    int needle_pos_y = y / sample_y * dist_y;
	    cvl_field_get(field, x, y, &v);
	    double n = cvl_vector3_norm_euc(v);
	    double alpha = (isfinite(v[2] / n) ? (M_PI_2 - acos(v[2] / cvl_vector3_norm_euc(v))) : 0.0);
	    int delta_x = cvl_iround(v[0] * factor * cos(alpha));
	    int delta_y = cvl_iround(v[1] * factor * sin(alpha));
	    cvl_draw_circle(vis, cvl_pixel_gray(160), needle_pos_x, needle_pos_y, 1);
	    cvl_draw_line(vis, cvl_pixel_gray(0), needle_pos_x, needle_pos_y,
		    needle_pos_x + delta_x, needle_pos_y + delta_y);
	}
    }
    return vis;
}
