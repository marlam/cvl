/*
 * mh_alloc.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mh_msg.h"
#include "mh_math.h"
#include "mh_crash.h"
#define extern /* empty */	/* Create a symbol for the inline functions */
#include "mh_alloc.h"


void mh_alloc_failure(void)
{
    mh_msg_err("%s", strerror(ENOMEM));
    mh_failure_handler();
}

char *mh_strdup(const char *s)
{
    char *p;

    if (!(p = strdup(s)))
	mh_alloc_failure();
    return p;
}

char *mh_strndup(const char *s, size_t n)
{
    char *p;

    if (!(p = strndup(s, n)))
	mh_alloc_failure();
    return p;
}

char *mh_vasprintf(const char *format, va_list ap)
{
    char *strp;
    if (vasprintf(&strp, format, ap) < 0)
	mh_alloc_failure();
    return strp;
}

char *mh_asprintf(const char *format, ...)
{
    char *strp;
    va_list args;
    
    va_start(args, format);
    strp = mh_vasprintf(format, args);
    va_end(args);
    
    return strp;
}
