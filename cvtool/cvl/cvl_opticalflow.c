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

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_field.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_draw.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_opticalflow.h"


/*
 * This function calculates the approximations for the derivatives Ex, Ey, Et of
 * the frames frame0 and frame1.
 */
static inline void cvl_opticalflow_differentiate(
	const cvl_frame_t *frame0, const cvl_frame_t *frame1, 
	int x, int y, double *Ex, double *Ey, double *Et)
{
    *Ex = (double)
	(+ 1 * (int)cvl_frame_get_r(frame0, x    , y + 1)
     	 - 1 * (int)cvl_frame_get_r(frame0, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame0, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x + 1, y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x    , y + 1)
	 - 1 * (int)cvl_frame_get_r(frame1, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame1, x + 1, y    )) / 4.0;
    *Ey = (double)
	(+ 1 * (int)cvl_frame_get_r(frame0, x + 1, y    )
     	 - 1 * (int)cvl_frame_get_r(frame0, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame0, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x    , y + 1)
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y    )
	 - 1 * (int)cvl_frame_get_r(frame1, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame1, x    , y + 1)) / 4.0;
    *Et = (double)
	(+ 1 * (int)cvl_frame_get_r(frame1, x    , y    )
     	 - 1 * (int)cvl_frame_get_r(frame0, x    , y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y    )
	 - 1 * (int)cvl_frame_get_r(frame0, x + 1, y    )
	 + 1 * (int)cvl_frame_get_r(frame1, x    , y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x    , y + 1)
	 + 1 * (int)cvl_frame_get_r(frame1, x + 1, y + 1)
	 - 1 * (int)cvl_frame_get_r(frame0, x + 1, y + 1)) / 4.0;
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
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));

    int w = cvl_frame_width(f1);
    int h = cvl_frame_height(f1);
    cvl_field_t *Ex = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *Ey = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *Et = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *flowtmp = cvl_field_new(2 * sizeof(double), w, h);
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
		    cvl_field_ref(Ex, x, y),
		    cvl_field_ref(Ey, x, y),
		    cvl_field_ref(Et, x, y));
	    double *flow = cvl_field_ref(flowtmp, x, y);
	    flow[0] = 0.0;
	    flow[1] = 0.0;
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
		double *flow = cvl_field_ref(flowtmp, x, y);
		double ex = *(const double *)cvl_field_get(Ex, x, y);
		double ey = *(const double *)cvl_field_get(Ey, x, y);
		double et = *(const double *)cvl_field_get(Et, x, y);
		double frac = (ex * flow[0] + ey * flow[1] + et) 
		    / (1.0 + lambda * (ex * ex + ey * ey));
		flow[0] -= ex * frac;
		flow[1] -= ey * frac;
	    }
	}
    }
    cvl_field_free(Ex);
    cvl_field_free(Ey);
    cvl_field_free(Et);

    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), w, h);
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    const double *flow = cvl_field_get(flowtmp, x, y);
	    int *flowi = cvl_field_ref(flowfield, x, y);
	    flowi[0] = cvl_iround(flow[0]);
    	    flowi[1] = cvl_iround(flow[1]);
	}
    }
    cvl_field_free(flowtmp);
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
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(k > 0);

    int w = cvl_frame_width(f1);
    int h = cvl_frame_height(f1);
    cvl_field_t *Ex = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *Ey = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *Et = cvl_field_new(sizeof(double), w, h);
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
		    cvl_field_ref(Ex, x, y),
		    cvl_field_ref(Ey, x, y),
		    cvl_field_ref(Et, x, y));
	}
    }
    cvl_frame_free(f1_gray);
    cvl_frame_free(f2_gray);
    
    /* Compute the weight matrix. It has the size 2k+1 x 2k+1 and is the same
     * matrix that would be used for Gauss filtering. */
    double weight_sigma = cvl_smooth_gauss_k_to_sigma(k);
    double weight_gauss_vals1[k + 1];
    for (int i = 0; i <= k; i++)
    {
	weight_gauss_vals1[i] = gsl_ran_gaussian_pdf((double)i, weight_sigma);
    }
    int weight_gauss_vals2[2 * k + 1];
    weight_gauss_vals2[0] = 1;
    weight_gauss_vals2[2 * k] = 1;
    for (int i = 1; i <= k; i++)
    {
	weight_gauss_vals2[i] = cvl_iround(weight_gauss_vals1[k - i] / weight_gauss_vals1[k]);
    }
    for (int i = k + 1; i < 2 * k; i++)
    {
	weight_gauss_vals2[i] = cvl_iround(weight_gauss_vals1[i - k] / weight_gauss_vals1[k]);
    }
    int weight[2 * k + 1][2 * k + 1];
    int weight_sum = 0;
    for (int r = 0; r < 2 * k + 1; r++)
    {
	for (int c = 0; c < 2 * k + 1; c++)
	{
	    weight[r][c] = sqrt(weight_gauss_vals2[r] * weight_gauss_vals2[c]);
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
	    double sum_wfxfx = 0.0;
	    double sum_wfxfy = 0.0;
	    double sum_wfxft = 0.0;
	    double sum_wfyfy = 0.0;
	    double sum_wfyft = 0.0;
	    for (int r = -k; r <= k; r++)
	    {
		for (int c = -k; c <= k; c++)
		{
		    int yy = cvl_reflect(y + r, h);
		    int xx = cvl_reflect(x + c, w);
		    double ww = (double)weight[r + k][c + k] / (double)weight_sum;
		    double fx = *(const double *)cvl_field_get(Ex, xx, yy);
		    double fy = *(const double *)cvl_field_get(Ey, xx, yy);
		    double ft = *(const double *)cvl_field_get(Et, xx, yy);
		    sum_wfxfx += ww * fx * fx;
		    sum_wfxfy += ww * fx * fy;
		    sum_wfxft += ww * fx * ft;
		    sum_wfyfy += ww * fy * fy;
		    sum_wfyft += ww * fy * ft;
		}
	    }
	    double det = sum_wfxfx * sum_wfyfy - sum_wfxfy * sum_wfxfy;
	    int *flow = cvl_field_ref(flowfield, x, y);
	    if (fabs(det) > DBL_MIN)
	    {
		double invdet = 1.0 / det;
		flow[0] = cvl_iround((- sum_wfxft * sum_wfyfy + sum_wfyft * sum_wfxfy) * invdet);
		flow[1] = cvl_iround((+ sum_wfxft * sum_wfxfy - sum_wfyft * sum_wfxfx) * invdet);
	    }
	    else
	    {
		flow[0] = 0;
		flow[1] = 0;
	    }
	}
    }	
    cvl_field_free(Ex);
    cvl_field_free(Ey);
    cvl_field_free(Et);
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
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2) 
	    && cvl_frame_height(f1) == cvl_frame_height(f2));

    int w = cvl_frame_width(f1);
    int h = cvl_frame_height(f1);
    double factor;
    cvl_field_t *J12 = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *J13 = cvl_field_new(sizeof(double), w, h);
    //cvl_field_t *J21 = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *J21 = J12;
    cvl_field_t *J23 = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *divisor1 = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *divisor2 = cvl_field_new(sizeof(double), w, h);
    cvl_field_t *flowtmp = cvl_field_new(2 * sizeof(double), w, h);
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
	    double fx, fy, ft, J11, J22;
	    
	    cvl_opticalflow_differentiate(f1_gray, f2_gray, x, y, &fx, &fy, &ft);
	    *(double *)cvl_field_ref(J12, x, y) = fx * fy * ((double)weight[0][1] / (double)weight_sum);
	    *(double *)cvl_field_ref(J13, x, y) = fx * ft * ((double)weight[0][2] / (double)weight_sum);
	    //*(double *)cvl_field_ref(J21, x, y) = fy * fx * ((double)weight[1][0] / (double)weight_sum);
	    *(double *)cvl_field_ref(J23, x, y) = fy * ft * ((double)weight[1][2] / (double)weight_sum);
	    J11 = fx * fx * ((double)weight[0][0] / (double)weight_sum);
	    J22 = fy * fy * ((double)weight[1][1] / (double)weight_sum);
	    *(double *)cvl_field_ref(divisor1, x, y) = 4.0 + factor * J11;
	    *(double *)cvl_field_ref(divisor2, x, y) = 4.0 + factor * J22;

	    double *flow = cvl_field_ref(flowtmp, x, y);
	    flow[0] = 0.0;
	    flow[1] = 0.0;
	}
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
		double j12 = *(const double *)cvl_field_get(J12, x, y);
		double j13 = *(const double *)cvl_field_get(J13, x, y);
		double j21 = *(const double *)cvl_field_get(J21, x, y);
		double j23 = *(const double *)cvl_field_get(J23, x, y);
		double d1 = *(const double *)cvl_field_get(divisor1, x, y);
		double d2 = *(const double *)cvl_field_get(divisor2, x, y);
		const double *neighborflow;
		double neighborsum[2];

		cvl_vector2_zero(neighborsum);
		neighborflow = cvl_field_get_r(flowtmp, x, y - 1);
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		neighborflow = cvl_field_get_r(flowtmp, x, y + 1);
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		neighborflow = cvl_field_get_r(flowtmp, x - 1, y);
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		neighborflow = cvl_field_get_r(flowtmp, x + 1, y);
		cvl_vector2_add(neighborsum, neighborflow, neighborsum);
		
		int *flow = cvl_field_ref(flowtmp, x, y);
		flow[0] = (1.0 - omega) * flow[0] 
		    + omega * (neighborsum[0] - factor * (j12 * flow[1] + j13)) / d1;
	    	flow[1] = (1.0 - omega) * flow[1] 
		    + omega * (neighborsum[1] - factor * (j21 * flow[0] + j23)) / d2;
	    }
	}
    }
    cvl_field_free(J12);
    cvl_field_free(J13);
    //cvl_field_free(J21);
    cvl_field_free(J23);
    cvl_field_free(divisor1);
    cvl_field_free(divisor2);

    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), w, h);
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    const double *flow = cvl_field_get(flowtmp, x, y);
	    int *flowi = cvl_field_ref(flowfield, x, y);
	    flowi[0] = cvl_iround(flow[0]);
    	    flowi[1] = cvl_iround(flow[1]);
	}
    }
    cvl_field_free(flowtmp);
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
	    double mincost = DBL_MAX;
	    int minflow[2] = { 0, 0 };
	    for (int i = 0; i < (2 * maxdist + 1) * (2 * maxdist + 1); i++)
	    {
		double cost = distweight * ((double)cvl_maxi(abs(testflow[i][0]), 
			    abs(testflow[i][1])) / (double)maxdist);
		// these loops can stop when cost >= mincost
		for (int r = -k; r <= k && cost < mincost; r++)
		{
		    for (int c = -k; c <= k && cost < mincost; c++)
		    {
			double pcost = 0.0;
			cvl_pixel_t p1 = cvl_frame_get_r(f1yuv, 
				x + c, y + r);
			cvl_pixel_t p2 = cvl_frame_get_r(f2yuv, 
				x + testflow[i][0] + c, y + testflow[i][1] + r);
			// 1. Luminance difference
			pcost += lumweight * ((double)abs((int)cvl_pixel_yuv_to_y(p1)
				    - (int)cvl_pixel_yuv_to_y(p2)) / 219.0);
			// 2. Chrominance difference
			pcost += (1.0 - lumweight) * (((double)abs((int)cvl_pixel_yuv_to_u(p1)
					- (int)cvl_pixel_yuv_to_u(p2)) / 448.0
				    + (double)abs((int)cvl_pixel_yuv_to_v(p1)
					- (int)cvl_pixel_yuv_to_v(p2)) / 448.0));
			cost += (1.0 - distweight) * (pcost / (double)((2 * k + 1) * (2 * k + 1)));
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
    cvl_field_t *f1_cielab = cvl_field_new(3 * sizeof(double), width, height);
    cvl_field_t *f2_cielab = cvl_field_new(3 * sizeof(double), width, height);
    cvl_frame_t *f1_yuv = cvl_frame_clone(f1);
    cvl_frame_t *f2_yuv = cvl_frame_clone(f2);
    cvl_field_t *flowfield = cvl_field_new(2 * sizeof(int), width, height);
    int pyramid_steps;

    cvl_msg_dbg("computing RGB and CIE Lab representations...");
    cvl_frame_to_yuv(f1_yuv);
    cvl_frame_to_yuv(f2_yuv);
    for (int i = 0; i < width * height; i++)
    {
	double *Lab;
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
	Lab = cvl_field_ref_i(f1_cielab, i);
	cvl_srgb_to_cielab(p, &(Lab[0]), &(Lab[1]), &(Lab[2]));
	
	p = cvl_frame_get_i(f2, i);
	if (cvl_frame_pixel_type(f2) == CVL_PIXEL_GRAY)
	{
	    p = cvl_pixel_gray_to_rgb(p);
	}
	else if (cvl_frame_pixel_type(f2) == CVL_PIXEL_YUV)
	{
	    p = cvl_pixel_yuv_to_rgb(p);
	}	
	Lab = cvl_field_ref_i(f2_cielab, i);
	cvl_srgb_to_cielab(p, &(Lab[0]), &(Lab[1]), &(Lab[2]));
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
		const double *p_lab = cvl_field_get(f1_cielab, x * (1 << i), y * (1 << i));
		double mincost = DBL_MAX;
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
		    double cost = 0.0;
		    const double *ps_lab = cvl_field_get_r(f2_cielab, 
			(x + baseflow[0] + testflow[t][0]) * (1 << i), 
			(y + baseflow[1] + testflow[t][1]) * (1 << i));
		    double wsum = 0.0;
		    for (int r = -k; r <= k; r++)
		    {
			for (int c = -k; c <= k; c++)
			{
			    double eucdist = sqrt((double)(r * r + c * c));
			    cvl_pixel_t q_yuv = cvl_frame_get_r(f1_yuv, 
				    (x + c) * (1 << i), 
				    (y + r) * (1 << i));
			    cvl_pixel_t qs_yuv = cvl_frame_get_r(f2_yuv, 
				    (x + baseflow[0] + testflow[t][0] + c) * (1 << i), 
				    (y + baseflow[1] + testflow[t][1] + r) * (1 << i));
			    /*
			    double rawcost = 
				(double)(2 * abs((int)cvl_pixel_yuv_to_y(q_yuv) - (int)cvl_pixel_yuv_to_y(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_u(q_yuv) - (int)cvl_pixel_yuv_to_u(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_v(q_yuv) - (int)cvl_pixel_yuv_to_v(qs_yuv)))
				/ 886.0;
			    */
			    double rawcost = 
				(double)(2 * abs((int)cvl_pixel_yuv_to_y(q_yuv) - (int)cvl_pixel_yuv_to_y(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_u(q_yuv) - (int)cvl_pixel_yuv_to_u(qs_yuv))
					 + abs((int)cvl_pixel_yuv_to_v(q_yuv) - (int)cvl_pixel_yuv_to_v(qs_yuv)));
			    const double *q_lab = cvl_field_get_r(f1_cielab, 
				    (x + c) * (1 << i), 
				    (y + r) * (1 << i));
			    const double *qs_lab = cvl_field_get_r(f2_cielab, 
				    (x + baseflow[0] + testflow[t][0] + c) * (1 << i), 
				    (y + baseflow[1] + testflow[t][1] + r) * (1 << i));
			    double wpq = exp(- (cvl_vector3_dist_euc(p_lab, q_lab) / gamma_c 
					+ eucdist / gamma_p));
			    double wpsqs = exp(- (cvl_vector3_dist_euc(ps_lab, qs_lab) / gamma_c 
					+ eucdist / gamma_p));
			    wsum += wpq * wpsqs;
			    cost += wpq * wpsqs * rawcost;
			}
		    }
		    cost /= wsum;
		    //cost /= (double)((2 * k + 1) * (2 * k + 1));
		    //cost += 0.05 * ((double)(cvl_maxi(abs(testflow[t][0]), abs(testflow[t][1]))) / (double)maxdist);		    
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
		int *flow = cvl_field_ref(newflow, x, y);
		flow[0] = 0;
		flow[1] = 0;
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
		int *f = cvl_field_ref(newflow, x, y);
		f[0] = cvl_iround((double)x_sum / (double)curmax);
		f[1] = cvl_iround((double)y_sum / (double)curmax);
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
