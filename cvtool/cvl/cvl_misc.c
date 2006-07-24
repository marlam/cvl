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
#include <string.h>
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
	    int val = g[l] + (h[l] - i[d]) * (h[l] - i[d]);
	    cvl_field_set(dt[i[2]], i[0], i[1], &val);
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
	    int val = (cvl_frame_get(frame, x, y) == 0x00 ? 0x00 : INT_MAX);
	    cvl_field_set(dt, x, y, &val);
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
		int val = (cvl_frame_get(frames[z], x, y) == 0x00 ? 0x00 : INT_MAX);
		cvl_field_set(dt[z], x, y, &val);
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


/**
 * \param frame		The frame.
 * \return		The skeleton of the frame.
 *
 * Computes the skeleton of the graylevel frame \a frame, with the following
 * properties:\n
 * - The skeleton is thin (1 pixel wide)\n
 * - The skeleton is connected\n
 * - The original shape can be recovered from the skeleton with a tolerance
 *   of 1 pixel\n
 * Black pixels in the input will be interpreted as background, all others as
 * foreground.\n
 * Pixels that do not belong to the skeleton of \a frame will have the value 0
 * in the resulting fiels. The value of skeleton pixels will be the chessboard distance 
 * to the next background pixel.\n
 * Note that the 1-pixel-border of the frame is ignored by this function.
 *
 * See also:
 * R. Cardoner and F. Thomas. Residuals + Directional Gaps = Skeletons. Pattern
 * Recognition Letters, 18(4):343-353, 1997
 */
cvl_field_t *cvl_skeleton(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    cvl_assert(cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY);

    const int w = cvl_frame_width(frame);
    const int h = cvl_frame_height(frame);
    /* The following array stores the x and y difference of a pixel to one of
     * its 8 neighbor pixels. For example, for neighbor 0, dx is +1 and dy
     * is 0. We store the 8 value pairs three times in a row and start to index
     * them at d=8, so that we can access entrys d-4, d+2, ... without wrapping
     * the index. */
    const int dx[24] = 
    { 
	+1, +1,  0, -1, -1, -1, 0, +1,
	+1, +1,  0, -1, -1, -1, 0, +1,
	+1, +1,  0, -1, -1, -1, 0, +1,
    };
    const int dy[24] = 
    {  
	0, +1, +1, +1, 0, -1, -1, -1,
	0, +1, +1, +1, 0, -1, -1, -1,
	0, +1, +1, +1, 0, -1, -1, -1,
    };
    cvl_field_t *S;	/* the skeleton */
    bool *X;		/* original shape */
    bool *L;		/* skeleton increment */
    bool *E;		/* eroded image */
    bool *I;		/* skeleton increment */
    bool X_is_empty;
    bool p;
    int chessboard_distance;

    S = cvl_field_new(sizeof(int), w, h);
    L = xmalloc(w * h * sizeof(bool));
    E = xmalloc(w * h * sizeof(bool));
    I = xmalloc(w * h * sizeof(bool));
    X = xmalloc(w * h * sizeof(bool));
    for (int i = 0; i < w * h; i++)
    {
	X[i] = cvl_frame_get_i(frame, i) != 0x00;
    }
    
    chessboard_distance = 0;
    cvl_field_zero(S);
    memset(L, 0, w * h * sizeof(bool));
    X_is_empty = false;
    do
    {
	cvl_msg_dbg("%s: chessboard_distance = %d", __func__, chessboard_distance);
	memset(I, 0, w * h * sizeof(bool));
	for (int d = 8; d < 16; d += 2)
	{
	    for (int y = 1; y < h - 1; y++)
	    {
		for (int x = 1; x < w - 1; x++)
		{
		    /* G */
		    p = X[y * w + x] && !X[(y + dy[d]) * w + (x + dx[d])]
			&& ((X[(y + dy[d + 1]) * w + (x + dx[d + 1])]
				    && !X[(y + dy[d + 2]) * w + (x + dx[d + 2])])
				|| (X[(y + dy[d - 1]) * w + (x + dx[d - 1])]
				    && !X[(y + dy[d - 2]) * w + (x + dx[d - 2])]));
		    I[y * w + x] = I[y * w + x] || p;
		    /* E */
		    p = X[y * w + x] && X[(y + dy[d]) * w + (x + dx[d])];
		    E[y * w + x] = p;
		}
	    }
	    for (int y = 1; y < h - 1; y++)
	    {
		for (int x = 1; x < w - 1; x++)
		{
		    /* R */
		    p = X[y * w + x] && !(E[y * w + x] || E[(y + dy[d + 4]) * w + (x + dx[d + 4])]);
		    I[y * w + x] = I[y * w + x] || p;
		    /* X */
		    p = E[y * w + x] || I[y * w + x];
		    X[y * w + x] = p;
		}
	    }
	}
	/* X */
	X_is_empty = true;
	for (int y = 1; y < h - 1; y++)
    	{
	    for (int x = 1; x < w - 1; x++)
	    {
		p = X[y * w + x] && !L[y * w + x];
		X[y * w + x] = p;
		if (p)
		{
		    X_is_empty = false;
		}
	    }
	}
    	/* S */
	for (int y = 1; y < h - 1; y++)
    	{
	    for (int x = 1; x < w - 1; x++)
	    {
    		int p = *cvl_field_i_get(S, x, y) + (L[y * w + x] ? chessboard_distance : 0);
		cvl_field_set(S, x, y, &p);
	    }
	}
	/* L */
	if (!X_is_empty)
	{
	    for (int y = 1; y < h - 1; y++)
	    {
		for (int x = 1; x < w - 1; x++)
		{
		    p = I[y * w + x] && !L[y * w + x];
		    L[y * w + x] = p;
		}
	    }
    	    chessboard_distance++;
	}
    }
    while (!X_is_empty);

    free(X);
    free(L);
    free(E);
    free(I);
    return S;
}


