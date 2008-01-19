/*
 * cvl_filter.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_FILTER_H
#define CVL_FILTER_H

#include "cvl_frame.h"

void cvl_convolve(cvl_frame_t *dst, cvl_frame_t *src, const float *kernel, int h_len, int v_len);
void cvl_convolve_separable(cvl_frame_t *dst, cvl_frame_t *src, 
	const float *h, int h_len, const float *v, int v_len);
void cvl_convolve3d(cvl_frame_t *dst, cvl_frame_t **srcs, 
	const float *kernel, int h_len, int v_len, int t_len);
void cvl_convolve3d_separable(cvl_frame_t *dst, cvl_frame_t **srcs, 
	const float *h, int h_len, const float *v, int v_len, const float *t, int t_len);

void cvl_mean(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v);
void cvl_mean3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t);

float cvl_gauss_k_to_sigma(int k);
int cvl_gauss_sigma_to_k(float sigma);
void cvl_gauss(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v, float sigma_h, float sigma_v);
void cvl_gauss3d(cvl_frame_t *dst, cvl_frame_t **srcs, 
	int k_h, int k_v, int k_t, float sigma_h, float sigma_v, float sigma_t);

void cvl_min(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v);
void cvl_min3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t);

void cvl_max(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v);
void cvl_max3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t);

void cvl_median(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v);
void cvl_median3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t);
void cvl_median_separated(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v);
void cvl_median3d_separated(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t);

void cvl_laplace(cvl_frame_t *dst, cvl_frame_t *src, float c);
void cvl_unsharpmask(cvl_frame_t *dst, cvl_frame_t *src, cvl_frame_t *smoothed, float c);

#endif
