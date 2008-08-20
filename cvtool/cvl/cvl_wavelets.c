/*
 * cvl_wavelets.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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
 * \file cvl_wavelets.h
 * \brief Functions to work with Wavelets.
 *
 * Functions to work with Wavelets.
 */

#include "config.h"

#include <stdlib.h>

#include "mh.h"

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_wavelets.h"

#include "glsl/wavelets/dwt_step1.glsl.h"
#include "glsl/wavelets/dwt_step2.glsl.h"
#include "glsl/wavelets/idwt_step1.glsl.h"
#include "glsl/wavelets/idwt_step2.glsl.h"
#include "glsl/wavelets/hard_thresholding.glsl.h"
#include "glsl/wavelets/soft_thresholding.glsl.h"


static void cvl_wavelets_dwt_helper(cvl_frame_t *pong, cvl_frame_t *ping, float level_boundary)
{
    /* This function works just like cvl_transform(pong, ping) except that it
     * covers only a subset of the ping and pong frames, defined by
     * level_boundary. */
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(pong));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(pong), 0);
    glViewport(0, 0, cvl_frame_width(pong), cvl_frame_height(pong));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(ping));
    cvl_gl_set_texture_state();
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(level_boundary, 0.0f);
    glVertex2f(2.0f * level_boundary - 1.0f, -1.0f);
    glTexCoord2f(level_boundary, level_boundary);
    glVertex2f(2.0f * level_boundary - 1.0f, 2.0f * level_boundary - 1.0f);
    glTexCoord2f(0.0f, level_boundary);
    glVertex2f(-1.0f, 2.0f * level_boundary - 1.0f);
    glEnd();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param tmp		A frame to store intermediate results.
 * \param D		The Daubechies wavelet to use.
 * \param level		The level.
 *
 * Performs a Discrete Wavelet Transform (DWT) on \a src and stores the result
 * in \a dst. The Daubechies wavelet can be selected from D2 (aka Haar wavelet)
 * to D20 (the parameter \a D must be even). Level 0 is the original image,
 * therefore the \a level parameter must be 1 or greater. The frame \a dst
 * should always be of type #CVL_FLOAT or #CVL_FLOAT16. The frame \a tmp
 * should have the same dimensions, type, and channels as \a dst.
 */
void cvl_wavelets_dwt(cvl_frame_t *dst, cvl_frame_t *src, cvl_frame_t *tmp, int D, int level)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(tmp != NULL);
    cvl_assert(dst != src);
    cvl_assert(dst != tmp);
    cvl_assert(src != tmp);
    cvl_assert(cvl_frame_type(dst) != CVL_UINT8);
    cvl_assert(cvl_frame_type(tmp) != CVL_UINT8);
    cvl_assert(cvl_frame_channels(tmp) == cvl_frame_channels(dst));
    cvl_assert(D >= 2 && D <= 20 && D % 2 == 0);
    cvl_assert(level >= 1);
    if (cvl_error())
	return;
    
    GLuint step1_prg;
    char *step1_prgname = cvl_asprintf("cvl_wavelets_dwt_step1_D=%d", D);
    if ((step1_prg = cvl_gl_program_cache_get(step1_prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_DWT_STEP1_GLSL_STR), "$D=%d", D);
	step1_prg = cvl_gl_program_new_src(step1_prgname, NULL, src);
	cvl_gl_program_cache_put(step1_prgname, step1_prg);
	free(src);
    }
    free(step1_prgname);
    GLuint step2_prg;
    char *step2_prgname = cvl_asprintf("cvl_wavelets_dwt_step2_D=%d", D);
    if ((step2_prg = cvl_gl_program_cache_get(step2_prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_DWT_STEP2_GLSL_STR), "$D=%d", D);
	step2_prg = cvl_gl_program_new_src(step2_prgname, NULL, src);
	cvl_gl_program_cache_put(step2_prgname, step2_prg);
	free(src);
    }
    free(step2_prgname);
    
    float xstep = 1.0f / (float)cvl_frame_width(src);
    float ystep = 1.0f / (float)cvl_frame_height(src);

    cvl_frame_t *ping = src;
    cvl_frame_t *pong = tmp;

    for (int l = 0; l < level; l++)
    {
	float level_boundary = 1.0f / (float)mh_powi(2, l);

	glUseProgram(step1_prg);
	glUniform1f(glGetUniformLocation(step1_prg, "xstep"), xstep);
	glUniform1f(glGetUniformLocation(step1_prg, "level_boundary"), level_boundary);
	cvl_wavelets_dwt_helper(pong, ping, level_boundary);

	ping = pong;
	pong = (ping == tmp ? dst : tmp);

	glUseProgram(step2_prg);
	glUniform1f(glGetUniformLocation(step2_prg, "ystep"), ystep);
	glUniform1f(glGetUniformLocation(step2_prg, "level_boundary"), level_boundary);
	cvl_wavelets_dwt_helper(pong, ping, level_boundary);

	ping = pong;
	pong = (ping == tmp ? dst : tmp);
    }

    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param tmp		A frame to store intermediate results.
 * \param D		The Daubechies wavelet to use.
 * \param level		The level.
 *
 * Performs an Inverse Discrete Wavelet Transform (IDWT) on \a src and stores the
 * result in \a dst. The parameters \a D and \a level must be the same that were
 * given to cvl_wavelets_dwt(). See also cvl_wavelets_dwt().
 */
