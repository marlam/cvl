/*
 * cvl_stereo.c
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
 * \file cvl_stereo.h
 * \brief Stereoscopic videos.
 *
 * Creating and manipulating stereoscopic videos.
 */

#include "config.h"

#include <stdbool.h>
#include <stdint.h>

#include "xalloc.h"

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_field.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_stereo.h"
#include "cvl_assert.h"


/******************************************************************************
 *
 * Depth image based rendering (DIBR)
 *
 *****************************************************************************/

/**
 * \typedef cvl_dibr_fill_technique_t
 * 
 * Chooses how to fill disocclusion holes.
 */
/** \var CVL_DIBR_FILL_NONE
 * No filling. */
/** \var CVL_DIBR_FILL_AVG
 * Use average of the border colors. */
/** \var CVL_DIBR_FILL_NEAR
 * Use color of the nearer border. */
/** \var CVL_DIBR_FILL_FAR
 * Use color of the farther border. */
/** \var CVL_DIBR_FILL_LINEAR
 * Use linear color gradient. */


/* A helper function to compute the average of two pixels. */
static inline cvl_pixel_t cvl_dibr_pixel_average(cvl_pixel_type_t pixel_type, cvl_pixel_t p1, cvl_pixel_t p2)
{
    cvl_pixel_t p;
    
    if (pixel_type == CVL_PIXEL_GRAY)
    {
	p = (p1 + p2) / 2;
    }
    else if (pixel_type == CVL_PIXEL_RGB)
    {
	p = cvl_pixel_rgb(
		(cvl_pixel_rgb_to_r(p1) + cvl_pixel_rgb_to_r(p2)) / 2,
		(cvl_pixel_rgb_to_g(p1) + cvl_pixel_rgb_to_g(p2)) / 2,
		(cvl_pixel_rgb_to_b(p1) + cvl_pixel_rgb_to_b(p2)) / 2);
    }
    else // CVL_PIXEL_YUV
    {
	p = cvl_pixel_yuv(
		(cvl_pixel_yuv_to_y(p1) + cvl_pixel_yuv_to_y(p2)) / 2,
		(cvl_pixel_yuv_to_u(p1) + cvl_pixel_yuv_to_u(p2)) / 2,
		(cvl_pixel_yuv_to_v(p1) + cvl_pixel_yuv_to_v(p2)) / 2);
    }

    return p;
}

/* A helper function to compute a linear gradient between to pixel values. */
static inline void cvl_dibr_color_gradient(cvl_pixel_t *gradient, int steps, 
	cvl_pixel_type_t pixel_type, cvl_pixel_t p1, cvl_pixel_t p2)
{
    double divisor = (double)(steps + 1);
    
    if (pixel_type == CVL_PIXEL_GRAY)
    {
	double gstep = (double)((int)p2 - (int)p1) / divisor;
	for (int i = 0; i < steps; i++)
	{
	    gradient[i] = cvl_pixel_gray(
		    cvl_pixel_clip(cvl_iround((double)cvl_pixel_gray(p1) + (double)(i + 1) * gstep)));
	}
    }
    else if (pixel_type == CVL_PIXEL_RGB)
    {
	double rstep = (double)((int)cvl_pixel_rgb_to_r(p2) - (int)cvl_pixel_rgb_to_r(p1)) / divisor;
	double gstep = (double)((int)cvl_pixel_rgb_to_g(p2) - (int)cvl_pixel_rgb_to_g(p1)) / divisor;
	double bstep = (double)((int)cvl_pixel_rgb_to_b(p2) - (int)cvl_pixel_rgb_to_b(p1)) / divisor;
	for (int i = 0; i < steps; i++)
	{
	    gradient[i] = cvl_pixel_rgb(
		cvl_pixel_clip(cvl_iround((double)cvl_pixel_rgb_to_r(p1) + (double)(i + 1) * rstep)),
		cvl_pixel_clip(cvl_iround((double)cvl_pixel_rgb_to_g(p1) + (double)(i + 1) * gstep)),
		cvl_pixel_clip(cvl_iround((double)cvl_pixel_rgb_to_b(p1) + (double)(i + 1) * bstep)));
	}
    }
    else // (pixel_type == CVL_PIXEL_YUV)
    {
	double ystep = (double)((int)cvl_pixel_yuv_to_y(p2) - (int)cvl_pixel_yuv_to_y(p1)) / divisor;
	double ustep = (double)((int)cvl_pixel_yuv_to_u(p2) - (int)cvl_pixel_yuv_to_u(p1)) / divisor;
	double vstep = (double)((int)cvl_pixel_yuv_to_v(p2) - (int)cvl_pixel_yuv_to_v(p1)) / divisor;
	for (int i = 0; i < steps; i++)
	{
	    gradient[i] = cvl_pixel_yuv(
		cvl_pixel_clip(cvl_iround((double)cvl_pixel_yuv_to_y(p1) + (double)(i + 1) * ystep)),
		cvl_pixel_clip(cvl_iround((double)cvl_pixel_yuv_to_u(p1) + (double)(i + 1) * ustep)),
		cvl_pixel_clip(cvl_iround((double)cvl_pixel_yuv_to_v(p1) + (double)(i + 1) * vstep)));
	}
    }
}

