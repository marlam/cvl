/*
 * cvl_convolve.c
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
 * \file cvl_convolve.h
 * \brief Applying convolution matrices.
 *
 * Applying convolution matrices to frames, separable or arbitrary.
 */

#include "config.h"

#include <math.h>

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_convolve.h"
#include "cvl_assert.h"


/*
 * Helper functions to handle sums of different pixel types.
 */

static inline void cvl_convolve_sum_helper1(cvl_pixel_type_t pixel_type, cvl_pixel_t p, int factor, int *sum)
{
    if (pixel_type == CVL_PIXEL_GRAY)
    {
	sum[0] += factor * (int)p;
    }
    else if (pixel_type == CVL_PIXEL_RGB)
    {
	sum[0] += factor * (int)cvl_pixel_rgb_to_r(p);
	sum[1] += factor * (int)cvl_pixel_rgb_to_g(p);
	sum[2] += factor * (int)cvl_pixel_rgb_to_b(p);
    }
    else // PIXEL_YUV
    {
	sum[0] += factor * (int)cvl_pixel_yuv_to_y(p);
	sum[1] += factor * (int)cvl_pixel_yuv_to_u(p);
	sum[2] += factor * (int)cvl_pixel_yuv_to_v(p);
    }
}

static inline cvl_pixel_t cvl_convolve_sum_helper2(cvl_pixel_type_t pixel_type, double factor, int *sum)
{
    if (pixel_type == CVL_PIXEL_GRAY)
    {
	return cvl_pixel_clip(cvl_iround(factor * (double)(sum[0])));
    }
    else if (pixel_type == CVL_PIXEL_RGB)
    {
	return cvl_pixel_rgb(
		cvl_pixel_clip(cvl_iround(factor * (double)(sum[0]))),
		cvl_pixel_clip(cvl_iround(factor * (double)(sum[1]))),
		cvl_pixel_clip(cvl_iround(factor * (double)(sum[2]))));
    }
    else // PIXEL_YUV
    {
	int Y = cvl_iround(factor * (double)(sum[0]));
	int U = cvl_iround(factor * (double)(sum[1]));
	int V = cvl_iround(factor * (double)(sum[2]));
	if (Y < 16)
	{
	    Y = 16;
	}
	else if (Y > 235)
	{
	    Y = 235;
	}
	if (U < 16)
	{
	    U = 16;
	}
	else if (U > 240)
	{
	    U = 240;
	}
	if (V < 16)
	{
	    V = 16;
	}
	else if (V > 240)
	{
	    V = 240;
	}
	return cvl_pixel_yuv(Y, U, V);
    }
}


/**
 * \param frame		The frame.
 * \param h		The horizontal part of the separable convolution matrix.
 * \param h_len		The length of the horizontal part.
 * \param v		The vertical part of the separable convolution matrix.
 * \param v_len		The length of the vertical part.
 * \return		The resulting frame.
 * 
 * Applies a separable convolution matrix on a frame.
 */
cvl_frame_t *cvl_frame_convolve_separable(const cvl_frame_t *frame, 
	const int *h, int h_len, const int *v, int v_len)
{
    cvl_assert(frame != NULL && h != NULL && v != NULL);
    cvl_assert(h_len > 0 && v_len > 0);
    
    cvl_frame_t *tmp_frame = cvl_frame_new(cvl_frame_pixel_type(frame), 
	    cvl_frame_width(frame), cvl_frame_height(frame));
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), 
	    cvl_frame_width(frame), cvl_frame_height(frame));
    int kh = h_len / 2;
    int kv = v_len / 2;
    int sum_h = 0, sum_v = 0;
    double factor_h, factor_v;
    
    for (int i = 0; i < h_len; i++)
    {
	sum_h += h[i];
    }
    factor_h = 1.0 / (double)sum_h;
    for (int i = 0; i < v_len; i++)
    {
	sum_v += v[i];
    }
    factor_v = 1.0 / (double)sum_v;
    
    /* h */
    for (int x = 0; x < cvl_frame_width(frame); x++)
    {
	for (int y = 0; y < cvl_frame_height(frame); y++)
	{
	    int sum[3] = { 0, 0, 0 };
	    for (int c = -kh; c <= kh; c++)
    	    {
		cvl_convolve_sum_helper1(cvl_frame_pixel_type(frame), 
			cvl_frame_get_r(frame, x + c, y), 
			h[c + kh], sum);
	    }
	    cvl_frame_set(tmp_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(frame), factor_h, sum));
	}
    }
    /* v */
    for (int x = 0; x < cvl_frame_width(frame); x++)
    {
	for (int y = 0; y < cvl_frame_height(frame); y++)
	{
	    int sum[3] = { 0, 0, 0 };
	    for (int r = -kv; r <= kv; r++)
	    {
		cvl_convolve_sum_helper1(cvl_frame_pixel_type(tmp_frame), 
			cvl_frame_get_r(tmp_frame, x, y + r), 
			v[r + kv], sum);
	    }
	    cvl_frame_set(new_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(frame), factor_v, sum));
	}
    }
    
    cvl_frame_free(tmp_frame);
    return new_frame;
}