/**
 * \param frames	The array of frames.
 * \param depth		The number of frames in the array.
 * \return		The skeleton of the frames.
 *
 * Computes the 3D skeleton of the given array of frames, with the following
 * properties:\n
 * - The skeleton is thin (1 pixel wide)\n
 * - The skeleton is connected\n
 * - The original shape can be recovered from the skeleton with a tolerance
 *   of 1 pixel\n
 * Black pixels in the input will be interpreted as background, all others as
 * foreground.\n
 * The result will be stored in an array of integer fields that has the same 
 * dimensions as the array of frames.
 * Pixels that do not belong to the skeleton of \a frame will have the value 0
 * in the resulting fiels. The value of skeleton pixels will be the chessboard distance 
 * to the next background pixel.\n
 * Note that the 1-pixel-border is ignored by this function.
 *
 * See also:
 * R. Cardoner and F. Thomas. Residuals + Directional Gaps = Skeletons. Pattern
 * Recognition Letters, 18(4):343-353, 1997\n
 * F. Romero, L. Ruos, and F. Thomas. Fast Skeletonization of Spatially Encoded
 * Objects. Proceedings of the 15th International Conference on Pattern Recognition 
 * (ICPR '00), vol. 3, pp. 510-513, 2000.
 */
cvl_field_t **cvl_skeleton3d(cvl_frame_t * const *frames, int depth)
{
    cvl_assert(frames != NULL);
    cvl_assert(depth > 0);
    cvl_assert(frames[0] != NULL);
    cvl_assert(cvl_frame_pixel_type(frames[0]) == CVL_PIXEL_GRAY);
    for (int i = 1; i < depth; i++)
    {
	cvl_assert(frames[i] != NULL);
	cvl_assert(cvl_frame_pixel_type(frames[i]) == CVL_PIXEL_GRAY);
	cvl_assert(cvl_frame_width(frames[i]) == cvl_frame_width(frames[0]));
	cvl_assert(cvl_frame_height(frames[i]) == cvl_frame_height(frames[i]));
    }	

    const int w = cvl_frame_width(frames[0]);
    const int h = cvl_frame_height(frames[0]);
    const int d = depth;
    /* The following array stores the x, y, and z difference of a pixel to one of
     * its 26 neighbor pixels. For example, for neighbor 0, dx is +1, dy is 0,
     * and dz is 0. */
    const int dx[26] = 
    { 
	+1, +1, 0, -1, -1, -1, 0, +1, 0, +1, 0, -1, +1, -1, 0, 0, 0, 0, -1, -1, +1, +1, +1, +1, -1, -1
    };
    const int dy[26] = 
    {  
	0, +1, +1, +1, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, +1, -1, +1, -1, -1, -1, -1, +1, +1, +1, +1
    };
    const int dz[26] =
    {
	0, 0, 0, 0, 0, 0, 0, 0, +1, +1, -1, -1, -1, +1, -1, -1, +1, +1, -1, +1, +1, -1, +1, -1, -1, +1
    };
    /* The directions x+, y+, z+, x-, y-, z- needed for the main computation loop. */
    const int directions[6] = { 0, 6, 8, 4, 2, 10 };
    /* These values are needed for the gap computation for all 6 directions.
     * See the paper "Fast Skeletonization of Spatially Encoded Objects". */
    /*
    const int gapdirs[6][8][2] = 
    { 
	{ { 21, 14 }, { 7, 6 }, { 20, 16 }, { 12, 10 }, { 9, 8 }, { 23, 15 }, { 1, 2 }, { 22, 17 } },
	{ { 18, 11 }, { 14, 10 }, { 21, 12 }, { 5, 4 }, { 7, 0 }, { 19, 13 }, { 16, 8 }, { 20, 9 } },
	{ { 19, 5 }, { 16, 6 }, { 20, 7 }, { 13, 4 }, { 9, 0 }, { 25, 3 }, { 17, 2 }, { 22, 1 } },
	{ { 18, 14 }, { 5, 6 }, { 19, 16 }, { 11, 10 }, { 13, 8 }, { 24, 15 }, { 3, 2 }, { 25, 17 } },
	{ { 24, 11 }, { 15, 10 }, { 23, 12 }, { 3, 4 }, { 1, 0 }, { 25, 13 }, { 17, 8 }, { 22, 9 } },
	{ { 18, 5 }, { 14, 6 }, { 21, 7 }, { 11, 4 }, { 12, 0 }, { 24, 3 }, { 15, 2 }, { 23, 1 } }
    };
    */
    const int gapdirs[6][4][2] = 
    { 
	{ { 7, 6 }, { 12, 10 }, { 9, 8 }, { 1, 2 } },
	{ { 14, 10 }, { 5, 4 }, { 7, 0 }, { 16, 8 } },
	{ { 16, 6 }, { 13, 4 }, { 9, 0 }, { 17, 2 } },
	{ { 5, 6 }, { 11, 10 }, { 13, 8 }, { 3, 2 } },
	{ { 15, 10 }, { 3, 4 }, { 1, 0 }, { 17, 8 } },
	{ { 14, 6 }, { 11, 4 }, { 12, 0 }, { 15, 2 } }
    };
    const int resdirs[6] = { 4, 2, 10, 0, 6, 8 };
    cvl_field_t **S;	/* the skeleton */
    bool *X;		/* original shape */
    bool *L;		/* skeleton increment */
    bool *E;		/* eroded image */
    bool *I;		/* skeleton increment */
    bool X_is_empty;
    bool p;
    int chessboard_distance;

    S = xnmalloc(d, sizeof(cvl_field_t *));
    for (int z = 0; z < d; z++)
    {
	S[z] = cvl_field_new(sizeof(int), w, h);
	cvl_field_zero(S[z]);
    }
    L = xnmalloc(w * h, d * sizeof(bool));
    memset(L, 0, w * h * d * sizeof(bool));
    E = xnmalloc(w * h, d * sizeof(bool));
    I = xnmalloc(w * h, d * sizeof(bool));
    X = xnmalloc(w * h, d * sizeof(bool));
    for (int z = 0; z < d; z++)
    {
	for (int i = 0; i < w * h; i++)
	{
	    X[z * h * w + i] = cvl_frame_get_i(frames[z], i) != 0x00;
	}
    }
    
    chessboard_distance = 0;
    X_is_empty = false;
    do
    {
	cvl_msg_dbg("%s: chessboard_distance = %d", __func__, chessboard_distance);
	memset(I, 0, w * h * d * sizeof(bool));
	for (int direction = 0; direction < 6; direction++)
	{
	    for (int z = 1; z < d - 1; z++)
	    {
		for (int y = 1; y < h - 1; y++)
		{
		    for (int x = 1; x < w - 1; x++)
		    {
			bool px = X[z * h * w + y * w + x];
			bool px0 = X[(z + dz[directions[direction]]) * h * w
			    + (y + dy[directions[direction]]) * w
			    + (x + dx[directions[direction]])];
			/* G */
			p = px && !px0;
			if (p)
			{
			    bool pt = false;
			    //for (int gapd = 0; !pt && gapd < 8; gapd++)
			    for (int gapd = 0; !pt && gapd < 4; gapd++)
			    {
				pt = pt || (X[(z + dz[gapdirs[direction][gapd][0]]) * h * w 
					+ (y + dy[gapdirs[direction][gapd][0]]) * w 
					+ (x + dx[gapdirs[direction][gapd][0]])]
					&& !X[(z + dz[gapdirs[direction][gapd][1]]) * h * w 
					+ (y + dy[gapdirs[direction][gapd][1]]) * w 
					+ (x + dx[gapdirs[direction][gapd][1]])]);
			    }
			    p = p && pt;
			}
			I[z * h * w + y * w + x] = I[z * h * w + y * w + x] || p;
			/* E */
			p = px && px0;
			E[z * h * w + y * w + x] = p;
		    }
		}
	    }
	    for (int z = 1; z < d - 1; z++)
	    {
		for (int y = 1; y < h - 1; y++)
		{
		    for (int x = 1; x < w - 1; x++)
		    {
			/* R */
			p = X[z * h * w + y * w + x] 
			    && !(E[z * h * w + y * w + x] 
				    || E[(z + dz[resdirs[direction]]) * h * w 
				    + (y + dy[resdirs[direction]]) * w 
				    + (x + dx[resdirs[direction]])]);
			I[z * h * w + y * w + x] = I[z * h * w + y * w + x] || p;
			/* X */
			X[z * h * w + y * w + x] = E[z * h * w + y * w + x] || I[z * h * w + y * w + x];
	    	    }
		}
	    }
	}
	/* X */
	X_is_empty = true;
	for (int z = 1; z < d - 1; z++)
	{
	    for (int y = 1; y < h - 1; y++)
	    {
		for (int x = 1; x < w - 1; x++)
		{
		    p = X[z * h * w + y * w + x] && !L[z * h * w + y * w + x];
		    X[z * h * w + y * w + x] = p;
		    if (p)
		    {
			X_is_empty = false;
		    }
		}
	    }
	}
    	/* S */
	for (int z = 1; z < d - 1; z++)
	{
	    for (int y = 1; y < h - 1; y++)
	    {
		for (int x = 1; x < w - 1; x++)
		{
		    int p = *cvl_field_i_get(S[z], x, y) + (L[z * h * w + y * w + x] ? chessboard_distance : 0);
		    cvl_field_set(S[z], x, y, &p);
		}
	    }
	}
	/* L */
	if (!X_is_empty)
	{
	    for (int z = 1; z < d - 1; z++)
	    {
		for (int y = 1; y < h - 1; y++)
		{
		    for (int x = 1; x < w - 1; x++)
		    {
    			p = I[z * h * w + y * w + x] && !L[z * h * w + y * w + x];
    			L[z * h * w + y * w + x] = p;
    		    }
    		}
	    }
    	    chessboard_distance++;
	}
    }
    while (!X_is_empty);

    free(X);
    free(L);
    free(E);
    free(I);
    return S;
}


