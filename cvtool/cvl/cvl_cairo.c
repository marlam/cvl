/*
 * cvl_cairo.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2006  Martin Lambers <marlam@marlam.de>
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
 * \file cvl_cairo.h
 * \brief Interface to the CAIRO graphics library.
 * 
 * Functions that allow easy usage of the CAIRO graphics library with CVL frames.
 */

#include "config.h"

#include <stdlib.h>

#include <cairo.h>

#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_msg.h"
#include "cvl/cvl_cairo.h"


/**
 * \param frame		The frame.
 * \param cr		The CAIRO drawing context.
 * \param original_pixel_type	Storage space for the original pixel type of \a frame.
 *
 * Initializes a CAIRO drawing context for the given frame.
 * The idea is to call this function, then use CAIRO to work on \a cr, and then call
 * cvl_cairo_stop() when finished.
 * Do not use CVL functions on the frame while in CAIRO mode.
 */
void cvl_cairo_start(cvl_frame_t *frame, cairo_t **cr, cvl_pixel_type_t *original_pixel_type)
{
    cvl_assert(frame != NULL);
    cvl_assert(cr != NULL);

    cairo_surface_t *surface;

    *original_pixel_type = cvl_frame_pixel_type(frame);
    cvl_frame_convert(frame, CVL_PIXEL_RGB);
    surface = cairo_image_surface_create_for_data(
	    (void *)(frame->_p), CAIRO_FORMAT_RGB24, 
	    cvl_frame_width(frame), cvl_frame_height(frame),
	    sizeof(cvl_pixel_t) * cvl_frame_width(frame));    
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS)
    {
	cvl_msg_err("cannot create CAIRO surface");
	abort();
    }
    *cr = cairo_create(surface);
    if (cairo_status(*cr) != CAIRO_STATUS_SUCCESS)
    {
	cvl_msg_err("cannot create CAIRO context");
	abort();
    }
}

/**
 * \param frame		The frame.
 * \param cr		The CAIRO drawing context.
 * \param original_pixel_type	The original pixel type of \a frame.
 *
 * Destroys \a cr and converts the frame data back to the original pixel type.
 * See also cvl_cairo_start().
 */
void cvl_cairo_stop(cvl_frame_t *frame, cairo_t *cr, cvl_pixel_type_t original_pixel_type)
{
    cvl_assert(frame != NULL);
    cvl_assert(cr != NULL);

    cairo_surface_t *surface;

    surface = cairo_get_target(cr);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
    cvl_frame_convert(frame, original_pixel_type);
}

/**
 * \param cr		CAIRO drawing context.
 * \param color		CVL color.
 *
 * Shortcut to call cairo_set_source_rgb() with a CVL color.
 */
void cvl_cairo_set_source_rgb(cairo_t *cr, cvl_color_t color)
{
    cvl_pixel_t rgb = cvl_color_to_pixel(color, CVL_PIXEL_RGB);
    cairo_set_source_rgb(cr, 
	    (double)cvl_pixel_rgb_to_r(rgb) / 255.0,
	    (double)cvl_pixel_rgb_to_g(rgb) / 255.0,
	    (double)cvl_pixel_rgb_to_b(rgb) / 255.0);
}
