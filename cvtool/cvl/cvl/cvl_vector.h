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

bool cvl_vector2i_to_string(char *s, size_t s_size, const int *v);
bool cvl_vector2i_from_string(const char *s, int *v);
inline void cvl_vector2i_copy(int *dst, const int *src);
inline void cvl_vector2i_zero(int *v);
inline void cvl_vector2i_add(int *result, const int *a, const int *b);
inline void cvl_vector2i_sub(int *result, const int *a, const int *b);
inline void cvl_vector2i_scale(int *v, float lambda);
inline int cvl_vector2i_mul(const int *a, const int *b);
inline float cvl_vector2i_norm_euc(const int *v);
inline float cvl_vector2i_dist_arc(const int *a, const int *b);
inline float cvl_vector2i_dist_euc(const int *a, const int *b);
cvl_frame_t *cvl_vector2i_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

bool cvl_vector2_to_string(char *s, size_t s_size, const float *v);
bool cvl_vector2_from_string(const char *s, float *v);
inline void cvl_vector2_copy(float *dst, const float *src);
inline void cvl_vector2_zero(float *v);
inline bool cvl_vector2_equal(const float *a, const float *b, float epsilon);
inline void cvl_vector2_add(float *result, const float *a, const float *b);
inline void cvl_vector2_sub(float *result, const float *a, const float *b);
inline void cvl_vector2_scale(float *v, float lambda);
inline float cvl_vector2_mul(const float *a, const float *b);
inline float cvl_vector2_norm_euc(const float *v);
inline void cvl_vector2_normalize(float *v);
inline float cvl_vector2_dist_arc(const float *a, const float *b);
inline float cvl_vector2_dist_euc(const float *a, const float *b);
cvl_frame_t *cvl_vector2_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

bool cvl_vector3_to_string(char *s, size_t s_size, const float *v);
bool cvl_vector3_from_string(const char *s, float *v);
inline void cvl_vector3_copy(float *dst, const float *src);
inline void cvl_vector3_zero(float *v);
inline bool cvl_vector3_equal(const float *a, const float *b, float epsilon);
inline void cvl_vector3_add(float *result, const float *a, const float *b);
inline void cvl_vector3_sub(float *result, const float *a, const float *b);
inline void cvl_vector3_scale(float *v, float lambda);
inline float cvl_vector3_mul(const float *a, const float *b);
inline void cvl_vector3_crossproduct(float *result, const float *a, const float *b);
inline float cvl_vector3_norm_euc(const float *v);
inline void cvl_vector3_normalize(float *v);
inline float cvl_vector3_dist_arc(const float *a, const float *b);
inline float cvl_vector3_dist_euc(const float *a, const float *b);
cvl_frame_t *cvl_vector3_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

#endif
