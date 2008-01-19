/*
 * mh_alloc.h
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#ifndef MH_ALLOC_H
#define MH_ALLOC_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

#ifndef SIZE_MAX
#ifdef __cplusplus
#define SIZE_MAX (static_cast<size_t>(-1))
#else
#define SIZE_MAX ((size_t)-1)
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void mh_alloc_failure(void);

static inline bool mh_alloc_check(size_t a, size_t b)
{
    return (b == 0 || !(SIZE_MAX / b < a));
}

static inline size_t mh_alloc_mul(size_t a, size_t b)
{
    if (!mh_alloc_check(a, b))
	mh_alloc_failure();
    return a * b;
}

static inline size_t mh_alloc_mul3(size_t a, size_t b, size_t c)
{
    if (!mh_alloc_check(a, b))
	mh_alloc_failure();
    return mh_alloc_mul(a * b, c);
}

static inline void *mh_alloc(size_t size)
{
    void *ptr;
    if (!(ptr = malloc(size))) 
	mh_alloc_failure();
    return ptr;
}

static inline void *mh_realloc(void *ptr, size_t newsize)
{
    if (!(ptr = realloc(ptr, newsize)))
	mh_alloc_failure();
    return ptr;
}

char *mh_strdup(const char *s);
char *mh_strndup(const char *si, size_t n);

#ifdef __GNUC__
char *mh_vasprintf(const char *format, va_list ap) __attribute__ ((format (printf, 1, 0)));
char *mh_asprintf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#else
char *mh_vasprintf(const char *format, va_list ap);
char *mh_asprintf(const char *format, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif
