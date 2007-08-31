/*
 * cvl_intern.c
 * 
 * This file is part of CVL, a computer vision library.
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

#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>

#include "cvl/cvl_error.h"
#include "cvl_intern.h"


// Get the CVL context from the current GL context
cvl_context_t *cvl_context(void)
{
    GLvoid *ctx;
    glGetPointerv(GL_INDEX_ARRAY_POINTER, &ctx);
    return ctx;
}

void cvl_gl_set_texture_state(void)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

char *cvl_strdup(const char *s)
{
    char *p;

    if (!(p = strdup(s)))
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
	return NULL;
    }
    return p;
}

char *cvl_vasprintf(const char *format, va_list ap)
{
#ifdef HAVE_VASPRINTF
    char *strp;
    if (vasprintf(&strp, format, ap) < 0)
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
	return NULL;
    }
    return strp;
#else
    char *strp = NULL;
    int size = 256;
    int r;
    for (;;)
    {
	strp = realloc(strp, size * sizeof(char));
	if (!strp)
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
	    return NULL;
	}
	r = vsnprintf(strp, size, format, ap);
	if (r < 0)
	{
	    cvl_error_set(CVL_ERROR_IO, "Data error");
	    return NULL;
	}
	else if (r < size)
	{
	    break;
	}
	else
	{
	    size += 256;
	}
    }
#endif
    return strp;
}

char *cvl_asprintf(const char *format, ...)
{
    char *strp;
    va_list args;
    
    va_start(args, format);
    strp = cvl_vasprintf(format, args);
    va_end(args);
    
    return strp;
}
