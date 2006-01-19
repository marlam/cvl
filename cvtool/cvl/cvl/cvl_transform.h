/*
 * cvl_transform.h
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

#ifndef CVL_TRANSFORM_H
#define CVL_TRANSFORM_H

#include "cvl_frame.h"

typedef enum
{
    CVL_INTERPOLATION_NONE = 0,
    CVL_INTERPOLATION_BILINEAR = 1
} cvl_interpolation_type_t;

cvl_frame_t *cvl_frame_flip(const cvl_frame_t *frame);
cvl_frame_t *cvl_frame_flop(const cvl_frame_t *frame);
cvl_frame_t *cvl_frame_resize(const cvl_frame_t *frame, cvl_color_t color, 
	int new_width, int new_height, int new_x_offset, int new_y_offset);
cvl_frame_t *cvl_frame_cut(const cvl_frame_t *frame, int x, int y, int w, int h);
cvl_frame_t *cvl_frame_scale(const cvl_frame_t *frame, cvl_interpolation_type_t interpolation_type, 
	int new_width, int new_height);
cvl_frame_t *cvl_frame_rotate(const cvl_frame_t *frame, cvl_interpolation_type_t interpolation_type, 
	cvl_color_t color, double angle);
cvl_frame_t *cvl_frame_shear(const cvl_frame_t *frame, cvl_interpolation_type_t interpolation_type,
	cvl_color_t color, double shear_angle_x, double shear_angle_y);
cvl_frame_t *cvl_frame_affine(const cvl_frame_t *frame, cvl_interpolation_type_t interpolation_type,
	cvl_color_t color, const double *matrix);

#endif
