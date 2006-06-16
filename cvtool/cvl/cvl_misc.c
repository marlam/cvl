/*
 * cvl_misc.c
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
 * \file cvl_misc.h
 * \brief Miscellaneous.
 *
 * Miscellaneous operations on frames.
 */

#include "config.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>

#include "strverscmp.h"
#include "xalloc.h"

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_misc.h"


/**
 * \param req_version		The required version.
 *
 * Check that the version of the library is at minimum the requested one
 * and return the version string; return NULL if the condition is not
 * satisfied. If a NULL is passed to this function, no check is done,
 * but the version string is simply returned.
 */
const char *cvl_check_version(const char *req_version)
{
    if (!req_version || strverscmp(req_version, PACKAGE_VERSION) <= 0)
    {
	return PACKAGE_VERSION;
    }
    return NULL;	 
}


/* Helper for cvl_diffstat(): Compare two uint8_ts. */
int cvl_uint8_cmp(uint8_t *x1, uint8_t *x2)
{
    return (*x1 < *x2 ? -1 : (*x1 > *x2 ? +1 : 0));
}

/**
 * \param f1		The first frame.
 * \param f2		The second frame.
 * \param diff		The frame to store the differences in, or NULL.
 * \param min		Pointer to 1 (gray, yuv) or 3 (rgb) integers.
 * \param max		Pointer to 1 (gray, yuv) or 3 (rgb) integers.
 * \param med		Pointer to 1 (gray, yuv) or 3 (rgb) integers.
 * \param avg		Pointer to 1 (gray, yuv) or 3 (rgb) doubles.
 * \param dev		Pointer to 1 (gray, yuv) or 3 (rgb) doubles.
 *
 * Visualizes the differences between two frames, and computes some simple
 * statistics.\n
 * \a f1 and \a f2 must have the same dimensions and the same pixel type.
 * The resulting frame \a diff will inherit these properties. The pixel values in 
 * it will be the absolute of the difference of the 
 * corresponding pixels in the two sources. For RGB frames, the difference
 * is computed separated by color channels: p1 = (r11, g12, b13), 
 * p2 = (r3, g11, b13) => (r8, g1, b0). For other pixel types, the 
 * luminosity difference is measured and represented by a gray value.\n
 * The minimum, maximum, median, and average error are stored in \a min, \a max,
 * \a med, \a avg respectively. The standard deviation is stored in \a dev.\n
 * If \a f1 and \a f2 are of type GRAY or YUV, then \a min, \a max, \a med, \a
 * avg, and \a dev need only point to one integer or double. For RGB, they must
 * point to three integers or doubles, to store the values for each channel
 * separately.\n
 * If any of \a diff, \a min, \a max, \a med, \a avg, \a dev is NULL, then the
 * corresponding computation will not be done.
 */
