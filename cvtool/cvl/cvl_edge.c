/*
 * cvl_edge.c
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
 * \file cvl_edge.h
 * \brief Detecting edges.
 *
 * Detecting edges in graylevel frames with the Sobel and Canny operators.
 */

#include "config.h"

#include <stdbool.h>
#include <math.h>

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_field.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_filter.h"
#include "cvl/cvl_edge.h"
#include "cvl/cvl_assert.h"


/**
 * \param frame		The frame.
 * \param x		x coordinate at which the derivate should be computed.
 * \param y		y coordinate at which the derivate should be computed.
 * \param fx		Place where the first partial derivate will be stored.
 * \param fy		Place where the second partial derivate will be stored.
 *
 * Computes the derivative using the Sobel method.
 * \a x must be in [1, width - 2], and \a y must be in [1, height - 2].
 * Use cvl_differentiate_sobel_r() to compute the derivative at arbitrary
 * coordinates.
 */
inline void cvl_differentiate_sobel(const cvl_frame_t *frame, int x, int y, int *fx, int *fy)
{
    *fx = cvl_iround((double)(
		(- 1 * (int)cvl_frame_get(frame, x - 1, y - 1) 
		 + 1 * (int)cvl_frame_get(frame, x + 1, y - 1)
		 - 2 * (int)cvl_frame_get(frame, x - 1, y    ) 
		 + 2 * (int)cvl_frame_get(frame, x + 1, y    )
		 - 1 * (int)cvl_frame_get(frame, x - 1, y + 1) 
		 + 1 * (int)cvl_frame_get(frame, x + 1, y + 1))) / 4.0);
    *fy = cvl_iround((double)(
		(+ 1 * (int)cvl_frame_get(frame, x - 1, y - 1) 
		 - 1 * (int)cvl_frame_get(frame, x - 1, y + 1)
		 + 2 * (int)cvl_frame_get(frame, x    , y - 1) 
		 - 2 * (int)cvl_frame_get(frame, x    , y + 1)
		 + 1 * (int)cvl_frame_get(frame, x + 1, y - 1) 
		 - 1 * (int)cvl_frame_get(frame, x + 1, y + 1))) / 4.0);
}

/**
 * \param frame		The frame.
 * \param x		x coordinate at which the derivate should be computed.
 * \param y		y coordinate at which the derivate should be computed.
 * \param fx		Place where the first partial derivate will be stored.
 * \param fy		Place where the second partial derivate will be stored.
 *
 * Computes the derivate using the Sobel method.
 * This function accepts arbitrary \a x and \a y values because it uses reflective
 * indexing. This comes at the cost of slightly higher computation costs.
 * See also cvl_differentiate_sobel().
 */
inline void cvl_differentiate_sobel_r(const cvl_frame_t *frame, int x, int y, int *fx, int *fy)
{
    *fx = cvl_iround((double)(
		(- 1 * (int)cvl_frame_get_r(frame, x - 1, y - 1) 
		 + 1 * (int)cvl_frame_get_r(frame, x + 1, y - 1)
		 - 2 * (int)cvl_frame_get_r(frame, x - 1, y    ) 
		 + 2 * (int)cvl_frame_get_r(frame, x + 1, y    )
		 - 1 * (int)cvl_frame_get_r(frame, x - 1, y + 1) 
		 + 1 * (int)cvl_frame_get_r(frame, x + 1, y + 1))) / 4.0);
    *fy = cvl_iround((double)(
		(+ 1 * (int)cvl_frame_get_r(frame, x - 1, y - 1) 
		 - 1 * (int)cvl_frame_get_r(frame, x - 1, y + 1)
		 + 2 * (int)cvl_frame_get_r(frame, x    , y - 1) 
		 - 2 * (int)cvl_frame_get_r(frame, x    , y + 1)
		 + 1 * (int)cvl_frame_get_r(frame, x + 1, y - 1) 
		 - 1 * (int)cvl_frame_get_r(frame, x + 1, y + 1))) / 4.0);
}

/**
 * \param frame			A graylevel frame.
 * \param edge_dir_field	A field to store edge directions in, or NULL.
 * \return 			A graylevel frame containing the edges.
 *
 * Detects edges using the Sobel edge detector.
 * \a edge_dir_field must be a preallocated field of doubles with the same
 * dimensions as the frame, or NULL is no edge directions are needed.
 * If it is not NULL, the direction of each edge will be stored in it, from
 * [0,2PI].
 */
