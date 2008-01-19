/*
 * cvl_misc.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_MISC_H
#define CVL_MISC_H

#include "cvl_frame.h"

typedef enum
{
    CVL_REDUCE_MIN			= 0,
    CVL_REDUCE_MIN_GREATER_ZERO		= 1,
    CVL_REDUCE_ABSMIN			= 2,
    CVL_REDUCE_ABSMIN_GREATER_ZERO	= 3,
    CVL_REDUCE_MAX			= 4,
    CVL_REDUCE_ABSMAX			= 5,
    CVL_REDUCE_SUM			= 6
} cvl_reduce_mode_t;

void cvl_resize_seq(cvl_frame_t *dst, cvl_frame_t *src, const float *fillvalue);

void cvl_reduce(cvl_frame_t *frame, cvl_reduce_mode_t mode, int channel, float *result);

void cvl_sort(cvl_frame_t *dst, cvl_frame_t *src, int channel);

void cvl_quantil(cvl_frame_t *frame, int channel, float q, float *result);

void cvl_statistics(cvl_frame_t *frame, float *min, float *max, float *median, 
	float *mean, float *stddev, float *dynrange);

void cvl_diff(cvl_frame_t *dst, cvl_frame_t *src1, cvl_frame_t *src2);

void cvl_pyramid_gaussian(cvl_frame_t *frame, int n, cvl_frame_t **pyramid);

void cvl_pyramid_laplacian(cvl_frame_t **gaussian_pyramid, int n, cvl_frame_t **laplacian_pyramid);

void cvl_histogram(cvl_frame_t *frame, int channel, int bins, const float *min, const float *max, int *histogram);

#endif
