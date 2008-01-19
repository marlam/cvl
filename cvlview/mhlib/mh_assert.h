/*
 * mh_assert.h
 * 
 * This file is part of cvtool, a computer vision tool.
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

#ifndef MH_ASSERT_H
#define MH_ASSERT_H

/**
 * \param condition	Condition to assert.
 *
 * This macro works like assert(3), except that it uses a mh_msg_* function
 * to print the error message, and that it is activated by defining DEBUG to
 * 1. If DEBUG is undefined or defined to 0, this macro is empty.
 */

#if defined DEBUG && DEBUG

# include "mh_msg.h"
# include "mh_crash.h"

# define mh_assert(condition) \
    if (!(condition)) \
    { \
	mh_msg_fmt_err("assertion failed in %s, line %d, function %s():\n\"%s\"", \
		__FILE__, __LINE__, __func__, #condition); \
	mh_crash(); \
    }

#else

# define mh_assert(condition)

#endif

#endif
