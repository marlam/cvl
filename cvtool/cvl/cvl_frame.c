/*
 * cvl_frame.c
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
 * \file cvl_frame.h
 * \brief Frame handling.
 *
 * Manipulating frames: Create and destroy frames, get and set pixels, convert
 * between pixel types (RGB, YUV, and gray), fill or copy rectangles.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "xalloc.h"

#include "cvl/cvl_math.h"
#include "cvl/cvl_pixel.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_msg.h"
#include "cvl/cvl_frame.h"


/**
 * \struct cvl_frame_t
 * \brief An image frame.
 * 
 * Structure that stores a frame. Use the functions from \link cvl_frame.h
 * cvl_frame.h \endlink to manipulate frames.
 */


/**
 * \param pixel_type	The pixel type.
 * \param width		The width.
 * \param height	The height.
 * \return		The frame.
 *
 * Creates a new frame.
 * The pixel type must be #CVL_PIXEL_GRAY, #CVL_PIXEL_RGB, or #CVL_PIXEL_YUV.
 * This function will abort the program if (\a width * \a height) does not fit
 * into an int or if (\a width * \a height * sizeof(cvl_pixel_t)) does not fit
 * into a size_t, to prevent arithmetic overflows.
 */
cvl_frame_t *cvl_frame_new(cvl_pixel_type_t pixel_type, int width, int height)
{
    cvl_assert(width > 0 && height > 0);
    cvl_assert(pixel_type == CVL_PIXEL_GRAY 
	    || pixel_type == CVL_PIXEL_RGB
	    || pixel_type == CVL_PIXEL_YUV);

    if (!cvl_product_fits_in_int(width, height)
	    || !cvl_product_fits_in_size_t(width * height, sizeof(cvl_pixel_t)))
    {
	cvl_msg_err("frame with width=%d height=%d would cause arithmetic overflow", width, height);
	abort();
    }
    else
    {
	cvl_frame_t *frame = xmalloc(sizeof(cvl_frame_t));
	frame->_width = width;
	frame->_height = height;
	frame->_pixel_type = pixel_type;
	frame->_p = xmalloc(width * height * sizeof(cvl_pixel_t));
	return frame;
    }
}

/**
 * \param frame		The frame to be freed.
 * 
 * Frees a frame.
 * All allocated memory will be freed. \a frame may be NULL; this
 * function will do nothing in this case.
 */
void cvl_frame_free(cvl_frame_t *frame)
{
    if (frame)
    {
	free(frame->_p);
	free(frame);
    }
}

/**
 * \param frame		The frame.
 * \param type		The new pixel type of the frame.
 *
 * Stes the pixel type of a frame.
 */
void cvl_frame_set_pixel_type(cvl_frame_t *frame, cvl_pixel_type_t type)
{
    cvl_assert(frame != NULL);
    cvl_assert(type == CVL_PIXEL_GRAY 
	    || type == CVL_PIXEL_RGB
	    || type == CVL_PIXEL_YUV);

    frame->_pixel_type = type;
}

/**
 * \param frame		The frame.
 * \return		The pixel type of the frame.
 *
 * Returns the pixel type of a frame.
 */
cvl_pixel_type_t cvl_frame_pixel_type(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    return frame->_pixel_type;
}

/**
 * \param frame		The frame.
 * \return		The width of the frame.
 *
 * Returns the width of a frame.
 */
int cvl_frame_width(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    return frame->_width;
}


/**
 * \param frame		The frame.
 * \return		The height of the frame.
 *
 * Returns the height of a frame.
 */
int cvl_frame_height(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    return frame->_height;
}

	
/**
 * \param frame		The frame.
 * \return		The number of pixels in the frame.
 *
 * Returns the size of a frame (which means the number of pixels in it).
 */
int cvl_frame_size(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    return cvl_frame_width(frame) * cvl_frame_height(frame);
}

	
/**
 * \param frame		The frame.
 * 
 * Sets all pixels in \a frame to zero. Note that this results in invalid pixel
 * values for YUV frames, because the Y part must be in [16,235].
 */
