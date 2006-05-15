/*
 * cvl_mix.c
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
 * \file cvl_mix.h
 * \brief Mixing frame contents.
 *
 * Mixing frame contents by layering or blending them.
 */

#include "config.h"


#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_mix.h"
#include "cvl_assert.h"

#include "xalloc.h"


/**
 * \param frame		The frame.
 * \param dst_x		The x coordinate of the block's destination in the frame.
 * \param dst_y		The y coordinate of the block's destination in the frame.
 * \param block		The block to be copied into the frame.
 * \param block_alpha	The alpha map for the block.
 * 
 * Copies \a block into \a frame at the position specified by \a dst_x and \a
 * dst_y. The transparency of the block is read from \a block_alpha, which must be of
 * pixel type #CVL_PIXEL_GRAY. The block and the frame must have the same pixel
 * type. The block must fit in the frame.
 */
void cvl_frame_blend(cvl_frame_t *frame, int dst_x, int dst_y,
	const cvl_frame_t *block, const cvl_frame_t *block_alpha)
{
    cvl_assert(frame != NULL && block != NULL && block_alpha != NULL);
    cvl_assert(dst_x >= 0 && dst_x < cvl_frame_width(frame) && dst_y >= 0 && cvl_frame_height(frame));
    cvl_assert(cvl_frame_width(block) == cvl_frame_width(block_alpha) 
	    && cvl_frame_height(block) == cvl_frame_height(block_alpha));
    cvl_assert(dst_x + cvl_frame_width(block) <= cvl_frame_width(frame) 
	    && dst_x + cvl_frame_height(block) <= cvl_frame_height(frame));
    cvl_assert(cvl_frame_pixel_type(frame) == cvl_frame_pixel_type(block));
    cvl_assert(cvl_frame_pixel_type(block_alpha) == CVL_PIXEL_GRAY);

    for (int y = 0; y < cvl_frame_height(block); y++)
    {
	for (int x = 0; x < cvl_frame_width(block); x++)
	{
	    int alpha = (cvl_frame_get(block_alpha, x, y) << 12);
	    int invalpha = (255 << 12) - alpha;
	    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
	    {
		cvl_frame_set(frame, x + dst_x, y + dst_y, 
			(alpha * cvl_frame_get(frame, x + dst_x, y + dst_y) 
			 + invalpha * cvl_frame_get(block, x, y)) / (255 << 12));
	    }
	    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
	    {
		cvl_pixel_t fp = cvl_frame_get(frame, x + dst_x, y + dst_y);
		cvl_pixel_t bp = cvl_frame_get(block, x, y);
		cvl_frame_set(frame, x + dst_x, y + dst_y, cvl_pixel_rgb(
			    (alpha * cvl_pixel_rgb_to_r(fp) 
			     + invalpha * cvl_pixel_rgb_to_r(bp)) / (255 << 12),
			    (alpha * cvl_pixel_rgb_to_g(fp) 
			     + invalpha * cvl_pixel_rgb_to_g(bp)) / (255 << 12),
			    (alpha * cvl_pixel_rgb_to_b(fp) 
			     + invalpha * cvl_pixel_rgb_to_b(bp)) / (255 << 12)));
	    }
	    else // cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV
	    {
		cvl_pixel_t fp = cvl_frame_get(frame, x + dst_x, y + dst_y);
		cvl_pixel_t bp = cvl_frame_get(block, x, y);
		cvl_frame_set(frame, x + dst_x, y + dst_y, cvl_pixel_yuv(
			    (alpha * cvl_pixel_yuv_to_y(fp) 
			     + invalpha * cvl_pixel_yuv_to_y(bp)) / (255 << 12),
			    (alpha * cvl_pixel_yuv_to_u(fp) 
			     + invalpha * cvl_pixel_yuv_to_u(bp)) / (255 << 12),
			    (alpha * cvl_pixel_yuv_to_v(fp) 
			     + invalpha * cvl_pixel_yuv_to_v(bp)) / (255 << 12)));
	    }
	}
    }    
}


