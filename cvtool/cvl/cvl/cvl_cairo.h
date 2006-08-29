/*
 * cvl_cairo.h
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

#ifndef CVL_CAIRO_H
#define CVL_CAIRO_H

#include <cairo.h>

#include "cvl_pixel.h"
#include "cvl_frame.h"
#include "cvl_color.h"

void cvl_cairo_start(cvl_frame_t *frame, cairo_t **cr, cvl_pixel_type_t *original_pixel_type);
void cvl_cairo_stop(cvl_frame_t *frame, cairo_t *cr, cvl_pixel_type_t original_pixel_type);

/**
 * \param cr		CAIRO drawing context.
 * \param color		CVL color.
 *
 * Shortcut to call cairo_set_source_rgb() with a CVL color.
 */
extern inline void cvl_cairo_set_source_rgb(cairo_t *cr, cvl_color_t color)
{
    cvl_pixel_t rgb = cvl_color_to_pixel(color, CVL_PIXEL_RGB);
    cairo_set_source_rgb(cr, 
	    (double)cvl_pixel_rgb_to_r(rgb) / 255.0,
	    (double)cvl_pixel_rgb_to_g(rgb) / 255.0,
	    (double)cvl_pixel_rgb_to_b(rgb) / 255.0);
}

#endif
