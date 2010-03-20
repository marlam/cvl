/*
 * cvl_features.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
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
 * \file cvl_features.h
 * \brief Detect features in frames.
 *
 * Detect features in frames.
 */

#include "config.h"

#include <stdlib.h>

#include <GL/glew.h>

#define CVL_BUILD
#include "cvl_intern.h"
#include "cvl/cvl.h"

#include "glsl/features/sobel.glsl.h"
#include "glsl/features/canny_nms.glsl.h"
#include "glsl/features/canny_hysterese1.glsl.h"
#include "glsl/features/canny_hysterese2.glsl.h"


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 *
 * Detects edges in channel \a channel (0-3) of the frame \a src using the Sobel edge detector.
 * The first channel of \a dst will contain the edge strength (from [0,1]) and the second 
 * channel storing the edge directions (from [-PI, +PI]).
 */
void cvl_edge_sobel(cvl_frame_t *dst, cvl_frame_t *src, int channel)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= 0 && channel < cvl_frame_channels(src));
    if (cvl_error())
	return;

    const char *channel_names[] = { "r", "g", "b", "a" };
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_edge_sobel_channel=%s", channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_SOBEL_GLSL_STR),
		"$channel=%s", channel_names[channel]);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)cvl_frame_width(src));
    glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * 
 * Performs Canny Non-Maximum Suppression on the edge frame \a src. The input
 * should result from a call to cvl_edge_sobel().
 */
void cvl_edge_canny_nms(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_edge_canny_nms")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_edge_canny_nms", NULL, CVL_CANNY_NMS_GLSL_STR);
	cvl_gl_program_cache_put("cvl_edge_canny_nms", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)cvl_frame_width(src));
    glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param tl		Lower threshold.
 * \param th		Higher threshold.
 * 
 * Performs Canny Hysterese Thresholding on the input edge frame \a src and
 * writes the result to \a dst as a binary edge map. Edge points with a strength greater
 * than or equal to \a th are immediately accepted as edge points. Edge points 
 * with a strength lower than \a tl are immediately rejected. Edge points with
 * a strength between \a tl and \a th are accepted if there is a path of edge
 * points with a strength greater than or equal to \a tl that connects them with
 * an edge point of strength grater than or equal to \a th.\n
 * The input edge data should result from a call to cvl_edge_canny_nms(). 
 */
void cvl_edge_canny_hysterese(cvl_frame_t *dst, cvl_frame_t *src, float tl, float th)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(tl >= 0.0f && tl <= 1.0f);
    cvl_assert(th >= 0.0f && th <= 1.0f);
    cvl_assert(tl <= th);
    if (cvl_error())
	return;

    GLuint prg;
    GLint query_bits;
    GLuint query;
    GLuint samples_passed;
    cvl_frame_t *frame1, *frame2;

    glGetQueryiv(GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &query_bits);
    if (query_bits < 32)
    {
	cvl_error_set(CVL_ERROR_GL, "Need 32 bits for occlusion query counter, "
		"but the OpenGL implementation provides only %d", (int)query_bits);
	return;
    }
    glGenQueries(1, &query);
    if ((prg = cvl_gl_program_cache_get("cvl_edge_canny_hysterese1")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_edge_canny_hysterese1", NULL, CVL_CANNY_HYSTERESE1_GLSL_STR);
	cvl_gl_program_cache_put("cvl_edge_canny_hysterese1", prg);
    }
    frame1 = cvl_frame_new_tpl(src);
    frame2 = cvl_frame_new_tpl(src);
    cvl_copy(frame1, src);
    do
    {
	cvl_copy(frame2, frame1);
	glUseProgram(prg);
	glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)cvl_frame_width(src));
	glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)cvl_frame_height(src));
	glUniform1f(glGetUniformLocation(prg, "tl"), tl);
	glUniform1f(glGetUniformLocation(prg, "th"), th);
	glBeginQuery(GL_SAMPLES_PASSED, query);
	cvl_transform(frame2, frame1);
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samples_passed);
	cvl_frame_t *tmp = frame1;
	frame1 = frame2;
	frame2 = tmp;
    }
    while (samples_passed > 0);
    cvl_frame_free(frame2);

    if ((prg = cvl_gl_program_cache_get("cvl_edge_canny_hysterese2")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_edge_canny_hysterese2", NULL, CVL_CANNY_HYSTERESE2_GLSL_STR);
	cvl_gl_program_cache_put("cvl_edge_canny_hysterese2", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, frame1);
    cvl_frame_free(frame1);

    glDeleteQueries(1, &query);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The channel.
 * \param sigma		The sigma value for gauss filtering.
 * \param tl		The tl parameter of Hysterese binarization.
 * \param th		The th parameter of Hysterese binarization.
 * 
 * Detects edges in channel \a channel (0-3) of the frame \a src using the Canny edge detector.
 * The Canny edge detection algorithm consists of the following steps:
 * Gauss filtering with parameter \a sigma, Sobel edge detector, non-maximum
 * suppression, and binarization with the Hysterese method using the parameters
 * \a tl and \a th (both from [0,1]).\n
 * The result will be written to \a dst, with the first channel
 * storing the edge strength (from [0,1]) and the second channel storing the
 * edge directions (from [-PI, +PI]).
 */
void cvl_edge_canny(cvl_frame_t *dst, cvl_frame_t *src, int channel, float sigma, float tl, float th)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= 0 && channel <= 3);
    cvl_assert(sigma > 0.0f);
    cvl_assert(tl >= 0.0f && tl <= 1.0f);
    cvl_assert(th >= 0.0f && th <= 1.0f);
    cvl_assert(tl <= th);
    if (cvl_error())
	return; 

    cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
    cvl_gauss(tmpframe, src, cvl_gauss_sigma_to_k(sigma), cvl_gauss_sigma_to_k(sigma), sigma, sigma);
    cvl_edge_sobel(dst, tmpframe, channel);
    cvl_edge_canny_nms(tmpframe, dst);
    cvl_edge_canny_hysterese(dst, tmpframe, tl, th);
    cvl_frame_free(tmpframe);
}