void cvl_diffstat(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	cvl_frame_t **diff, uint8_t *min, uint8_t *max, uint8_t *med, double *avg, double *dev)
{
    cvl_assert(f1 != NULL);
    cvl_assert(f2 != NULL);
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2));
    cvl_assert(cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(cvl_frame_pixel_type(f1) == cvl_frame_pixel_type(f2));

    if ((unsigned long long)INT_MAX >= 
	    ULLONG_MAX / ((unsigned long long)UINT8_MAX * (unsigned long long)UINT8_MAX))
    {
	/* This happens only if (sizeof(long long) < 2*sizeof(int)), which
	 * should never be the case. */
	cvl_msg_wrn("%s: arithmetic overflow might occur", __func__);
    }
    
    int size = cvl_frame_size(f1);
    bool rgb = (cvl_frame_pixel_type(f1) == CVL_PIXEL_RGB);
    unsigned long long errsum[3];
    unsigned long long errsumsq[3];
    uint8_t *errvals[3];
    
    if (diff)
    {
	*diff = cvl_frame_new(cvl_frame_pixel_type(f1), cvl_frame_width(f1), cvl_frame_height(f1));
    }
    for (int j = 0; j < (rgb ? 3 : 1); j++)
    {
	if (min)
	{
	    min[j] = 0xff;
	}
	if (max)
	{
	    max[j] = 0;
	}
	if (med)
	{
	    errvals[j] = xmalloc(size * sizeof(uint8_t));
	}
	if (avg || dev)
	{
	    errsum[j] = 0;
	}
	if (dev)
	{
	    errsumsq[j] = 0;
	}
    }
    
    for (int i = 0; i < cvl_frame_size(f1); i++)
    {
	cvl_pixel_t p1 = cvl_frame_get_i(f1, i);
	cvl_pixel_t p2 = cvl_frame_get_i(f2, i);
	uint8_t pd[3];

	if (cvl_frame_pixel_type(f1) == CVL_PIXEL_GRAY)
	{
	    pd[0] = abs((int)p1 - (int)p2);
	    if (diff)
	    {
		cvl_frame_set_i(*diff, i, pd[0]);
	    }
	}
	else if (cvl_frame_pixel_type(f1) == CVL_PIXEL_RGB)
	{
	    pd[0] = abs((int)cvl_pixel_rgb_to_r(p1) - (int)cvl_pixel_rgb_to_r(p2));
	    pd[1] = abs((int)cvl_pixel_rgb_to_g(p1) - (int)cvl_pixel_rgb_to_g(p2));
	    pd[2] = abs((int)cvl_pixel_rgb_to_b(p1) - (int)cvl_pixel_rgb_to_b(p2));
	    if (diff)
	    {
		cvl_frame_set_i(*diff, i, cvl_pixel_rgb(pd[0], pd[1], pd[2]));
	    }
	}
	else
	{
	    pd[0] = abs((int)cvl_pixel_yuv_to_y(p1) - (int)cvl_pixel_yuv_to_y(p2));
	    if (diff)
	    {
		cvl_frame_set_i(*diff, i, cvl_pixel_yuv(pd[0] + 16, 128, 128));
	    }
	}
	for (int j = 0; j < (rgb ? 3 : 1); j++)
	{
	    if (min && pd[j] < min[j])
	    {
		min[j] = pd[j];
	    }
	    if (max && pd[j] > max[j])
	    {
		max[j] = pd[j];
	    }
	    if (med)
	    {
		errvals[j][i] = pd[j];
	    }
	    if (avg || dev)
	    {
		errsum[j] += pd[j];
	    }
	    if (dev)
	    {
		errsumsq[j] += (int)pd[j] * (int)pd[j];
	    }
	}
    }
    if (med)
    {
	for (int j = 0; j < (rgb ? 3 : 1); j++)
	{
	    qsort(errvals[j], size, sizeof(uint8_t), (int (*)(const void *, const void *))cvl_uint8_cmp);
	    med[j] = errvals[j][size / 2];
	    free(errvals[j]);
	}
    }
    if (avg)
    {
	for (int j = 0; j < (rgb ? 3 : 1); j++)
	{
	    avg[j] = (double)errsum[j] / (double)size;
	}
    }
    if (dev)
    {
	for (int j = 0; j < (rgb ? 3 : 1); j++)
	{
	    dev[j] = sqrt(((double)size * (double)errsumsq[j] 
			- ((double)errsum[j] * (double)errsum[j]))
		    / ((double)size * (double)(size - 1)));
	}
    }
}


/**
 * \param f1		The first frame.
 * \param f2		The second frame.
 * \return		The differences between the frames.
 *
 * Visualizes the differences between two frames.
 * \a f1 and \a f2 must have the same dimensions and the same pixel type.
 * The resulting frame will inherit these properties. The pixel values in 
 * the resulting frame will be the absolute of the difference of the 
 * corresponding pixels in the two sources. For RGB frames, the difference
 * is computed separated by color channels: p1 = (r11, g12, b13), 
 * p2 = (r3, g11, b13) => (r8, g1, b0). For other pixel types, the 
 * luminosity difference is measured and represented by a gray value.
 */
cvl_frame_t *cvl_diff(const cvl_frame_t *f1, const cvl_frame_t *f2)
{
    cvl_assert(f1 != NULL);
    cvl_assert(f2 != NULL);
    cvl_assert(cvl_frame_width(f1) == cvl_frame_width(f2));
    cvl_assert(cvl_frame_height(f1) == cvl_frame_height(f2));
    cvl_assert(cvl_frame_pixel_type(f1) == cvl_frame_pixel_type(f2));

    cvl_frame_t *diff;
    cvl_diffstat(f1, f2, &diff, NULL, NULL, NULL, NULL, NULL);
    return diff;
}


