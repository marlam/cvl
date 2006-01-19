/*
 * cvl_frame.h
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

#ifndef CVL_FRAME_H
#define CVL_FRAME_H

#include "cvl_pixel.h"
#include "cvl_field.h"

typedef struct
{
    int _width;
    int _height;
    int _pixel_type;
    cvl_pixel_t *_p;
} cvl_frame_t;

cvl_frame_t *cvl_frame_new(cvl_pixel_type_t pixel_type, int width, int height);
void cvl_frame_free(cvl_frame_t *frame);

inline cvl_pixel_type_t cvl_frame_pixel_type(const cvl_frame_t *frame);
inline int cvl_frame_width(const cvl_frame_t *frame);
inline int cvl_frame_height(const cvl_frame_t *frame);

void cvl_frame_zero(cvl_frame_t *frame);
void cvl_frame_copy(cvl_frame_t *dst, const cvl_frame_t *src);
cvl_frame_t *cvl_frame_clone(const cvl_frame_t *frame);

inline cvl_pixel_t cvl_frame_get(const cvl_frame_t *frame, int x, int y);
inline cvl_pixel_t cvl_frame_get_r(const cvl_frame_t *frame, int x, int y);
inline cvl_pixel_t cvl_frame_get_i(const cvl_frame_t *frame, int i);

inline void cvl_frame_set(cvl_frame_t *frame, int x, int y, cvl_pixel_t p);
inline void cvl_frame_set_i(cvl_frame_t *frame, int i, cvl_pixel_t p);

void cvl_frame_to_gray(cvl_frame_t *frame);
void cvl_frame_to_rgb(cvl_frame_t *frame);
void cvl_frame_to_yuv(cvl_frame_t *frame);
inline void cvl_frame_convert(cvl_frame_t *frame, cvl_pixel_type_t type);

void cvl_frame_rgb_to_r(cvl_frame_t *frame);
void cvl_frame_rgb_to_g(cvl_frame_t *frame);
void cvl_frame_rgb_to_b(cvl_frame_t *frame);

void cvl_frame_fill_rect(cvl_frame_t *frame, int x, int y, int w, int h, cvl_pixel_t p);

void cvl_frame_copy_rect(cvl_frame_t *dst, int dst_x, int dst_y, 
	const cvl_frame_t *src, int src_x, int src_y, int rwidth, int rheight);

#endif
