/*
 * cvl_transform.c
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
 * \file cvl_transform.h
 * \brief Geometric transformations.
 *
 * Apply geometric transformations to frames: Mirroring, resizing, or affine
 * transformations such as rotation, scale, or shear.
 */

#include "config.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_transform.h"


/**
 * \typedef cvl_interpolation_type_t
 * The interpolation technique.
 */
/** \var CVL_INTERPOLATION_NONE
 *  No interpolation (nearest neighbor). */
/** \var CVL_INTERPOLATION_BILINEAR
 *  Bilinear interpolation. */


/*
 * Interpolation functions.
 */

static cvl_pixel_t cvl_interpolation_none(const cvl_frame_t *frame, cvl_color_t color, double x_old, double y_old)
{
    int xo = (int)x_old;
    int yo = (int)y_old;
    
    if (yo < 0 || yo >= cvl_frame_height(frame) || xo < 0 || xo >= cvl_frame_width(frame))
    {
	return cvl_color_to_pixel(color, cvl_frame_pixel_type(frame));
    }
    else
    {
	return cvl_frame_get(frame, xo, yo);
    }
}

static inline cvl_pixel_t cvl_interpolation_bilinear_helper(double x_diff, double y_diff, 
	cvl_pixel_t p_x1y1, cvl_pixel_t p_x2y1, cvl_pixel_t p_x1y2, cvl_pixel_t p_x2y2)
{
    double fp = (double)p_x1y1 + y_diff * ((double)p_x1y2 - (double)p_x1y1);
    double fq = (double)p_x2y1 + y_diff * ((double)p_x2y2 - (double)p_x2y1);
    return cvl_iround(fp + x_diff * (fq - fp));	    
}

static cvl_pixel_t cvl_interpolation_bilinear(const cvl_frame_t *frame, 
	cvl_color_t color, double x_old, double y_old)
{
    int x1 = (int)floor(x_old);
    int x2 = x1 + 1;
    int y1 = (int)floor(y_old);
    int y2 = y1 + 1;

    if (x1 < 0 || y1 < 0 || x1 >= cvl_frame_width(frame) || y1 >= cvl_frame_height(frame))
    {
	return cvl_color_to_pixel(color, cvl_frame_pixel_type(frame));
    }
    else
    {
	cvl_pixel_t p_x1y1 = cvl_frame_get(frame, x1, y1);
	cvl_pixel_t p_x2y1 = cvl_frame_get_r(frame, x2, y1);
	cvl_pixel_t p_x1y2 = cvl_frame_get_r(frame, x1, y2);
	cvl_pixel_t p_x2y2 = cvl_frame_get_r(frame, x2, y2);
	double y_diff = y_old - (double)y1;
	double x_diff = x_old - (double)x1;
	if (cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY)
	{
	    return cvl_interpolation_bilinear_helper(x_diff, y_diff, 
		    p_x1y1, p_x2y1, p_x1y2, p_x2y2);
	}
	else if (cvl_frame_pixel_type(frame) == CVL_PIXEL_RGB)
	{
	    return cvl_pixel_rgb(
		    cvl_interpolation_bilinear_helper(x_diff, y_diff,
			cvl_pixel_rgb_to_r(p_x1y1), cvl_pixel_rgb_to_r(p_x2y1), 
			cvl_pixel_rgb_to_r(p_x1y2), cvl_pixel_rgb_to_r(p_x2y2)),
		    cvl_interpolation_bilinear_helper(x_diff, y_diff,
			cvl_pixel_rgb_to_g(p_x1y1), cvl_pixel_rgb_to_g(p_x2y1), 
			cvl_pixel_rgb_to_g(p_x1y2), cvl_pixel_rgb_to_g(p_x2y2)),
		    cvl_interpolation_bilinear_helper(x_diff, y_diff,
			cvl_pixel_rgb_to_b(p_x1y1), cvl_pixel_rgb_to_b(p_x2y1), 
			cvl_pixel_rgb_to_b(p_x1y2), cvl_pixel_rgb_to_b(p_x2y2)));
	}
	else
	{
	    return cvl_pixel_yuv(
		    cvl_interpolation_bilinear_helper(x_diff, y_diff,
			cvl_pixel_yuv_to_y(p_x1y1), cvl_pixel_yuv_to_y(p_x2y1),
			cvl_pixel_yuv_to_y(p_x1y2), cvl_pixel_yuv_to_y(p_x2y2)),
		    cvl_interpolation_bilinear_helper(x_diff, y_diff,
			cvl_pixel_yuv_to_u(p_x1y1), cvl_pixel_yuv_to_u(p_x2y1),
			cvl_pixel_yuv_to_u(p_x1y2), cvl_pixel_yuv_to_u(p_x2y2)),
		    cvl_interpolation_bilinear_helper(x_diff, y_diff,
			cvl_pixel_yuv_to_v(p_x1y1), cvl_pixel_yuv_to_v(p_x2y1), 
			cvl_pixel_yuv_to_v(p_x1y2), cvl_pixel_yuv_to_v(p_x2y2)));
	}
    }
}
    
