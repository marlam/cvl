/*
 * cvl_filter.c
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
 * \file cvl_filter.h
 * \brief Filtering frames.
 *
 * Filtering frames: Gauss, Median, and Mean filter.
 */

#include "config.h"

#include <math.h>
#include <stdlib.h>

#include <gsl/gsl_randist.h>

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_convolve.h"
#include "cvl/cvl_filter.h"


/**
 * \param k		The parameter k of cvl_filter_gauss()
 * \return		Sigma.
 *
 * Computes a sigma value that matches the given \a k: roughly 95% of the
 * mass should lie within the mask.
 */
double cvl_filter_gauss_k_to_sigma(int k)
{
    return (double)k / 2.5;
}

/**
 * \param sigma		The parameter sigma of cvl_filter_gauss()
 * \return		k.
 *
 * Computes a k value that matches the given \a sigma: roughly 95% of the
 * mass should lie within the mask.
 */
int cvl_filter_gauss_sigma_to_k(double sigma)
{
    int k = cvl_iround(2.5 * sigma);

    if (k < 1)
    {
	k = 1;
    }
    return k;    
}

/**
 * \param frame		The frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param sigma_h	Sigma value in horizontal direction.
 * \param sigma_v	Sigma value in vertical direction.
 * \return		The filtered frame.
 *
 * Applies Gauss filtering to the given frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1. All pixel types are supported. See also cvl_filter_gauss_k_to_sigma()
 * and cvl_filter_gauss_sigma_to_k().
 */
cvl_frame_t *cvl_filter_gauss(const cvl_frame_t *frame, int k_h, int k_v, double sigma_h, double sigma_v)
{
    double g_h[k_h + 1];
    double g_v[k_v + 1];
    int m_h[2 * k_h + 1];
    int m_v[2 * k_v + 1];

    for (int i = 0; i <= k_h; i++)
    {
	g_h[i] = gsl_ran_gaussian_pdf((double)i, sigma_h);
    }
    for (int i = 0; i <= k_v; i++)
    {
	g_v[i] = gsl_ran_gaussian_pdf((double)i, sigma_v);
    }

    m_h[0] = 1;
    m_h[2 * k_h] = 1;
    for (int i = 1; i <= k_h; i++)
    {
	m_h[i] = cvl_iround(g_h[k_h - i] / g_h[k_h]);
    }
    for (int i = k_h + 1; i < 2 * k_h; i++)
    {
	m_h[i] = cvl_iround(g_h[i - k_h] / g_h[k_h]);
    }

    m_v[0] = 1;
    m_v[2 * k_v] = 1;
    for (int i = 1; i <= k_v; i++)
    {
	m_v[i] = cvl_iround(g_v[k_v - i] / g_v[k_v]);
    }
    for (int i = k_v + 1; i < 2 * k_v; i++)
    {
	m_v[i] = cvl_iround(g_v[i - k_v] / g_v[k_v]);
    }

    return cvl_convolve_separable(frame, m_h, 2 * k_h + 1, m_v, 2 * k_v + 1);
}

/**
 * \param frame		The frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \return		The filtered frame.
 *
 * Applies average filtering to the given frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1. All pixel types are supported.
 */
cvl_frame_t *cvl_filter_average(const cvl_frame_t *frame, int k_h, int k_v)
{
    int mh_len = 2 * k_h + 1;
    int mv_len = 2 * k_v + 1;
    int mh[mh_len];
    int mv[mv_len];
    
    for (int i = 0; i < mh_len; i++)
    {
	mh[i] = 1;
    }
    for (int i = 0; i < mv_len; i++)
    {
	mv[i] = 1;
    }
    return cvl_convolve_separable(frame, mh, mh_len, mv, mv_len);     
}

/**
 * \param frame		The frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \return		The filtered frame.
 *
 * Applies minimum filtering to the given frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1. All pixel types are supported.
 */
