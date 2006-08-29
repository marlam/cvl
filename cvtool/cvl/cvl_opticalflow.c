/*
 * cvl_opticalflow.c
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
 * \file cvl_opticalflow.h
 * \brief Optical flow.
 *
 * Computing the optical flow between subsequent frames.
 */

#include "config.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include <gsl/gsl_randist.h>

#include "xalloc.h"

#include "cvl/cvl_assert.h"
#include "cvl/cvl_msg.h"
#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_field.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_filter.h"
#include "cvl/cvl_misc.h"
#include "cvl/cvl_vector.h"
#include "cvl/cvl_opticalflow.h"


/*
 * This function calculates the approximations for the derivatives Ex, Ey, Et of
 * the frames frame0 and frame1.
 */
static inline void cvl_opticalflow_differentiate(
	const cvl_frame_t *frame0, const cvl_frame_t *frame1, 
	int x, int y, float *Ex, float *Ey, float *Et)
{
    *Ex = (float)
	(+ 1 * (int)cvl_frame_get_r(frame0, x    , y + 1)
     	 - 1 * (int)cvl_frame_get_r(frame0, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame0, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x + 1, y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x    , y + 1)
	 - 1 * (int)cvl_frame_get_r(frame1, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame1, x + 1, y    )) / 4.0f;
    *Ey = (float)
	(+ 1 * (int)cvl_frame_get_r(frame0, x + 1, y    )
     	 - 1 * (int)cvl_frame_get_r(frame0, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame0, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x    , y + 1)
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y    )
	 - 1 * (int)cvl_frame_get_r(frame1, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame1, x    , y + 1)) / 4.0f;
    *Et = (float)
	(+ 1 * (int)cvl_frame_get_r(frame1, x    , y    )
     	 - 1 * (int)cvl_frame_get_r(frame0, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y    )
	 - 1 * (int)cvl_frame_get_r(frame0, x + 1, y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x    , y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x    , y + 1)
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x + 1, y + 1)) / 4.0f;
}

/**
 * \param f1		The frame at t = 0. Must be graylevel.
 * \param f2		The frame at t = 1. Must be graylevel.
 * \param lambda	The lambda parameter of the Horn/Schunck algorithm. Try 10.0.
 * \param iterations	Number of iterations. Try 100.
 * \return		The optical flow in a field of int[2] elements.
 *
 * Computes the optical flow between to equally big frames. This function
 * implements the Horn/Schunck algorithm.
 */
cvl_field_t *cvl_opticalflow_hs(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	double lambda, int iterations)
{
    cvl_assert(f1 != NULL);
    cvl_assert(f2 != NULL);
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(iterations > 0);

    int w = cvl_frame_width(f1);
    int h = cvl_frame_height(f1);
    float *Ex = xnmalloc(w * h, sizeof(float));
    float *Ey = xnmalloc(w * h, sizeof(float));
    float *Et = xnmalloc(w * h, sizeof(float));
    float *flowtmp = xnmalloc(w * h, 2 * sizeof(float));
    cvl_frame_t *f1_gray, *f2_gray;
    
    f1_gray = cvl_frame_clone(f1);
    cvl_frame_to_gray(f1_gray);
    f2_gray = cvl_frame_clone(f2);
    cvl_frame_to_gray(f2_gray);
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    cvl_opticalflow_differentiate(f1_gray, f2_gray, x, y, 
		    Ex + y * w + x, Ey + y * w + x, Et + y * w + x);
	    flowtmp[2 * (y * w + x) + 0] = 0.0f;
	    flowtmp[2 * (y * w + x) + 1] = 0.0f;
	}
    }
    cvl_frame_free(f1_gray);
    cvl_frame_free(f2_gray);

    for (int i = 0; i < iterations; i++)
    {
	for (int y = 0; y < h; y++)
	{
	    for (int x = 0; x < w; x++)
	    {
		float flow0 = flowtmp[2 * (y * w + x) + 0];
		float flow1 = flowtmp[2 * (y * w + x) + 1];
		float ex = Ex[y * w + x];
		float ey = Ey[y * w + x];
		float et = Et[y * w + x];
		float frac = (ex * flow0 + ey * flow1 + et) 
		    / (1.0 + lambda * (ex * ex + ey * ey));
		flowtmp[2 * (y * w + x) + 0] -= ex * frac;
		flowtmp[2 * (y * w + x) + 1] -= ey * frac;
	    }
	}
    }
    free(Ex);
    free(Ey);
    free(Et);

    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), w, h);
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    int flowi[2];
	    flowi[0] = cvl_iroundf(flowtmp[2 * (y * w + x) + 0]);
    	    flowi[1] = cvl_iroundf(flowtmp[2 * (y * w + x) + 1]);
	    cvl_field_set(flowfield, x, y, flowi);
	}
    }
    free(flowtmp);
    return flowfield;
}

