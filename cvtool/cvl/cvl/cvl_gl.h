/*
 * cvl_gl.h
 * 
 * This file is part of CVL, a computer vision library.
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

#ifndef CVL_GL_H
#define CVL_GL_H

#include <stdarg.h>

#include <GL/gl.h>

typedef unsigned long cvl_gl_context_t;

cvl_gl_context_t *cvl_gl_context_new(const char *display_name);
void cvl_gl_context_make_current(cvl_gl_context_t *ctx);
void cvl_gl_context_free(cvl_gl_context_t *ctx);

void cvl_gl_state_save(void);
void cvl_gl_state_restore(void);

#ifdef __GNUC__
void cvl_gl_check_fb(const char *what, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_gl_check_errors(const char *what, ...) __attribute__ ((format (printf, 1, 2)));
#else
void cvl_gl_check_fb(const char *what, ...);
void cvl_gl_check_errors(const char *what, ...);
#endif

#ifdef __GNUC__
char *cvl_gl_srcprep(char *src, const char *defines, ...) __attribute__ ((format (printf, 2, 3)));
#else
char *cvl_gl_srcprep(char *src, const char *defines, ...);
#endif
GLuint cvl_gl_shader(const char *name, GLenum type, const char *src);
GLuint cvl_gl_program_new(const char *name, GLuint vshader, GLuint fshader);
GLuint cvl_gl_program_new_src(const char *name, const char *vshader_src, const char *fshader_src);
void cvl_gl_program_free(GLuint program);

GLuint cvl_gl_program_cache_get(const char *name);
void cvl_gl_program_cache_put(const char *name, GLuint program);

#endif
