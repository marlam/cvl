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
inline void cvl_vector2i_scale(int *v, double lambda);
inline int cvl_vector2i_mul(const int *a, const int *b);
inline double cvl_vector2i_norm_euc(const int *v);
inline double cvl_vector2i_dist_arc(const int *a, const int *b);
inline double cvl_vector2i_dist_euc(const int *a, const int *b);
cvl_frame_t *cvl_vector2i_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor);

bool cvl_vector2_to_string(char *s, size_t s_size, const double *v);
bool cvl_vector2_from_string(const char *s, double *v);
inline void cvl_vector2_copy(double *dst, const double *src);
inline void cvl_vector2_zero(double *v);
inline bool cvl_vector2_equal(const double *a, const double *b, double epsilon);
inline void cvl_vector2_add(double *result, const double *a, const double *b);
inline void cvl_vector2_sub(double *result, const double *a, const double *b);
inline void cvl_vector2_scale(double *v, double lambda);
inline double cvl_vector2_mul(const double *a, const double *b);
inline double cvl_vector2_norm_euc(const double *v);
inline void cvl_vector2_normalize(double *v);
inline double cvl_vector2_dist_arc(const double *a, const double *b);
inline double cvl_vector2_dist_euc(const double *a, const double *b);
cvl_frame_t *cvl_vector2_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor);

bool cvl_vector3_to_string(char *s, size_t s_size, const double *v);
bool cvl_vector3_from_string(const char *s, double *v);
inline void cvl_vector3_copy(double *dst, const double *src);
inline void cvl_vector3_zero(double *v);
inline bool cvl_vector3_equal(const double *a, const double *b, double epsilon);
inline void cvl_vector3_add(double *result, const double *a, const double *b);
inline void cvl_vector3_sub(double *result, const double *a, const double *b);
inline void cvl_vector3_scale(double *v, double lambda);
inline double cvl_vector3_mul(const double *a, const double *b);
inline void cvl_vector3_crossproduct(double *result, const double *a, const double *b);
inline double cvl_vector3_norm_euc(const double *v);
inline void cvl_vector3_normalize(double *v);
inline double cvl_vector3_dist_arc(const double *a, const double *b);
inline double cvl_vector3_dist_euc(const double *a, const double *b);
cvl_frame_t *cvl_vector3_visualize(const cvl_field_t *field, 
	int sample_x, int sample_y, int dist_x, int dist_y, double factor);

#endif
