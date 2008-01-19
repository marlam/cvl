/*
 * cvl_math.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_MATH_H
#define CVL_MATH_H

#include "cvl_frame.h"

void cvl_add(cvl_frame_t *dst, cvl_frame_t *src, const float *summand);
void cvl_mul(cvl_frame_t *dst, cvl_frame_t *src, const float *factor);
void cvl_first_derivative(cvl_frame_t *dst, cvl_frame_t *src, int channel);
void cvl_laplacian(cvl_frame_t *dst, cvl_frame_t *src, int channel);

#endif
