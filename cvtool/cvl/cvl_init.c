/*
 * cvl_init.c
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

/** 
 * \file cvl_init.h
 * \brief Library initialization.
 *
 * Library initialization.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>

#define CVL_BUILD
#include "cvl_intern.h"
#include "cvl/cvl.h"


/* This CVL context is only used if no real context could be allocated. It then
 * allows CVL to return an error status. */
cvl_context_t cvl_fallback_context;


/**
 * Initializes CVL, using the currently active OpenGL context.\n
 * After calling this function, the application can use cvl_error() and
 * cvl_error_msg() to query the CVL error status.\n
 * Initialization includes correct setup of all OpenGL extensions (via GLEW).
 */
void cvl_init(void)
{
    const char *gl_extension_list[] =
    {
	"GL_ARB_texture_non_power_of_two",
	"GL_ARB_texture_float",
	"GL_EXT_framebuffer_object",
	"GL_ARB_fragment_shader",
	/*"GL_EXT_gpu_shader4",*/
	/*"GL_ARB_shading_language_120",*/
	"GL_ARB_multitexture",
	"GL_ARB_draw_buffers"
    };
    const size_t gl_extension_list_len = 
	sizeof(gl_extension_list) / sizeof(gl_extension_list[0]);
    cvl_context_t *ctx;

    /* Allocate the CVL context and store it in the GL context */
    if (!(ctx = malloc(sizeof(cvl_context_t))))
    {
	ctx = &cvl_fallback_context;
	glIndexPointer(GL_INT, 0, ctx);
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	return;
    }
    glIndexPointer(GL_INT, 0, ctx);

    /* Initialize CVL context */
    ctx->error = CVL_OK;
    ctx->error_msg = NULL;
    ctx->cvl_gl_fbo = 0;
    ctx->cvl_gl_fbo_initialized = false;
    ctx->cvl_gl_std_quad = 0;
    ctx->cvl_gl_std_quad_initialized = false;
    ctx->cvl_gl_program_cache_length = 0;
    ctx->cvl_gl_program_cache_size = 0;
    ctx->cvl_gl_program_cache_names = NULL;
    ctx->cvl_gl_program_cache_values = NULL;

    /* Check GL version and extensions */
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
	cvl_error_set(CVL_ERROR_GL, "Cannot initialize GLEW: %s", glewGetErrorString(err));
	return;
    }
    for (size_t i = 0; i < gl_extension_list_len; i++)
    {
	if (!glewIsSupported(gl_extension_list[i]))
	{
	    cvl_error_set(CVL_ERROR_GL, "OpenGL extension %s is not available", gl_extension_list[i]);
	    return;
	}
    }

    /* Initialize table of texture formats. */
    // type 0 is for 8bit integer, type 1 is for float, type 2 is for half.
    if (glewIsSupported("GL_ARB_texture_rg"))
    {
	ctx->cvl_gl_texture_formats[0][0] = GL_R8;
	ctx->cvl_gl_texture_formats[0][1] = GL_RG8;
	ctx->cvl_gl_texture_formats[0][2] = GL_RGB;
	ctx->cvl_gl_texture_formats[0][3] = GL_RGBA;
	ctx->cvl_gl_texture_formats[1][0] = GL_R32F;
	ctx->cvl_gl_texture_formats[1][1] = GL_RG32F;
	ctx->cvl_gl_texture_formats[1][2] = GL_RGB32F_ARB;
	ctx->cvl_gl_texture_formats[1][3] = GL_RGBA32F_ARB;
	ctx->cvl_gl_texture_formats[2][0] = GL_R16F;
	ctx->cvl_gl_texture_formats[2][1] = GL_RG16F;
	ctx->cvl_gl_texture_formats[2][2] = GL_RGB16F_ARB;
	ctx->cvl_gl_texture_formats[2][3] = GL_RGBA16F_ARB;
    }
    else
    {
	ctx->cvl_gl_texture_formats[0][0] = GL_RGB;
	ctx->cvl_gl_texture_formats[0][1] = GL_RGB;
	ctx->cvl_gl_texture_formats[0][2] = GL_RGB;
	ctx->cvl_gl_texture_formats[0][3] = GL_RGBA;
	ctx->cvl_gl_texture_formats[1][0] = GL_RGB32F_ARB;
	ctx->cvl_gl_texture_formats[1][1] = GL_RGB32F_ARB;
	ctx->cvl_gl_texture_formats[1][2] = GL_RGB32F_ARB;
	ctx->cvl_gl_texture_formats[1][3] = GL_RGBA32F_ARB;
	ctx->cvl_gl_texture_formats[2][0] = GL_RGB16F_ARB;
	ctx->cvl_gl_texture_formats[2][1] = GL_RGB16F_ARB;
	ctx->cvl_gl_texture_formats[2][2] = GL_RGB16F_ARB;
	ctx->cvl_gl_texture_formats[2][3] = GL_RGBA16F_ARB;
    }

    /* Create framebuffer object */
    glGenFramebuffersEXT(1, &(ctx->cvl_gl_fbo));
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, ctx->cvl_gl_fbo);
    ctx->cvl_gl_fbo_initialized = true;
    
    /* Create 1:1 standard quad */
    const GLfloat quaddata[] =
    {
    	// 4 x TexCoord2f
	 0.0f,  0.0f, +1.0f,  0.0f, +1.0f, +1.0f,  0.0f, +1.0f,
	// 4 x Vertex3f
	-1.0f, -1.0f,  0.0f, +1.0f, -1.0f,  0.0f, +1.0f, +1.0f,  0.0f, -1.0f, +1.0f,  0.0f
    };
    glGenBuffersARB(1, &(ctx->cvl_gl_std_quad));
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, ctx->cvl_gl_std_quad);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 4 * (2 + 3) * sizeof(float),
  	    quaddata, GL_STATIC_DRAW_ARB);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    glVertexPointer(3, GL_FLOAT, 0, (const GLvoid *)(8 * sizeof(float)));
    ctx->cvl_gl_std_quad_initialized = true;

    /* Set texture state */
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DITHER);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    /* Set memory<->texture conversion parameters */
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Misc */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLuint draw_buffers[1] = { GL_COLOR_ATTACHMENT0_EXT };
    glDrawBuffers(1, draw_buffers);

    /* Get limitation values */
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(ctx->cvl_gl_max_tex_size));
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(ctx->cvl_gl_max_texture_units));
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(ctx->cvl_gl_max_render_targets));

    /* Initialize program cache */
    ctx->cvl_gl_program_cache_length = 0;
    ctx->cvl_gl_program_cache_size = 0;
    ctx->cvl_gl_program_cache_names = NULL;
    ctx->cvl_gl_program_cache_values = NULL;

    /* Check */
    cvl_check_errors();
}


