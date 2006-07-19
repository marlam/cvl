/*
 * cvl_stereo.h
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

#ifndef CVL_STEREO_H
#define CVL_STEREO_H

#include <stdint.h>

#include "cvl_frame.h"

typedef enum
{
    CVL_DIBR_FILL_NONE   = 0,
    CVL_DIBR_FILL_AVG    = 1,
    CVL_DIBR_FILL_NEAR   = 2,
    CVL_DIBR_FILL_FAR    = 3,
    CVL_DIBR_FILL_LINEAR = 4
} cvl_dibr_fill_technique_t;
cvl_frame_t *cvl_dibr(const cvl_frame_t *frame, const cvl_frame_t *depthmap, 
	double position, double zps, int b, double f, cvl_dibr_fill_technique_t fill_technique);

void cvl_trackdepth_init_first_trackmap(cvl_field_t *trackmap, const cvl_frame_t *depth);
void cvl_trackdepth_init_next_trackmap(cvl_field_t *trackmap, 
	const cvl_field_t *prev_trackmap, const cvl_field_t *flow);
cvl_frame_t *cvl_trackdepth(int N, int req, const cvl_field_t *trackmap_A, const cvl_field_t *trackmap_O);

typedef enum
{
    CVL_ANAGLYPH_RED_CYAN  = 0,
    CVL_ANAGLYPH_RED_GREEN = 1,
    CVL_ANAGLYPH_RED_BLUE  = 2
} cvl_anaglyph_type_t;
cvl_frame_t *cvl_anaglyph(const cvl_frame_t *stereoframe, bool color, cvl_anaglyph_type_t type);

#endif