/**
 * \typedef cvl_layer_mode_t
 * The layering mode.
 */
/** \var CVL_LAYER_MODE_MIN
 *  Use minimum value. */
/** \var CVL_LAYER_MODE_MAX
 *  Use maximum value. */
/** \var CVL_LAYER_MODE_MEDIAN
 *  Use median value. */
/** \var CVL_LAYER_MODE_OR
 *  Bitwise or. */
/** \var CVL_LAYER_MODE_AND
 *  Bitwise and. */
/** \var CVL_LAYER_MODE_XOR
 *  Bitwise xor. */
/** \var CVL_LAYER_MODE_DIFF
 *  Use difference between maximum and minimum value. */
/** \var CVL_LAYER_MODE_ADD
 *  Use sum of values. */
/** \var CVL_LAYER_MODE_XADD
 *  Use sum of values. The ranges are transformed so that the results fit in
 *  [0,255]. Example for two layers: X = (A/2) + (B/2). */
/** \var CVL_LAYER_MODE_SUB
 *  Subtract values from the first value. */
/** \var CVL_LAYER_MODE_XSUB
 *  Subtract values from the first value. The ranges are transformed so that the results fit in
 *  [0,255]. Example for two layers: X = (A/2) - (B/2) + 255/2. */
/** \var CVL_LAYER_MODE_MUL
 *  Multiply values. */
/** \var CVL_LAYER_MODE_DIV
 *  Divide values. */

