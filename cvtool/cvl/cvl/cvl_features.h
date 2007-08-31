/*
 * cvl_features.h
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

#ifndef CVL_FEATURES_H
#define CVL_FEATURES_H

#include "cvl_frame.h"

void cvl_edge_sobel(cvl_frame_t *dst, cvl_frame_t *src, int channel);
void cvl_edge_canny_nms(cvl_frame_t *dst, cvl_frame_t *src);
void cvl_edge_canny_hysterese(cvl_frame_t *dst, cvl_frame_t *src, float tl, float th);
void cvl_edge_canny(cvl_frame_t *dst, cvl_frame_t *src, int channel, float sigma, float tl, float th);

#endif