/**
 * \param f1		The frame at t = 0. Must be graylevel.
 * \param f2		The frame at t = 1. Must be graylevel.
 * \param k		The size of the neighborhood (2k+1 x 2k+1).
 * \return		The optical flow in a field of int[2] elements.
 *
 * Computes the optical flow between to equally big frames. This function
 * implements the Lucas/Kanade algorithm. The frames should be smoothed with a
 * Gaussian filter first.
 */
cvl_field_t *cvl_opticalflow_lk(const cvl_frame_t *f1, const cvl_frame_t *f2, int k)
{
    cvl_assert(f1 != NULL);
    cvl_assert(f2 != NULL);
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(k > 0);

    int w = cvl_frame_width(f1);
    int h = cvl_frame_height(f1);
    float *Ex = xnmalloc(w * h, sizeof(float));
    float *Ey = xnmalloc(w * h, sizeof(float));
    float *Et = xnmalloc(w * h, sizeof(float));
    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), w, h);
    cvl_frame_t *f1_gray, *f2_gray;

    /* Compute the approximations of the derivatives */
    f1_gray = cvl_frame_clone(f1);
    cvl_frame_to_gray(f1_gray);
    f2_gray = cvl_frame_clone(f2);
    cvl_frame_to_gray(f2_gray);
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    cvl_opticalflow_differentiate(f1_gray, f2_gray, x, y, 
		    Ex + y * w + x, Ey + y * w + x, Et + y * w + x);
	}
    }
    cvl_frame_free(f1_gray);
    cvl_frame_free(f2_gray);
    
    /* Compute the weight matrix. It has the size 2k+1 x 2k+1 and is the same
     * matrix that would be used for Gauss filtering. */
    float weight_sigma = cvl_filter_gauss_k_to_sigma(k);
    float weight_gauss_vals1[k + 1];
    for (int i = 0; i <= k; i++)
    {
	weight_gauss_vals1[i] = gsl_ran_gaussian_pdf((double)i, weight_sigma);
    }
    int weight_gauss_vals2[2 * k + 1];
    weight_gauss_vals2[0] = 1;
    weight_gauss_vals2[2 * k] = 1;
    for (int i = 1; i <= k; i++)
    {
	weight_gauss_vals2[i] = cvl_iroundf(weight_gauss_vals1[k - i] / weight_gauss_vals1[k]);
    }
    for (int i = k + 1; i < 2 * k; i++)
    {
	weight_gauss_vals2[i] = cvl_iroundf(weight_gauss_vals1[i - k] / weight_gauss_vals1[k]);
    }
    int weight[2 * k + 1][2 * k + 1];
    int weight_sum = 0;
    for (int r = 0; r < 2 * k + 1; r++)
    {
	for (int c = 0; c < 2 * k + 1; c++)
	{
	    weight[r][c] = cvl_iroundf(sqrtf(weight_gauss_vals2[r] * weight_gauss_vals2[c]));
    	    weight_sum += weight[r][c];
	}	
    }

    /* Compute the optical flow */
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    /*      (Sum(weight*fx*fx)  Sum(weight*fx*fy))
	     * A := (                                    )
	     *      (Sum(weight*fx*fy)  Sum(weight*fy*fy))
	     *
	     * (flow_x)    -1   (- Sum(weight*fx*ft))
	     * (      ) = A   * (                   )
	     * (flow_y)         (- Sum(weight*fy*ft))
	     */
	    float sum_wfxfx = 0.0;
	    float sum_wfxfy = 0.0;
	    float sum_wfxft = 0.0;
	    float sum_wfyfy = 0.0;
	    float sum_wfyft = 0.0;
	    for (int r = -k; r <= k; r++)
	    {
		for (int c = -k; c <= k; c++)
		{
		    int yy = cvl_reflect(y + r, h);
		    int xx = cvl_reflect(x + c, w);
		    float ww = (float)weight[r + k][c + k] / (float)weight_sum;
		    float fx = Ex[yy * w + xx];
		    float fy = Ey[yy * w + xx];
		    float ft = Et[yy * w + xx];
		    sum_wfxfx += ww * fx * fx;
		    sum_wfxfy += ww * fx * fy;
		    sum_wfxft += ww * fx * ft;
		    sum_wfyfy += ww * fy * fy;
		    sum_wfyft += ww * fy * ft;
		}
	    }
	    float det = sum_wfxfx * sum_wfyfy - sum_wfxfy * sum_wfxfy;
	    int flow[2];
	    if (fabsf(det) > FLT_MIN)
	    {
		float invdet = 1.0f / det;
		flow[0] = cvl_iroundf((- sum_wfxft * sum_wfyfy + sum_wfyft * sum_wfxfy) * invdet);
		flow[1] = cvl_iroundf((+ sum_wfxft * sum_wfxfy - sum_wfyft * sum_wfxfx) * invdet);
	    }
	    else
	    {
		flow[0] = 0;
		flow[1] = 0;
	    }
	    cvl_field_set(flowfield, x, y, flow);
	}
    }	
    free(Ex);
    free(Ey);
    free(Et);
    return flowfield;
}

