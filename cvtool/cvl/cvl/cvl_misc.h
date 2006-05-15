/*
 * cvl_misc.h
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

#ifndef CVL_MISC_H
#define CVL_MISC_H

#include <stdbool.h>

#include "cvl_frame.h"

const char *cvl_check_version(const char *req_version);
cvl_frame_t *cvl_frame_diff(const cvl_frame_t *f1, const cvl_frame_t *f2);
cvl_field_t *cvl_edt(const cvl_frame_t *frame);
cvl_field_t **cvl_edt3d(cvl_frame_t * const *frames, int depth);

#endif