/*
 * The following is an implementation of
 * E. Remy and E. Thiel. Exact Medial Axis with Euclidean Distance. Image and
 * Vision Computing, 23(2):167-175, 2005.
 *
 * All functions that start with the 'remythiel_' prefix were adapted
 * from LutEucli2D.c and LutEucli3D.c as downloaded from 
 * http://www.lim.univ-mrs.fr/~thiel/IVC2004/index.html
 * on 2006-07-06. The original license and copyright notice reads:
 *
 *  "Exact Medial Axis with Euclidean Distance in 2D
 *   Copyright (C) Eric Remy and Edouard Thiel - Oct 2005
 *   http://www.lif-sud.univ-mrs.fr/~thiel/IVC2004/LutEucli2D.c
 *   This program is free software under the terms of the 
 *   GNU Lesser General Public License (LGPL) version 2.1."
 *  
 *  "Exact Medial Axis with Euclidean Distance in 3D
 *   Copyright (C) Eric Remy and Edouard Thiel - Oct 2005
 *   http://www.lif-sud.univ-mrs.fr/~thiel/IVC2004/LutEucli3D.c
 *   This program is free software under the terms of the 
 *   GNU Lesser General Public License (LGPL) version 2.1."
 *
 * Note that all modifications to the original source code are subject to the
 * GNU GPL, version 2 or later (see also the head of this file).
 */

typedef struct 
{
    bool three_dim;	/* Are we in 2D or 3D mode? */
    int r_max;		/* Maximal radius in the SEDT. */
    int m_ng;		/* Number of neighborhood vectors. */
    int m_vg_size;	/* Size of the buffers m_vg_[xyr] for the neighborhood vectors. 
			   The buffers grow dynamically if necessary. */
    int *m_vg_x;	/* Buffer for the x part of the neighborhood vectors. */
    int *m_vg_y;	/* Buffer for the y part of the neighborhood vectors. */
    int *m_vg_z;	/* Buffer for the z part of the neighborhood vectors. */
    //int *m_vg_r;	/* Buffer for the radii of the neighborhood vectors. */
    int L;		/* Dimension of the lookup table. */
    int *ctg;		/* Cone transform. */
    int *dtg;		/* Distance transform of the cone. */
    int *lut;		/* The lookup table: width r_max, height m_vg_size (grows dynamically). */
    bool *possible;	/* Whether a radius r is possible: possible[r] */
} remythiel_data_t;

/* Add a weighting in the mask M.
 * Input:  M the generator of the mask, (x,y,[,z],r) the weighting.
 * Output: the number of the weighting in the mask. */