/**
 * \param f1		The frame at t = 0. Must be graylevel.
 * \param f2		The frame at t = 1. Must be graylevel.
 * \param lambda	The lambda parameter of the Horn/Schunck algorithm. Try 10.0.
 * \param omega		The relaxation parameter, from (0,2). Try 1.95.
 * \param iterations	Number of iterations. Try 100.
 * \return		The optical flow in a field of int[2] elements.
 *
 * Computes the optical flow between to equally big frames. This function
 * implements the combined local/global approach suggested by Weickert/Schnoerr
 * in "Lucas/Kanade Meets Horn/Schunck: Combining Local and Global Optic Flow
 * Methods".
 */
cvl_field_t *cvl_opticalflow_clg(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	double lambda, double omega, int iterations)
{
    cvl_assert(f1 != NULL);
    cvl_assert(f2 != NULL);
    cvl_assert(iterations > 0);
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));

    int w = cvl_frame_width(f1);
    int h = cvl_frame_height(f1);
    float factor;
    float *J12 = xnmalloc(w * h, sizeof(float));
    float *J13 = xnmalloc(w * h, sizeof(float));
    float *J21 = J12;
    float *J23 = xnmalloc(w * h, sizeof(float));
    float *divisor1 = xnmalloc(w * h, sizeof(float));
    float *divisor2 = xnmalloc(w * h, sizeof(float));
    float *flowtmp = xnmalloc(w * h, 2 * sizeof(float));
    const int weight[3][3] = { { 1, 2, 1 }, { 2, 4, 2 }, { 1, 2, 1 } };
    const int weight_sum = 16;
    cvl_frame_t *f1_gray, *f2_gray;

    /* Precompute some values for the following iterative method. */
    f1_gray = cvl_frame_clone(f1);
    cvl_frame_to_gray(f1_gray);
    f2_gray = cvl_frame_clone(f2);
    cvl_frame_to_gray(f2_gray);
    factor = 1.0 / lambda;
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    float fx, fy, ft, j11, j22;
	    cvl_opticalflow_differentiate(f1_gray, f2_gray, x, y, &fx, &fy, &ft);
	    J12[y * w + x] = fx * fy * ((float)weight[0][1] / (float)weight_sum);
	    J13[y * w + x] = fx * ft * ((float)weight[0][2] / (float)weight_sum);
	    J23[y * w + x] = fy * ft * ((float)weight[1][2] / (float)weight_sum);
	    j11 = fx * fx * ((float)weight[0][0] / (float)weight_sum);
	    j22 = fy * fy * ((float)weight[1][1] / (float)weight_sum);
	    divisor1[y * w + x] = 4.0 + factor * j11;
	    divisor2[y * w + x] = 4.0 + factor * j22;
	}
    }
    for (int i = 0; i < w * h; i++)
    {
	flowtmp[2 * i + 0] = 0.0f;
	flowtmp[2 * i + 1] = 0.0f;
    }
    cvl_frame_free(f1_gray);
    cvl_frame_free(f2_gray);

    /* Iterative computing of the optical flow. */
    for (int i = 0; i < iterations; i++)
    {
	for (int y = 0; y < h; y++)
	{
	    for (int x = 0; x < w; x++)
	    {
		float j12 = J12[y * w + x];
		float j13 = J13[y * w + x];
		float j21 = J21[y * w + x];
		float j23 = J23[y * w + x];
		float d1 = divisor1[y * w + x];
		float d2 = divisor2[y * w + x];
		float *neighborflow;
		float neighborsum[2];

		cvl_vector2_zero(neighborsum);
		neighborflow = flowtmp + 2 * (cvl_reflect(y - 1, h) * w + x);
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		neighborflow = flowtmp + 2 * (cvl_reflect(y + 1, h) * w + x);
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		neighborflow = flowtmp + 2 * (y * w + cvl_reflect(x - 1, w));
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		neighborflow = flowtmp + 2 * (y * w + cvl_reflect(x + 1, w));
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		
		flowtmp[2 * (y * w + x) + 0] = (1.0 - omega) * flowtmp[2 * (y * w + x) + 0] 
		    + omega * (neighborsum[0] - factor * (j12 * flowtmp[2 * (y * w + x) + 1] + j13)) / d1;
	    	flowtmp[2 * (y * w + x) + 1] = (1.0 - omega) * flowtmp[2 * (y * w + x) + 1] 
		    + omega * (neighborsum[1] - factor * (j21 * flowtmp[2 * (y * w + x) + 0] + j23)) / d2;
	    }
	}
    }
    free(J12);
    free(J13);
    free(J23);
    free(divisor1);
    free(divisor2);

    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), w, h);
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    int flowi[2];
	    flowi[0] = cvl_iroundf(flowtmp[2 * (y * w + x) + 0]);
    	    flowi[1] = cvl_iroundf(flowtmp[2 * (y * w + x) + 1]);
	    cvl_field_set(flowfield, x, y, flowi);
	}
    }
    free(flowtmp);
    return flowfield;
}

