/*
 * cvl_wavelets.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_WAVELETS_H
#define CVL_WAVELETS_H

#include "cvl_frame.h"

void cvl_wavelets_dwt(cvl_frame_t *dst, cvl_frame_t *src, cvl_frame_t *tmp, int D, int level);
void cvl_wavelets_idwt(cvl_frame_t *dst, cvl_frame_t *src, cvl_frame_t *tmp, int D, int level);
void cvl_wavelets_hard_thresholding(cvl_frame_t *dst, cvl_frame_t *src, int level, const float *T);
void cvl_wavelets_soft_thresholding(cvl_frame_t *dst, cvl_frame_t *src, int level, const float *T);

#endif