static int remythiel_add_weighting(remythiel_data_t *data, int x, int y, int z, int r UNUSED)
{
    cvl_assert(data != NULL);
    cvl_assert(x > 0);
    cvl_assert(x < data->L);
    cvl_assert(y >= 0);
    cvl_assert(y <= x);
    if (data->three_dim)
    {
	cvl_assert(z >= 0);
	cvl_assert(z <= y);
    }
    cvl_assert(r > 0);

    int i = data->m_ng;
    if (i >= data->m_vg_size)
    {
	cvl_msg_dbg("%s: resizing remythiel data from %d to %d", __func__, data->m_vg_size, 2 * data->m_vg_size);
	data->m_vg_x = xnrealloc(data->m_vg_x, 2 * data->m_vg_size, sizeof(int));
	data->m_vg_y = xnrealloc(data->m_vg_y, 2 * data->m_vg_size, sizeof(int));
	if (data->three_dim)
	{
	    data->m_vg_z = xnrealloc(data->m_vg_z, 2 * data->m_vg_size, sizeof(int));
	}
	//data->m_vg_r = xnrealloc(data->m_vg_r, 2 * data->m_vg_size, sizeof(int));	
	data->lut = xnrealloc(data->lut, 2 * data->m_vg_size * (data->r_max + 1), sizeof(int)); 
	data->m_vg_size *= 2;
    }
    data->m_vg_x[i] = x;
    data->m_vg_y[i] = y;
    if (data->three_dim)
    {
	data->m_vg_z[i] = z;
    }
    //data->m_vg_r[i] = r;
    data->m_ng++;
    return i;
}

/* Lut column Computation Algorithm.
 * Input:  CTg the distance cone to origin, L the side length, vg the
 *         weighting and ivg its number, Rmax the greatest radius
 *         to be verified in Lut.
 * Output: The Lut column Lut[ivg] is filled with the correct values. */
static void remythiel_comp_lut_col(remythiel_data_t *data, int ivg)
{
    cvl_assert(data != NULL);
    cvl_assert(ivg >= 0 && ivg < data->m_ng);

    int x, y, z, r, r1, r2, ra, rb;
    
    /* Initializes Lut[ivg] to 0 */
    for (r = 0; r <= data->r_max; r++)
    {
	data->lut[ivg * (data->r_max + 1) + r] = 0;
    }

    for (x = 0; x < data->L - data->m_vg_x[ivg]; x++)
    {
	for (y = 0; y <= x; y++)
	{
	    if (data->three_dim)
	    {
		for (z = 0; z <= y; z++)
		{
		    /* Radius of the ball where p1 is located */
		    r1 = data->ctg[z * data->L * data->L + y * data->L + x] + 1;		
		    /* Same for p2 */
		    cvl_assert(z + data->m_vg_z[ivg] >= 0 && z + data->m_vg_z[ivg] < data->L);
		    cvl_assert(y + data->m_vg_y[ivg] >= 0 && y + data->m_vg_y[ivg] < data->L);
		    cvl_assert(x + data->m_vg_x[ivg] >= 0 && x + data->m_vg_x[ivg] < data->L);
		    r2 = data->ctg[(z + data->m_vg_z[ivg]) * data->L * data->L 
			+ (y + data->m_vg_y[ivg]) * data->L + x + data->m_vg_x[ivg]] + 1;
		    if (r1 <= data->r_max && r2 > data->lut[ivg * (data->r_max + 1) + r1])
		    {
			data->lut[ivg * (data->r_max + 1) + r1] = r2;
		    }
		}
	    }
	    else
	    {
		/* Radius of the ball where p1 is located */
		r1 = data->ctg[y * data->L + x] + 1;
		/* Same for p2 */
		cvl_assert(y + data->m_vg_y[ivg] >= 0 && y + data->m_vg_y[ivg] < data->L);
		cvl_assert(x + data->m_vg_x[ivg] >= 0 && x + data->m_vg_x[ivg] < data->L);
		r2 = data->ctg[(y + data->m_vg_y[ivg]) * data->L + x + data->m_vg_x[ivg]] + 1;
		if (r1 <= data->r_max && r2 > data->lut[ivg * (data->r_max + 1) + r1])
		{
		    data->lut[ivg * (data->r_max + 1) + r1] = r2;
		}
	    }
	}
    }

    rb = 0;
    for (ra = 0; ra <= data->r_max; ra++)
    {
	if (data->lut[ivg * (data->r_max + 1) + ra] > rb)
	{
	    rb = data->lut[ivg * (data->r_max + 1) + ra];
	}
	else
	{
	    data->lut[ivg * (data->r_max + 1) + ra] = rb;
	}
    }
}

/* Fast Distance Transform in G(Z^2) or G(Z^3).
 * Input:  L the side length,
 *         CTg the distance cone to the origin,
 *         R the radius of the ball in CTg.
 * Output: DTg the distance image to the background of the ball.
 *
 * Algorithm derived from Hirata's SEDT, with improvements (mark unpropagated 
 * distances with -1 in cones, stop loops ASAP., store intersections).
 * See: T. Hirata, "A unified linear-time algorithm fo computing distance maps",
 * in Information Processing Letters 58:129-133, 1996. */
