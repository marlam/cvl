/*
 * cvl_pixel.c
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

/** 
 * \file cvl_pixel.h
 * \brief Pixel handling.
 *
 * Handling the pixel types RGB, YUV, and gray.
 */

#include "config.h"

#include <stdint.h>

#include "cvl/cvl_math.h"
#include "cvl/cvl_pixel.h"


/**
 * \typedef cvl_pixel_t
 * Type to store a pixel value. It is an unsigned type. The size is
 * machine-dependant, but at least 32 bit.
 */

/**
 * \typedef cvl_pixel_type_t
 * Type of a pixel.
 */
/** \var CVL_PIXEL_GRAY
 *  Gray level in [0,255]: 0x0000gg */
/** \var CVL_PIXEL_RGB
 *  RGB values, each in [0,255]: 0xrrggbb */
/** \var CVL_PIXEL_YUV
 *  YUV values, with Y in [16,235], U and V in [16,240]: 0xyyuuvv. 
 *  When both U and V are 128, the pixel represents a graylevel. */

/**
 * \param x		The integer.
 * \return		0 if x < 0, 255 if x > 255, x itself in all other cases.
 * 
 * Clips an integer to [0,255].
 */
cvl_pixel_t cvl_pixel_clip(int x)
{
    return (x < 0 ? 0 : (x > 255) ? 255 : x);
}

/**
 * \param xmin		Lower bound for \a x.
 * \param xmax		Higher bound for \a x.
 * \param x		The integer.
 * \return		f(x) where f transforms [xmin,xmax] into [0,255] in the obvious way.
 * 
 * Transforms an integer into [0,255].
 */
cvl_pixel_t cvl_pixel_clamp(int xmin, int xmax, int x)
{
    return cvl_iround((double)((x - xmin) * 255) / (double)(xmax - xmin));
}


/**
 * \param gray		The gray value.
 * \return		The gray pixel.
 *
 * Constructs a gray pixel from the given gray value.
 * This does nothing more than to ensure that the returned value is in [0,255]. If
 * \a gray was a valid gray pixel before, it will be returned unchanged.
 */
cvl_pixel_t cvl_pixel_gray(cvl_pixel_t gray)
{
    return (gray & 0xff);
}

/**
 * \param y		The Y value.
 * \param u		The U value.
 * \param v		The V value.
 * \return		The YUV pixel.
 * 
 * Constructs a YUV pixel.
 */
cvl_pixel_t cvl_pixel_yuv(cvl_pixel_t y, cvl_pixel_t u, cvl_pixel_t v)
{
    return (y << 16) | (u << 8) | v;
}

/**
 * \param r		The red value.
 * \param g		The green value.
 * \param b		The blue value.
 * \return		The RGB pixel.
 * 
 * Constructs an RGB pixel.
 */
cvl_pixel_t cvl_pixel_rgb(cvl_pixel_t r, cvl_pixel_t g, cvl_pixel_t b)
{
    return (r << 16) | (g << 8) | b;
}


/**
 * \param gray		The gray value.
 * \return		The RGB value.
 * 
 * Converts a gray pixel into RGB.
 */
cvl_pixel_t cvl_pixel_gray_to_rgb(cvl_pixel_t gray)
{
    return cvl_pixel_rgb(gray, gray, gray);
}

/**
 * \param gray		The gray value.
 * \return		The YUV value.
 * 
 * Converts a gray pixel into YUV.
 */
cvl_pixel_t cvl_pixel_gray_to_yuv(cvl_pixel_t gray)
{
    return cvl_pixel_yuv((((int)cvl_pixel_gray(gray) * 220) >> 8) + 16, 128, 128);
}

/**
 * \param rgb		The RGB pixel.
 * \return		The red value.
 * 
 * Gets the red part of an RGB pixel.
 */
cvl_pixel_t cvl_pixel_rgb_to_r(cvl_pixel_t rgb)
{
    return (rgb >> 16) & 0xff;
}

/**
 * \param rgb		The RGB pixel.
 * \return		The green value.
 * 
 * Gets the green part of an RGB pixel.
 */
cvl_pixel_t cvl_pixel_rgb_to_g(cvl_pixel_t rgb)
{
    return (rgb >> 8) & 0xff;
}

/**
 * \param rgb		The RGB pixel.
 * \return		The blue value.
 * 
 * Gets the blue part of an RGB pixel.
 */
cvl_pixel_t cvl_pixel_rgb_to_b(cvl_pixel_t rgb)
{
    return rgb & 0xff;
}

/**
 * \param rgb		The RGB pixel.
 * \return		The gray pixel.
 * 
 * Converts an RGB pixel to gray.
 */
cvl_pixel_t cvl_pixel_rgb_to_gray(cvl_pixel_t rgb)
{
    return ((cvl_pixel_rgb_to_r(rgb) + cvl_pixel_rgb_to_g(rgb) + cvl_pixel_rgb_to_b(rgb)) / 3);
}

/**
 * \param rgb		The RGB pixel.
 * \return		The YUV pixel.
 * 
 * Converts an RGB pixel to YUV.
 */