static cvl_pixel_t (*cvl_interpolation[])(const cvl_frame_t *frame, 
	cvl_color_t color, double x_old, double y_old) =
{
    cvl_interpolation_none,
    cvl_interpolation_bilinear
};

/**
 * \param frame		The frame.
 * \return		The flipped frame.
 * 
 * Flips a frame left/right.
 */
cvl_frame_t *cvl_flip(const cvl_frame_t *frame)
{
    cvl_frame_t *flipped = cvl_frame_new(cvl_frame_pixel_type(frame), cvl_frame_width(frame), cvl_frame_height(frame));
    for (int x = 0; x < cvl_frame_width(flipped); x++)
    {
	cvl_frame_copy_rect(flipped, cvl_frame_width(frame) - 1 - x, 0, frame, x, 0, 1, cvl_frame_height(frame));
    }
    return flipped;
}

/**
 * \param frame		The frame.
 * \return		The flopped frame.
 * 
 * Flops a frame top/bottom.
 */
cvl_frame_t *cvl_flop(const cvl_frame_t *frame)
{
    cvl_frame_t *flopped = cvl_frame_new(cvl_frame_pixel_type(frame), cvl_frame_width(frame), cvl_frame_height(frame));
    for (int y = 0; y < cvl_frame_height(flopped); y++)
    {
	cvl_frame_copy_rect(flopped, 0, cvl_frame_height(frame) - 1 - y, frame, 0, y, cvl_frame_width(frame), 1);
    }
    return flopped;
}

/* A helper function for cvl_resize() */
static void cvl_resize_fill_rect_checked(cvl_frame_t *frame, int x, int y, int w, int h, cvl_pixel_t p)
{
    if (x >= 0 && x < cvl_frame_width(frame)
	    && y >= 0 && y < cvl_frame_height(frame)
	    && x + w <= cvl_frame_width(frame)
	    && y + h <= cvl_frame_height(frame))
    {
	cvl_frame_fill_rect(frame, x, y, w, h, p);
    }
}

/**
 * \param frame		The frame.
 * \param color		The color that resulting holes will be filled with.
 * \param new_width	The new width of the frame.
 * \param new_height	The new height of the frame.
 * \param new_x_offset	The x position of the original frame contents in the resized frame.
 * \param new_y_offset	The y position of the original frame contents in the resized frame.
 * \return		The resized frame.
 * 
 * Resizes a frame.
 */
cvl_frame_t *cvl_resize(const cvl_frame_t *frame, cvl_color_t color, 
	int new_width, int new_height, int new_x_offset, int new_y_offset)
{
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), new_width, new_height);
    cvl_pixel_t p = cvl_color_to_pixel(color, cvl_frame_pixel_type(new_frame));
    int srx, sry, drx, dry, rw, rh;
    /* Compute the rectangle from 'frame' that must be copied to 'new_frame' */
    srx = new_x_offset < 0 ? - new_x_offset : 0;
    sry = new_y_offset < 0 ? - new_y_offset : 0;
    drx = new_x_offset < 0 ? 0 : new_x_offset;
    dry = new_y_offset < 0 ? 0 : new_y_offset;
    rw = cvl_frame_width(frame) - srx;
    if (drx + rw - 1 >= cvl_frame_width(new_frame))
    {
	rw = cvl_frame_width(new_frame) - drx;
    }
    rh = cvl_frame_height(frame) - sry;
    if (dry + rh - 1 >= cvl_frame_height(new_frame))
    {
	rh = cvl_frame_height(new_frame) - dry;
    }
    if (srx < cvl_frame_width(frame) && sry < cvl_frame_height(frame) 
	    && drx < cvl_frame_width(new_frame) && dry < cvl_frame_height(new_frame)
	    && rw > 0 && rh > 0)
    {
	cvl_frame_copy_rect(new_frame, drx, dry, frame, srx, sry, rw, rh);
	/* Now fill the space around this rectangle, divided into 4 rectangles */
	cvl_resize_fill_rect_checked(new_frame, 
		0, 0, cvl_frame_width(new_frame), dry, p);
	cvl_resize_fill_rect_checked(new_frame, 
		0, dry + rh, cvl_frame_width(new_frame), cvl_frame_height(new_frame) - dry - rh, p);
	cvl_resize_fill_rect_checked(new_frame, 
		0, dry, drx, rh, p);
	cvl_resize_fill_rect_checked(new_frame, 
		drx + rw, dry, cvl_frame_width(new_frame) - drx - rw, rh, p);
    }
    else
    {
	cvl_frame_fill_rect(new_frame, 0, 0, cvl_frame_width(new_frame), cvl_frame_height(new_frame), p);
    }

    return new_frame;
}

