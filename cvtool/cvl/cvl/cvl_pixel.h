/*
 * cvl_pixel.h
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

#ifndef CVL_PIXEL_H
#define CVL_PIXEL_H

#include <stdint.h>

typedef uint32_t cvl_pixel_t;

typedef enum 
{ 
    CVL_PIXEL_GRAY = 0,
    CVL_PIXEL_RGB  = 1,
    CVL_PIXEL_YUV  = 2
} cvl_pixel_type_t;

inline cvl_pixel_t cvl_pixel_clip(int x);
inline cvl_pixel_t cvl_pixel_clamp(int xmin, int xmax, int x);

inline cvl_pixel_t cvl_pixel_gray(cvl_pixel_t gray);
inline cvl_pixel_t cvl_pixel_rgb(cvl_pixel_t r, cvl_pixel_t g, cvl_pixel_t b);
inline cvl_pixel_t cvl_pixel_yuv(cvl_pixel_t y, cvl_pixel_t u, cvl_pixel_t v);

inline cvl_pixel_t cvl_pixel_gray_to_rgb(cvl_pixel_t gray);
inline cvl_pixel_t cvl_pixel_gray_to_yuv(cvl_pixel_t gray);

inline cvl_pixel_t cvl_pixel_rgb_to_r(cvl_pixel_t rgb);
inline cvl_pixel_t cvl_pixel_rgb_to_g(cvl_pixel_t rgb);
inline cvl_pixel_t cvl_pixel_rgb_to_b(cvl_pixel_t rgb);
inline cvl_pixel_t cvl_pixel_rgb_to_gray(cvl_pixel_t rgb);
inline cvl_pixel_t cvl_pixel_rgb_to_yuv(cvl_pixel_t rgb);

inline cvl_pixel_t cvl_pixel_yuv_to_y(cvl_pixel_t yuv);
inline cvl_pixel_t cvl_pixel_yuv_to_u(cvl_pixel_t yuv);
inline cvl_pixel_t cvl_pixel_yuv_to_v(cvl_pixel_t yuv);
inline cvl_pixel_t cvl_pixel_yuv_to_gray(cvl_pixel_t yuv);
inline cvl_pixel_t cvl_pixel_yuv_to_rgb(cvl_pixel_t yuv);

inline cvl_pixel_t cvl_pixel_to_gray(cvl_pixel_t p, cvl_pixel_type_t type);
inline cvl_pixel_t cvl_pixel_to_rgb(cvl_pixel_t p, cvl_pixel_type_t type);
inline cvl_pixel_t cvl_pixel_to_yuv(cvl_pixel_t p, cvl_pixel_type_t type);
inline cvl_pixel_t cvl_pixel_convert(cvl_pixel_type_t dst_type, cvl_pixel_t p, cvl_pixel_type_t src_type);

#endif
