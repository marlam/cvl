/*
 * cvl_filter.h
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

#ifndef CVL_FILTER_H
#define CVL_FILTER_H

#include "cvl_frame.h"

double cvl_filter_gauss_k_to_sigma(int k);
int cvl_filter_gauss_sigma_to_k(double sigma);

cvl_frame_t *cvl_filter_gauss(const cvl_frame_t *frame, int k_h, int k_v, double sigma_h, double sigma_v);
cvl_frame_t *cvl_filter_average(const cvl_frame_t *frame, int k_h, int k_v);
cvl_frame_t *cvl_filter_median(const cvl_frame_t *frame, int k_h, int k_v);

cvl_frame_t *cvl_filter3d_gauss(const cvl_frame_t *frames[], 
	int k_h, int k_v, int k_t, double sigma_h, double sigma_v, double sigma_t);
cvl_frame_t *cvl_filter3d_average(const cvl_frame_t *frames[], int k_h, int k_v, int k_t);
cvl_frame_t *cvl_filter3d_median(const cvl_frame_t *frames[], int k_h, int k_v, int k_t);

#endif
