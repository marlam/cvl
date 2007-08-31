/*
 * cvl_intern.h
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

#ifndef CVL_INTERN_H
#define CVL_INTERN_H

#include <stdarg.h>
#include <stdbool.h>

#include <GL/glew.h>
#ifdef W32_NATIVE
# include <GL/wglew.h>
#elif defined _CVL_MESA
# include <stdint.h>
# include <GL/osmesa.h>
#else
# include <GL/glx.h>
#endif

#include "cvl/cvl_error.h"

typedef struct
{
#ifdef W32_NATIVE
    HWND hwnd;
    HDC hdc;
    HGLRC hglrc;
#elif defined _CVL_MESA
    OSMesaContext context;
    uint32_t buffer[1];
#else
    Display *display;
    GLXPbuffer pbuffer;
    GLXContext context;
#endif
} cvl__gl_context_t;

typedef struct
{
    /* Error status. */
    int error;
    char *error_msg;
    /* Texture formats. */
    GLuint cvl_gl_texture_formats[2][4];
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

#endif