static inline double remythiel_d_intersec(int u, int gu, int v, int gv)
{
    return ((double)(u + v) + (double)(gu - gv) / (double)(u - v)) / 2.0;
}
static void remythiel_comp_dtg_hirata(remythiel_data_t *data, int R)
{
    int x, xM, y, z, p, dp, k, propag;
    int *Si, *Sv, Sn;		/* index, values, number */
    double *Sr;			/* intersections */
 
    Si = xnmalloc(data->L, sizeof(int));
    Sv = xnmalloc(data->L, sizeof(int));
    Sr = xnmalloc(data->L, sizeof(double));
    
    /* compute bound xM[ and verify that xM < L */
    for (xM = 0; xM < data->L; xM++)
    {
	if (data->ctg[xM] > R) 
	{
	    break;
	}
    }
    cvl_assert(xM < data->L);
    
    if (data->three_dim)
    {
	/* First scan: x++, y++, z-- */
	for (x = 0; x <= xM; x++)
	{
	    for (y = 0; y <= x; y++)
	    {
		k = 0; 
		propag = 0;
		for (z = y; z >= 0; z--)
		{
		    p = z * data->L * data->L + y * data->L + x;
		    if (data->ctg[p] > R)
		    {
			/* outside the ball: background */
			propag = 1;
		    }
		    else if (propag)
		    {
			/* inside the ball, mark to dist. k*k from bg */
		       	k++; 
			data->dtg[p] = k * k; 
		    }
		    else
		    {
			/* inside the ball, no distance propagated */
			data->dtg[p] = -1;
		    }
		}
	    }
	}

	/* Intermediate scan: x++, z++, y++ */
	for (x = 0; x <= xM; x++)
	{
	    for (z = 0; z <= x; z++)
	    {
		Sn = 0;  /* empty stacks */
		/* Compute stacks indices Si[Sn] and values Sv[Sn] */
		for (y = z; y <= x; y++)
		{
		    p = z * data->L * data->L + y * data->L + x; 
		    dp = data->dtg[p];
		    if (dp < 0)
		    { 
			/* Non propagated value */
			continue;  
		    }
		    /* To speedup algorithm, stop at the second consecutive 0 */
		    if (dp == 0 && y > z && data->dtg[p - data->L] == 0)
		    {
			break;
		    }
		    while (Sn >= 2 && remythiel_d_intersec(Si[Sn - 1], Sv[Sn - 1], y, dp) < Sr[Sn - 1])
		    {
			/* pop */
			Sn--;  
		    }
		    /* push */    
		    Si[Sn] = y; 
		    Sv[Sn] = dp; 
		    if (Sn >= 1) 
		    {
			Sr[Sn] = remythiel_d_intersec(Si[Sn - 1], Sv[Sn - 1], Si[Sn], Sv[Sn]);
		    }
		    Sn++;
		}
		if (Sn == 0)
		{
		    /* Empty stack */
		    continue;  
		}
		/* Compute new DTg values using stacks */
		for (y = x; y >= z; y--)
		{
		    p = z * data->L * data->L + y * data->L + x;
		    if (data->dtg[p] == 0)
		    {
			continue;
		    }
		    while (Sn >= 2 && y < Sr[Sn - 1])
		    {
			/* pop */
			Sn--;  
		    }
		    data->dtg[p] = (y - Si[Sn - 1]) * (y - Si[Sn - 1]) + Sv[Sn - 1];
		} 
	    }
	}

	/* Final scan: y++, z++, x++ */
	for (y = 0; y <= xM; y++)
	{
	    for (z = 0; z <= y; z++)
	    {
		Sn = 0;  /* empty stacks */
		/* Compute stacks indices Si[Sn] and values Sv[Sn] */
		for (x = y; x <= xM; x++)
		{
		    p = z * data->L * data->L + y * data->L + x;
		    dp = data->dtg[p];
		    if (dp < 0)
		    {
			/* Non propagated value */
			continue;  
		    }
		    /* To speedup algorithm, stop at the second consecutive 0 */
		    if (dp == 0 && x > y && data->dtg[p - 1] == 0)
		    {
			break;
		    }
		    while (Sn >= 2 && remythiel_d_intersec(Si[Sn - 1], Sv[Sn - 1], x, dp) < Sr[Sn - 1])
		    {
			/* pop */
			Sn--;  
		    }
		    /* push */    
		    Si[Sn] = x; 
		    Sv[Sn] = dp; 
		    if (Sn >= 1)
		    {
			Sr[Sn] = remythiel_d_intersec(Si[Sn - 1], Sv[Sn - 1], Si[Sn], Sv[Sn]);
		    }
		    Sn++;
		}
		if (Sn == 0)
		{
		    /* Empty stack */
		    continue;  
		}
		/* Compute new DTg values using stacks */
		for (x = xM; x >= y; x--)
		{
		    p = z * data->L * data->L + y * data->L + x;
		    if (data->dtg[p] == 0)
		    {
			continue;
		    }
		    while (Sn >= 2 && x < Sr[Sn - 1])
		    {
			/* pop */
			Sn--;
		    }
		    data->dtg[p] = (x - Si[Sn - 1]) * (x - Si[Sn - 1]) + Sv[Sn - 1];
		} 
	    }
	}
    }
    else
    {
	/* First scan: x++, y-- */
	for (x = 0; x <= xM; x++)
	{
    	    k = 0; 
   	    propag = 0;
  	    for (y = x; y >= 0; y--)
 	    {
		p = y * data->L + x;
    		if (data->ctg[p] > R)
   		{
  		    /* outside the ball: background */	    
 		    propag = 1;
		}
    		else if (propag)
   		{
  		    /* inside the ball, mark to dist. k*k from bg */
 		    k++; 
    		    data->dtg[p] = k * k;
    		}
    		else
    		{ 
    		    /* inside the ball, no distance propagated */
    		    data->dtg[p] = -1;
    		}
    	    }
       	}

	/* Second and final scan: y++, x++ */
	for (y = 0; y <= xM; y++)
	{
	    Sn = 0;  /* empty stacks */

	    /* Compute stacks indices Si[Sn]=x and values Sv[Sn]=DTg[x,y] */
	    for (x = y; x <= xM; x++)
	    {
	       	p = y * data->L + x; 
		dp = data->dtg[p];
		if (dp < 0)
		{
	    	    /* Non propagated value */
	    	    continue;
	       	}            
		/* To speedup algorithm, stop at the second consecutive 0 */
		if (dp == 0 && x > y && data->dtg[p - 1] == 0)
		{
	    	    break;
	       	}            
		while (Sn >= 2 && remythiel_d_intersec(Si[Sn - 1], Sv[Sn - 1], x, dp) < Sr[Sn - 1])
		{
		    Sn--;	/* pop */
		}                  
		/* push */    
		Si[Sn] = x; 
		Sv[Sn] = dp; 
		if (Sn >= 1)
		{
	    	    Sr[Sn] = remythiel_d_intersec(Si[Sn - 1], Sv[Sn - 1], Si[Sn], Sv[Sn]);
	       	}
		Sn++;
	    }

	    if (Sn == 0)
	    {
	       	continue;	/* Empty stack */
	    }

	    /* Compute new DTg values using stacks */
	    for (x = xM; x >= y; x--)
	    {
	       	p = y * data->L + x;
		if (data->dtg[p] == 0)
		{
	    	    continue;
	       	}            
		while (Sn >= 2 && x < Sr[Sn - 1])
		{
	    	    Sn--;	/* pop */
	       	}                  
		data->dtg[p] = (x - Si[Sn - 1]) * (x - Si[Sn - 1]) + Sv[Sn - 1];
	    } 
	}
    }

    free(Si);
    free(Sv);
    free(Sr);
}

/* Fast extraction of MA points from Bd inter G(Z^2) or G(Z^3).
 * Input:  x,y[,z] the point to test, MgL the generator of the Lut mask,
 *         Lut the look-up table, DTg the distance transform of the section of
 *         the ball, L the side length.
 * Output: returns 1 if point x,y[,z] is detected as MA in the DTg. */