/* A helper function to fill the holes that result from disocclusions. */
static int cvl_dibr_fill_hole(cvl_frame_t *frame, int offset, int *mask, int l, int y, int technique)
{
    cvl_pixel_t p = 0;
    int steps = 0;
    cvl_pixel_t *gradient = NULL;
    int r = l + 1;

    /* Determine the size of the hole */
    while (r < cvl_frame_width(frame) / 2 && mask[r] == -1)
    {
	r++;
    }
    r--;
    
    /* Determine the color(s) to fill the hole with. This depends on the
     * technique. */
    if (technique == CVL_DIBR_FILL_LINEAR)
    {
	steps = r - l + 1;
	gradient = xmalloc(steps * sizeof(cvl_pixel_t));
    }
    if (l > 0 && r < cvl_frame_width(frame) / 2 - 1)
    {
	cvl_pixel_t p1 = cvl_frame_get(frame, l - 1 + offset, y);
	cvl_pixel_t p2 = cvl_frame_get(frame, r + 1 + offset, y);
	if (technique == CVL_DIBR_FILL_NONE)
	{
	    p = cvl_color_to_pixel(CVL_COLOR_WHITE, cvl_frame_pixel_type(frame));
	}
	else if (technique == CVL_DIBR_FILL_AVG)
	{
	    p = cvl_dibr_pixel_average(cvl_frame_pixel_type(frame), p1, p2);
	}
	else if (technique == CVL_DIBR_FILL_NEAR)
	{
	    p = (mask[l - 1] > mask[r + 1]) ? p1 : p2;
	}
	else if (technique == CVL_DIBR_FILL_FAR)
	{
	    p = (mask[l - 1] < mask[r + 1]) ? p1 : p2;
	}
	else if (technique == CVL_DIBR_FILL_LINEAR)
    	{
	    cvl_dibr_color_gradient(gradient, steps, cvl_frame_pixel_type(frame), p1, p2);
	}
    }
    else if (l > 0)
    {
	if (technique == CVL_DIBR_FILL_NONE)
	{
	    p = cvl_color_to_pixel(CVL_COLOR_WHITE, cvl_frame_pixel_type(frame));
	}
	else if (technique == CVL_DIBR_FILL_LINEAR)
	{
	    for (int i = 0; i < steps; i++)
	    {
		gradient[i] = cvl_frame_get(frame, l - 1 + offset, y);
	    }
	}
	else
	{		
	    p = cvl_frame_get(frame, l - 1 + offset, y);
	}
    }
    else if (r < cvl_frame_width(frame) / 2 - 1)
    {
	if (technique == CVL_DIBR_FILL_NONE)
	{
	    p = cvl_color_to_pixel(CVL_COLOR_WHITE, cvl_frame_pixel_type(frame));
	}
	else if (technique == CVL_DIBR_FILL_LINEAR)
	{
	    for (int i = 0; i < steps; i++)
	    {
		gradient[i] = cvl_frame_get(frame, r + 1 + offset, y);
	    }
	}
	else
	{		
	    p = cvl_frame_get(frame, r + 1 + offset, y);
	}
    }
    else
    {
	if (technique == CVL_DIBR_FILL_NONE)
	{
	    p = cvl_color_to_pixel(CVL_COLOR_WHITE, cvl_frame_pixel_type(frame));
	}
	else if (technique == CVL_DIBR_FILL_LINEAR)
	{
    	    gradient[0] = cvl_color_to_pixel(CVL_COLOR_BLACK, cvl_frame_pixel_type(frame));
	}
	else
	{		
	    p = cvl_color_to_pixel(CVL_COLOR_BLACK, cvl_frame_pixel_type(frame));
	}
    }

    /* Fill the hole. */
    for (int x = l; x <= r; x++)
    {
    	if (technique == CVL_DIBR_FILL_LINEAR)
	{
	    cvl_frame_set(frame, x + offset, y, gradient[x - l]);
	}
	else
	{
	    cvl_frame_set(frame, x + offset, y, p);
	}
    }

    if (technique == CVL_DIBR_FILL_LINEAR)
    {
	free(gradient);
    }
    return r;
}

