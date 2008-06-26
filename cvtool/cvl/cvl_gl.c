/*
 * cvl_gl.c
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

/** 
 * \file cvl_gl.h
 * \brief OpenGL helper functions. 
 *
 * Functions to ease the use of OpenGL in CVL and in programs that use CVL.
 */

#include "config.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <GL/glew.h>
#ifdef W32_NATIVE
# include <GL/wglew.h>
#else
# include <GL/glx.h>
#endif

#include "mh.h"

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_gl.h"


/**
 * \param display_name		The X display to connect to.
 * \return			A minimal OpenGL context, or NULL.
 *
 * Creates a minimal OpenGL context and makes it the current context.
 * The context is created for the display given by \a display_name (this
 * parameter is ignored on W32). It is only usable for offscreen rendering.
 * There is no associated window.\n
 * If \a display_name is NULL or if no context can be created, NULL is 
 * returned.\n
 * This function does not use the CVL error status, because that is only
 * available after cvl_init().\n
 */
cvl_gl_context_t *cvl_gl_context_new(const char *display_name)
{
    /* The code in this function was adapted from GLEW version 1.4.0, 
     * file glew/src/glewinfo.c, function glewCreateContext().
     *
     * Original copyright and license notice:
     *
     ** Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
     ** Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
     ** Copyright (C) 2002, Lev Povalahev
     ** All rights reserved.
     ** 
     ** Redistribution and use in source and binary forms, with or without 
     ** modification, are permitted provided that the following conditions are met:
     ** 
     ** * Redistributions of source code must retain the above copyright notice, 
     **   this list of conditions and the following disclaimer.
     ** * Redistributions in binary form must reproduce the above copyright notice, 
     **   this list of conditions and the following disclaimer in the documentation 
     **   and/or other materials provided with the distribution.
     ** * The name of the author may be used to endorse or promote products 
     **   derived from this software without specific prior written permission.
     **
     ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
     ** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
     ** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
     ** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
     ** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
     ** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
     ** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
     ** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
     ** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
     ** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
     ** THE POSSIBILITY OF SUCH DAMAGE.
     */

#ifdef W32_NATIVE

    cvl__gl_context_t *c;
    WNDCLASS wc;
    PIXELFORMATDESCRIPTOR pfd;
    int pixelformat;

    if (!(c = malloc(sizeof(cvl__gl_context_t))))
    {
	return NULL;
    }
    /* Register window class */
    ZeroMemory(&wc, sizeof(WNDCLASS));
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = "CVL";
    if (0 == RegisterClass(&wc))
    {
	return NULL;
    }
    /* Create window */
    if (!(c->hwnd = CreateWindow("CVL", "CVL", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    CW_USEDEFAULT, NULL, NULL, GetModuleHandle(NULL), NULL)))
    {
	return NULL;
    }
    /* Get the device context */
    if (!(c->hdc = GetDC(c->hwnd)))
    {
	return NULL;
    }
    /* Find pixel format */
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    if (!(pixelformat = ChoosePixelFormat(c->hdc, &pfd)))
    {
	return NULL;
    }
    /* Set the pixel format for the device context */
    if (!SetPixelFormat(c->hdc, pixelformat, &pfd))
    {
	return NULL;
    }
    /* Create rendering context */
    if (!(c->hglrc = wglCreateContext(c->hdc)))
    {
	return NULL;
    }

    cvl_gl_context_make_current((cvl_gl_context_t *)c);
    return (cvl_gl_context_t *)c;

#else

    cvl__gl_context_t *c;
    int attrib[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
    XSetWindowAttributes swa;

    if (!display_name)
    {
	return NULL;
    }
    if (!(c = malloc(sizeof(cvl__gl_context_t))))
    {
	return NULL;
    }
    /* Open display */
    if (!(c->display = XOpenDisplay(display_name)))
    {
	free(c);
	return NULL;
    }
    /* Query for glx */
    if (!glXQueryExtension(c->display, NULL, NULL))
    {
	free(c);
	return NULL;
    }
    /* Choose visual */
    if (!(c->visualinfo = glXChooseVisual(c->display, DefaultScreen(c->display), attrib)))
    {
	free(c);
	return NULL;
    }
    /* Create context */
    if (!(c->context = glXCreateContext(c->display, c->visualinfo, None, True)))
    {
	free(c);
	return NULL;
    }
    /* Create window */
    c->colormap = XCreateColormap(c->display, RootWindow(c->display, c->visualinfo->screen), c->visualinfo->visual, AllocNone);
    swa.border_pixel = 0;
    swa.colormap = c->colormap;
    c->window = XCreateWindow(c->display, RootWindow(c->display, c->visualinfo->screen), 
	    0, 0, 1, 1, 0, c->visualinfo->depth, InputOutput, c->visualinfo->visual, 
	    CWBorderPixel | CWColormap, &swa);
    
    cvl_gl_context_make_current((cvl_gl_context_t *)c);
    return (cvl_gl_context_t *)c;

#endif
}

/**
 * \param ctx	The OpenGL context.
 *
 * Makes the OpenGL context \a ctx the current context. If you have multiple
 * contexts, yo must activate the right one before calling any CVL function.
 */
void cvl_gl_context_make_current(cvl_gl_context_t *ctx)
{
    cvl__gl_context_t *_ctx = (cvl__gl_context_t *)ctx;

#ifdef W32_NATIVE

    wglMakeCurrent(_ctx->hdc, _ctx->hglrc);

#else

    glXMakeCurrent(_ctx->display, _ctx->window, _ctx->context);

#endif
}

/**
 * \param context	The OpenGL context.
 *
 * Frees an OpenGL context that was created by cvl_gl_context_new().
 */
void cvl_gl_context_free(cvl_gl_context_t *context)
{
    cvl__gl_context_t *_context = (cvl__gl_context_t *)context;

#ifdef W32_NATIVE

    if (_context)
    {
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(_context->hglrc);
	ReleaseDC(_context->hwnd, _context->hdc);
	DestroyWindow(_context->hwnd);
	UnregisterClass("CVL", GetModuleHandle(NULL));
	free(_context);
    }

#else

    if (_context)
    {
	glXDestroyContext(_context->display, _context->context);
	XDestroyWindow(_context->display, _context->window);
	XFreeColormap(_context->display, _context->colormap);
	XFree(_context->visualinfo);
	XCloseDisplay(_context->display);
	free(_context);
    }

#endif
}


/**
 * \param what		The last executed action.
 * \param ...		Variable argument list.
 *
 * Checks the framebuffer status. The CVL error status is adjusted accordingly.
 * The \a what string is used for the error message which is of the form 
 * "what: errstr". Example: 
 * "Rendering object: framebuffer error INCOMPLETE_DRAW_BUFFER_EXT.".
 */
void cvl_gl_check_fb(const char *what, ...)
{
    cvl_assert(what != NULL);
    if (cvl_error())
	return;

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); 
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
	va_list args;
	char *str;
	const char *estr;
	switch(status) 
	{ 
	    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		estr = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT";
		break;

	    case GL_FRAMEBUFFER_UNSUPPORTED_EXT: 
		estr = "GL_FRAMEBUFFER_UNSUPPORTED_EXT";
		break; 
		
	    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: 
		estr = "FRAMEBUFFER_MISSING_ATTACHMENT_EXT";
		break; 
		
	    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: 
		estr = "FRAMEBUFFER_DIMENSIONS_EXT";
		break; 
		
	    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: 
		estr = "INCOMPLETE_FORMATS_EXT";
		break; 
		
	    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: 
		estr = "INCOMPLETE_DRAW_BUFFER_EXT";
		break; 
		
	    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: 
		estr = "INCOMPLETE_READ_BUFFER_EXT";
		break; 
		
	    default: 
		estr = NULL;
		break;
	}
	va_start(args, what);
	if (vasprintf(&str, what, args) < 0)
	    str = NULL;
	va_end(args);
	if (estr)
	    cvl_error_set(CVL_ERROR_GL, "%s: framebuffer error %s.", str, estr);
	else
	    cvl_error_set(CVL_ERROR_GL, "%s: framebuffer error 0x%X.", str, (unsigned int)status);
	free(str);
    }
}

