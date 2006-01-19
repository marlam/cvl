/*
 * cvl_convolve.h
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

#ifndef CVL_CONVOLVE_H
#define CVL_CONVOLVE_H

#include "cvl_frame.h"

cvl_frame_t *cvl_frame_convolve_separable(const cvl_frame_t *frame, 
	const int *h, int h_len, const int *v, int v_len);
cvl_frame_t *cvl_frame_convolve(const cvl_frame_t *frame, 
	const int *kernel, int dim_y, int dim_x);

cvl_frame_t *cvl_frame_convolve3d_separable(const cvl_frame_t *frames[], 
	const int *h, int h_len, const int *v, int v_len, const int *t, int t_len);
cvl_frame_t *cvl_frame_convolve3d(const cvl_frame_t *frames[],
	const int *kernel, int dim_t, int dim_y, int dim_x);

#endif