cvl_frame_t *cvl_filter_min(const cvl_frame_t *frame, int k_h, int k_v)
{
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), 
	    cvl_frame_width(frame), cvl_frame_height(frame));
    
    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    int min_g = 0xff + 1;
	    cvl_pixel_t min_p = 0;
	    for (int r = - k_v; r <= k_v; r++)
	    {
		for (int c = - k_h; c <= k_h; c++)
		{
		    cvl_pixel_t p = cvl_frame_get_r(frame, x + c, y + r);
		    int g = cvl_pixel_to_gray(p, cvl_frame_pixel_type(frame));
		    if (g < min_g)
		    {
			min_g = g;
			min_p = p;
		    }			
		}
	    }
	    cvl_frame_set(new_frame, x, y, min_p);
	}
    }
    return new_frame;
}

/**
 * \param frame		The frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \return		The filtered frame.
 *
 * Applies maximum filtering to the given frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1. All pixel types are supported.
 */
cvl_frame_t *cvl_filter_max(const cvl_frame_t *frame, int k_h, int k_v)
{
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), 
	    cvl_frame_width(frame), cvl_frame_height(frame));
    
    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    int max_g = 0x00 - 1;
	    cvl_pixel_t max_p = 0;
	    for (int r = - k_v; r <= k_v; r++)
	    {
		for (int c = - k_h; c <= k_h; c++)
		{
		    cvl_pixel_t p = cvl_frame_get_r(frame, x + c, y + r);
		    int g = cvl_pixel_to_gray(p, cvl_frame_pixel_type(frame));
		    if (g > max_g)
		    {
			max_g = g;
			max_p = p;
		    }			
		}
	    }
	    cvl_frame_set(new_frame, x, y, max_p);
	}
    }
    return new_frame;
}

/* Helper for cvl_filter_median() */
static int cvl_filter_median_maskcmp(const cvl_pixel_t *p1, const cvl_pixel_t *p2)
{
    return (p1[1] < p2[1] ? -1 : (p1[1] > p2[1] ? +1 : 0));
}

/**
 * \param frame		The frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \return		The filtered frame.
 *
 * Applies median filtering to the given frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1. All pixel types are supported.
 */
cvl_frame_t *cvl_filter_median(const cvl_frame_t *frame, int k_h, int k_v)
{
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), 
	    cvl_frame_width(frame), cvl_frame_height(frame));
    int masklen = (2 * k_h + 1) * (2 * k_v + 1);
    cvl_pixel_t mask[2 * masklen];
    
    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    int mask_index = 0;
	    for (int r = - k_v; r <= k_v; r++)
	    {
		for (int c = - k_h; c <= k_h; c++)
		{
		    mask[2 * mask_index] = cvl_frame_get_r(frame, x + c, y + r);
		    mask[2 * mask_index + 1] = cvl_pixel_to_gray(mask[2 * mask_index],
			    cvl_frame_pixel_type(frame));
		    mask_index++;
		}
	    }
	    qsort(mask, (size_t)masklen, 2 * sizeof(cvl_pixel_t), 
		    (int (*)(const void *, const void *))cvl_filter_median_maskcmp);
	    cvl_frame_set(new_frame, x, y, mask[2 * (masklen / 2)]);
	}
    }
    return new_frame;
}

/**
 * \param frames	The frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * \param sigma_h	Sigma value in horizontal direction.
 * \param sigma_v	Sigma value in vertical direction.
 * \param sigma_t	Sigma value in temporal direction.
 * \return		The filtered frame.
 *
 * Applies Gauss filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 * See also cvl_filter_gauss_k_to_sigma() and cvl_filter_gauss_sigma_to_k().
 */