static cvl_pixel_t cvl_layer_mode_min(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	if (layered_pixels[i] < p)
	{
	    p = layered_pixels[i];
	}
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_max(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	if (layered_pixels[i] > p)
	{
	    p = layered_pixels[i];
	}
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_median(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p;
    cvl_pixel_t *tmparray = xmemdup(layered_pixels, number_of_layers * sizeof(cvl_pixel_t));
    cvl_sort_array_pixel(tmparray, number_of_layers);
    p = tmparray[number_of_layers / 2];
    free(tmparray);
    return p;
}

static cvl_pixel_t cvl_layer_mode_or(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	p |= layered_pixels[i];
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_and(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	p &= layered_pixels[i];
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_xor(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	p ^= layered_pixels[i];
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_diff(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    return cvl_layer_mode_max(layered_pixels, number_of_layers)
	- cvl_layer_mode_min(layered_pixels, number_of_layers);
}

static cvl_pixel_t cvl_layer_mode_add(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	p += layered_pixels[i];
	if (p > 0xff)
	{
	    p = 0xff;
	    break;
	}
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_xadd(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0] / number_of_layers;
    for (int i = 1; i < number_of_layers; i++)
    {
	p += layered_pixels[i] / number_of_layers;
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_sub(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	if (layered_pixels[i] >= p)
	{
	    p = 0;
	    break;
	}
	p -= layered_pixels[i];
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_xsub(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0] / number_of_layers;
    for (int i = 1; i < number_of_layers; i++)
    {
	p = p + 255 / number_of_layers - layered_pixels[i] / number_of_layers;
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_mul(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	p *= layered_pixels[i];
	if (p > 0xff)
	{
	    p = 0xff;
	    break;
	}
    }
    return p;
}

static cvl_pixel_t cvl_layer_mode_div(const cvl_pixel_t *layered_pixels, int number_of_layers)
{
    cvl_pixel_t p = layered_pixels[0];
    for (int i = 1; i < number_of_layers; i++)
    {
	if (layered_pixels[i] == 0)
	{
	    p = 0xff;
	    break;
	}
	p /= layered_pixels[i];
    }
    return p;
}

/* This table must correspond to the values defined in cvl/cvl_mix.h */
static cvl_pixel_t (*cvl_layer_mode[])(const cvl_pixel_t *layered_pixels, int number_of_layers) =
{
    cvl_layer_mode_min,
    cvl_layer_mode_max,
    cvl_layer_mode_median,
    cvl_layer_mode_or,
    cvl_layer_mode_and,
    cvl_layer_mode_xor,
    cvl_layer_mode_diff,
    cvl_layer_mode_add,
    cvl_layer_mode_xadd,
    cvl_layer_mode_sub,
    cvl_layer_mode_xsub,
    cvl_layer_mode_mul,
    cvl_layer_mode_div
};


/**
 * \param frame		The destination frame.
 * \param mode		The layering mode.
 * \param layers	The source frames.
 * \param offset_x	The x offsets of each source frame.
 * \param offset_y	The y offsets of each source frame.
 * \param number_of_layers	The number of source frames.
 *
 * Layers the given source frames on top of each other, using the given mode.
 * The pixel type of the destination frame must be either #CVL_PIXEL_GRAY or
 * #CVL_PIXEL_RGB. Layering is done for each channel separately.
 * The offset values define the position in the destination frame at which the
 * corresponding source frame will be layered. If the pointers are NULL, then
 * the offsets default to zero.
 * There are no restrictions for the destination frame size, the source frame
 * size, the source frame pixel type, or the offset values.
 * At least one layer must be present.
 */
void cvl_frame_layer(cvl_frame_t *frame, cvl_layer_mode_t mode,
	cvl_frame_t * const *layers, const int *offset_x, const int *offset_y, 
	int number_of_layers)
{
    cvl_assert(frame != NULL);
    cvl_assert(cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY 
	    || cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB);
    cvl_assert(layers != NULL);
    cvl_assert(layers[0] != NULL);
    cvl_assert(number_of_layers >= 1);
    
    cvl_pixel_t *layered_pixels;

    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
    {
	layered_pixels = xmalloc(number_of_layers * sizeof(cvl_pixel_t));
    }
    else
    {
	layered_pixels = xmalloc(3 * number_of_layers * sizeof(cvl_pixel_t));
    }
    
    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    int number_of_layered_pixels = 0;
	    for (int i = 0; i < number_of_layers; i++)
	    {
		int xx = offset_x ? (x - offset_x[i]) : x;
		int yy = offset_y ? (y - offset_y[i]) : y;
		if (xx >= 0 && xx < cvl_frame_width(layers[i])
			&& yy >= 0 && yy < cvl_frame_height(layers[i]))
		{
		    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
		    {
			layered_pixels[number_of_layered_pixels++] =
			    cvl_pixel_to_gray(cvl_frame_get(layers[i], xx, yy),
				    cvl_frame_pixel_type(layers[i]));
		    }
		    else
		    {
			cvl_pixel_t ptmp = cvl_pixel_to_rgb(
				cvl_frame_get(layers[i], xx, yy), cvl_frame_pixel_type(layers[i]));
			layered_pixels[number_of_layered_pixels + 0 * number_of_layers] = 
			    cvl_pixel_rgb_to_r(ptmp); 
			layered_pixels[number_of_layered_pixels + 1 * number_of_layers] = 
			    cvl_pixel_rgb_to_g(ptmp); 
			layered_pixels[number_of_layered_pixels + 2 * number_of_layers] = 
			    cvl_pixel_rgb_to_b(ptmp); 
			number_of_layered_pixels++;
		    }
		}
	    }
	    cvl_pixel_t p;
	    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
	    {
		p = cvl_layer_mode[mode](layered_pixels, number_of_layered_pixels);
	    }
	    else
	    {
		p = cvl_pixel_rgb(
			cvl_layer_mode[mode](layered_pixels + 0 * number_of_layers, number_of_layered_pixels),
			cvl_layer_mode[mode](layered_pixels + 1 * number_of_layers, number_of_layered_pixels),
			cvl_layer_mode[mode](layered_pixels + 2 * number_of_layers, number_of_layered_pixels));
	    }
	    cvl_frame_set(frame, x, y, p);
	}
    }
    
    free(layered_pixels);
    return;
}
