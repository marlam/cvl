/*
 * cvl_mix.h
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

#ifndef CVL_MIX_H
#define CVL_MIX_H

#include "cvl_frame.h"

void cvl_frame_blend(cvl_frame_t *frame, int dst_x, int dst_y,
	const cvl_frame_t *block, const cvl_frame_t *block_alpha);

typedef enum
{
    CVL_LAYER_MODE_MIN 		= 0,
    CVL_LAYER_MODE_MAX 		= 1,
    CVL_LAYER_MODE_MEDIAN	= 2,
    CVL_LAYER_MODE_OR 		= 3,
    CVL_LAYER_MODE_AND 		= 4,
    CVL_LAYER_MODE_XOR 		= 5,
    CVL_LAYER_MODE_DIFF 	= 6,
    CVL_LAYER_MODE_ADD 		= 7,
    CVL_LAYER_MODE_SUB 		= 8,
    CVL_LAYER_MODE_MUL 		= 9,
    CVL_LAYER_MODE_DIV 		= 10
} cvl_layer_mode_t;

void cvl_frame_layer(cvl_frame_t *frame, cvl_layer_mode_t mode,
	const cvl_frame_t **layers, const int *src_x, const int *src_y, int number_of_layers);

#endif