/**
 * \param frame		The frame.
 * \param kernel	The convolution kernel as an array of ints.
 * \param dim_y		Number of rows of the kernel.
 * \param dim_x		Number of columns of the kernel.
 * \return		The resulting frame.
 * 
 * Applies a convolution kernel on a frame.
 */
cvl_frame_t *cvl_frame_convolve(const cvl_frame_t *frame, const int *kernel, int dim_y, int dim_x)
{
    cvl_assert(frame != NULL && kernel != 0);
    cvl_assert(dim_x > 0 && dim_y > 0);
    
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frame), 
	    cvl_frame_width(frame), cvl_frame_height(frame));
    int msum = 0;
    double factor = 0.0;

    for (int i = 0; i < dim_y * dim_x; i++)
    {
	msum += kernel[i];
    }
    factor = 1.0 / (double)msum;
    if (!isfinite(factor))
    {
	factor = 0.0;
    }

    for (int y = 0; y < cvl_frame_height(new_frame); y++)
    {
	for (int x = 0; x < cvl_frame_width(new_frame); x++)
	{
	    int kernel_index = 0;
	    int sum[3] = { 0, 0, 0 };
	    for (int r = - dim_y / 2; r <= dim_y / 2; r++)
	    {
		for (int c = - dim_x / 2; c <= dim_x / 2; c++)
		{
		    cvl_convolve_sum_helper1(cvl_frame_pixel_type(frame), 
			    cvl_frame_get_r(frame, x + c, y + r),
			    kernel[kernel_index++], sum);
		}
	    }	    
	    cvl_frame_set(new_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(frame), factor, sum));
	}
    }

    return new_frame;
}


/**
 * \param frames	The frames.
 * \param h		The horizontal part of the separable convolution kernel.
 * \param h_len		The length of the horizontal part.
 * \param v		The vertical part of the separable convolution kernel.
 * \param v_len		The length of the vertical part.
 * \param t		The temporal part of the separable convolution matrix.
 * \param t_len		The length of the temporal part.
 * \return		The resulting frame.
 * 
 * Applies a separable 3D convolution matrix on a frame.
 * The number of frames in the array \a frames must be \a t_len. Only the entry
 * at (\a t_len / 2) must be non-NULL, because this is the frame that will be
 * processed. The number of past and future frames (those with indices less than or
 * greater than \a t_len / 2) can be limited; in this case, some array entries
 * can be NULL. This function will use reflective indexing to compensate that.
 */
cvl_frame_t *cvl_frame_convolve3d_separable(cvl_frame_t * const *frames, 
	const int *h, int h_len, const int *v, int v_len, const int *t, int t_len)
{
    cvl_assert(frames != NULL && h != NULL && v != NULL && t != NULL);
    cvl_assert(h_len > 0 && v_len > 0 && t_len > 0);

    const cvl_frame_t *framebuf[t_len];
    cvl_frame_t *tmp1_frame = cvl_frame_new(cvl_frame_pixel_type(frames[t_len / 2]), 
	    cvl_frame_width(frames[t_len / 2]), cvl_frame_height(frames[t_len / 2]));
    cvl_frame_t *tmp2_frame = cvl_frame_new(cvl_frame_pixel_type(frames[t_len / 2]), 
	    cvl_frame_width(frames[t_len / 2]), cvl_frame_height(frames[t_len / 2]));
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frames[t_len / 2]), 
	    cvl_frame_width(frames[t_len / 2]), cvl_frame_height(frames[t_len / 2]));
    int kh = h_len / 2;
    int kv = v_len / 2;
    int kt = t_len / 2;
    int sum_h = 0, sum_v = 0, sum_t = 0;
    double factor_h, factor_v, factor_t;
    
    for (int i = 0; i < h_len; i++)
    {
	sum_h += h[i];
    }
    factor_h = 1.0 / (double)sum_h;
    for (int i = 0; i < v_len; i++)
    {
	sum_v += v[i];
    }
    factor_v = 1.0 / (double)sum_v;
    for (int i = 0; i < t_len; i++)
    {
	sum_t += t[i];
    }
    factor_t = 1.0 / (double)sum_t;
    
    // Use reflective indexing to fill the frame buffer
    int future_frames = 0;
    while (future_frames < t_len / 2 && frames[t_len / 2 + future_frames + 1])
    {
	future_frames++;
    }
    int past_frames = 0;
    while (past_frames < t_len / 2 && frames[t_len / 2 - past_frames - 1])
    {
	past_frames++;
    }
    int known_frames = past_frames + 1 + future_frames;
    int first_known = t_len / 2 - past_frames;
    for (int i = 0; i < t_len; i++)
    {
	if (i < t_len / 2 - past_frames || i > t_len / 2 + future_frames)
	{
	    
	    framebuf[i] = frames[cvl_reflect(i - first_known, known_frames) + first_known];
	}
     	else
	{
	    framebuf[i] = frames[i];
	}
    }

    /* t */
    for (int x = 0; x < cvl_frame_width(new_frame); x++)
    {
	for (int y = 0; y < cvl_frame_height(new_frame); y++)
	{
	    int sum[3] = { 0, 0, 0 };
	    for (int z = -kt; z <= kt; z++)
	    {
    		cvl_convolve_sum_helper1(cvl_frame_pixel_type(new_frame), 
			cvl_frame_get_r(framebuf[z + kt], x, y), 
			t[z + kt], sum);
	    }
	    cvl_frame_set(tmp1_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(new_frame), factor_v, sum));
	}
    }
    /* h */
    for (int x = 0; x < cvl_frame_width(new_frame); x++)
    {
	for (int y = 0; y < cvl_frame_height(new_frame); y++)
	{
	    int sum[3] = { 0, 0, 0 };
	    for (int c = -kh; c <= kh; c++)
    	    {
		cvl_convolve_sum_helper1(cvl_frame_pixel_type(new_frame),
			cvl_frame_get_r(tmp1_frame, x + c, y), 
			h[c + kh], sum);
	    }
	    cvl_frame_set(tmp2_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(new_frame), factor_h, sum));
	}
    }
    /* v */
    for (int x = 0; x < cvl_frame_width(new_frame); x++)
    {
	for (int y = 0; y < cvl_frame_height(new_frame); y++)
	{
	    int sum[3] = { 0, 0, 0 };
	    for (int r = -kv; r <= kv; r++)
	    {
		cvl_convolve_sum_helper1(cvl_frame_pixel_type(tmp1_frame), 
			cvl_frame_get_r(tmp2_frame, x, y + r), 
			v[r + kv], sum);
	    }
	    cvl_frame_set(new_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(new_frame), factor_v, sum));
	}
    }
    
    cvl_frame_free(tmp1_frame);
    cvl_frame_free(tmp2_frame);
    return new_frame;
}