/**
 * \param frame		The frame.
 * \param x		The x coordinate of the rectangle.
 * \param y		The y coordinate of the rectangle.
 * \param w		The width of the rectangle.
 * \param h		The height of the rectangle.
 * \return		A frame containing only the specified rectangle.
 * 
 * Cuts a rectangle out of a frame.
 */
cvl_frame_t *cvl_cut(const cvl_frame_t *frame, int x, int y, int w, int h)
{
    cvl_frame_t *rect = cvl_frame_new(cvl_frame_pixel_type(frame), w, h);
    cvl_frame_copy_rect(rect, 0, 0, frame, x, y, w, h);
    return rect;
}


/**
 * \param frame		The frame.
 * \param interpolation_type	The interpolation type to use.
 * \param new_width	The new width.
 * \param new_height	The new height.
 * \return		The scaled frame.
 * 
 * Scales a frame.
 */
cvl_frame_t *cvl_scale(const cvl_frame_t *frame, 
	cvl_interpolation_type_t interpolation_type, int new_width, int new_height)
{
    double factor_h = (double)new_width / (double)cvl_frame_width(frame);
    double factor_v = (double)new_height / (double)cvl_frame_height(frame);
    /* Implemented directly, though it's only a special affine transformation with
     * matrix 2x2:factor_h,0,0,factor_v. It might be a little bit faster. */
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), new_width, new_height);
    
    for (int y = 0; y < new_height; y++)
    {
	for (int x = 0; x < new_width; x++)
	{
	    double xo = (double)x / factor_h;
	    double yo = (double)y / factor_v;
	    cvl_frame_set(new_frame, x, y, 
		    cvl_interpolation[interpolation_type](frame, CVL_COLOR_BLACK, xo, yo));
	}
    }

    return new_frame;
}

/**
 * \param frame		The frame.
 * \param interpolation_type	The interpolation type to use.
 * \param color		The color to fill resulting holes with.
 * \param angle		The rotation angle in radians.
 * \return		The rotated frame.
 *
 * Rotates a frame.
 * Rotation is done counterclockwise. The rotated frame will be big enough to
 * hold all information from the source, therefore it will in general be bigger
 * than the original frame. Simple rotations (0, 90, 180, 270) are implemented
 * as fast special cases.
 */
