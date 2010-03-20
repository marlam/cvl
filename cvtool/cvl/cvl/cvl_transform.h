/*
 * cvl_transform.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
 * Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_TRANSFORM_H
#define CVL_TRANSFORM_H

typedef enum
{
    CVL_NONE			= 0,
    CVL_BILINEAR		= 1,
    CVL_BIQUADRATIC		= 2,
    CVL_BICUBIC			= 3,
    CVL_BICUBIC_B_SPLINE 	= 4,
    CVL_BICUBIC_CR_SPLINE	= 5
} cvl_interpolation_type_t;

extern CVL_EXPORT cvl_frame_t *cvl_affine(cvl_frame_t *frame, const float *matrix, 
	cvl_interpolation_type_t interpolation_type, const float *val);
extern CVL_EXPORT cvl_frame_t *cvl_rotate(cvl_frame_t *frame, float angle,
	cvl_interpolation_type_t interpolation_type, const float *val);
extern CVL_EXPORT cvl_frame_t *cvl_shear(cvl_frame_t *frame, float shear_angle_x, float shear_angle_y,
	cvl_interpolation_type_t interpolation_type, const float *val);
extern CVL_EXPORT cvl_frame_t *cvl_scale(cvl_frame_t *frame, int new_width, int new_height,
	cvl_interpolation_type_t interpolation_type);
extern CVL_EXPORT void cvl_flip(cvl_frame_t *dst, cvl_frame_t *src);
extern CVL_EXPORT void cvl_flop(cvl_frame_t *dst, cvl_frame_t *src);

#endif