static bool remythiel_is_mag(remythiel_data_t *data, int x, int y, int z)
{
    cvl_assert(data != NULL);
    cvl_assert(x >= 0);
    cvl_assert(x < data->L);
    cvl_assert(y >= 0);
    cvl_assert(y < data->L);
    if (data->three_dim)
    {
	cvl_assert(z >= 0);
	cvl_assert(z < data->L);
    }
    
    int xx, yy, zz, val, i;

    if (data->three_dim)
    {
	val = data->dtg[z * data->L * data->L + y * data->L + x];
	for (i = 0; i < data->m_ng; i++)
	{
	    xx = x - data->m_vg_x[i];
	    yy = y - data->m_vg_y[i];
	    zz = z - data->m_vg_z[i];
	    if (0 <= zz && zz <= yy && yy <= xx)
	    {
		if (data->dtg[zz * data->L * data->L + yy * data->L + xx] 
			>= data->lut[i * (data->r_max + 1) + val])
		{
		    return false;
		}
	    }
	}
    }
    else
    {
	val = data->dtg[y * data->L + x];
	for (i = 0; i < data->m_ng; i++)
	{
	    xx = x - data->m_vg_x[i];
	    yy = y - data->m_vg_y[i];
	    if (0 <= yy && yy <= xx)
	    {
		cvl_assert(xx >= 0 && xx < data->L && yy >= 0 && yy < data->L);
		if (data->dtg[yy * data->L + xx] >= data->lut[i * (data->r_max + 1) + val])
		{
		    return false;
		}
	    }
	}
    }

    return true;
}

/* Full Lut Computation Algorithm with determination of MgLut.
 * Input:  CTg and DTg two images, L the side length, MgL the generator of the
 *         Lut mask, Lut the look-up table, Rknown the last verified radius, 
 *         Rtarget the maximum radius to be verified.
 * Output: MgL and Lut are filled with the correct values.
 * To compute MgL and Lut from beginning to the maximum radius allowed with L:
 *    MgL.ng = 0;
 *    Rknown = 0;
 *    Rtarget = GreatestRadius (L);
 *    CompLutMask (CTg, DTg, L, &MgL, Lut, Rknown, Rtarget);
 *    Rknown = Rtarget; */
static void remythiel_comp_lut_mask(remythiel_data_t *data)
{
    int x, y, z, r, i, val;

    /* Init CTg */
    if (data->three_dim)
    {
	data->ctg = xnmalloc(data->L * data->L * data->L, sizeof(int));    
    }
    else
    {
	data->ctg = xnmalloc(data->L * data->L, sizeof(int));    
    }
    data->ctg[0] = 0;
    for (x = 1; x < data->L; x++)
    {
	for (y = 0; y <= x; y++)
	{
	    if (data->three_dim)
	    {
		for (z = 0; z <= y; z++)
		{
		    data->ctg[z * data->L * data->L + y * data->L + x] = x * x + y * y + z * z;
		}
	    }
	    else
	    {
		data->ctg[y * data->L + x] = x * x + y * y;
	    }
	}
    }
    
    /* Init DTg to 0 */ 
    if (data->three_dim)
    {
    	data->dtg = xnmalloc(data->L * data->L * data->L, sizeof(int));
    }
    else
    {
    	data->dtg = xnmalloc(data->L * data->L, sizeof(int));
    }
    data->dtg[0] = 0;
    for (x = 1; x < data->L; x++)
    {
       	for (y = 0; y <= x; y++)
	{
	    if (data->three_dim)
	    {
		for (z = 0; z <= y; z++)
		{
		    data->dtg[z * data->L * data->L + y * data->L + x] = 0;
		}
	    }
	    else
	    {
		data->dtg[y * data->L + x] = 0;
	    }
	}
    }
    
    /* Mark possible values in Possible[] */
    data->possible = xcalloc(data->r_max + 1, sizeof(bool));
    for (x = 1; x < data->L; x++)
    {
	for (y = 0; y <= x; y++)
	{
	    if (data->three_dim)
	    {
		for (z = 0; z <= y; z++)
		{
		    val = data->ctg[z * data->L * data->L + y * data->L + x];
		    if (val <= data->r_max)
		    {
			data->possible[val] = true;
		    }
		}
	    }
	    else
	    {
		val = data->ctg[y * data->L + x];
		if (val <= data->r_max)
		{
		    data->possible[val] = true;
		}
	    }
	}
    }

    /* Compute Lut columns for current Lut mask */
    for (i = 0; i < data->m_ng; i++)
    {
        remythiel_comp_lut_col(data, i);
    }

    for (r = 1; r <= data->r_max; r++)
    {
	if (data->possible[r])
	{
	    cvl_msg_dbg("%s: computing lookup table for r = %d / %d", __func__, r, data->r_max);
	    
	    /* Here we can avoid to init DTg because the ball grows with R. */
	    remythiel_comp_dtg_hirata(data, r);
	    
	    if (data->three_dim)
	    {
		for (x = 1; x < data->L; x++)
		{
		    if (data->dtg[x] == 0) 
		    {
			/* Outside the ball */
			break;
		    }
		    for (y = 0; y <= x; y++)
		    {
			if (data->dtg[y * data->L + x] == 0) 
			{
			    /* Outside the ball */
			    break;
			}
			for (z = 0; z <= y; z++)
			{
			    if (data->dtg[z * data->L * data->L + y * data->L + x] == 0)
			    {
				/* Outside the ball */
				break;
			    }
			    if (remythiel_is_mag(data, x, y, z))
			    {
				/* Add a new weighting to MgL */
				i = remythiel_add_weighting(data, x, y, z, r);
				/*
				   cvl_msg_wrn("i=%3d  (x,y,z)= (%2d, %2d, %2d)  added for r= %7d   %s",
				   i + 1, x, y, z, r, cvl_gcd(x, cvl_gcd(y, z)) == 1 ? "visible" : "** NON VISIBLE");
				*/
				/* New column in Lut */
				remythiel_comp_lut_col(data, i);
				cvl_assert(!remythiel_is_mag(data, x, y, z));
			    }
			}
		    }
		}
	    }
    	    else
	    {
		for (x = 1; x < data->L; x++)
		{
		    if (data->dtg[0 * data->L + x] == 0)
		    {
			/* Outside the ball */
			break;
		    }
		    for (y = 0; y <= x; y++)
		    {
			if (data->dtg[y * data->L + x] == 0)
			{
			    /* Outside the ball */
			    break;
			}
			if (remythiel_is_mag(data, x, y, -1))
			{
			    /* Add a new weighting to MgL */
			    i = remythiel_add_weighting(data, x, y, -1, r);
			    /*
			       cvl_msg_dbg("i=%3d  (x,y)= ( %2d, %2d)  added for r= %7d   %s",
			       i + 1, x, y, r, cvl_gcd(x, y) == 1 ? "visible" : "** NON VISIBLE");
			     */
			    /* New column in Lut */
			    remythiel_comp_lut_col(data, i);
			    cvl_assert(!remythiel_is_mag(data, x, y, -1));
			}
	    	    }
		}
	    }
	}
    }
    free(data->possible);
    free(data->ctg);
    free(data->dtg);
}