cvl_frame_t *cvl_rotate(const cvl_frame_t *frame, 
	cvl_interpolation_type_t interpolation_type, cvl_color_t color, double angle)
{
    /* implemented directly because the simple rotations 0, 90, 180, 270 should be
     * fast. the general rotation is only a special affine transformation with
     * matrix 2x2:cos(a),sin(a),-sin(a),cos(a), but it is implemented directly
     * nevertheless. */
    double sina = sin(angle);
    double cosa = cos(angle);
    /* determine where the four edge points will be */
    double w = (double)(cvl_frame_width(frame) - 1);
    double h = (double)(cvl_frame_height(frame) - 1);
    /* 1. (0,0) -> (0,0) */
    int ep0x = 0;
    int ep0y = 0;
    /* 2. (w,0) -> (w*cosa,-w*sina)*/
    int ep1x = cvl_iround(w * cosa);
    int ep1y = cvl_iround(- w * sina);
    /* 3. (w,h) -> (w*cosa+h*sina,-w*sina+h*cosa) */
    int ep2x = cvl_iround(w * cosa + h * sina);
    int ep2y = cvl_iround(- w * sina + h * cosa);
    /* 4. (0,h) -> (h*sina,h*cosa) */
    int ep3x = cvl_iround(h * sina);
    int ep3y = cvl_iround(h * cosa);
    /* Bounding box of the rotation: 
     * (minx, maxy), (maxx, maxy), (maxx, miny), (minx, miny) */
    int maxx = cvl_max4i(ep0x, ep1x, ep2x, ep3x);
    int minx = cvl_min4i(ep0x, ep1x, ep2x, ep3x);
    int maxy = cvl_max4i(ep0y, ep1y, ep2y, ep3y);
    int miny = cvl_min4i(ep0y, ep1y, ep2y, ep3y);

    cvl_frame_t *rotframe = cvl_frame_new(cvl_frame_pixel_type(frame), maxx - minx + 1, maxy - miny + 1);

    if (cvl_frame_height(rotframe) == cvl_frame_height(frame) 
	    && cvl_frame_width(rotframe) == cvl_frame_width(frame)
	    && (angle < 0.25 * M_PI || angle > 1.75 * M_PI))
    {
	/* 0° rotation */
	cvl_frame_copy(rotframe, frame);
    }
    else if (cvl_frame_height(rotframe) == cvl_frame_width(frame) 
	    && cvl_frame_width(rotframe) == cvl_frame_height(frame)
	    && angle > 0.25 * M_PI && angle < 0.75 * M_PI)
    {
	/* 90° rotation */
	for (int y = 0; y < cvl_frame_height(rotframe); y++)
	{
	    for (int x = 0; x < cvl_frame_width(rotframe); x++)
	    {
		cvl_frame_set(rotframe, x, y, cvl_frame_get(frame, cvl_frame_height(rotframe) - 1 - y, x));
	    }
	} 
    }
    else if (cvl_frame_height(rotframe) == cvl_frame_height(frame) 
	    && cvl_frame_width(rotframe) == cvl_frame_width(frame)
	    && angle > 0.75 * M_PI && angle < 1.25 * M_PI)
    {
	/* 180° rotation */
	for (int y = 0; y < cvl_frame_height(rotframe); y++)
	{
	    for (int x = 0; x < cvl_frame_width(rotframe); x++)
	    {
		cvl_frame_set(rotframe, x, y, cvl_frame_get(frame, cvl_frame_width(frame) - 1 - x, cvl_frame_height(frame) - 1 - y));
	    }
	}
    }
    else if (cvl_frame_height(rotframe) == cvl_frame_width(frame) 
	    && cvl_frame_width(rotframe) == cvl_frame_height(frame)
	    && angle > 1.25 * M_PI && angle < 1.75 * M_PI)
    {
	/* 270° rotation */
	for (int y = 0; y < cvl_frame_height(rotframe); y++)
	{
	    for (int x = 0; x < cvl_frame_width(rotframe); x++)
	    {
		cvl_frame_set(rotframe, x, y, cvl_frame_get(frame, y, cvl_frame_width(rotframe) - 1 - x));
	    }
	} 
    }
    else
    {
	for (int y = 0; y < cvl_frame_height(rotframe); y++)
	{
	    for (int x = 0; x < cvl_frame_width(rotframe); x++)
	    {
		double xo = (double)(x + minx) * cosa - (double)(y + miny) * sina;
		double yo = (double)(x + minx) * sina + (double)(y + miny) * cosa;
		cvl_frame_set(rotframe, x, y,
			cvl_interpolation[interpolation_type](frame, color, xo, yo));
	    }
	}
    }
    
    return rotframe;
}

/**
 * \param frame		The frame.
 * \param interpolation_type	The interpolation type to use.
 * \param color		The color to fill resulting holes with.
 * \param shear_angle_x	The shear angle in horizontal direction, in radians.
 * \param shear_angle_y	The shear angle in vertical direction, in radians.
 * \return		The sheared frame.
 *
 * Shears a frame.
 * The angles should be from (-PI/2, PI/2). Negative angles shear clockwise.
 * The angles should not be to close to -PI/2 or PI/2, or the resulting frame 
 * will be unreasonable wide.
 */