/**
 * \param frame		The intermediate view.
 * \param depthmap	The depth map.
 * \param position	Position of the intermediate view.
 * \param zps		Zero parallax setting, should be 0.
 * \param b		Virtual eye distance in pixels.
 * \param f		focal length, usually 1.
 * \param fill_technique	Technique to fill disocclusion holes.
 * \return		A stereoscopic image pair in one frame, left and right view side by side.
 *
 * Calculate a stereoscopic image pair from an intermediate view and a depth 
 * map. The returned frame will contain both the left and right view, side by 
 * side. The \a position parameter refers to the source frame: it must be in 
 * [-1,+1], where -1 means left view, +1 means right view, and 0 means 
 * intermediate view. \a zps is the zero parallax setting, it must be in [0,1]. 
 * 1 means near, 0 means far. \a b is the "eye" distance in pixels. \a f is the 
 * camera focal length; usually set to 1.0. Disocclusion holes will be filled
 * with \a fill_technique. Make sure that left and right view fit into one frame
 * without overflowing an int.
 */
cvl_frame_t *cvl_dibr(const cvl_frame_t *frame, const cvl_frame_t *depthmap, 
	double position, double zps, int b, double f, cvl_dibr_fill_technique_t fill_technique)
{
    cvl_assert(frame != NULL && depthmap != NULL);
    cvl_assert(cvl_frame_pixel_type(depthmap) == CVL_PIXEL_GRAY);
    cvl_assert(cvl_frame_width(frame) == cvl_frame_width(depthmap) && cvl_frame_height(frame) == cvl_frame_height(depthmap));
    cvl_assert(cvl_product_fits_in_int(cvl_frame_width(frame), 2));
    cvl_assert(position >= -1.0 && position <= 1.0);
    cvl_assert(zps >= 0.0 && zps <= 1.0);
    cvl_assert(b >= 0);
    cvl_assert(f >= 0.0);

    int *L_mask = xmalloc(cvl_frame_width(frame) * sizeof(int));
    int *R_mask = xmalloc(cvl_frame_width(frame) * sizeof(int));
    double factor = 0.5 * (double)b * f;
    cvl_frame_t *stereoframe = cvl_frame_new(cvl_frame_pixel_type(frame), cvl_frame_width(frame) * 2, cvl_frame_height(frame));

    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    L_mask[x] = -1;
	    R_mask[x] = -1;
	}

	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    double shift = factor * (zps - (double)(cvl_frame_get(depthmap, x, y) + 1) / 256.0);
	    int xl = x - cvl_iround(shift * (1.0 + position));
	    int xr = x + cvl_iround(shift * (1.0 - position));

    	    if (xl >= 0 && xl < cvl_frame_width(frame) && L_mask[xl] < (int)cvl_frame_get(depthmap, x, y))
	    {
		L_mask[xl] = cvl_frame_get(depthmap, x, y);
		cvl_frame_set(stereoframe, xl + 0, y, cvl_frame_get(frame, x, y));
	    }
	    if (xr >= 0 && xr < cvl_frame_width(frame) && R_mask[xr] < (int)cvl_frame_get(depthmap, x, y))
	    {
		R_mask[xr] = cvl_frame_get(depthmap, x, y);
		cvl_frame_set(stereoframe, xr + cvl_frame_width(frame), y, cvl_frame_get(frame, x, y));
	    }
	}

	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
    	    if (L_mask[x] == -1)
	    {
		x = cvl_dibr_fill_hole(stereoframe, 0, L_mask, x, y, fill_technique);
	    }
	}
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    if (R_mask[x] == -1)
	    {
    		x = cvl_dibr_fill_hole(stereoframe, cvl_frame_width(frame), R_mask, x, y, fill_technique);
	    }
	}
    }

    free(L_mask);
    free(R_mask);
    return stereoframe;
}