/**
 * \param what		The last executed action.
 * \param ...		Variable argument list.
 *
 * Checks the OpenGL status. The CVL error status is adjusted accordingly.
 * The \a what string is used for the error message which is of the form 
 * "what: errstr". Example:
 * "Rendering object: framebuffer error INCOMPLETE_DRAW_BUFFER_EXT.".
 */
void cvl_gl_check_errors(const char *what, ...)
{
    cvl_assert(what != NULL);
    if (cvl_error())
	return;

    GLenum e = glGetError();
    if (e != GL_NO_ERROR)
    {
	char *str;
	va_list args;
    
	va_start(args, what);
	if (vasprintf(&str, what, args) < 0)
	    str = NULL;
	va_end(args);
	cvl_error_set(CVL_ERROR_GL, "%s: GL error 0x%X: %s", 
		str, (unsigned int)e, gluErrorString(e));
	free(str);
    }
}


static size_t cvl_kill_crlf(char *str)
{
    size_t l = strlen(str);
    if (l > 0 && str[l - 1] == '\n')
	str[--l] = '\0';
    if (l > 0 && str[l - 1] == '\r')
	str[--l] = '\0';
    return l;
}

static int cvl_skip_whitespace(const char *s, int i)
{
    while (isspace(s[i]))
	i++;
    return i;
}