void cvl_frame_zero(cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    memset(frame->_p, 0, cvl_frame_size(frame) * sizeof(cvl_pixel_t));
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * 
 * Copies the contents (all pixels and the pixel type) of \a src into \a dst. Both
 * frames must have the same dimensions.
 */
void cvl_frame_copy(cvl_frame_t *dst, const cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(cvl_frame_width(dst) == cvl_frame_width(src));
    cvl_assert(cvl_frame_height(dst) == cvl_frame_height(src));

    cvl_frame_set_pixel_type(dst, cvl_frame_pixel_type(src));
    memcpy(dst->_p, src->_p, cvl_frame_size(src) * sizeof(cvl_pixel_t));
}

/**
 * \param frame		The frame to be cloned.
 * \return		An exact copy of the given frame.
 * 
 * Clones a frame.
 */
cvl_frame_t *cvl_frame_clone(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    cvl_frame_t *clone = cvl_frame_new(cvl_frame_pixel_type(frame),
	    cvl_frame_width(frame), cvl_frame_height(frame));
    cvl_frame_copy(clone, frame);
    return clone;
}


/**
 * \param frame		The frame to be converted.
 * 
 * Converts a frame to gray:
 * Sets the pixel type to #CVL_PIXEL_GRAY and converts all pixels if necessary.
 */
void cvl_frame_to_gray(cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
    {
	return;
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
    {
	for (int i = 0; i < cvl_frame_size(frame); i++)
	{
	    cvl_frame_set_i(frame, i, cvl_pixel_rgb_to_gray(cvl_frame_get_i(frame, i)));
	}
	cvl_frame_set_pixel_type(frame, CVL_PIXEL_GRAY);
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	for (int i = 0; i < cvl_frame_size(frame); i++)
	{
	    cvl_frame_set_i(frame, i, cvl_pixel_yuv_to_gray(cvl_frame_get_i(frame, i)));
	}
	cvl_frame_set_pixel_type(frame, CVL_PIXEL_GRAY);
    }
}

/**
 * \param frame		The frame to be converted.
 * 
 * Converts a frame to RGB:
 * sets the pixel type to #CVL_PIXEL_RGB and converts all pixels if necessary.
 */
void cvl_frame_to_rgb(cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
    {
	for (int i = 0; i < cvl_frame_size(frame); i++)
	{
	    cvl_frame_set_i(frame, i, cvl_pixel_gray_to_rgb(cvl_frame_get_i(frame, i)));
	}
	cvl_frame_set_pixel_type(frame, CVL_PIXEL_RGB);
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
    {
	return;
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	for (int i = 0; i < cvl_frame_size(frame); i++)
	{
	    cvl_frame_set_i(frame, i, cvl_pixel_yuv_to_rgb(cvl_frame_get_i(frame, i)));
	}
	cvl_frame_set_pixel_type(frame, CVL_PIXEL_RGB);
    }
}

/**
 * \param frame		The frame to be converted.
 * 
 * Converts a frame to YUV: 
 * sets the pixel type to #CVL_PIXEL_YUV and converts all pixels if necessary.
 */
void cvl_frame_to_yuv(cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);

    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
    {
	for (int i = 0; i < cvl_frame_size(frame); i++)
	{
	    cvl_frame_set_i(frame, i, cvl_pixel_gray_to_yuv(cvl_frame_get_i(frame, i)));
	}
	cvl_frame_set_pixel_type(frame, CVL_PIXEL_YUV);
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
    {
	for (int i = 0; i < cvl_frame_size(frame); i++)
	{
	    cvl_frame_set_i(frame, i, cvl_pixel_rgb_to_yuv(cvl_frame_get_i(frame, i)));
	}
	cvl_frame_set_pixel_type(frame, CVL_PIXEL_YUV);
    }
    else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	return;
    }
}

/**
 * \param frame		The frame to be converted.
 * \param type		The pixel type to convert the frame to.
 * 
 * Converts a frame to the given pixel type:
 * sets the pixel type and converts all pixels if necessary.
 */
void cvl_frame_convert(cvl_frame_t *frame, cvl_pixel_type_t type)
{
    cvl_assert(frame != NULL);

    if (type == CVL_PIXEL_GRAY)
    {
	cvl_frame_to_gray(frame);
    }
    else if (type == CVL_PIXEL_RGB)
    {
	cvl_frame_to_rgb(frame);
    }
    else // type == CVL_PIXEL_YUV
    {
	cvl_frame_to_yuv(frame);
    }
}

/**
 * \param frame		The frame.
 * \param x		The x coordinate of the upper left corner of the rectangle.
 * \param y		The y coordinate of the upper left corner of the rectangle.
 * \param w		The width of the rectangle.
 * \param h		The height of the rectangle.
 * \param p		The pixel value that the rectangle will be filled with.
 * 
 * Fills a rectangle in a frame.
 */
void cvl_frame_fill_rect(cvl_frame_t *frame, int x, int y, int w, int h, cvl_pixel_t p)
{
    cvl_assert(frame != NULL);
    cvl_assert(x >= 0);
    cvl_assert(y >= 0);
    cvl_assert(w >= 0);
    cvl_assert(h >= 0);
    cvl_assert(x + w <= cvl_frame_width(frame));
    cvl_assert(y + h <= cvl_frame_height(frame));

    for (int ry = y; ry < y + h; ry++)
    {
	for (int rx = x; rx < x + w; rx++)
	{
	    cvl_frame_set(frame, rx, ry, p);
	}
    }
}

/**
 * \param dst		The destination frame.
 * \param dst_x		The x coordinate of the destination rectangle's upper left corner.
 * \param dst_y		The y coordinate of the destination rectangle's upper left corner.
 * \param src		The source frame.
 * \param src_x		The x coordinate of the source rectangle's upper left corner.
 * \param src_y		The y coordinate of the source rectangle's upper left corner.
 * \param rwidth	The width of the rectangle.
 * \param rheight	The height of the rectangle.
 * 
 * Copies the given rectangle. The areas may be in the same frame, but they must
 * not overlap!
 */
void cvl_frame_copy_rect(cvl_frame_t *dst, int dst_x, int dst_y, 
	const cvl_frame_t *src, int src_x, int src_y, int rwidth, int rheight)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst_x >= 0);
    cvl_assert(dst_x < cvl_frame_width(dst));
    cvl_assert(dst_y >= 0);
    cvl_assert(dst_y < cvl_frame_height(dst));
    cvl_assert(src_x >= 0);
    cvl_assert(src_x < cvl_frame_width(src));
    cvl_assert(src_y >= 0);
    cvl_assert(src_y < cvl_frame_height(src));
    cvl_assert(rwidth >= 0);
    cvl_assert(dst_x + rwidth <= cvl_frame_width(dst));
    cvl_assert(rheight >= 0);
    cvl_assert(dst_y + rheight <= cvl_frame_height(dst));

    for (int y = 0; y < rheight; y++)
    {
	memcpy(&(dst->_p[(dst_y + y) * cvl_frame_width(dst) + dst_x]),
 		&(src->_p[(src_y + y) * cvl_frame_width(src) + src_x]),
 		(size_t)rwidth * sizeof(cvl_pixel_t));
    }	
}