void cvl_wavelets_idwt(cvl_frame_t *dst, cvl_frame_t *src, cvl_frame_t *tmp, int D, int level)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(tmp != NULL);
    cvl_assert(dst != src);
    cvl_assert(dst != tmp);
    cvl_assert(src != tmp);
    cvl_assert(cvl_frame_type(tmp) != CVL_UINT8);
    cvl_assert(cvl_frame_channels(tmp) == cvl_frame_channels(dst));
    cvl_assert(D >= 2 && D <= 20 && D % 2 == 0);
    cvl_assert(level >= 1);
    if (cvl_error())
	return;
    
    GLuint step1_prg;
    char *step1_prgname = cvl_asprintf("cvl_wavelets_idwt_step1_D=%d", D);
    if ((step1_prg = cvl_gl_program_cache_get(step1_prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_IDWT_STEP1_GLSL_STR), "$D=%d", D);
	step1_prg = cvl_gl_program_new_src(step1_prgname, NULL, src);
	cvl_gl_program_cache_put(step1_prgname, step1_prg);
	free(src);
    }
    free(step1_prgname);
    GLuint step2_prg;
    char *step2_prgname = cvl_asprintf("cvl_wavelets_idwt_step2_D=%d", D);
    if ((step2_prg = cvl_gl_program_cache_get(step2_prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_IDWT_STEP2_GLSL_STR), "$D=%d", D);
	step2_prg = cvl_gl_program_new_src(step2_prgname, NULL, src);
	cvl_gl_program_cache_put(step2_prgname, step2_prg);
	free(src);
    }
    free(step2_prgname);
    
    float texwidth = (float)cvl_frame_width(src);
    float texheight = (float)cvl_frame_height(src);

    cvl_frame_t *ping = src;
    cvl_frame_t *pong = tmp;
    
    for (int l = level - 1; l >= 0; l--)
    {
	float level_boundary = 1.0f / (float)mh_powi(2, l);

	glUseProgram(step1_prg);
	glUniform1f(glGetUniformLocation(step1_prg, "texwidth"), texwidth);
	glUniform1f(glGetUniformLocation(step1_prg, "texheight"), texheight);
	glUniform1f(glGetUniformLocation(step1_prg, "level_boundary"), level_boundary);
	cvl_wavelets_dwt_helper(pong, ping, level_boundary);

	ping = pong;
	pong = (ping == tmp ? dst : tmp);
	
	glUseProgram(step2_prg);
	glUniform1f(glGetUniformLocation(step2_prg, "texwidth"), texwidth);
	glUniform1f(glGetUniformLocation(step2_prg, "texheight"), texheight);
	glUniform1f(glGetUniformLocation(step2_prg, "level_boundary"), level_boundary);
	cvl_wavelets_dwt_helper(pong, ping, level_boundary);

	ping = pong;
	pong = (ping == tmp ? dst : tmp);
    }
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param D		The Daubechies wavelet to use.
 * \param level		The level.
 * \param T		The threshold (one value for each channel).
 *
 * Performs Hard Thresholding on \a src and stores the result in \a dst. The
 * source frame \a src must be the result of a previous call to cvl_wavelets_dwt() with
 * the same parameters \a D and \a level. See also cvl_wavelets_dwt().
 */
void cvl_wavelets_hard_thresholding(cvl_frame_t *dst, cvl_frame_t *src, int D, int level, const float *T)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(D >= 2 && D <= 20 && D % 2 == 0);
    cvl_assert(level >= 1);
    if (cvl_error())
	return;
    
    float threshold[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    for (int t = 0; t < cvl_frame_channels(src); t++)
    {
	threshold[t] = T[t];
    }
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_wavelets_hard_thresholding")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_wavelets_hard_thresholding", NULL, CVL_HARD_THRESHOLDING_GLSL_STR);
    	cvl_gl_program_cache_put("cvl_wavelets_hard_thresholding", prg);
    }
    glUseProgram(prg);
    glUniform4fv(glGetUniformLocation(prg, "T"), 1, threshold);
    cvl_transform(dst, src);
    cvl_check_errors();
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param D		The Daubechies wavelet to use.
 * \param level		The level.
 * \param T		The threshold (one value for each channel).
 *
 * Performs Soft Thresholding on \a src and stores the result in \a dst. The
 * source frame \a src must be the result of a previous call to cvl_wavelets_dwt() with
 * the same parameters \a D and \a level. See also cvl_wavelets_dwt().
 */
void cvl_wavelets_soft_thresholding(cvl_frame_t *dst, cvl_frame_t *src, int D, int level, const float *T)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(D >= 2 && D <= 20 && D % 2 == 0);
    cvl_assert(level >= 1);
    if (cvl_error())
	return;
    
    float threshold[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    for (int t = 0; t < cvl_frame_channels(src); t++)
    {
	threshold[t] = T[t];
    }
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_wavelets_soft_thresholding")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_wavelets_soft_thresholding", NULL, CVL_SOFT_THRESHOLDING_GLSL_STR);
    	cvl_gl_program_cache_put("cvl_wavelets_soft_thresholding", prg);
    }
    glUseProgram(prg);
    glUniform4fv(glGetUniformLocation(prg, "T"), 1, threshold);
    cvl_transform(dst, src);
    cvl_check_errors();
}