/*
 * \param s	The string in which the replacements will be made.
 * \param a	The string that will be replaced.
 * \param b	The replacement for the string a.
 * \return	The resulting string.
 *
 * Replaces all occurences of \a a in \a s with \a b.
 * \a s must be an allocated string. Its new value is returned.
 * If memory allocation fails, NULL is returned.
 */
static char *cvl_str_replace(char *s, const char *a, const char *b)
{
    char *p;
    size_t next_pos = 0;
    size_t slen = strlen(s);
    size_t alen = strlen(a);
    size_t blen = strlen(b);

    if (slen == 0 || alen == 0)
	return s;
    while ((p = strstr(s + next_pos, a)))
    {
	size_t i = p - s;
	slen = slen - alen + blen;
	if (blen < alen)
	{
	    for (size_t j = i + blen; j < slen + 1; j++)
		s[j] = s[j + (alen - blen)];
	    if (!(s = realloc(s, (slen + 1) * sizeof(char))))
		return NULL;
	}
	else if (blen > alen)
	{
	    if (!(s = realloc(s, (slen + 1) * sizeof(char))))
		return NULL;
	    for (size_t j = slen; j >= i + blen - alen; j--)
		s[j] = s[j - (blen - alen)];
	}
	strncpy(s + i, b, blen);
	next_pos = i + blen;
    }
    return s;
}

/*
 * \param s		The string from which a part is copied.
 * \param i		The start index of the part.
 * \param l		The length of the part.
 *
 * Copies the the part from \a s that starts at index \a i and has length \a l
 * and returns it as an allocated string. If memory allocation fails, NULL is
 * returned.
 */
static char *cvl_str_part(const char *s, size_t i, size_t l)
{
    char *p;
    if (!(p = malloc((l + 1) * sizeof(char))))
	return NULL;
    strncpy(p, s + i, l);
    p[l] = '\0';
    return p;
}

/**
 * \param src		The shader source code.
 * \param defines	Variable substitutions for the source code.
 * \param ...		Variable argument list.
 *
 * Prepares a shader source code string by replacing compile time variables with
 * their values. Compile time variables in the source code start with a dollar
 * sign $. They are defined in the \a defines string, which is a comma separated
 * list of definitions. Example: "$kv=3, $kh=3, $sigma=0.625" would replace all
 * occurences of "$kv", "$kh", "$sigma" in \a src with the values 3, 3, 0.625
 * respectively. The string \a src must be allocated.
 */
char *cvl_gl_srcprep(char *src, const char *defines, ...)
{
    cvl_assert(src != NULL);
    cvl_assert(defines != NULL);
    if (cvl_error())
	return NULL;

    char *str;
    va_list args;
    
    va_start(args, defines);
    if (vasprintf(&str, defines, args) < 0)
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	return NULL;
    }
    va_end(args);

    int index = 0;
    for (;;)
    {
	index = cvl_skip_whitespace(str, index);
	if (str[index] == '\0')
	    break;
	if (str[index] != '$')
	{
	    cvl_error_set(CVL_ERROR_ASSERT, "%s(): variable name does not start with '$'", __func__);
	    free(str);
	    return NULL;
	}
	int name_start = index;
	index++;
	while (isalnum(str[index]) || str[index] == '_')
	    index++;
	int name_end = index - 1;
	char *name = cvl_str_part(str, name_start, name_end - name_start + 1);
	index = cvl_skip_whitespace(str, index);
	if (str[index] != '=')
	{
	    cvl_error_set(CVL_ERROR_ASSERT, "%s(): no '=' in variable definition", __func__);
	    free(str);
	    free(name);
	    return NULL;
	}
	index = cvl_skip_whitespace(str, index + 1);
	int value_start = index;
	while (isalnum(str[index]) || str[index] == '_')
	    index++;
	int value_end = index - 1;
	char *value = cvl_str_part(str, value_start, value_end - value_start + 1);
	if (!name || !value || !(src = cvl_str_replace(src, name, value)))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    free(str);
	    free(name);
	    free(value);
	    free(src);
	    return NULL;
	}
	free(name);
	free(value);
	index = cvl_skip_whitespace(str, index);
	if (str[index] == ',')
	    index++;
    }
    free(str);
    return src;
}