/**
 * \param sedt		Squared Euclidean Distance Transform.
 * \return		The Medial Axis Transform.
 *
 * Computes the Medial Axis Transform from the Squared Euclidean Distance
 * Transform of a 2D image. See also cvl_sedt().\n
 * The result is a field of integers: background pixels have the value 0, 
 * MA pixels have the corresponding value from the SEDT. 
 * 
 * See also 
 * E. Remy and E. Thiel. Exact Medial Axis with Euclidean Distance. Image and
 * Vision Computing, 23(2):167-175, 2005.
 */
cvl_field_t *cvl_mat(const cvl_field_t *sedt)
{
    cvl_assert(sedt != NULL);
    cvl_assert(cvl_field_element_size(sedt) == sizeof(int));
    
    remythiel_data_t data;
    cvl_field_t *mat;
        
    data.three_dim = false;

    /* Search r_max in SEDT */
    data.r_max = 0;
    for (int i = 0; i < cvl_field_size(sedt); i++)
    {
	int sd = *cvl_field_i_get_i(sedt, i);
	if (sd > data.r_max)
	{
	    data.r_max = sd;
	}
    }
    if (data.r_max < 1 || data.r_max == INT_MAX)
    {
	/* Return a special MAT. */
	mat = cvl_field_new(sizeof(int), cvl_field_width(sedt), cvl_field_height(sedt));
	cvl_field_zero(mat);
	if (data.r_max < 1)
	{
	    cvl_msg_dbg("%s: image does not contain an object", __func__);
	}
	else
	{
	    cvl_msg_dbg("%s: image does not contain a background", __func__);
	    int intmax = INT_MAX;
	    cvl_field_set(mat, cvl_field_width(mat) / 2, cvl_field_height(mat) / 2, &intmax);
	}
	return mat;
    }
    cvl_msg_dbg("%s: highest value in SEDT = %d", __func__, data.r_max);
    
    /* Initialize weightings */
    data.m_vg_size = 4;
    data.m_vg_x = xnmalloc(data.m_vg_size, sizeof(int));
    data.m_vg_y = xnmalloc(data.m_vg_size, sizeof(int));
    //data.m_vg_r = xnmalloc(data.m_vg_size, sizeof(int));
    
    /* CompLutMask(r_max) -> MLut(r_max) */
    data.L = cvl_iround(ceil(sqrt((double)(data.r_max + 1)))) + 1;
    cvl_msg_dbg("%s: lookup table size = %d", __func__, data.L);
    data.lut = xnmalloc(data.m_vg_size * (data.r_max + 1), sizeof(int));
    data.m_ng = 0;
    remythiel_comp_lut_mask(&data);
    cvl_msg_dbg("%s: number of neighborhood vectors in G(Z^2) = %d", __func__, data.m_ng);

    /* Extract MA */
    mat = cvl_field_new(sizeof(int), cvl_field_width(sedt), cvl_field_height(sedt));
    for (int y = 0; y < cvl_field_height(mat); y++)
    {
	for (int x = 0; x < cvl_field_width(mat); x++)
	{
	    bool is_in_mat = false;
	    if (*cvl_field_i_get(sedt, x, y) != 0)
	    {
		bool ok = true;
		/* for all v in M_Lut: DT[p + v] < Lut[vg][DT[p]] */
		for (int i = 0; ok && i < data.m_ng; i++)
		{
		    int val2 = data.lut[i * (data.r_max + 1) + *cvl_field_i_get(sedt, x, y)];
		    for (int ysign = -1; ok && ysign <= +1; ysign += 2)
		    {
			for (int xsign = -1; ok && xsign <= +1; xsign += 2)
			{
			    int val1;

			    val1 = *cvl_field_i_get_r(sedt, 
				    x + xsign * data.m_vg_x[i], 
				    y + ysign * data.m_vg_y[i]);
			    if (val1 >= val2)
			    {
				ok = false;
				break;
			    }
			    val1 = *cvl_field_i_get_r(sedt, 
				    x + ysign * data.m_vg_y[i],
				    y + xsign * data.m_vg_x[i]); 
			    if (val1 >= val2)
			    {
				ok = false;
				break;
			    }
			}
		    }
		}	
		is_in_mat = ok;
	    }
	    int value = (is_in_mat ? *cvl_field_i_get(sedt, x, y) : 0);
	    cvl_field_set(mat, x, y, &value);
	}
    }
    
    free(data.m_vg_x);
    free(data.m_vg_y);
    //free(data.m_vg_r);
    free(data.lut);
    return mat;
}

/**
 * \param sedts		Squared Euclidean Distance Transform of a cuboid.
 * \param depth		Depth of the cuboid.
 * \return		The Medial Axis Transform.
 *
 * Computes the Medial Axis Transform from the Squared Euclidean Distance
 * Transform of a 3D cuboid. See also cvl_sedt3d().\n
 * The result is stored in fields of integers: background pixels have the value 0, 
 * MA pixels have the corresponding value from the SEDT. 
 * 
 * See also 
 * E. Remy and E. Thiel. Exact Medial Axis with Euclidean Distance. Image and
 * Vision Computing, 23(2):167-175, 2005.
 */
