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

typedef int cvl_vector2i_t[2];
typedef double cvl_vector2_t[2];
typedef double cvl_vector3_t[3];

bool cvl_vector2i_to_string(char *s, size_t s_size, cvl_vector2i_t v);
bool cvl_vector2i_from_string(const char *s, cvl_vector2i_t v);
inline void cvl_vector2i_copy(cvl_vector2i_t dst, const cvl_vector2i_t src);
inline void cvl_vector2i_zero(cvl_vector2i_t v);
inline void cvl_vector2i_add(const cvl_vector2i_t a, const cvl_vector2i_t b, cvl_vector2i_t result);
inline void cvl_vector2i_sub(const cvl_vector2i_t a, const cvl_vector2i_t b, cvl_vector2i_t result);
inline void cvl_vector2i_scale(cvl_vector2i_t v, double lambda);
inline int cvl_vector2i_mul(const cvl_vector2i_t a, const cvl_vector2i_t b);
inline double cvl_vector2i_norm_euc(const cvl_vector2i_t v);
inline double cvl_vector2i_dist_arc(const cvl_vector2i_t a, const cvl_vector2i_t b);
inline double cvl_vector2i_dist_euc(const cvl_vector2i_t a, const cvl_vector2i_t b);
cvl_frame_t *cvl_vector2i_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor);

bool cvl_vector2_to_string(char *s, size_t s_size, cvl_vector2_t v);
bool cvl_vector2_from_string(const char *s, cvl_vector2_t v);
inline void cvl_vector2_copy(cvl_vector2_t dst, const cvl_vector2_t src);
inline void cvl_vector2_zero(cvl_vector2_t v);
inline bool cvl_vector2_equal(const cvl_vector2_t a, const cvl_vector2_t b, double epsilon);
inline void cvl_vector2_add(const cvl_vector2_t a, const cvl_vector2_t b, cvl_vector2_t result);
inline void cvl_vector2_sub(const cvl_vector2_t a, const cvl_vector2_t b, cvl_vector2_t result);
inline void cvl_vector2_scale(cvl_vector2_t v, double lambda);
inline double cvl_vector2_mul(const cvl_vector2_t a, const cvl_vector2_t b);
inline double cvl_vector2_norm_euc(const cvl_vector2_t v);
inline void cvl_vector2_normalize(cvl_vector2_t v);
inline double cvl_vector2_dist_arc(const cvl_vector2_t a, const cvl_vector2_t b);
inline double cvl_vector2_dist_euc(const cvl_vector2_t a, const cvl_vector2_t b);
void cvl_vector2_avg_bary(const cvl_vector2_t *vectors, int n, cvl_vector2_t result);
cvl_frame_t *cvl_vector2_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor);

bool cvl_vector3_to_string(char *s, size_t s_size, cvl_vector3_t v);
bool cvl_vector3_from_string(const char *s, cvl_vector3_t v);
inline void cvl_vector3_copy(cvl_vector3_t dst, const cvl_vector3_t src);
inline void cvl_vector3_zero(cvl_vector3_t v);
inline bool cvl_vector3_equal(const cvl_vector3_t a, const cvl_vector3_t b, double epsilon);
inline void cvl_vector3_add(const cvl_vector3_t a, const cvl_vector3_t b, cvl_vector3_t result);
inline void cvl_vector3_sub(const cvl_vector3_t a, const cvl_vector3_t b, cvl_vector3_t result);
inline void cvl_vector3_scale(cvl_vector3_t v, double lambda);
inline double cvl_vector3_mul(const cvl_vector3_t a, const cvl_vector3_t b);
inline void cvl_vector3_crossproduct(const cvl_vector3_t a, const cvl_vector3_t b, cvl_vector3_t result);
inline double cvl_vector3_norm_euc(const cvl_vector3_t v);
inline void cvl_vector3_normalize(cvl_vector3_t v);
inline double cvl_vector3_dist_arc(const cvl_vector3_t a, const cvl_vector3_t b);
inline double cvl_vector3_dist_euc(const cvl_vector3_t a, const cvl_vector3_t b);
cvl_frame_t *cvl_vector3_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor);

#endif