/******************************************************************************
 *
 * Depth tracking
 *
 *****************************************************************************/

/**
 * \param trackmap	The first trackmap to be initialized.
 * \param depth		The depth frame for the first trackmap, or NULL if unavailable.
 *
 * Initializes the first trackmap for one depth tracking direction.
 */
void cvl_trackdepth_init_first_trackmap(cvl_field_t *trackmap, const cvl_frame_t *depth)
{
    int16_t d;
    for (int i = 0; i < cvl_field_width(trackmap) * cvl_field_height(trackmap); i++)
    {
	d = (depth ? (int16_t)cvl_frame_get_i(depth, i) : -1);
	cvl_field_set_i(trackmap, i, &d);
    }
}

/**
 * \param trackmap	The next trackmap to be initialized.
 * \param prev_trackmap	The previous, already initialized trackmap.
 * \param flow		The flow in direction from \a trackmap to \a prev_trackmap.
 *
 * Initializes a trackmap for one depth tracking direction.
 */
void cvl_trackdepth_init_next_trackmap(cvl_field_t *trackmap, 
	const cvl_field_t *prev_trackmap, const cvl_field_t *flow)
{
    for (int y = 0; y < cvl_field_height(trackmap); y++)
    {
	for (int x = 0; x < cvl_field_width(trackmap); x++)
	{
	    int16_t d;
	    cvl_vector2i_t flow_vector;
	    cvl_field_get(flow, x, y, &flow_vector);
	    int next_pos_x = x + flow_vector[0];
	    int next_pos_y = y + flow_vector[1];
	    if (next_pos_x >= 0 && next_pos_x < cvl_field_width(trackmap) 
		    && next_pos_y >= 0 && next_pos_y < cvl_field_height(trackmap))
	    {
		cvl_field_get(prev_trackmap, next_pos_x, next_pos_y, &d);
	    }
	    else
	    {
		d = -1;
	    }
	    cvl_field_set(trackmap, x, y, &d);
	}
    }
}

/**
 * \param N		The number of frames for which depth tracking is done.
 * \param req		The number of the requested depth map, 0 <= req <= N-1.
 * \param trackmap_A	The trackmap number \a req for the depth at the first frame.
 * \param trackmap_O	The trackmap number \a req for the depth at the last frame.
 * \return		The requested depth map.
 *
 * Computes a depth map with depth tracking.
 * The complete depth tracking process works as follows:\n
 * 1. Initialze N trackmaps for depth A (= depth at frame 0) with
 *    cvl_trackdepth_init_first_trackmap().\n
 * 2. Initialze N trackmaps for depth O (= depth at frame N-1) with
 *    cvl_trackdepth_init_first_trackmap().\n
 * 3. Call cvl_trackdepth() for \a req=0,...,\a req=N-1. 
 */
