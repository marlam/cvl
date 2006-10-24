/*
 * cvl_misc.h
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

#ifndef CVL_MISC_H
#define CVL_MISC_H

#include <stddef.h>
#include <stdbool.h>

#include "cvl_pixel.h"
#include "cvl_frame.h"
#include "cvl_field.h"


void cvl_sort_array_pixel(cvl_pixel_t *a, size_t a_len);
void cvl_sort_array_int(int *a, size_t a_len);
void cvl_sort_array_float(float *a, size_t a_len);
void cvl_sort_array_double(double *a, size_t a_len);

const char *cvl_check_version(const char *req_version);

void cvl_diffstat(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	cvl_frame_t **diff, uint8_t *min, uint8_t *max, uint8_t *med, double *mean, double *dev);
cvl_frame_t *cvl_diff(const cvl_frame_t *f1, const cvl_frame_t *f2);

cvl_field_t *cvl_sedt(const cvl_frame_t *frame);
cvl_field_t **cvl_sedt3d(cvl_frame_t * const *frames, int depth);

cvl_field_t *cvl_skeleton(const cvl_frame_t *frame);
cvl_field_t **cvl_skeleton3d(cvl_frame_t * const *frames, int depth);

cvl_field_t *cvl_mat(const cvl_field_t *sedt);
cvl_field_t **cvl_mat3d(cvl_field_t * const *sedts, int depth);

#endif