cvl_frame_t *cvl_filter3d_gauss(cvl_frame_t * const *frames, 
	int k_h, int k_v, int k_t, double sigma_h, double sigma_v, double sigma_t)
{
    double g_h[k_h + 1];
    double g_v[k_v + 1];
    double g_t[k_t + 1];
    int m_h[2 * k_h + 1];
    int m_v[2 * k_v + 1];
    int m_t[2 * k_t + 1];

    for (int i = 0; i <= k_h; i++)
    {
	g_h[i] = gsl_ran_gaussian_pdf((double)i, sigma_h);
    }
    for (int i = 0; i <= k_v; i++)
    {
	g_v[i] = gsl_ran_gaussian_pdf((double)i, sigma_v);
    }
    for (int i = 0; i <= k_t; i++)
    {
	g_t[i] = gsl_ran_gaussian_pdf((double)i, sigma_t);
    }

    m_h[0] = 1;
    m_h[2 * k_h] = 1;
    for (int i = 1; i <= k_h; i++)
    {
	m_h[i] = cvl_iround(g_h[k_h - i] / g_h[k_h]);
    }
    for (int i = k_h + 1; i < 2 * k_h; i++)
    {
	m_h[i] = cvl_iround(g_h[i - k_h] / g_h[k_h]);
    }

    m_v[0] = 1;
    m_v[2 * k_v] = 1;
    for (int i = 1; i <= k_v; i++)
    {
	m_v[i] = cvl_iround(g_v[k_v - i] / g_v[k_v]);
    }
    for (int i = k_v + 1; i < 2 * k_v; i++)
    {
	m_v[i] = cvl_iround(g_v[i - k_v] / g_v[k_v]);
    }

    m_t[0] = 1;
    m_t[2 * k_t] = 1;
    for (int i = 1; i <= k_t; i++)
    {
	m_t[i] = cvl_iround(g_t[k_t - i] / g_t[k_t]);
    }
    for (int i = k_t + 1; i < 2 * k_t; i++)
    {
	m_t[i] = cvl_iround(g_t[i - k_t] / g_t[k_t]);
    }

    return cvl_convolve3d_separable(frames, m_h, 2 * k_h + 1, m_v, 2 * k_v + 1, m_t, 2 * k_t + 1);
}

