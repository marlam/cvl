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

#include "cvl_math.h"


typedef uint32_t cvl_pixel_t;

typedef enum 
{ 
    CVL_PIXEL_GRAY = 0,
    CVL_PIXEL_RGB  = 1,
    CVL_PIXEL_YUV  = 2
} cvl_pixel_type_t;

cvl_pixel_t cvl_pixel_clip(int x);
extern inline cvl_pixel_t cvl_pixel_clip(int x)
{
    return (x < 0 ? 0 : (x > 255) ? 255 : x);
}

cvl_pixel_t cvl_pixel_clamp(int xmin, int xmax, int x);
extern inline cvl_pixel_t cvl_pixel_clamp(int xmin, int xmax, int x)
{
    return cvl_iround((double)((x - xmin) * 255) / (double)(xmax - xmin));
}

cvl_pixel_t cvl_pixel_gray(cvl_pixel_t gray);
extern inline cvl_pixel_t cvl_pixel_gray(cvl_pixel_t gray)
{
    return (gray & 0xff);
}

cvl_pixel_t cvl_pixel_yuv(cvl_pixel_t y, cvl_pixel_t u, cvl_pixel_t v);
extern inline cvl_pixel_t cvl_pixel_yuv(cvl_pixel_t y, cvl_pixel_t u, cvl_pixel_t v)
{
    return (y << 16) | (u << 8) | v;
}

cvl_pixel_t cvl_pixel_rgb(cvl_pixel_t r, cvl_pixel_t g, cvl_pixel_t b);
extern inline cvl_pixel_t cvl_pixel_rgb(cvl_pixel_t r, cvl_pixel_t g, cvl_pixel_t b)
{
    return (r << 16) | (g << 8) | b;
}

cvl_pixel_t cvl_pixel_gray_to_rgb(cvl_pixel_t gray);
extern inline cvl_pixel_t cvl_pixel_gray_to_rgb(cvl_pixel_t gray)
{
    return cvl_pixel_rgb(gray, gray, gray);
}

cvl_pixel_t cvl_pixel_gray_to_yuv(cvl_pixel_t gray);
extern inline cvl_pixel_t cvl_pixel_gray_to_yuv(cvl_pixel_t gray)
{
    return cvl_pixel_yuv((((int)cvl_pixel_gray(gray) * 220) >> 8) + 16, 128, 128);
}

cvl_pixel_t cvl_pixel_rgb_to_r(cvl_pixel_t rgb);
extern inline cvl_pixel_t cvl_pixel_rgb_to_r(cvl_pixel_t rgb)
{
    return (rgb >> 16) & 0xff;
}

cvl_pixel_t cvl_pixel_rgb_to_g(cvl_pixel_t rgb);
extern inline cvl_pixel_t cvl_pixel_rgb_to_g(cvl_pixel_t rgb)
{
    return (rgb >> 8) & 0xff;
}

cvl_pixel_t cvl_pixel_rgb_to_b(cvl_pixel_t rgb);
extern inline cvl_pixel_t cvl_pixel_rgb_to_b(cvl_pixel_t rgb)
{
    return rgb & 0xff;
}

cvl_pixel_t cvl_pixel_rgb_to_gray(cvl_pixel_t rgb);
extern inline cvl_pixel_t cvl_pixel_rgb_to_gray(cvl_pixel_t rgb)
{
    return ((cvl_pixel_rgb_to_r(rgb) + cvl_pixel_rgb_to_g(rgb) + cvl_pixel_rgb_to_b(rgb)) / 3);
}