/*
 * Squared Euclidean Distance Transform.
 *
 * See
 * C.R. Maurer, R. Qi, and V. Raghavan, A Linear Time Algorithm for Computing
 * Exact Euclidean Distance Transforms of Binary Images in Arbitrary Dimensions,
 * IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 25(2),
 * February 2003.
 */

static inline bool cvl_sedt_remove(int d2u, int d2v, int d2w, int ud, int vd, int wd)
{
    const int a = vd - ud;
    const int b = wd - vd;
    const int c = a + b;
    return (c * d2v - b * d2u - a * d2w > a * b * c);
}

static void cvl_sedt_voronoi(cvl_field_t **dt, 
	int *g,		// temporary storage space
	int *h,		// temporary storage space
	int d,		// number of dimension: 0 = x, 1 = y, 2 = z
	int nd,		// size in that dimension (width/height/depth)
	int i0, int i1, int i2)
{
    int i[3] = { i0, i1, i2 };
    int l = 0;
        
    for (i[d] = 0; i[d] < nd; i[d]++)
    {
	int fi = *(const int *)cvl_field_get(dt[i[2]], i[0], i[1]);
	if (fi != INT_MAX)
	{
	    if (l < 2)
	    {
		l++;
	      	g[l] = fi;
    		h[l] = i[d];
	    }
	    else
	    {
		while (l >= 2 
			&& cvl_sedt_remove(g[l - 1], g[l], fi, 
			    h[l - 1], h[l], i[d]))
		{
		    l--;
		}
		l++;
		cvl_assert(l < nd + 1);		// TODO: Remove this line
		g[l] = fi;
		h[l] = i[d];
	    }
	}
    }
    int ns = l;
    if (ns != 0)
    {
	l = 1;
	for (i[d] = 0; i[d] < nd; i[d]++)
	{
	    while (l < ns 
		    && (g[l] + (h[l] - i[d]) * (h[l] - i[d]) 
			> g[l + 1] + (h[l + 1] - i[d]) * (h[l + 1] - i[d])))
	    {
		l++;
		cvl_assert(l < nd + 1);		// TODO: Remove this line
	    }
	    *(int *)cvl_field_ref(dt[i[2]], i[0], i[1]) 
		=  g[l] + (h[l] - i[d]) * (h[l] - i[d]);
	}	    
    }
}

/**
 * \param frame		The frame. Must be graylevel.
 * \return		The distance transform.
 *
 * Computes the Squared Euclidean Distance Transform (SEDT) of the given frame. The
 * result will be stored in an integer field that has the same dimensions as the
 * frame. If the pixel at position (x,y) in the frame is a background pixel
 * (its value is zero), then its entry in the distance map will be zero.
 * If the pixel is an object pixel (its value is not zero), then its entry in
 * the distance map will be its squared euclidean distance to the next
 * background pixel.\n
 * The frame must be graylevel. The result can only be guaranteed to be correct
 * if the width and height of the frame are smaller than 2*sqrt(INT_MAX/2)
 * pixels, or if it is known that the maximum distance between object pixel and
 * background pixels in the frame is less than sqrt(INT_MAX/2).
 * 
 * See also 
 * C.R. Maurer, R. Qi, and V. Raghavan. A Linear Time Algorithm for Computing
 * Exact Euclidean Distance Transforms of Binary Images in Arbitrary Dimensions.
 * IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 25(2),
 * February 2003.
 */
cvl_field_t *cvl_sedt(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    
    cvl_field_t *dt;
    int *g, *h;
    int ghsize;
    
    ghsize = cvl_maxi(cvl_frame_width(frame), cvl_frame_height(frame));
    g = xnmalloc((size_t)ghsize + (size_t)1, sizeof(int));
    h = xnmalloc((size_t)ghsize + (size_t)1, sizeof(int));
    dt = cvl_field_new(sizeof(int), cvl_frame_width(frame), cvl_frame_height(frame));
    
    // ComputeEDT for d == 1
    for (int y = 0; y < cvl_frame_height(frame); y++)
    {
	// ComputeEDT for d == 0
	for (int x = 0; x < cvl_frame_width(frame); x++)
	{
	    *(int *)cvl_field_ref(dt, x, y) = 
		(cvl_frame_get(frame, x, y) != 0x00 ? 0x00 : INT_MAX);
	}
    	cvl_sedt_voronoi(&dt, g, h, 0, cvl_frame_width(frame), 0, y, 0);
    }
    for (int x = 0; x < cvl_frame_width(frame); x++)
    {
	cvl_sedt_voronoi(&dt, g, h, 1, cvl_frame_height(frame), x, 0, 0);
    }

    free(g);
    free(h);
    return dt;
}