cvl_frame_t *cvl_edge_sobel(const cvl_frame_t *frame, cvl_field_t *edge_dir_field)
{
    cvl_assert(frame != NULL);
    cvl_assert(edge_dir_field == NULL || (cvl_field_width(edge_dir_field) == cvl_frame_width(frame) 
		&& cvl_field_height(edge_dir_field) == cvl_frame_height(frame)));
    
    int fx, fy;
    cvl_frame_t *edge_frame = cvl_frame_new(CVL_PIXEL_GRAY, cvl_frame_width(frame), cvl_frame_height(frame));

    // Don't detect edges at the frame border.
    for (int y = 1; y < cvl_frame_height(frame) - 1; y++)
    {
	for (int x = 1; x < cvl_frame_width(frame) - 1; x++)
	{
	    cvl_differentiate_sobel(frame, x, y, &fx, &fy);
	    cvl_pixel_t g = cvl_pixel_clip(cvl_iround(sqrt((double)(fx * fx + fy * fy))));
	    cvl_frame_set(edge_frame, x, y, g);
	    if (edge_dir_field)
	    {
		double *edge_dir = cvl_field_ref(edge_dir_field, x, y);
		*edge_dir = (g > 0) ? atan2(fx, fy) : 0.0;
	    }
	}
    }
    return edge_frame;
}

/*
 * edge_non_max_suppression()
 * Used by the Canny edge detector
 */
static void cvl_frame_edge_non_max_suppression(cvl_frame_t *edge_frame, const cvl_field_t *edge_dir_field)
{
    int sp = 0;
    int sq = 0;

    cvl_frame_t *tmpframe = cvl_frame_new(CVL_PIXEL_GRAY, 
	    cvl_frame_width(edge_frame), cvl_frame_height(edge_frame));
    cvl_frame_zero(tmpframe);

    // ignore borders
    for (int y = 1; y < cvl_frame_height(edge_frame) - 1; y++)
    {
	for (int x = 1; x < cvl_frame_width(edge_frame) - 1; x++)
	{
	    if (cvl_frame_get(edge_frame, x, y) > 0)
	    {
		const double *edge_dir = cvl_field_get(edge_dir_field, x, y);
		int edge_dir_int = cvl_iround(cvl_rad_to_deg(*edge_dir));
		switch ((((edge_dir_int + 22) % 360) / 45) % 4)
		{   // Yes, that works :)
		    case 0:
			sp = cvl_frame_get(edge_frame, x, y - 1);
			sq = cvl_frame_get(edge_frame, x, y + 1);
			break;

		    case 1:
	    		sp = cvl_frame_get(edge_frame, x - 1, y - 1);
    			sq = cvl_frame_get(edge_frame, x + 1, y + 1);
			break;

		    case 2:
	    		sp = cvl_frame_get(edge_frame, x - 1, y);
    			sq = cvl_frame_get(edge_frame, x + 1, y);
			break;

		    case 3:
	    		sp = cvl_frame_get(edge_frame, x + 1, y - 1);
    			sq = cvl_frame_get(edge_frame, x - 1, y + 1);
			break;
		}
		if (!(sp > (int)cvl_frame_get(edge_frame, x, y) 
			    || sq > (int)cvl_frame_get(edge_frame, x, y)))
		{
		    cvl_frame_set(tmpframe, x, y, cvl_frame_get(edge_frame, x, y));
		}
	    }
	}
    }

    cvl_frame_copy(edge_frame, tmpframe);
    cvl_frame_free(tmpframe);
}

/**
 * \param frame		A graylevel frame.
 * \param sigma		The sigma value for gauss filtering.
 * \param tl		The tl parameter of Hysterese binarization.
 * \param th		The th parameter of Hysterese binarization.
 * \param edge_dir_field	A field to store edge directions in.
 * \return		A graylevel frame containing the binarized edges.
 * 
 * Detects edges using the Canny edge detector.
 * The Canny edge detection algorithm consists of the following steps:
 * Gauss filtering with parameter \a sigma, Sobel edge detector, non-maximum
 * suppression, and binarization with the Hysterese method using the parameters
 * \a tl and \a th.
 * \a edge_dir_field must be a preallocated field of doubles with the same
 * dimensions as the frame. It must not be NULL. The direction of each edge
 * will be stored in it, from [0,2PI].
 */
cvl_frame_t *cvl_edge_canny(const cvl_frame_t *frame, 
	double sigma, int tl, int th, cvl_field_t *edge_dir_field)
{
    cvl_assert(frame != NULL);
    cvl_assert(edge_dir_field != NULL);
    cvl_assert(cvl_field_width(edge_dir_field) == cvl_frame_width(frame));
    cvl_assert(cvl_field_height(edge_dir_field) == cvl_frame_height(frame));
    
    int gauss_k = cvl_filter_gauss_sigma_to_k(sigma);
    cvl_frame_t *smoothed_frame;
    cvl_frame_t *edge_frame;
    
    smoothed_frame = cvl_filter_gauss(frame, sigma, sigma, gauss_k, gauss_k);
    edge_frame = cvl_edge_sobel(smoothed_frame, edge_dir_field);
    cvl_frame_free(smoothed_frame);
    cvl_frame_edge_non_max_suppression(edge_frame, edge_dir_field);
    cvl_frame_binarize_hysterese(edge_frame, tl, th);
    
    return edge_frame;
}
