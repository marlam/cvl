/*
 * cvl.h
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

/**
 * \file 
 * \brief All interfaces of the library.
 *
 * Include this file to access all interfaces of this library.
 * Do not include cvl_*.h files directly.
 */

#ifndef CVL_H
#define CVL_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include "cvl_version.h"
#include "cvl_error.h"
#include "cvl_gl.h"
#include "cvl_init.h"
#include "cvl_taglist.h"
#include "cvl_frame.h"
#include "cvl_color.h"
#include "cvl_basic.h"
#include "cvl_io.h"
#include "cvl_transform.h"
#include "cvl_math.h"
#include "cvl_mix.h"
#include "cvl_filter.h"
#include "cvl_misc.h"
#include "cvl_features.h"
#include "cvl_hdr.h"
#include "cvl_visualization.h"

#ifdef __cplusplus
}
#endif

#endif