/**
 * \param frames	The frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * \return		The filtered frame.
 *
 * Applies average filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
cvl_frame_t *cvl_filter3d_average(cvl_frame_t * const *frames, int k_h, int k_v, int k_t)
{
    int mh_len = 2 * k_h + 1;
    int mv_len = 2 * k_v + 1;
    int mt_len = 2 * k_t + 1;
    int mh[mh_len];
    int mv[mv_len];
    int mt[mt_len];
    
    for (int i = 0; i < mh_len; i++)
    {
	mh[i] = 1;
    }
    for (int i = 0; i < mv_len; i++)
    {
	mv[i] = 1;
    }
    for (int i = 0; i < mt_len; i++)
    {
	mt[i] = 1;
    }
    return cvl_convolve3d_separable(frames, mh, mh_len, mv, mv_len, mt, mt_len);
}

/**
 * \param frames	The frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * \return		The filtered frame.
 *
 * Applies minimum filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
cvl_frame_t *cvl_filter3d_min(cvl_frame_t * const *frames, int k_h, int k_v, int k_t)
{
    const cvl_frame_t *framebuf[2 * k_t + 1];
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frames[k_t]), 
	    cvl_frame_width(frames[k_t]), cvl_frame_height(frames[k_t]));
    
    // Use reflective indexing to fill the frame buffer
    int future_frames = 0;
    while (future_frames < k_t && frames[k_t + future_frames + 1])
    {
	future_frames++;
    }
    int past_frames = 0;
    while (past_frames < k_t && frames[k_t - past_frames - 1])
    {
	past_frames++;
    }
    int known_frames = past_frames + 1 + future_frames;
    int first_known = k_t - past_frames;
    for (int i = 0; i < 2 * k_t + 1; i++)
    {
	if (i < k_t - past_frames || i > k_t + future_frames)
	{
	    
	    framebuf[i] = frames[cvl_reflect(i - first_known, known_frames) + first_known];
	}
     	else
	{
	    framebuf[i] = frames[i];
	}
    }

    for (int y = 0; y < cvl_frame_height(new_frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(new_frame); x++)
	{
	    int min_g = 0xff + 1;
	    cvl_pixel_t min_p = 0;
	    for (int t = - k_t; t <= k_t; t++)
	    {
		for (int r = - k_v; r <= k_v; r++)
		{
		    for (int c = - k_h; c <= k_h; c++)
		    {
			cvl_pixel_t p = cvl_frame_get_r(framebuf[t + k_t], x + c, y + r);
			int g = cvl_pixel_to_gray(p, cvl_frame_pixel_type(framebuf[t + k_t]));
			if (g < min_g)
			{
			    min_g = g;
			    min_p = p;
			}		
		    }
		}
	    }
	    cvl_frame_set(new_frame, x, y, min_p);
	}
    }
    return new_frame;
}

/**
 * \param frames	The frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * \return		The filtered frame.
 *
 * Applies maximum filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
cvl_frame_t *cvl_filter3d_max(cvl_frame_t * const *frames, int k_h, int k_v, int k_t)
{
    const cvl_frame_t *framebuf[2 * k_t + 1];
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frames[k_t]), 
	    cvl_frame_width(frames[k_t]), cvl_frame_height(frames[k_t]));
    
    // Use reflective indexing to fill the frame buffer
    int future_frames = 0;
    while (future_frames < k_t && frames[k_t + future_frames + 1])
    {
	future_frames++;
    }
    int past_frames = 0;
    while (past_frames < k_t && frames[k_t - past_frames - 1])
    {
	past_frames++;
    }
    int known_frames = past_frames + 1 + future_frames;
    int first_known = k_t - past_frames;
    for (int i = 0; i < 2 * k_t + 1; i++)
    {
	if (i < k_t - past_frames || i > k_t + future_frames)
	{
	    
	    framebuf[i] = frames[cvl_reflect(i - first_known, known_frames) + first_known];
	}
     	else
	{
	    framebuf[i] = frames[i];
	}
    }

    for (int y = 0; y < cvl_frame_height(new_frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(new_frame); x++)
	{
	    int max_g = 0x00 - 1;
	    cvl_pixel_t max_p = 0;
	    for (int t = - k_t; t <= k_t; t++)
	    {
		for (int r = - k_v; r <= k_v; r++)
		{
		    for (int c = - k_h; c <= k_h; c++)
		    {
			cvl_pixel_t p = cvl_frame_get_r(framebuf[t + k_t], x + c, y + r);
			int g = cvl_pixel_to_gray(p, cvl_frame_pixel_type(framebuf[t + k_t]));
			if (g > max_g)
			{
			    max_g = g;
			    max_p = p;
			}		
		    }
		}
	    }
	    cvl_frame_set(new_frame, x, y, max_p);
	}
    }
    return new_frame;
}

/**
 * \param frames	The frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * \return		The filtered frame.
 *
 * Applies median filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
cvl_frame_t *cvl_filter3d_median(cvl_frame_t * const *frames, int k_h, int k_v, int k_t)
{
    const cvl_frame_t *framebuf[2 * k_t + 1];
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frames[k_t]), 
	    cvl_frame_width(frames[k_t]), cvl_frame_height(frames[k_t]));
    int masklen = (2 * k_h + 1) * (2 * k_v + 1) * (2 * k_t + 1);
    cvl_pixel_t mask[2 * masklen];
    
    // Use reflective indexing to fill the frame buffer
    int future_frames = 0;
    while (future_frames < k_t && frames[k_t + future_frames + 1])
    {
	future_frames++;
    }
    int past_frames = 0;
    while (past_frames < k_t && frames[k_t - past_frames - 1])
    {
	past_frames++;
    }
    int known_frames = past_frames + 1 + future_frames;
    int first_known = k_t - past_frames;
    for (int i = 0; i < 2 * k_t + 1; i++)
    {
	if (i < k_t - past_frames || i > k_t + future_frames)
	{
	    
	    framebuf[i] = frames[cvl_reflect(i - first_known, known_frames) + first_known];
	}
     	else
	{
	    framebuf[i] = frames[i];
	}
    }

    for (int y = 0; y < cvl_frame_height(new_frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(new_frame); x++)
	{
	    int mask_index = 0;
	    for (int t = - k_t; t <= k_t; t++)
	    {
		for (int r = - k_v; r <= k_v; r++)
		{
		    for (int c = - k_h; c <= k_h; c++)
		    {
			mask[2 * mask_index] = cvl_frame_get_r(framebuf[t + k_t], x + c, y + r);
			mask[2 * mask_index + 1] = cvl_pixel_to_gray(mask[2 * mask_index],
			    	cvl_frame_pixel_type(framebuf[t + k_t]));
			mask_index++;
		    }
		}
	    }
	    qsort(mask, (size_t)masklen, 2 * sizeof(cvl_pixel_t), 
		    (int (*)(const void *, const void *))cvl_filter_median_maskcmp);
	    cvl_frame_set(new_frame, x, y, mask[2 * (masklen / 2)]);
	}
    }
    return new_frame;
}
