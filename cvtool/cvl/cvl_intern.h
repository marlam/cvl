/*
 * cvl_intern.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008, 2009, 2010
 * Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_INTERN_H
#define CVL_INTERN_H

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>

#include <GL/glew.h>
#ifdef W32_NATIVE
# include <GL/wglew.h>
#else
# include <GL/glx.h>
#endif

#define CVL_BUILD
#include "cvl/cvl.h"

typedef struct
{
#ifdef W32_NATIVE
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
#else
    Display *display;
    GLXContext context;
    XVisualInfo* visualinfo;
    Colormap colormap;
    Window window;
#endif
} cvl__gl_context_t;

typedef struct
{
    /* Error status. */
    int error;
    char *error_msg;
    /* Texture formats. */
    GLuint cvl_gl_texture_formats[3][4];
    /* The FBO. */
    GLuint cvl_gl_fbo;
    bool cvl_gl_fbo_initialized;
    /* The 1:1 standard quad. */
    GLuint cvl_gl_std_quad;
    bool cvl_gl_std_quad_initialized;
    /* OpenGL limits */
    GLint cvl_gl_max_tex_size;
    GLint cvl_gl_max_render_targets;
    GLint cvl_gl_max_texture_units;
    /* The GL program cache. */
    int cvl_gl_program_cache_length;
    int cvl_gl_program_cache_size;
    char **cvl_gl_program_cache_names;
    GLuint *cvl_gl_program_cache_values;
} cvl_context_t;

cvl_context_t *cvl_context(void);

void cvl_gl_set_texture_state(void);

#define cvl_assert(condition) \
    if (!cvl_error() && !(condition)) \
    { \
	cvl_error_set(CVL_ERROR_ASSERT, "%s(): assertion \"%s\" failed", __func__, #condition); \
    }

#define cvl_check_errors() \
    cvl_gl_check_errors(__func__);

char *cvl_strdup(const char *s);
char *cvl_vasprintf(const char *format, va_list ap);
#ifdef __GNUC__
char *cvl_vasprintf(const char *format, va_list ap) __attribute__ ((format (printf, 1, 0)));
char *cvl_asprintf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#else
char *cvl_vasprintf(const char *format, va_list ap);
char *cvl_asprintf(const char *format, ...);
#endif

static inline int cvl_iroundf(float x)
{
    return (int)lroundf(x);
}

static inline int cvl_powi(int x, int y)
{
    cvl_assert(y >= 0);

    int result = 1;
    if (x == 2 && y > 0)
    {
	result = x << (y - 1);
    }
    else
    {
	for (int i = 0; i < y; i++)
	    result *= x;
    }
    return result;
}

static inline float cvl_minf(float a, float b)
{
    return fminf(a, b);
}

static inline float cvl_maxf(float a, float b)
{
    return fmaxf(a, b);
}

static inline float cvl_min3f(float a, float b, float c)
{
    return cvl_minf(cvl_minf(a, b), c);
}

static inline float cvl_max3f(float a, float b, float c)
{
    return cvl_maxf(cvl_maxf(a, b), c);
}

static inline float cvl_min4f(float a, float b, float c, float d)
{
    return cvl_minf(cvl_minf(a, b), cvl_minf(c, d));
}

static inline float cvl_max4f(float a, float b, float c, float d)
{
    return cvl_maxf(cvl_maxf(a, b), cvl_maxf(c, d));
}

static inline int cvl_mini(int a, int b)
{
    return a < b ? a : b;
}

static inline int cvl_maxi(int a, int b)
{
    return a > b ? a : b;
}

static inline int cvl_min3i(int a, int b, int c)
{
    return cvl_mini(cvl_mini(a, b), c);
}

static inline int cvl_max3i(int a, int b, int c)
{
    return cvl_maxi(cvl_maxi(a, b), c);
}

static inline int cvl_min4i(int a, int b, int c, int d)
{
    return cvl_mini(cvl_mini(a, b), cvl_mini(c, d));
}

static inline int cvl_max4i(int a, int b, int c, int d)
{
    return cvl_maxi(cvl_maxi(a, b), cvl_maxi(c, d));
}

static inline int cvl_clampi(int x, int min, int max)
{
    return cvl_mini(max, cvl_maxi(min, x));
}

static inline int cvl_next_power_of_two(int x)
{
    int p = 1;
    while (p < x)
	p *= 2;
    return p;
}

static inline int cvl_log2(int x)
{
    cvl_assert(x > 0);

    int log2 = 0;
    while (x > 1)
    {
	x /= 2;
	log2++;
    }
    return log2;
}

void cvl_gauss_mask(int k, float s, float *mask, float *weight_sum);

#endif