/**
 * Deinitializes CVL and frees all allocated ressources.
 */
void cvl_deinit(void)
{
    cvl_context_t *ctx = cvl_context();
    if (ctx != &cvl_fallback_context)
    {
	free(ctx->error_msg);
	if (ctx->cvl_gl_fbo_initialized)
	    glDeleteFramebuffersEXT(1, &(ctx->cvl_gl_fbo));
	if (ctx->cvl_gl_std_quad_initialized)
	    glDeleteBuffersARB(1, &(ctx->cvl_gl_std_quad));
	for (int i = 0; i < ctx->cvl_gl_program_cache_length; i++)
	{
	    free(ctx->cvl_gl_program_cache_names[i]);
	    cvl_gl_program_free(ctx->cvl_gl_program_cache_values[i]);
	}
	free(ctx->cvl_gl_program_cache_names);
	free(ctx->cvl_gl_program_cache_values);
	free(ctx);
    }
}

/**
 * \brief       Get the libcvl version.
 * \param major Buffer for the major version number, or NULL.
 * \param minor Buffer for the minor version number, or NULL.
 * \param patch Buffer for the patch version number, or NULL.
 * \return      The libcvl version string.
 *
 * This function returns the version string "MAJOR.MINOR.PATCH".
 * If the pointers \a major, \a minor, \a patch are not NULL,
 * the requested version number will be stored there.
 */
const char *cvl_version(int *major, int *minor, int *patch)
{
    if (major)
    {
        *major = CVL_VERSION_MAJOR;
    }
    if (minor)
    {
        *minor = CVL_VERSION_MINOR;
    }
    if (patch)
    {
        *patch = CVL_VERSION_PATCH;
    }
    return CVL_VERSION;
}