cvl_frame_t *cvl_trackdepth(int N, int req, const cvl_field_t *trackmap_A, const cvl_field_t *trackmap_O)
{
    int16_t dA, dO;
    cvl_pixel_t depth;
    cvl_frame_t *depthmap;
    
    depthmap = cvl_frame_new(CVL_PIXEL_GRAY, cvl_field_width(trackmap_A), cvl_field_height(trackmap_A));
    for (int i = 0; i < cvl_field_width(trackmap_A) * cvl_field_height(trackmap_A); i++)
    {
	cvl_field_get_i(trackmap_A, i, &dA);
	cvl_field_get_i(trackmap_O, i, &dO);
	if (dA == -1 && dO == -1)
	{
	    depth = 0x00;
	}
	else if (dA == -1)
	{
	    depth = dO;
	}
	else if (dO == -1)
	{
	    depth = dA;
	}
	else
	{
	    depth = cvl_iround((double)dA + (((double)req / (double)(N - 1)) * (double)(dO - dA)));
	}
	cvl_frame_set_i(depthmap, i, depth);
    }
    return depthmap;
}


/******************************************************************************
 *
 * Anaglyph images
 *
 *****************************************************************************/

/**
 * \param stereoframe	Left and right view, side by side.
 * \param color		Make a full color anaglyph?
 * \param type		Type of glasses that the anaglyph will be viewed with.
 * \return		The anaglyph frame.
 * 
 * Computes an anaglyph image from a stereo view.
 * Left and right view must have the same dimensions.
 * If \a color is true, the anaglyph will have full color. This makes only sense
 * when the type is #CVL_ANAGLYPH_RED_CYAN.
 * If \a color is false, the anaglyph will appear monochrome. This works with all
 * types.
 * The anaglyph frame will always be an RGB frame.
 */
cvl_frame_t *cvl_anaglyph(const cvl_frame_t *stereoframe, bool color, cvl_anaglyph_type_t type)
{
    cvl_frame_t *frame = cvl_frame_new(CVL_PIXEL_RGB, 
	    cvl_frame_width(stereoframe) / 2, cvl_frame_height(stereoframe));
    
    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    cvl_pixel_t pl = cvl_frame_get(stereoframe, x, y);
	    cvl_pixel_t pr = cvl_frame_get(stereoframe, x + cvl_frame_width(frame), y);
	    cvl_pixel_t r, g, b;
	    if (color)
	    {
		if (cvl_frame_pixel_type(stereoframe) == CVL_PIXEL_YUV)
		{
		    pl = cvl_pixel_yuv_to_rgb(pl);
		    pr = cvl_pixel_yuv_to_rgb(pr);
		}
		else if (cvl_frame_pixel_type(stereoframe) == CVL_PIXEL_GRAY)
		{
		    pl = cvl_pixel_gray_to_rgb(pl);
		    pr = cvl_pixel_gray_to_rgb(pr);
		}
    		r = cvl_pixel_rgb_to_r(pl);
		g = (type != CVL_ANAGLYPH_RED_BLUE) ? cvl_pixel_rgb_to_g(pr) : 0;
		b = (type != CVL_ANAGLYPH_RED_GREEN) ? cvl_pixel_rgb_to_b(pr) : 0;
	    }
	    else
	    {
		if (cvl_frame_pixel_type(stereoframe) == CVL_PIXEL_YUV)
		{
		    pl = cvl_pixel_yuv_to_gray(pl);
		    pr = cvl_pixel_yuv_to_gray(pr);
		}
		else if (cvl_frame_pixel_type(stereoframe) == CVL_PIXEL_RGB)
		{
		    pl = cvl_pixel_rgb_to_gray(pl);
		    pr = cvl_pixel_rgb_to_gray(pr);
		}
		r = pl;
		g = (type != CVL_ANAGLYPH_RED_BLUE) ? pr : 0;
		b = (type != CVL_ANAGLYPH_RED_GREEN) ? pr : 0;
	    }
	    cvl_frame_set(frame, x, y, cvl_pixel_rgb(r, g, b));
	}
    }

    return frame;
}