cvl_frame_t *cvl_shear(const cvl_frame_t *frame, 
	cvl_interpolation_type_t interpolation_type, cvl_color_t color, double shear_angle_x, double shear_angle_y)
{
    /* implemented directly for efficiency, though it is simply a special affine
       transformation with matrix 2x2:1,0,sx,1 (x_shearing), 2x2:1,sy,0,1 (y_shearing). */
    int shear_x = cvl_frame_height(frame) * tan(shear_angle_x);
    int shear_y = cvl_frame_width(frame) * tan(shear_angle_y);
    cvl_frame_t *x_sheared_frame = NULL;
    cvl_frame_t *y_sheared_frame = NULL;
    
    if (shear_x != 0 && cvl_frame_height(frame) > 1)
    {
	x_sheared_frame = cvl_frame_new(cvl_frame_pixel_type(frame), cvl_frame_width(frame) + abs(shear_x), cvl_frame_height(frame));
	int base_x = shear_x > 0 ? 0 : shear_x;
	for (int y = 0; y < cvl_frame_height(x_sheared_frame); y++)
	{
	    for (int x = 0; x < cvl_frame_width(x_sheared_frame); x++)
	    {
		double xo = (double)(x + base_x) - (double)shear_x * ((double)y / (double)(cvl_frame_height(frame) - 1));
		double yo = (double)y;
		cvl_frame_set(x_sheared_frame, x, y, 
			cvl_interpolation[interpolation_type](frame, color, xo, yo));
	    }
	}
	frame = x_sheared_frame;
    }
    if (shear_y != 0 && cvl_frame_width(frame) > 1)
    {
	y_sheared_frame = cvl_frame_new(cvl_frame_pixel_type(frame), cvl_frame_width(frame), cvl_frame_height(frame) + abs(shear_y));
	int base_y = shear_y > 0 ? -shear_y : 0;
	for (int y = 0; y < cvl_frame_height(y_sheared_frame); y++)
	{
	    for (int x = 0; x < cvl_frame_width(y_sheared_frame); x++)
	    {
		double xo = (double)x;
		double yo = (double)(y + base_y) + (double)shear_y * ((double)x / (double)(cvl_frame_width(frame) - 1));
		cvl_frame_set(y_sheared_frame, x, y, 
			cvl_interpolation[interpolation_type](frame, color, xo, yo));
	    }
	}
    }

    if (y_sheared_frame && x_sheared_frame)
    {
	cvl_frame_free(x_sheared_frame);
	return y_sheared_frame;
    }
    else if (y_sheared_frame)
    {
	return y_sheared_frame;
    }
    else if (x_sheared_frame)
    {
	return x_sheared_frame;
    }
    else 
    {
	return cvl_frame_clone(frame);
    }
}

/**
 * \param frame		The frame.
 * \param interpolation_type	The interpolation type to use.
 * \param color		The color to fill resulting holes with.
 * \param matrix	A 2x2 transformation matrix as an array of 4 doubles.
 * \return		The transformed frame.
 * 
 * Applies an affine transformation on a frame.
 */
cvl_frame_t *cvl_affine(const cvl_frame_t *frame, 
	cvl_interpolation_type_t interpolation_type, cvl_color_t color, const double *matrix)
{
    /* determine where the four edge points will be */
    double w = (double)cvl_frame_width(frame);
    double h = (double)cvl_frame_height(frame);
    /* 1. (0,0) -> (0,0) */
    int ep0x = 0;
    int ep0y = 0;
    /* 2. (w,0) -> (w*a11-1,w*a21-1)*/
    int ep1x = cvl_iround(w * matrix[0]) - 1;
    int ep1y = cvl_iround(w * matrix[2]);
    /* 3. (w,h) -> (w*a11+h*a12,w*a21+h*a22) */
    int ep2x = cvl_iround(w * matrix[0] + h * matrix[1]) - 1;
    int ep2y = cvl_iround(w * matrix[2] + h * matrix[3]) - 1;
    /* 4. (0,h) -> (h*a12-1,h*a22-1) */
    int ep3x = cvl_iround(h * matrix[1]);
    int ep3y = cvl_iround(h * matrix[3]) - 1;
    /* Bounding box of the image: 
     * (minx, maxy), (maxx, maxy), (maxx, miny), (minx, miny) */
    int maxx = cvl_max4i(ep0x, ep1x, ep2x, ep3x);
    int minx = cvl_min4i(ep0x, ep1x, ep2x, ep3x);
    int maxy = cvl_max4i(ep0y, ep1y, ep2y, ep3y);
    int miny = cvl_min4i(ep0y, ep1y, ep2y, ep3y);

    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), maxx - minx + 1, maxy - miny + 1);
    double det = matrix[0] * matrix[3] - matrix[1] * matrix[2];
    double inv_matrix[4] = { matrix[3] / det, - matrix[1] / det, - matrix[2] / det, matrix[0] / det };
    for (int i = 0; i < 4; i++)
    {
	if (!isfinite(inv_matrix[i]))
	{
	    inv_matrix[i] = DBL_MAX;
	}
    }
    for (int y = 0; y < cvl_frame_height(new_frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(new_frame); x++)
	{
	    double xo = inv_matrix[0] * (double)(x + minx) + inv_matrix[1] * (double)(y + miny);
	    double yo = inv_matrix[2] * (double)(x + minx) + inv_matrix[3] * (double)(y + miny);
	    cvl_frame_set(new_frame, x, y, 
		    cvl_interpolation[interpolation_type](frame, color, xo, yo));
	}
    }

    return new_frame;
}