/**
 * \param name		The shader name.
 * \param type		GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 * \param src		The shader source code.
 * \return 		The shader object.
 *
 * Compiles a shader from its source code \a src. The shader \a name is only
 * used in error messages (if any). The \a type of the shader 
 * must be GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 */
GLuint cvl_gl_shader(const char *name, GLenum type, const char *src)
{
    cvl_assert(name != NULL);
    cvl_assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);
    cvl_assert(src != NULL);
    if (cvl_error())
	return 0;

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    char *log;
    GLint e, l;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &e);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &l);
    if (l > 0)
    {
	if ((log = malloc(l * sizeof(char))))
	{
	    glGetShaderInfoLog(shader, l, NULL, log);
	    l = cvl_kill_crlf(log);
	}
    }
    else
    {
	log = strdup("");
	l = 0;
    }

    if (e == GL_TRUE && l > 0)
    {
	/*
	cvl_error_set(CVL_ERROR_GL, "%s shader %s: compiler warning:\n%s", 
		type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", name, log);
	*/
    }
    else if (e != GL_TRUE)
    {
	cvl_error_set(CVL_ERROR_GL, "%s shader %s: compilation failed:\n%s", 
		type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", name,
		l > 0 ? log : "unknown error");
    }
    free(log);

    return shader;
}

/**
 * \param name		The program name.
 * \param vshader	The vertex shader, or 0.
 * \param fshader	The fragment shader, or 0.
 * \return 		The program object.
 *
 * Links a program usgin the shader objects \a vshader and \a fshader. One of \a
 * vshader or \a fshader may be 0 if no such shader should be used. The program 
 * \a name is only used in error messages (if any).
 */
GLuint cvl_gl_program_new(const char *name, GLuint vshader, GLuint fshader)
{
    cvl_assert(name != 0);
    cvl_assert(vshader != 0 || fshader != 0);
    if (cvl_error())
	return 0;

    GLuint program = glCreateProgram();
    if (vshader != 0)
	glAttachShader(program, vshader);
    if (fshader != 0)
	glAttachShader(program, fshader);
    glLinkProgram(program);

    char *log;
    GLint e, l;
    glGetProgramiv(program, GL_LINK_STATUS, &e);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &l);
    if (l > 0)
    {
	if ((log = malloc(l * sizeof(char))))
	{
	    glGetProgramInfoLog(program, l, NULL, log);
	    l = cvl_kill_crlf(log);
	}
    }
    else
    {
	log = strdup("");
	l = 0;
    }

    if (e == GL_TRUE && l > 0)
    {
	/*
	cvl_error_set(CVL_ERROR_GL, "Program %s: linker warning:\n%s", name, log);
	*/
    }
    else if (e != GL_TRUE)
    {
	cvl_error_set(CVL_ERROR_GL, "Program %s: linking failed:\n%s", 
		name, l > 0 ? log : "unknown error");
    }
    free(log);

    return program;
}

/**
 * \param name		The program name.
 * \param vshader_src	The source of the vertex shader, or NULL.
 * \param fshader_src	The source of the fragment shader, or NULL.
 * \return 		The program object.
 *
 * Convenience function that creates a program object directly from the shader
 * source codes. Only one of \a vshader_src and \a fshader_src may be NULL.
 * See also cvl_gl_program_new() and cvl_gl_shader().
 */
GLuint cvl_gl_program_new_src(const char *name, const char *vshader_src, const char *fshader_src)
{
    cvl_assert(name != 0);
    cvl_assert(vshader_src || fshader_src);
    if (cvl_error())
	return 0;

    GLuint vshader = 0, fshader = 0;
    if (vshader_src)
	vshader = cvl_gl_shader(name, GL_VERTEX_SHADER, vshader_src);
    if (fshader_src)
	fshader = cvl_gl_shader(name, GL_FRAGMENT_SHADER, fshader_src);
    return cvl_gl_program_new(name, vshader, fshader);
}

