/*
 * cvl_assert.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005  Martin Lambers <marlam@marlam.de>
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

#include "config.h"


/*
 * cvl_assert(condition)
 *
 * This function works like assert(3), except that it uses a cvl_msg_* function
 * to print the error message, and that it is activated by CVL_DEBUG != 0.
 */

#if CVL_DEBUG

# include <stdlib.h>

# include "cvl/cvl_msg.h"

# define cvl_assert(condition) \
    if (!(condition)) \
    { \
	cvl_msg_fmt_err(LIBRARY_NAME ": in %s, line %d: in function %s: assertion \"%s\" failed", \
		__FILE__, __LINE__, __func__, #condition); \
	abort(); \
    }

#else

# define cvl_assert(condition)

#endif


#endif
