/*
 * cvl_hdr.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CVL_HDR_H
#define CVL_HDR_H

#include "cvl_frame.h"

void cvl_tonemap_drago03(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, float bias, float max_disp_lum);

void cvl_tonemap_durand02(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, 
	int k, float sigma_spatial, float sigma_luminance, float base_contrast);

#endif