/**
 * \param program	The program object.
 *
 * Deletes the program and its associated shaders.
 */
void cvl_gl_program_free(GLuint program)
{
    if (cvl_error())
	return;

    if (glIsProgram(program))
    {
	GLint shader_count;
	GLuint *shaders;

	glGetProgramiv(program, GL_ATTACHED_SHADERS, &shader_count);
	if ((shaders = malloc(shader_count * sizeof(GLuint))))
	{
	    glGetAttachedShaders(program, shader_count, NULL, shaders);
	    for (int i = 0; i < shader_count; i++)
		glDeleteShader(shaders[i]);
	    free(shaders);
	    glDeleteProgram(program);
	}
	else
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	}
    }
}

/**
 * \param name		The program name.
 * \return 		The program, or NULL.
 *
 * Gets the program with the given \a name from the program cache. If there is
 * no program with the name in the cache, this function returns NULL.
 * See also cvl_gl_program_cache_put().
 */
GLuint cvl_gl_program_cache_get(const char *name)
{
    cvl_assert(name != NULL);
    if (cvl_error())
	return 0;

    cvl_context_t *ctx = cvl_context();
    int a = 0;
    int b = ctx->cvl_gl_program_cache_length - 1;
    while (b >= a)
    {
	int c = (a + b) / 2;
	int cmp = strcmp(ctx->cvl_gl_program_cache_names[c], name);
	if (cmp < 0)
	    a = c + 1;
	else if (cmp > 0)
	    b = c - 1;
	else
	    return ctx->cvl_gl_program_cache_values[c];
    }
    return 0;    
}

/**
 * \param name		The program name.
 * \param program	The program object.
 *
 * Puts the \a program with the given \a name into the program cache. The
 * program can later be retrieved with cvl_gl_program_cache_get().
 */
void cvl_gl_program_cache_put(const char *name, GLuint program)
{
    cvl_assert(name != NULL);
    if (cvl_error())
	return;

    cvl_context_t *ctx = cvl_context();
    int a = 0;
    int b = ctx->cvl_gl_program_cache_length - 1;
    while (b >= a)
    {
	int c = (a + b) / 2;
	int cmp = strcmp(ctx->cvl_gl_program_cache_names[c], name);
	if (cmp < 0)
	    a = c + 1;
	else if (cmp > 0)
	    b = c - 1;
	else
	{
	    cvl_gl_program_free(ctx->cvl_gl_program_cache_values[c]);
	    ctx->cvl_gl_program_cache_values[c] = program;
	    return;
	}
    }
    if (ctx->cvl_gl_program_cache_length == ctx->cvl_gl_program_cache_size)
    {
	ctx->cvl_gl_program_cache_size += 20;
	ctx->cvl_gl_program_cache_names = realloc(ctx->cvl_gl_program_cache_names, 
		ctx->cvl_gl_program_cache_size * sizeof(char *));
	ctx->cvl_gl_program_cache_values = realloc(ctx->cvl_gl_program_cache_values,
		ctx->cvl_gl_program_cache_size * sizeof(GLuint));
	if (!ctx->cvl_gl_program_cache_names || !ctx->cvl_gl_program_cache_values)
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    return;
	}
    }
    for (int i = ctx->cvl_gl_program_cache_length; i > a; i--)
    {
	ctx->cvl_gl_program_cache_names[i] = ctx->cvl_gl_program_cache_names[i - 1];
	ctx->cvl_gl_program_cache_values[i] = ctx->cvl_gl_program_cache_values[i - 1];
    }
    if (!(ctx->cvl_gl_program_cache_names[a] = strdup(name)))
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	return;
    }
    ctx->cvl_gl_program_cache_values[a] = program;
    ctx->cvl_gl_program_cache_length++;
}


/**
 * Saves the CVL GL state. It can later be restored with cvl_gl_state_restore().
 * Between both functions, you can change the GL state as you like, but you
 * cannot use any CVL functions.\n
 * You can use this to use your own GL code within a GL context
 * that is also used by CVL: encapsulate it into 
 * cvl_gl_state_save() and cvl_gl_state_restore().
 */
void cvl_gl_state_save(void)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    // Reset extensions to default values
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

/**
 * Restores the CVL GL state previously saved by cvl_gl_state_save().
 */
void cvl_gl_state_restore(void)
{
    // Restore pointer to CVL context
    glPopClientAttrib();
    // Reinitialize extensions
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, cvl_context()->cvl_gl_fbo);
    glPopAttrib();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