/**
 * \param f1		The frame at t = 0.
 * \param f2		The frame at t = 1.
 * \param k		The size of a block (2k+1 x 2k+1).
 * \param maxdist	The maximum movement distance.
 * \param distweight	Weight of the distance when calculating block costs.
 * \param lumweight	Weight of the luminance when calculating pixel costs.
 * \return		The optical flow in a field of int[2] elements.
 *
 * Computes the optical flow between to equally big frames. This function
 * implements a naive block matching algorithm.
 * The cost of a block are \a distweight * distance + (1 - \a distweight) *
 * average pixel difference. The maximum norm distance is used here.
 * Each pixel difference is calculated with \a lumweight * difference in
 * luminance + \a (1 - \a lumweight) * difference in chrominance. 
 * The block with the lowest cost is the match; it determines the optical flow.
 */
cvl_field_t *cvl_opticalflow_bm_sad(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	int k, int maxdist, double distweight, double lumweight)
{
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(k > 0 && maxdist > 0);

    int width = cvl_frame_width(f1);
    int height = cvl_frame_height(f1);
    int testflow[(2 * maxdist + 1) * (2 * maxdist + 1)][2];
    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), width, height);
    cvl_frame_t *f1yuv = cvl_frame_clone(f1);
    cvl_frame_t *f2yuv = cvl_frame_clone(f2);
    
    cvl_frame_to_yuv(f1yuv);
    cvl_frame_to_yuv(f2yuv);
    
    // Test flow vectors in a good order: nearest first, according to city-block
    // distance. This way, we don't have to assign cost to distance, because
    // later test vectors must be *better* than earlier test vectors.
    cvl_msg_dbg("initializing (2m+1)x(2m+1) test vectors for m = %d", maxdist);
    testflow[0][0] = 0;
    testflow[0][1] = 0;
    int i = 1;
    for (int cbdist = 1; cbdist <= 2 * maxdist; cbdist++)
    {
	for (int mdist = 1; mdist <= cvl_mini(maxdist, cbdist); mdist++)
	{
	    for (int absr = 0; absr <= mdist; absr++)
	    {
		for (int absc = 0; absc <= mdist; absc++)
		{
		    if (cvl_maxi(absr, absc) != mdist || absr + absc != cbdist)
		    {
			continue;
		    }
		    for (int sgnr = -1; sgnr <= +1; sgnr += 2)
		    {
			if (sgnr == -1 && absr == 0)
			{
			    continue;
			}
			for (int sgnc = -1; sgnc <= +1; sgnc += 2)
			{
			    if (sgnc == -1 && absc == 0)
			    {
				continue;
			    }
			    testflow[i][0] = sgnc * absc;
			    testflow[i][1] = sgnr * absr;
			    i++;
			}
		    }
		}
	    }
	}
    }
    
    for (int y = 0; y < height; y++)
    {
	cvl_msg_dbg("block matching in line %d of %d...", y + 1, height);
	for (int x = 0; x < width; x++)
	{
	    float mincost = FLT_MAX;
	    int minflow[2] = { 0, 0 };
	    for (int i = 0; i < (2 * maxdist + 1) * (2 * maxdist + 1); i++)
	    {
		float cost = distweight * ((float)cvl_maxi(abs(testflow[i][0]), 
			    abs(testflow[i][1])) / (float)maxdist);
		// these loops can stop when cost >= mincost
		for (int r = -k; r <= k && cost < mincost; r++)
		{
		    for (int c = -k; c <= k && cost < mincost; c++)
		    {
			float pcost = 0.0f;
			cvl_pixel_t p1 = cvl_frame_get_r(f1yuv, 
				x + c, y + r);
			cvl_pixel_t p2 = cvl_frame_get_r(f2yuv, 
				x + testflow[i][0] + c, y + testflow[i][1] + r);
			// 1. Luminance difference
			pcost += lumweight * ((float)abs((int)cvl_pixel_yuv_to_y(p1)
				    - (int)cvl_pixel_yuv_to_y(p2)) / 219.0);
			// 2. Chrominance difference
			pcost += (1.0 - lumweight) * (((float)abs((int)cvl_pixel_yuv_to_u(p1)
					- (int)cvl_pixel_yuv_to_u(p2)) / 448.0
				    + (float)abs((int)cvl_pixel_yuv_to_v(p1)
					- (int)cvl_pixel_yuv_to_v(p2)) / 448.0));
			cost += (1.0 - distweight) * (pcost / (float)((2 * k + 1) * (2 * k + 1)));
		    }
		}
		// Is this cheap?
		if (cost < mincost)
		{
		    mincost = cost;
		    minflow[0] = testflow[i][0];
		    minflow[1] = testflow[i][1];
		}
	    }
	    cvl_field_set(flowfield, x, y, minflow);
	}
    }

    cvl_frame_free(f1yuv);
    cvl_frame_free(f2yuv);
    return flowfield;
}

