/*
 * cvl_assert.h
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

#ifndef CVL_ASSERT_H
#define CVL_ASSERT_H


void cvl_print_backtrace(void);

/**
 * \param condition	Condition to assert.
 *
 * This macro works like assert(3), except that it uses a cvl_msg_* function
 * to print the error message, and that it is activated by defining CVL_DEBUG to
 * 1. If CVL_DEBUG is undefined or defined to 0, this macro is empty.
 */

#if defined CVL_DEBUG && CVL_DEBUG

# include <stdlib.h>

# include "cvl_msg.h"

# define cvl_assert(condition) \
    if (!(condition)) \
    { \
	cvl_msg_fmt_err("assertion failed in %s, line %d, function %s():\n\"%s\"", \
		__FILE__, __LINE__, __func__, #condition); \
	cvl_print_backtrace(); \
	abort(); \
    }

#else

# define cvl_assert(condition)

#endif

#endif
