/*
 * cvl_blend.c
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
 * \file cvl_blend.h
 * \brief Alpha blending.
 *
 * Merge pixel blocks with support for transparency.
 */

#include "config.h"


#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_blend.h"
#include "cvl_assert.h"


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
