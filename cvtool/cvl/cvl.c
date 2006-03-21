/*
 * cvl.c
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

#include "cvl_msg.c"
#include "cvl_math.c"
#include "cvl_pixel.c"
#include "cvl_vector.c"
#include "cvl_field.c"
#include "cvl_frame.c"
#include "cvl_io.c"
#include "cvl_color.c"
#include "cvl_cairo.c"
#include "cvl_convolve.c"
#include "cvl_smooth.c"
#include "cvl_edge.c"
#include "cvl_transform.c"
#include "cvl_blend.c"
#include "cvl_misc.c"
#include "cvl_stereo.c"
#include "cvl_opticalflow.c"
