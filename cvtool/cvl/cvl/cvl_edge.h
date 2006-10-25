/*
 * cvl_edge.h
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

#ifndef CVL_EDGE_H
#define CVL_EDGE_H

#include "cvl_field.h"
#include "cvl_frame.h"
#include "cvl_math.h"

extern inline void cvl_differentiate_sobel(const cvl_frame_t *frame, int x, int y, int *fx, int *fy)
{
    *fx = cvl_iroundf((float)(
		(- 1 * (int)cvl_frame_get(frame, x - 1, y - 1) 
		 + 1 * (int)cvl_frame_get(frame, x + 1, y - 1)
		 - 2 * (int)cvl_frame_get(frame, x - 1, y    ) 
		 + 2 * (int)cvl_frame_get(frame, x + 1, y    )
		 - 1 * (int)cvl_frame_get(frame, x - 1, y + 1) 
		 + 1 * (int)cvl_frame_get(frame, x + 1, y + 1))) / 4.0f);
    *fy = cvl_iroundf((float)(
		(+ 1 * (int)cvl_frame_get(frame, x - 1, y - 1) 
		 - 1 * (int)cvl_frame_get(frame, x - 1, y + 1)
		 + 2 * (int)cvl_frame_get(frame, x    , y - 1) 
		 - 2 * (int)cvl_frame_get(frame, x    , y + 1)
		 + 1 * (int)cvl_frame_get(frame, x + 1, y - 1) 
		 - 1 * (int)cvl_frame_get(frame, x + 1, y + 1))) / 4.0f);
}

extern inline void cvl_differentiate_sobel_r(const cvl_frame_t *frame, int x, int y, int *fx, int *fy)
{
    *fx = cvl_iroundf((float)(
		(- 1 * (int)cvl_frame_get_r(frame, x - 1, y - 1) 
		 + 1 * (int)cvl_frame_get_r(frame, x + 1, y - 1)
		 - 2 * (int)cvl_frame_get_r(frame, x - 1, y    ) 
		 + 2 * (int)cvl_frame_get_r(frame, x + 1, y    )
		 - 1 * (int)cvl_frame_get_r(frame, x - 1, y + 1) 
		 + 1 * (int)cvl_frame_get_r(frame, x + 1, y + 1))) / 4.0f);
    *fy = cvl_iroundf((float)(
		(+ 1 * (int)cvl_frame_get_r(frame, x - 1, y - 1) 
		 - 1 * (int)cvl_frame_get_r(frame, x - 1, y + 1)
		 + 2 * (int)cvl_frame_get_r(frame, x    , y - 1) 
		 - 2 * (int)cvl_frame_get_r(frame, x    , y + 1)
		 + 1 * (int)cvl_frame_get_r(frame, x + 1, y - 1) 
		 - 1 * (int)cvl_frame_get_r(frame, x + 1, y + 1))) / 4.0f);
}

cvl_frame_t *cvl_edge_sobel(const cvl_frame_t *frame, cvl_field_t *edge_dir_field);
cvl_frame_t *cvl_edge_canny(const cvl_frame_t *frame, 
	double sigma, int tl, int th, cvl_field_t *edge_dir_field);

#endif