cvl_pixel_t cvl_pixel_rgb_to_yuv(cvl_pixel_t rgb);
extern inline cvl_pixel_t cvl_pixel_rgb_to_yuv(cvl_pixel_t rgb)
{
    int r = cvl_pixel_rgb_to_r(rgb);
    int g = cvl_pixel_rgb_to_g(rgb);
    int b = cvl_pixel_rgb_to_b(rgb);
    int y = ((  66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
    int u = ((- 38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
    int v = (( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128;
    return cvl_pixel_yuv(y, u, v);
}

cvl_pixel_t cvl_pixel_yuv_to_y(cvl_pixel_t yuv);
extern inline cvl_pixel_t cvl_pixel_yuv_to_y(cvl_pixel_t yuv)
{
    return (yuv >> 16) & 0xff;
}

cvl_pixel_t cvl_pixel_yuv_to_u(cvl_pixel_t yuv);
extern inline cvl_pixel_t cvl_pixel_yuv_to_u(cvl_pixel_t yuv)
{
    return (yuv >> 8) & 0xff;
}

cvl_pixel_t cvl_pixel_yuv_to_v(cvl_pixel_t yuv);
extern inline cvl_pixel_t cvl_pixel_yuv_to_v(cvl_pixel_t yuv)
{
    return yuv & 0xff;
}

cvl_pixel_t cvl_pixel_yuv_to_gray(cvl_pixel_t yuv);
extern inline cvl_pixel_t cvl_pixel_yuv_to_gray(cvl_pixel_t yuv)
{
    return cvl_pixel_gray((cvl_pixel_clip((int)cvl_pixel_yuv_to_y(yuv) - 16) * 298) >> 8);
}

cvl_pixel_t cvl_pixel_yuv_to_rgb(cvl_pixel_t yuv);
extern inline cvl_pixel_t cvl_pixel_yuv_to_rgb(cvl_pixel_t yuv)
{
    int c = cvl_pixel_yuv_to_y(yuv) - 16;
    int d = cvl_pixel_yuv_to_u(yuv) - 128;
    int e = cvl_pixel_yuv_to_v(yuv) - 128;
    int r = cvl_pixel_clip((298 * c           + 409 * e + 128) >> 8);
    int g = cvl_pixel_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
    int b = cvl_pixel_clip((298 * c + 516 * d           + 128) >> 8);
    return cvl_pixel_rgb(r, g, b);
}

cvl_pixel_t cvl_pixel_to_gray(cvl_pixel_t p, cvl_pixel_type_t type);
extern inline cvl_pixel_t cvl_pixel_to_gray(cvl_pixel_t p, cvl_pixel_type_t type)
{
    return (type == CVL_PIXEL_GRAY ? p 
	    : (type == CVL_PIXEL_RGB ? cvl_pixel_rgb_to_gray(p) 
		: cvl_pixel_yuv_to_gray(p)));
}
    
cvl_pixel_t cvl_pixel_to_rgb(cvl_pixel_t p, cvl_pixel_type_t type);
extern inline cvl_pixel_t cvl_pixel_to_rgb(cvl_pixel_t p, cvl_pixel_type_t type)
{
    return (type == CVL_PIXEL_RGB ? p 
	    : (type == CVL_PIXEL_GRAY ? cvl_pixel_gray_to_rgb(p) 
		: cvl_pixel_yuv_to_rgb(p)));
}
    
cvl_pixel_t cvl_pixel_to_yuv(cvl_pixel_t p, cvl_pixel_type_t type);
extern inline cvl_pixel_t cvl_pixel_to_yuv(cvl_pixel_t p, cvl_pixel_type_t type)
{
    return (type == CVL_PIXEL_YUV ? p 
	    : (type == CVL_PIXEL_GRAY ? cvl_pixel_gray_to_yuv(p) 
		: cvl_pixel_rgb_to_yuv(p)));
}

cvl_pixel_t cvl_pixel_convert(cvl_pixel_type_t dst_type, cvl_pixel_t p, cvl_pixel_type_t src_type);
extern inline cvl_pixel_t cvl_pixel_convert(cvl_pixel_type_t dst_type, cvl_pixel_t p, cvl_pixel_type_t src_type)
{
    return (dst_type == CVL_PIXEL_GRAY ? cvl_pixel_to_gray(p, src_type)
	    : (dst_type == CVL_PIXEL_RGB ? cvl_pixel_to_rgb(p, src_type)
		: cvl_pixel_to_yuv(p, src_type)));
}

#endif