/**
 * \param frames	The array of frames.
 * \param depth		The number of frames in the array.
 * \return		The distance transform.
 *
 * Computes the Squared Euclidean Distance Transform (SEDT) of the given array of 
 * frames. The result will be stored in an array of integer fields that 
 * has the same dimensions as the array of frames.\n
 * This function interprets the array of frames as a 3D cuboid and computes the
 * 3D EDT: If the pixel at position (x,y,z) in the cuboid is a background pixel
 * (its value is zero), then its entry in the distance map will be zero.
 * If the pixel is an object pixel (its value is not zero), then its entry in
 * the distance map will be its squared euclidean distance to the next
 * background pixel.\n
 * The frames must be graylevel. and have the same dimensions. The result can 
 * only be guaranteed to be correct if the width, height, and depth of the
 * cuboid are smaller than 2*sqrt(INT_MAX/2) pixels, or if it is known that 
 * the maximum distance between object pixel and background pixels in the cuboid
 * is less than sqrt(INT_MAX/2).
 * 
 * See also 
 * C.R. Maurer, R. Qi, and V. Raghavan. A Linear Time Algorithm for Computing
 * Exact Euclidean Distance Transforms of Binary Images in Arbitrary Dimensions.
 * IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 25(2),
 * February 2003.
 */
cvl_field_t **cvl_sedt3d(cvl_frame_t * const *frames, int depth)
{
    cvl_assert(frames != NULL);
    cvl_assert(depth > 0);
    cvl_assert(frames[0] != NULL);
    for (int i = 1; i < depth; i++)
    {
	cvl_assert(frames[i] != NULL);
	cvl_assert(cvl_frame_width(frames[i]) == cvl_frame_width(frames[0]));
	cvl_assert(cvl_frame_height(frames[i]) == cvl_frame_height(frames[i]));
    }	
    
    cvl_field_t **dt;
    int *g, *h;
    int ghsize;
    
    ghsize = cvl_max3i(cvl_frame_width(frames[0]), cvl_frame_height(frames[0]), depth);
    g = xnmalloc((size_t)ghsize + (size_t)1, sizeof(int));
    h = xnmalloc((size_t)ghsize + (size_t)1, sizeof(int));
    dt = xnmalloc(depth, sizeof(cvl_field_t *));
    for (int i = 0; i < depth; i++)
    {
	dt[i] = cvl_field_new(sizeof(int), 
		cvl_frame_width(frames[0]), cvl_frame_height(frames[0]));
    }

    // ComputeEDT for d == 2
    for (int z = 0; z < depth; z++)
    {
	// ComputeEDT for d == 1
	for (int y = 0; y < cvl_frame_height(frames[0]); y++)
	{
	    // ComputeEDT for d == 0
	    for (int x = 0; x < cvl_frame_width(frames[0]); x++)
	    {
		*(int *)cvl_field_ref(dt[z], x, y) = 
		    (cvl_frame_get(frames[z], x, y) != 0x00 ? 0x00 : INT_MAX);
	    }
	    cvl_sedt_voronoi(dt, g, h, 0, cvl_frame_width(frames[0]), 0, y, z);
	}
	for (int x = 0; x < cvl_frame_width(frames[0]); x++)
	{
	    cvl_sedt_voronoi(dt, g, h, 1, cvl_frame_height(frames[0]), x, 0, z);
	}
    }
    for (int x = 0; x < cvl_frame_width(frames[0]); x++)
    {
	for (int y = 0; y < cvl_frame_height(frames[0]); y++)
	{
	    cvl_sedt_voronoi(dt, g, h, 2, depth, x, y, 0);
	}
    }

    free(g);
    free(h);
    return dt;
}
