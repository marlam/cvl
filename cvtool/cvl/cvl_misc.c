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

#include "strverscmp.h"

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
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
cvl_frame_t *cvl_frame_diff(const cvl_frame_t *f1, const cvl_frame_t *f2)
{
    cvl_frame_t *d = cvl_frame_new(cvl_frame_pixel_type(f1), cvl_frame_width(f1), cvl_frame_height(f1));

    for (int i = 0; i < cvl_frame_width(d) * cvl_frame_height(d); i++)
    {
	cvl_pixel_t p1 = cvl_frame_get_i(f1, i);
	cvl_pixel_t p2 = cvl_frame_get_i(f2, i);
	cvl_pixel_t pd;
	if (cvl_frame_pixel_type(d) == CVL_PIXEL_GRAY)
	{
	    pd = abs((int)p1 - (int)p2);
	}
	else if (cvl_frame_pixel_type(d) == CVL_PIXEL_RGB)
	{
	    pd = cvl_pixel_rgb(
	    	    abs((int)cvl_pixel_rgb_to_r(p1) - (int)cvl_pixel_rgb_to_r(p2)),
		    abs((int)cvl_pixel_rgb_to_g(p1) - (int)cvl_pixel_rgb_to_g(p2)),
		    abs((int)cvl_pixel_rgb_to_b(p1) - (int)cvl_pixel_rgb_to_b(p2)));
	}
	else
	{
	    pd = cvl_pixel_yuv(abs((int)cvl_pixel_yuv_to_y(p1) - (int)cvl_pixel_yuv_to_y(p2) + 16), 128, 128);
	}
	cvl_frame_set_i(d, i, pd);
    }
    
    return d;
}
