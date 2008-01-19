/*
 * cvl_visualization.h
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

#ifndef CVL_VISUALIZATION_H
#define CVL_VISUALIZATION_H

#include "cvl_frame.h"

void cvl_visualize_vector2_color(cvl_frame_t *dst, cvl_frame_t *src);

cvl_frame_t *cvl_visualize_vector2_needle(cvl_frame_t *frame, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor);

#endif