cvl_field_t **cvl_mat3d(cvl_field_t * const *sedts, int depth)
{
    cvl_assert(sedts != NULL);
    cvl_assert(depth > 0);
    cvl_assert(sedts[0] != NULL);
    cvl_assert(cvl_field_element_size(sedts[0]) == sizeof(int));
    for (int i = 1; i < depth; i++)
    {
	cvl_assert(sedts[i] != NULL);
	cvl_assert(cvl_field_width(sedts[i]) == cvl_field_width(sedts[0]));
	cvl_assert(cvl_field_height(sedts[i]) == cvl_field_height(sedts[0]));
	cvl_assert(cvl_field_element_size(sedts[i]) == sizeof(int));
    }
    
    remythiel_data_t data;
    cvl_field_t **mat;
        
    data.three_dim = true;

    /* Search r_max in SEDT */
    data.r_max = 0;
    for (int z = 0; z < depth; z++)
    {
	for (int i = 0; i < cvl_field_size(sedts[0]); i++)
	{
	    int sd = *cvl_field_i_get_i(sedts[z], i);
	    if (sd > data.r_max)
	    {
		data.r_max = sd;
	    }
	}
    }
    if (data.r_max < 1 || data.r_max == INT_MAX)
    {
	/* Return a special MAT. */
	mat = xnmalloc(depth, sizeof(cvl_field_t *));
	for (int z = 0; z < depth; z++)
	{
	    mat[z] = cvl_field_new(sizeof(int), cvl_field_width(sedts[0]), cvl_field_height(sedts[0]));
	    cvl_field_zero(mat[z]);
	}
	if (data.r_max < 1)
	{
	    cvl_msg_dbg("%s: cuboid does not contain an object", __func__);
	}
	else
	{
	    cvl_msg_dbg("%s: cuboid does not contain a background", __func__);
	    int intmax = INT_MAX;
	    cvl_field_set(mat[depth / 2], cvl_field_width(mat[0]) / 2, cvl_field_height(mat[0]) / 2, &intmax);
	}
	return mat;
    }
    cvl_msg_dbg("%s: highest value in SEDT = %d", __func__, data.r_max);
    
    /* Initialize weightings */
    data.m_vg_size = 4;
    data.m_vg_x = xnmalloc(data.m_vg_size, sizeof(int));
    data.m_vg_y = xnmalloc(data.m_vg_size, sizeof(int));
    data.m_vg_z = xnmalloc(data.m_vg_size, sizeof(int));
    //data.m_vg_r = xnmalloc(data.m_vg_size, sizeof(int));
    
    /* CompLutMask(r_max) -> MLut(r_max) */
    data.L = cvl_iround(ceil(sqrt((double)(data.r_max + 1)))) + 1;
    cvl_msg_dbg("%s: lookup table size = %d", __func__, data.L);
    data.lut = xnmalloc(data.m_vg_size * (data.r_max + 1), sizeof(int));
    data.m_ng = 0;
    remythiel_comp_lut_mask(&data);
    cvl_msg_dbg("%s: number of neighborhood vectors in G(Z^3) = %d", __func__, data.m_ng);
    /*
    for (int i = 0; i < data.m_ng; i++)
    {
	cvl_msg_wrn("VECTOR %d:    %d %d %d", i, data.m_vg_x[i], data.m_vg_y[i], data.m_vg_z[i]);
	for (int r = 0; r <= data.r_max; r++)
	{
	    cvl_msg_wrn("Lut entry [i=%d][r=%d] = %d", i, r, data.lut[i * (data.r_max + 1) + r]);
	}
    }
    */

    /* Extract MA */
    mat = xnmalloc(depth, sizeof(cvl_field_t *));
    for (int z = 0; z < depth; z++)
    {
	mat[z] = cvl_field_new(sizeof(int), cvl_field_width(sedts[0]), cvl_field_height(sedts[0]));
    }
    for (int z = 0; z < depth; z++)
    {
	for (int y = 0; y < cvl_field_height(mat[0]); y++)
	{
    	    for (int x = 0; x < cvl_field_width(mat[0]); x++)
    	    {
    		bool is_in_mat = false;
    		if (*cvl_field_i_get(sedts[z], x, y) != 0)
    		{
    		    bool ok = true;
    		    /* for all v in M_Lut: DT[p + v] < Lut[vg][DT[p]] */
    		    for (int i = 0; ok && i < data.m_ng; i++)
    		    {
    			int val2 = data.lut[i * (data.r_max + 1) + *cvl_field_i_get(sedts[z], x, y)];
			for (int zsign = -1; ok && zsign <= +1; zsign += 2)
			{
			    for (int ysign = -1; ok && ysign <= +1; ysign += 2)
			    {
			       	for (int xsign = -1; ok && xsign <= +1; xsign += 2)
				{
			    	    int val1;

				    /* x y z */
				    val1 = *cvl_field_i_get_r(
					    sedts[cvl_reflect(z + zsign * data.m_vg_z[i], depth)], 
					    x + xsign * data.m_vg_x[i], 
					    y + ysign * data.m_vg_y[i]);
				    if (val1 >= val2)
				    {
				       	ok = false;
					break;
				    }
				    /* x z y */
				    val1 = *cvl_field_i_get_r(
					    sedts[cvl_reflect(z + ysign * data.m_vg_y[i], depth)], 
					    x + xsign * data.m_vg_x[i], 
					    y + zsign * data.m_vg_z[i]);
				    if (val1 >= val2)
				    {
				       	ok = false;
					break;
				    }
				    /* y x z */
				    val1 = *cvl_field_i_get_r(
					    sedts[cvl_reflect(z + zsign * data.m_vg_z[i], depth)], 
					    x + ysign * data.m_vg_y[i], 
					    y + xsign * data.m_vg_x[i]);
				    if (val1 >= val2)
				    {
				       	ok = false;
					break;
				    }
				    /* y z x */
				    val1 = *cvl_field_i_get_r(
					    sedts[cvl_reflect(z + xsign * data.m_vg_x[i], depth)], 
					    x + ysign * data.m_vg_y[i], 
					    y + zsign * data.m_vg_z[i]);
				    if (val1 >= val2)
				    {
				       	ok = false;
					break;
				    }
				    /* z x y */
				    val1 = *cvl_field_i_get_r(
					    sedts[cvl_reflect(z + ysign * data.m_vg_y[i], depth)], 
					    x + zsign * data.m_vg_z[i], 
					    y + xsign * data.m_vg_x[i]);
				    if (val1 >= val2)
				    {
				       	ok = false;
					break;
				    }
				    /* z y x */
				    val1 = *cvl_field_i_get_r(
					    sedts[cvl_reflect(z + xsign * data.m_vg_x[i], depth)], 
					    x + zsign * data.m_vg_z[i], 
					    y + ysign * data.m_vg_y[i]);
				    if (val1 >= val2)
				    {
				       	ok = false;
					break;
				    }
				}
			    }
			}
		    }	
		    is_in_mat = ok;
		}
		int value = (is_in_mat ? *cvl_field_i_get(sedts[z], x, y) : 0);
		cvl_field_set(mat[z], x, y, &value);
	    }
	}
    }

    free(data.m_vg_x);
    free(data.m_vg_y);
    free(data.m_vg_z);
    //free(data.m_vg_r);
    free(data.lut);
    return mat;
}
