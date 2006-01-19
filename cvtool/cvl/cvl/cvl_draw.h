/*
 * cvl_draw.h
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

#ifndef CVL_DRAW_H
#define CVL_DRAW_H

#include "cvl_pixel.h"
#include "cvl_frame.h"

inline void cvl_draw_point(cvl_frame_t *frame, cvl_pixel_t p, int x, int y);
void cvl_draw_line(cvl_frame_t *frame, cvl_pixel_t p, int x0, int y0, int x1, int y1);
void cvl_draw_polygon(cvl_frame_t *frame, cvl_pixel_t p, int *points, int numpoints);
void cvl_draw_polygon_filled(cvl_frame_t *frame, cvl_pixel_t border, cvl_pixel_t fill,
	int *points, int numpoints);
void cvl_draw_circle(cvl_frame_t *frame, cvl_pixel_t p, int x0, int y0, int r);
void cvl_draw_circle_filled(cvl_frame_t *frame, cvl_pixel_t border, cvl_pixel_t fill, 
	int x0, int y0, int r);
    
#endif