cvl_pixel_t cvl_pixel_rgb_to_yuv(cvl_pixel_t rgb)
{
    /* See http://www.fourcc.org/fccyvrgb.php for different opinions about this. */
    int r = cvl_pixel_rgb_to_r(rgb);
    int g = cvl_pixel_rgb_to_g(rgb);
    int b = cvl_pixel_rgb_to_b(rgb);
    int y = ((  66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
    int u = ((- 38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
    int v = (( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128;
    return cvl_pixel_yuv(y, u, v);
}

/**
 * \param yuv		The YUV pixel.
 * \return		The Y value.
 * 
 * Gets the Y part of a YUV pixel.
 */
cvl_pixel_t cvl_pixel_yuv_to_y(cvl_pixel_t yuv)
{
    return (yuv >> 16) & 0xff;
}

/**
 * \param yuv		The YUV pixel.
 * \return		The U value.
 * 
 * Gets the U part of a YUV pixel.
 */
cvl_pixel_t cvl_pixel_yuv_to_u(cvl_pixel_t yuv)
{
    return (yuv >> 8) & 0xff;
}

/**
 * \param yuv		The YUV pixel.
 * \return		The V value.
 * 
 * Gets the V part of a YUV pixel.
 */
cvl_pixel_t cvl_pixel_yuv_to_v(cvl_pixel_t yuv)
{
    return yuv & 0xff;
}

/**
 * \param yuv		The YUV pixel.
 * \return		The gray pixel.
 * 
 * Converts a YUV pixel to gray.
 */
cvl_pixel_t cvl_pixel_yuv_to_gray(cvl_pixel_t yuv)
{
    return cvl_pixel_gray((cvl_pixel_clip((int)cvl_pixel_yuv_to_y(yuv) - 16) * 298) >> 8);
}

/**
 * \param yuv		The YUV pixel.
 * \return		The RGB pixel.
 * 
 * Converts a YUV pixel to RGB.
 */
cvl_pixel_t cvl_pixel_yuv_to_rgb(cvl_pixel_t yuv)
{
    /* See http://www.fourcc.org/fccyvrgb.php for different opinions about this. */
    int c = cvl_pixel_yuv_to_y(yuv) - 16;
    int d = cvl_pixel_yuv_to_u(yuv) - 128;
    int e = cvl_pixel_yuv_to_v(yuv) - 128;
    int r = cvl_pixel_clip((298 * c           + 409 * e + 128) >> 8);
    int g = cvl_pixel_clip((298 * c - 100 * d - 208 * e + 128) >> 8);
    int b = cvl_pixel_clip((298 * c + 516 * d           + 128) >> 8);
    return cvl_pixel_rgb(r, g, b);
}

/**
 * \param p		The pixel value.
 * \param type		The type of the pixel.
 * \return		The pixel value in gray.
 *
 * Converts a pixel to gray.
 */
cvl_pixel_t cvl_pixel_to_gray(cvl_pixel_t p, cvl_pixel_type_t type)
{
    return (type == CVL_PIXEL_GRAY ? p 
	    : (type == CVL_PIXEL_RGB ? cvl_pixel_rgb_to_gray(p) 
		: cvl_pixel_yuv_to_gray(p)));
}
    
/**
 * \param p		The pixel value.
 * \param type		The type of the pixel.
 * \return		The pixel value in RGB.
 *
 * Converts a pixel to RGB.
 */
cvl_pixel_t cvl_pixel_to_rgb(cvl_pixel_t p, cvl_pixel_type_t type)
{
    return (type == CVL_PIXEL_RGB ? p 
	    : (type == CVL_PIXEL_GRAY ? cvl_pixel_gray_to_rgb(p) 
		: cvl_pixel_yuv_to_rgb(p)));
}
    
/**
 * \param p		The pixel value.
 * \param type		The type of the pixel.
 * \return		The pixel value in YUV.
 *
 * Converts a pixel to YUV.
 */
cvl_pixel_t cvl_pixel_to_yuv(cvl_pixel_t p, cvl_pixel_type_t type)
{
    return (type == CVL_PIXEL_YUV ? p 
	    : (type == CVL_PIXEL_GRAY ? cvl_pixel_gray_to_yuv(p) 
		: cvl_pixel_rgb_to_yuv(p)));
}

/**
 * \param dst_type	The destination pixel type.
 * \param p		The pixel value.
 * \param src_type	The source pixel type.
 * \return		The pixel value.
 *
 * Converts a pixel from one pixel type to another.
 */
cvl_pixel_t cvl_pixel_convert(cvl_pixel_type_t dst_type, cvl_pixel_t p, cvl_pixel_type_t src_type)
{
    return (dst_type == CVL_PIXEL_GRAY ? cvl_pixel_to_gray(p, src_type)
	    : (dst_type == CVL_PIXEL_RGB ? cvl_pixel_to_rgb(p, src_type)
		: cvl_pixel_to_yuv(p, src_type)));
}
