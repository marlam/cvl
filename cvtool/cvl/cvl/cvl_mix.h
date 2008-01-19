/*
 * cvl_mix.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CVL_MIX_H
#define CVL_MIX_H

#include "cvl_frame.h"

typedef enum
{
    CVL_LAYER_MIN 	= 0,
    CVL_LAYER_MAX 	= 1,
    CVL_LAYER_MEDIAN	= 2,
    CVL_LAYER_OR 	= 3,
    CVL_LAYER_AND 	= 4,
    CVL_LAYER_XOR 	= 5,
    CVL_LAYER_DIFF 	= 6,
    CVL_LAYER_ADD 	= 7,
    CVL_LAYER_XADD 	= 8,
    CVL_LAYER_SUB 	= 9,
    CVL_LAYER_XSUB 	= 10,
    CVL_LAYER_MUL 	= 11,
    CVL_LAYER_DIV 	= 12
} cvl_layer_mode_t;

void cvl_layer(cvl_frame_t *dst, cvl_frame_t **layers, int number_of_layers, cvl_layer_mode_t mode);
void cvl_blend(cvl_frame_t *dst, int dst_x, int dst_y, cvl_frame_t *block, cvl_frame_t *block_alpha);
void cvl_mix(cvl_frame_t *frame, cvl_frame_t **srcs, const float *w, int n);

#endif
