/*
 * cvl_math.c
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
 * \file cvl_math.h
 * \brief Mathematical functions on frames.
 *
 * Mathematical functions on frames.
 */

#include "config.h"

#include <stdlib.h>

#include <GL/glew.h>

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_math.h"

#include "glsl/math/add.glsl.h"
#include "glsl/math/mul.glsl.h"
#include "glsl/math/first_derivative.glsl.h"
#include "glsl/math/laplacian.glsl.h"


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param summand	The summand (4 components).
 *
 * Adds the \a summand to the frame \a src and writes the result to \a dst. 
 * The summand must have 4 components (one for each channel).
 */
void cvl_add(cvl_frame_t *dst, cvl_frame_t *src, const float *summand)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(summand != NULL);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_add")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_add", NULL, CVL_ADD_GLSL_STR);
	cvl_gl_program_cache_put("cvl_add", prg);
    }
    glUseProgram(prg);
    glUniform4fv(glGetUniformLocation(prg, "summand"), 1, summand);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param factor	The factor (4 components).
 *
 * Multiplies the frame \a src with the \a factor and writes the result to 
 * \a dst. The factor must have 4 components (one for each channel).
 */
void cvl_mul(cvl_frame_t *dst, cvl_frame_t *src, const float *factor)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(factor != NULL);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_mul")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_mul", NULL, CVL_MUL_GLSL_STR);
	cvl_gl_program_cache_put("cvl_mul", prg);
    }
    glUseProgram(prg);
    glUniform4fv(glGetUniformLocation(prg, "factor"), 1, factor);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 *
 * Computes the first derivative of the given \a channel (0-3) in the 
 * frame \a src and writes the result to \a dst.\n
 * The frame \a dst must have at least two channels. The first channel contains
 * the first derivative in x direction, and the second channel in y
 * direction.
 */
void cvl_first_derivative(cvl_frame_t *dst, cvl_frame_t *src, int channel)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_channels(dst) >= 2);
    cvl_assert(channel >= 0 && channel < cvl_frame_channels(src));
    if (cvl_error())
	return;

    const char channel_names[] = { 'r', 'g', 'b', 'a'  };

    GLuint prg;
    char *prgname = cvl_asprintf("cvl_first_derivative_channel=%c", channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_FIRST_DERIVATIVE_GLSL_STR), 
		"$channel=%c", channel_names[channel]);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "hstep"), 1.0f / (float)cvl_frame_width(src));
    glUniform1f(glGetUniformLocation(prg, "vstep"), 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 *
 * Computes the laplacian of the given \a channel (0-3) in the given
 * frame \a src and writes the result to the first channel of \a dst.
 */
void cvl_laplacian(cvl_frame_t *dst, cvl_frame_t *src, int channel)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= 0 && channel < cvl_frame_channels(src));
    if (cvl_error())
	return;

    const char channel_names[] = { 'r', 'g', 'b', 'a'  };

    GLuint prg;
    char *prgname = cvl_asprintf("cvl_laplacian_channel=%c", channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_LAPLACIAN_GLSL_STR), 
		"$channel=%c", channel_names[channel]);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "hstep"), 1.0f / (float)cvl_frame_width(src));
    glUniform1f(glGetUniformLocation(prg, "vstep"), 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, src);
    cvl_check_errors();
}
