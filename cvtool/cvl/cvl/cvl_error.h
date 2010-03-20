/*
 * cvl_error.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008, 2009, 2010  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_ERROR_H
#define CVL_ERROR_H

#include <stdarg.h>

typedef enum
{
    CVL_OK		= 0,
    CVL_ERROR_MEM	= 1,
    CVL_ERROR_GL	= 2,
    CVL_ERROR_IO	= 3,
    CVL_ERROR_DATA	= 4,
    CVL_ERROR_SYS	= 5,
    CVL_ERROR_ASSERT	= 6
} cvl_error_t;

extern CVL_EXPORT cvl_error_t cvl_error(void);

extern CVL_EXPORT const char *cvl_error_msg(void);

#ifdef __GNUC__
extern CVL_EXPORT void cvl_error_set(cvl_error_t e, const char *msg, ...) __attribute__ ((format (printf, 2, 3)));
#else
extern CVL_EXPORT void cvl_error_set(cvl_error_t e, const char *msg, ...);
#endif

extern CVL_EXPORT void cvl_error_reset(void);

#endif