/**
 * \param f1		The frame at t = 0. Must be graylevel.
 * \param f2		The frame at t = 1. Must be graylevel.
 * \param k		The size of a block (2k+1 x 2k+1).
 * \param maxdist	The maximum movement distance.
 * \param gamma_c	Parameter for weight computation. Try 7.
 * \param gamma_p	Parameter for weight computation. Try 38.
 * \return		The optical flow in a field of int[2] elements.
 *
 * Computes the optical flow between to equally big frames. This function
 * implements a block matching algorithm with cost computation as described in
 * "Locally Adaptive Support-Weight Approach for Visual Correspondence Search"
 * by Kuk-Jin Yoon and In-So Kweon, in Proceeding of the IEEE Conference on
 * Computer Vision and Pattern Recognition (CVPR), vol. 2, pp. 924-931, 2005.
 */
cvl_field_t *cvl_opticalflow_bm_asw(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	int k, int maxdist, double gamma_c, double gamma_p)
{
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(k > 0 && maxdist > 0);

    int width = cvl_frame_width(f1);
    int height = cvl_frame_height(f1);
    cvl_field_t *f1_cielab = cvl_field_new(3 * sizeof(float), width, height);
    cvl_field_t *f2_cielab = cvl_field_new(3 * sizeof(float), width, height);
    cvl_frame_t *f1_yuv = cvl_frame_clone(f1);
    cvl_frame_t *f2_yuv = cvl_frame_clone(f2);
    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), width, height);
    int pyramid_steps;

    cvl_msg_dbg("computing RGB and CIE Lab representations...");
    cvl_frame_to_yuv(f1_yuv);
    cvl_frame_to_yuv(f2_yuv);
    for (int i = 0; i < width * height; i++)
    {
	float Lab[3];
	cvl_pixel_t p;
	
	p = cvl_frame_get_i(f1, i);
	if (cvl_frame_pixel_type(f1) == CVL_PIXEL_GRAY)
	{
	    p = cvl_pixel_gray_to_rgb(p);
	}
	else if (cvl_frame_pixel_type(f1) == CVL_PIXEL_YUV)
	{
	    p = cvl_pixel_yuv_to_rgb(p);
	}
	cvl_srgb_to_cielab(p, &(Lab[0]), &(Lab[1]), &(Lab[2]));
	cvl_field_set_i(f1_cielab, i, Lab);
	
	p = cvl_frame_get_i(f2, i);
	if (cvl_frame_pixel_type(f2) == CVL_PIXEL_GRAY)
	{
	    p = cvl_pixel_gray_to_rgb(p);
	}
	else if (cvl_frame_pixel_type(f2) == CVL_PIXEL_YUV)
	{
	    p = cvl_pixel_yuv_to_rgb(p);
	}	
	cvl_srgb_to_cielab(p, &(Lab[0]), &(Lab[1]), &(Lab[2]));
	cvl_field_set_i(f2_cielab, i, Lab);
    }

    cvl_msg_dbg("determining number of pyramid steps...");
    pyramid_steps = 1;
    while (width % (1 << pyramid_steps) == 0 
	    && height % (1 << pyramid_steps) == 0
	    && width / (1 << pyramid_steps) >= 2 * k + 1
	    && height / (1 << pyramid_steps) >= 2 * k + 1)
    {
	pyramid_steps++;
    }

    for (int i = pyramid_steps - 1; i >= 0; i--)
    {
	int step_maxdist = (i == pyramid_steps - 1) ? (maxdist / (1 << i) + 1) : 1;
	int testflow[(2 * step_maxdist + 1) * (2 * step_maxdist + 1)][2];
	// Test flow vectors in a good order: nearest first, according to city-block
	// distance. This way, we don't have to assign cost to distance, because
	// later test vectprs must be *better* than earlier test vectors.
	cvl_msg_dbg("initializing (2m+1)x(2m+1) test vectors for m = %d", step_maxdist);
	testflow[0][0] = 0;
	testflow[0][1] = 0;
	int t = 1;
	for (int cbdist = 1; cbdist <= 2 * step_maxdist; cbdist++)
	{
	    for (int mdist = 1; mdist <= cvl_mini(step_maxdist, cbdist); mdist++)
	    {
		for (int absr = 0; absr <= mdist; absr++)
		{
		    for (int absc = 0; absc <= mdist; absc++)
		    {
			if (cvl_maxi(absr, absc) != mdist || absr + absc != cbdist)
			{
			    continue;
			}
			for (int sgnr = -1; sgnr <= +1; sgnr += 2)
			{
			    if (sgnr == -1 && absr == 0)
			    {
				continue;
			    }
			    for (int sgnc = -1; sgnc <= +1; sgnc += 2)
			    {
				if (sgnc == -1 && absc == 0)
				{
				    continue;
				}
				testflow[t][0] = sgnc * absc;
				testflow[t][1] = sgnr * absr;
				t++;
			    }
			}
		    }
		}
	    }
	}
       	
	// Block matching
	cvl_msg_dbg("block matching for pyramid %d: width = %d, height = %d", 
		i, width / (1 << i), height / (1 << i));
	for (int y = 0; y < height / (1 << i); y++)
	{
	    for (int x = 0; x < width / (1 << i); x++)
	    {
		const float *p_lab = cvl_field_get(f1_cielab, x * (1 << i), y * (1 << i));
		float mincost = FLT_MAX;
		int minflow[2] = { 0, 0 };
		int baseflow[2];
		if (i == pyramid_steps - 1)
		{
		    baseflow[0] = 0;
		    baseflow[1] = 0;
		}
		else
		{
		    const int *prev_flow = cvl_field_get(flowfield, x * (1 << i), y * (1 << i));
		    baseflow[0] = 2 * prev_flow[0];
		    baseflow[1] = 2 * prev_flow[1];
		}
		for (int t = 0; t < (2 * step_maxdist + 1) * (2 * step_maxdist + 1); t++)
		{
		    float cost = 0.0;
		    const float *ps_lab = cvl_field_get_r(f2_cielab, 
			(x + baseflow[0] + testflow[t][0]) * (1 << i), 
			(y + baseflow[1] + testflow[t][1]) * (1 << i));
		    float wsum = 0.0;
		    for (int r = -k; r <= k; r++)
		    {
			for (int c = -k; c <= k; c++)
			{
			    float eucdist = sqrtf((float)(r * r + c * c));
			    cvl_pixel_t q_yuv = cvl_frame_get_r(f1_yuv, 
				    (x + c) * (1 << i), 
				    (y + r) * (1 << i));
			    cvl_pixel_t qs_yuv = cvl_frame_get_r(f2_yuv, 
				    (x + baseflow[0] + testflow[t][0] + c) * (1 << i), 
				    (y + baseflow[1] + testflow[t][1] + r) * (1 << i));
			    /*
			    float rawcost = 
				(float)(2 * abs((int)cvl_pixel_yuv_to_y(q_yuv) - (int)cvl_pixel_yuv_to_y(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_u(q_yuv) - (int)cvl_pixel_yuv_to_u(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_v(q_yuv) - (int)cvl_pixel_yuv_to_v(qs_yuv)))
				/ 886.0;
			    */
			    float rawcost = 
				(float)(2 * abs((int)cvl_pixel_yuv_to_y(q_yuv) - (int)cvl_pixel_yuv_to_y(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_u(q_yuv) - (int)cvl_pixel_yuv_to_u(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_v(q_yuv) - (int)cvl_pixel_yuv_to_v(qs_yuv)));
			    const float *q_lab = cvl_field_get_r(f1_cielab, 
				    (x + c) * (1 << i), 
				    (y + r) * (1 << i));
			    const float *qs_lab = cvl_field_get_r(f2_cielab, 
				    (x + baseflow[0] + testflow[t][0] + c) * (1 << i), 
				    (y + baseflow[1] + testflow[t][1] + r) * (1 << i));
			    float wpq = exp(- (cvl_vector3_dist_euc(p_lab, q_lab) / gamma_c 
					+ eucdist / gamma_p));
			    float wpsqs = exp(- (cvl_vector3_dist_euc(ps_lab, qs_lab) / gamma_c 
					+ eucdist / gamma_p));
			    wsum += wpq * wpsqs;
			    cost += wpq * wpsqs * rawcost;
			}
		    }
		    cost /= wsum;
		    //cost /= (float)((2 * k + 1) * (2 * k + 1));
		    //cost += 0.05 * ((float)(cvl_maxi(abs(testflow[t][0]), abs(testflow[t][1]))) / (float)maxdist);		    
		    /* Is this cheap? */
		    if (cost < mincost)
		    {
			mincost = cost;
			minflow[0] = testflow[t][0];
			minflow[1] = testflow[t][1];
		    }
		}
		cvl_vector2i_add(minflow, baseflow, minflow);
		cvl_field_fill_rect(flowfield, x * (1 << i), y * (1 << i), (1 << i), (1 << i), minflow);
	    }
	}
    }
    
    cvl_frame_free(f1_yuv);
    cvl_frame_free(f2_yuv);
    cvl_field_free(f1_cielab);
    cvl_field_free(f2_cielab);
    return flowfield;
}


/* Helper for cvl_opticalflow_cc() */
static int cvl_opticalflow_cc_cmp(const int *x, const int *y)
{
    return (y[2] < x[2] ? -1 : (y[2] > x[2] ? +1 : 0));
}

/**
 * \param fw		Forward flow between two frames.
 * \param bw		Backward flow between two frames.
 * \param threshold	Allowed difference (cityblock distance).
 * \param fraction	Pointer to a double to store the fraction of unreliable vectors in, or NULL.
 * \return		The revised forward flow field.
 *
 * Performs a consistency check for flow fields to improves their reliability. 
 * The flow fields \a fw and \a bw must be flow fields between two frames f1 
 * and f2; with \a fw being the optical flow from f1 to f2, and \a bw being 
 * the optical flow between f2 and f1. The \a threshold parameter determines
 * the maximum allowed difference between the two. If the differnce is higher
 * in one point, the corresponding flow vector of \a fw is thrown away and 
 * later replaced by interpolating neighbor flow vectors. The fraction of
 * unreliable vectors will be stored in \a fraction if that is not NULL.
 */
cvl_field_t *cvl_opticalflow_cc(const cvl_field_t *fw, const cvl_field_t *bw, 
	int threshold, double *fraction)
{
    cvl_assert(cvl_field_width(fw) == cvl_field_width(bw) 
	    && cvl_field_height(fw) == cvl_field_height(bw));

    int w = cvl_field_width(fw);
    int h = cvl_field_height(fw);
    bool *good = xmalloc(w * h * sizeof(bool));
    int holecounter, holeindex;
    int *holelist;
    cvl_field_t *newflow = cvl_field_clone(fw);
    
    // Find holes
    holecounter = 0;
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    const int *f = cvl_field_get(fw, x, y);
	    int xf = x + f[0];
	    int yf = y + f[1];
	    if (xf >= 0 && xf < w && yf >= 0 && yf < h)
	    {
		int d[2];
		const int *b = cvl_field_get(bw, xf, yf);
		cvl_vector2i_add(d, f, b);
		if (abs(d[0]) + abs(d[1]) <= threshold)
		{
		    good[y * w + x] = true;
		}
		else
		{
		    good[y * w + x] = false;
		    holecounter++;
		}
	    }
	    else
	    {
		good[y * w + x] = true;
	    }
	}
    }
    cvl_msg_dbg("%d vectors are not reliable", holecounter);
    if (fraction)
    {
	*fraction = (double)holecounter / (double)(w * h);
    }

    // Build the list of holes
    holelist = xmalloc(3 * holecounter * sizeof(int));
    holeindex = 0;
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    if (!good[y * w + x])
	    {
		int neighborcounter = 0;		
		for (int r = -1; r <= 1; r++)
		{
		    for (int c = -1; c <= 1; c++)
		    {
			if (r == 0 && c == 0)
			{
			    continue;
			}
			if (good[cvl_reflect(y + r, h) * w + cvl_reflect(x + c, w)])
			{
			    neighborcounter++;
			}
		    }
		}
		holelist[3 * holeindex + 0] = x;
		holelist[3 * holeindex + 1] = y;
		holelist[3 * holeindex + 2] = neighborcounter;
		holeindex++;
	    }
	}
    }

    // Fill the holes
    while (holecounter > 0)
    {
	qsort(holelist, holecounter, 3 * sizeof(int), 
	     	(int (*)(const void *, const void *))cvl_opticalflow_cc_cmp);
	int curmax = holelist[3 * 0 + 2];
	int curcnt = 1;
	
	while (curcnt < holecounter && holelist[3 * curcnt + 2] == curmax)
	{
	    curcnt++;
	}
	
	cvl_msg_dbg("interpolating %d vectors, each from %d neighbors", curcnt, curmax);
	
	for (int i = 0; i < curcnt; i++)
	{
	    int x = holelist[3 * i + 0];
	    int y = holelist[3 * i + 1];
	    if (curmax == 0)
	    {
		// This can only happen if there is not a single reliable vector
		// in the source flow field.
		const int flow[2] = { 0, 0 };
		cvl_field_set(newflow, x, y, flow);
	    }
	    else
	    {
		int x_sum = 0;
		int y_sum = 0;
		for (int r = -1; r <= 1; r++)
		{
		    for (int c = -1; c <= 1; c++)
		    {
			if (r == 0 && c == 0)
			{
			    continue;
			}
			int xx = cvl_reflect(x + c, w);
			int yy = cvl_reflect(y + r, h);
			if (good[yy * w + xx])
			{
			    const int *f = cvl_field_get(newflow, xx, yy);
			    x_sum += f[0];
			    y_sum += f[1];
			}
		    }
		}
		int f[2];
		f[0] = cvl_iroundf((float)x_sum / (float)curmax);
		f[1] = cvl_iroundf((float)y_sum / (float)curmax);
		cvl_field_set(newflow, x, y, f);
	    }
	    // Update
	    good[y * w + x] = true;
	    if (curmax < 8)
	    {
		// This hole has neighbors that are also holes. For each of
		// these neighbors, increase the number of known neighbors.
		for (int j = curcnt; j < holecounter; j++)
		{
		    if (abs(holelist[3 * j + 0] - x) <= 1 && abs(holelist[3 * j + 1] - y) <= 1)
		    {
			holelist[3 * j + 2]++;
		    }
		}
	    }
	}
	holecounter -= curcnt;
	memmove(holelist, &(holelist[3 * curcnt]), holecounter * 3 * sizeof(int));
    }
    
    free(good);
    free(holelist);
    return newflow;
}
