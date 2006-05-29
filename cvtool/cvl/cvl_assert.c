/*
 * cvl_assert.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2006  Martin Lambers <marlam@marlam.de>
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

/**
 * \file cvl_assert.h
 * \brief Debugging.
 *
 * Functions useful to debug both CVL and programs that use CVL.
 */

#include "config.h"

#include "cvl/cvl_assert.h"


#if HAVE_BACKTRACE

# include <stdlib.h>
# include <execinfo.h>

# include "cvl/cvl_msg.h"

/**
 * Obtains a backtrace and prints it. Useful for debugging purposes.
 * This function currently only works on GNU/Linux. On other systems, it does
 * nothing.
 */
void cvl_print_backtrace(void)
{
    /* Adapted from the example in the glibc manual. */
    void *array[64];
    int size;
    char **strings;
    
    size = backtrace(array, 64);
    
    if (size <= 1)
    {
	cvl_msg_err("No backtrace available.");
    }
    else
    {
	strings = backtrace_symbols(array, size);
	cvl_msg_err("Backtrace:");
	for (int i = 1; i < size; i++)
	{
	    cvl_msg_err("%s", strings[i]);
	}
	free(strings);
    }
}

#else

/**
 * Obtains a backtrace and prints it. Useful for debugging purposes.
 * This function currently only works on GNU/Linux. On other systems, it does
 * nothing.
 */
void cvl_print_backtrace(void)
{
}

#endif
