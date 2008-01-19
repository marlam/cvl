/*
 * cvl_basic.h
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

#ifndef CVL_BASIC_H
#define CVL_BASIC_H

#include "cvl_frame.h"
#include "cvl_color.h"

void cvl_get(cvl_frame_t *frame, int channel, int x, int y, float *result);

void cvl_fill_rect(cvl_frame_t *frame, int x, int y, int w, int h, const float *val);

void cvl_copy_rect(cvl_frame_t *dst, int dst_x, int dst_y,
	cvl_frame_t *src, int src_x, int src_y, int rwidth, int rheight);

void cvl_copy(cvl_frame_t *dst, cvl_frame_t *src);

void cvl_cut_rect(cvl_frame_t *dst, cvl_frame_t *src, int x, int y);

void cvl_resize(cvl_frame_t *dst, cvl_frame_t *src, int x_offset, int y_offset, const float *val);

#endif