/**
 * \param frames	The frames.
 * \param kernel	The convolution kernel as an array of ints.
 * \param dim_t		Dimension in temporal direction.
 * \param dim_y		Dimension in vertical direction.
 * \param dim_x		Dimension in horizontal direction.
 * \return		The resulting frame.
 * 
 * Applies a 3D convolution matrix on a frame.
 * The number of frames in the array \a frames must be \a mt_len. Only the entry
 * at (\a mt_len / 2) must be non-NULL, because this is the frame that will be
 * processed. The number of past and future frames (those with indices less than or
 * greater than \a mt_len / 2) can be limited; in this case, some array entries
 * can be NULL. This function will use reflective indexing to compensate that.
 */
cvl_frame_t *cvl_frame_convolve3d(cvl_frame_t * const *frames, 
	const int *kernel, int dim_t, int dim_y, int dim_x)
{
    cvl_assert(frames != NULL && kernel != NULL);
    cvl_assert(dim_t > 0 && dim_y > 0 && dim_x > 0);

    const cvl_frame_t *framebuf[dim_t];
    cvl_frame_t *new_frame = cvl_frame_new(cvl_frame_pixel_type(frames[dim_t / 2]), 
	    cvl_frame_width(frames[dim_t / 2]), cvl_frame_height(frames[dim_t / 2]));
    int ksum = 0;
    double factor = 0.0;

    for (int i = 0; i < dim_t * dim_y * dim_x; i++)
    {
	ksum += kernel[i];
    }
    factor = 1.0 / (double)ksum;
    if (!isfinite(factor))
    {
	factor = 0.0;
    }

    // Use reflective indexing to fill the frame buffer
    int future_frames = 0;
    while (future_frames < dim_t / 2 && frames[dim_t / 2 + future_frames + 1])
    {
	future_frames++;
    }
    int past_frames = 0;
    while (past_frames < dim_t / 2 && frames[dim_t / 2 - past_frames - 1])
    {
	past_frames++;
    }
    int known_frames = past_frames + 1 + future_frames;
    int first_known = dim_t / 2 - past_frames;
    for (int i = 0; i < dim_t; i++)
    {
	if (i < dim_t / 2 - past_frames || i > dim_t / 2 + future_frames)
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
	    int kernel_index = 0;
	    int sum[3] = { 0, 0, 0 };
	    for (int t = - dim_t / 2; t <= dim_t / 2; t++)
	    {
		for (int r = - dim_y / 2; r <= dim_y / 2; r++)
		{
		    for (int c = - dim_x / 2; c <= dim_x / 2; c++)
		    {
			    cvl_convolve_sum_helper1(cvl_frame_pixel_type(new_frame), 
				    cvl_frame_get_r(framebuf[t + dim_t / 2], x + c, y + r),
				    kernel[kernel_index++], sum);
		    }
		}
	    }
	    cvl_frame_set(new_frame, x, y, 
		    cvl_convolve_sum_helper2(cvl_frame_pixel_type(new_frame), factor, sum));
	}
    }

    return new_frame;
}
