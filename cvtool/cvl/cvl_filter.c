/*
 * cvl_filter.c
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
 * \file cvl_filter.h
 * \brief Filtering frames.
 *
 * Filtering frames.
 */

#include "config.h"

#include <stdlib.h>

#include <GL/glew.h>

#define CVL_BUILD
#include "cvl_intern.h"
#include "cvl/cvl.h"

#include "glsl/filter/convolve.glsl.h"
#include "glsl/filter/convolve_separable.glsl.h"
#include "glsl/filter/convolve3d.glsl.h"
#include "glsl/filter/convolve3d_separable.glsl.h"
#include "glsl/filter/min.glsl.h"
#include "glsl/filter/min3d.glsl.h"
#include "glsl/filter/max.glsl.h"
#include "glsl/filter/max3d.glsl.h"
#include "glsl/filter/median.glsl.h"
#include "glsl/filter/median3d.glsl.h"
#include "glsl/filter/median_separated.glsl.h"
#include "glsl/filter/median3d_separated.glsl.h"
#include "glsl/filter/laplace.glsl.h"
#include "glsl/filter/unsharpmask.glsl.h"


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param kernel	The convolution kernel as an array.
 * \param h_len		Number of columns of the kernel.
 * \param v_len		Number of rows of the kernel.
 * 
 * Applies a convolution kernel to a frame. Both \a h_len and \a v_len must be
 * odd.
 */
void cvl_convolve(cvl_frame_t *dst, cvl_frame_t *src, const float *kernel, int h_len, int v_len)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(kernel != NULL);
    cvl_assert(h_len > 0);
    cvl_assert(h_len % 2 == 1);
    cvl_assert(v_len > 0);
    cvl_assert(v_len % 2 == 1);
    if (cvl_error())
	return;
    
    int k_h = h_len / 2;
    int k_v = v_len / 2;
    float factor = 0.0f;
    for (int i = 0; i < h_len * v_len; i++)
	factor += kernel[i];
    factor = 1.0f / factor;

    GLuint prg;
    char *prgname = cvl_asprintf("cvl_convolve_hk=%d_vk=%d", k_h, k_v);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_CONVOLVE_GLSL_STR), "$hk=%d, $vk=%d", k_h, k_v);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "hstep"), 1.0f / (float)cvl_frame_width(src));
    glUniform1f(glGetUniformLocation(prg, "vstep"), 1.0f / (float)cvl_frame_height(src));
    glUniform1fv(glGetUniformLocation(prg, "kernel"), v_len * h_len, kernel);
    glUniform1f(glGetUniformLocation(prg, "factor"), factor);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param h		The horizontal part of the separable convolution matrix.
 * \param h_len		The length of the horizontal part.
 * \param v		The vertical part of the separable convolution matrix.
 * \param v_len		The length of the vertical part.
 * 
 * Applies a separable convolution matrix to a frame.
 * Both \a h_len and \a v_len must be odd numbers.
 */
void cvl_convolve_separable(cvl_frame_t *dst, cvl_frame_t *src, 
	const float *h, int h_len, const float *v, int v_len)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(h != NULL);
    cvl_assert(h_len > 0);
    cvl_assert(h_len % 2 == 1);
    cvl_assert(v != NULL);
    cvl_assert(v_len > 0);
    cvl_assert(v_len % 2 == 1);
    if (cvl_error())
	return;
    
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
    int k_h = h_len / 2;
    int k_v = v_len / 2;
    float factor_h = 0.0f;
    for (int i = 0; i < h_len; i++)
	factor_h += h[i];
    factor_h = 1.0f / factor_h;
    float factor_v = 0.0f;
    for (int i = 0; i < v_len; i++)
	factor_v += v[i];
    factor_v = 1.0f / factor_v;
    
    /* h */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_convolve_separable_k=%d", k_h);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_CONVOLVE_SEPARABLE_GLSL_STR), "$k=%d", k_h);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 1.0f / (float)cvl_frame_width(src), 0.0f);
    glUniform1fv(glGetUniformLocation(prg, "mask"), h_len, h);
    glUniform1f(glGetUniformLocation(prg, "factor"), factor_h);
    cvl_transform(tmpframe, src);
    /* v */
    prgname = cvl_asprintf("cvl_convolve_separable_k=%d", k_v);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_CONVOLVE_SEPARABLE_GLSL_STR), "$k=%d", k_v);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 0.0f, 1.0f / (float)cvl_frame_height(src));
    glUniform1fv(glGetUniformLocation(prg, "mask"), v_len, v);
    glUniform1f(glGetUniformLocation(prg, "factor"), factor_v);
    cvl_transform(dst, tmpframe);

    cvl_frame_free(tmpframe);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param kernel	The convolution kernel as an array of ints.
 * \param h_len		Dimension in horizontal direction.
 * \param v_len		Dimension in vertical direction.
 * \param t_len		Dimension in temporal direction.
 * 
 * Applies a 3D convolution matrix on a frame.
 * The number of frames in the array \a srcs must be \a t_len. Only the entry
 * at (\a t_len / 2) must be non-NULL, because this is the frame that will be
 * processed. The number of past and future frames (those with indices less than or
 * greater than \a t_len / 2) can be limited; in this case, some array entries
 * can be NULL. This function will use clamping in the t direction to compensate that.
 * The dimensions \a h_len, \a v_len, \a t_len must all be odd.
 */
void cvl_convolve3d(cvl_frame_t *dst, cvl_frame_t **srcs, 
	const float *kernel, int h_len, int v_len, int t_len)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(kernel != NULL);
    cvl_assert(h_len > 0);
    cvl_assert(h_len % 2 == 1);
    cvl_assert(v_len > 0);
    cvl_assert(v_len % 2 == 1);
    cvl_assert(t_len > 0);
    cvl_assert(t_len % 2 == 1);
    cvl_assert(srcs[t_len / 2] != NULL);
    cvl_assert(srcs[t_len / 2] != dst);
    if (cvl_error())
	return;

    cvl_frame_t *framebuf[t_len];
    int k_h = h_len / 2;
    int k_v = v_len / 2;
    int k_t = t_len / 2;
    float factor = 0.0f;
    for (int i = 0; i < h_len * v_len * t_len; i++)
	factor += kernel[i];
    factor = 1.0f / factor;
    
    // Use clamping to fill the frame buffer
    int first_known = -1, last_known = -1;
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	{
	    if (first_known < 0)
		first_known = i;
	    if (last_known < i)
		last_known = i;
	}
    }
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	    framebuf[i] = srcs[i];
	else if (i < first_known)
	    framebuf[i] = srcs[first_known];
	else
	    framebuf[i] = srcs[last_known];
    }

    GLuint prg;
    char *prgname = cvl_asprintf("cvl_convolve3d_k_h=%d_k_v=%d_k_t=%d", k_h, k_v, k_t);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_CONVOLVE3D_GLSL_STR),
		"$k_h=%d, $k_v=%d, $k_t=%d", k_h, k_v, k_t);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1fv(glGetUniformLocation(prg, "kernel"), h_len * v_len * t_len, kernel);
    glUniform1f(glGetUniformLocation(prg, "factor"), factor);
    cvl_transform_multi(&dst, 1, framebuf, t_len, "textures");
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param h		The horizontal part of the separable convolution kernel.
 * \param h_len		The length of the horizontal part.
 * \param v		The vertical part of the separable convolution kernel.
 * \param v_len		The length of the vertical part.
 * \param t		The temporal part of the separable convolution matrix.
 * \param t_len		The length of the temporal part.
 * 
 * Applies a separable 3D convolution matrix on a frame.
 * The number of frames in the array \a srcs must be \a t_len. Only the entry
 * at (\a t_len / 2) must be non-NULL, because this is the frame that will be
 * processed. The number of past and future frames (those with indices less than or
 * greater than \a t_len / 2) can be limited; in this case, some array entries
 * can be NULL. This function will use clamping in the t direction to compensate that.
 * The dimensions \a h_len, \a v_len, \a t_len must all be odd.
 */
void cvl_convolve3d_separable(cvl_frame_t *dst, cvl_frame_t **srcs, 
	const float *h, int h_len, const float *v, int v_len, const float *t, int t_len)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(h != NULL);
    cvl_assert(h_len > 0);
    cvl_assert(h_len % 2 == 1);
    cvl_assert(v != NULL);
    cvl_assert(v_len > 0);
    cvl_assert(v_len % 2 == 1);
    cvl_assert(t != NULL);
    cvl_assert(t_len > 0);
    cvl_assert(t_len % 2 == 1);
    cvl_assert(srcs[t_len / 2] != NULL);
    cvl_assert(srcs[t_len / 2] != dst);
    if (cvl_error())
	return;

    cvl_frame_t *framebuf[t_len];
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(srcs[t_len / 2]);

    int k_t = t_len / 2;
    float factor_t = 0.0f;
    for (int i = 0; i < t_len; i++)
	factor_t += t[i];
    factor_t = 1.0f / factor_t;
    
    // Use clamping to fill the frame buffer
    int first_known = -1, last_known = -1;
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	{
	    if (first_known < 0)
		first_known = i;
	    if (last_known < i)
		last_known = i;
	}
    }
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	    framebuf[i] = srcs[i];
	else if (i < first_known)
	    framebuf[i] = srcs[first_known];
	else
	    framebuf[i] = srcs[last_known];
    }

    /* t */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_convolve3d_separable_k_t=%d", k_t);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_CONVOLVE3D_SEPARABLE_GLSL_STR), "$k_t=%d", k_t);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1fv(glGetUniformLocation(prg, "mask_t"), t_len, t);
    glUniform1f(glGetUniformLocation(prg, "factor_t"), factor_t);
    cvl_transform_multi(&tmpframe, 1, framebuf, t_len, "textures");
    cvl_check_errors();

    /* h, v */
    cvl_convolve_separable(dst, tmpframe, h, h_len, v, v_len);
    cvl_frame_free(tmpframe);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 *
 * Applies Mean filtering to a frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1.
 */
void cvl_mean(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    if (cvl_error())
	return;
    
    float m_h[2 * k_h + 1];
    for (int i = 0; i < 2 * k_h + 1; i++)
	m_h[i] = 1.0f;
    float m_v[2 * k_v + 1];
    for (int i = 0; i < 2 * k_v + 1; i++)
	m_v[i] = 1.0f;
    
    cvl_convolve_separable(dst, src, m_h, 2 * k_h + 1, m_v, 2 * k_v + 1);
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 *
 * Applies Mean filtering.
 * See cvl_convolve3d_separable() for a description of \a srcs.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
void cvl_mean3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(k_t >= 0);
    cvl_assert(srcs[k_t] != NULL);
    cvl_assert(srcs[k_t] != dst);
    if (cvl_error())
	return;

    float m_h[2 * k_h + 1];
    for (int i = 0; i < 2 * k_h + 1; i++)
	m_h[i] = 1.0f;
    float m_v[2 * k_v + 1];
    for (int i = 0; i < 2 * k_v + 1; i++)
	m_v[i] = 1.0f;
    float m_t[2 * k_t + 1];
    for (int i = 0; i < 2 * k_t + 1; i++)
	m_t[i] = 1.0f;

    cvl_convolve3d_separable(dst, srcs, m_h, 2 * k_h + 1, m_v, 2 * k_v + 1, m_t, 2 * k_t + 1);
}


/**
 * \param k		The parameter k of cvl_gauss()
 * \return		Sigma.
 *
 * Computes a sigma value that matches the given \a k: roughly 95% of the
 * mass should lie within the mask.
 */
float cvl_gauss_k_to_sigma(int k)
{
    return (float)k / 2.5f;
}

/**
 * \param sigma		The parameter sigma of cvl_gauss()
 * \return		k.
 *
 * Computes a k value that matches the given \a sigma: roughly 95% of the
 * mass should lie within the mask.
 */
int cvl_gauss_sigma_to_k(float sigma)
{
    return cvl_maxi(1, cvl_iroundf(2.5f * sigma));
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param sigma_h	Sigma value in horizontal direction.
 * \param sigma_v	Sigma value in vertical direction.
 *
 * Applies Gauss filtering to a frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1. See also cvl_gauss_k_to_sigma() and cvl_gauss_sigma_to_k().
 */
void cvl_gauss(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v, float sigma_h, float sigma_v)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(sigma_h > 0.0f);
    cvl_assert(sigma_v > 0.0f);
    if (cvl_error())
	return;
    
    float m_h[2 * k_h + 1];
    cvl_gauss_mask(k_h, sigma_h, m_h, NULL);
    float m_v[2 * k_v + 1];
    cvl_gauss_mask(k_v, sigma_v, m_v, NULL);
    
    cvl_convolve_separable(dst, src, m_h, 2 * k_h + 1, m_v, 2 * k_v + 1);
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * \param sigma_h	Sigma value in horizontal direction.
 * \param sigma_v	Sigma value in vertical direction.
 * \param sigma_t	Sigma value in temporal direction.
 *
 * Applies Gauss filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 * See also cvl_gauss_k_to_sigma() and cvl_gauss_sigma_to_k().
 */
void cvl_gauss3d(cvl_frame_t *dst, cvl_frame_t **srcs, 
	int k_h, int k_v, int k_t, float sigma_h, float sigma_v, float sigma_t)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(k_t >= 0);
    cvl_assert(sigma_h > 0.0);
    cvl_assert(sigma_v > 0.0);
    cvl_assert(sigma_t > 0.0);
    cvl_assert(srcs[k_t] != NULL);
    cvl_assert(srcs[k_t] != dst);
    if (cvl_error())
	return;

    float m_h[2 * k_h + 1];
    cvl_gauss_mask(k_h, sigma_h, m_h, NULL);
    float m_v[2 * k_v + 1];
    cvl_gauss_mask(k_v, sigma_v, m_v, NULL);
    float m_t[2 * k_t + 1];
    cvl_gauss_mask(k_t, sigma_t, m_t, NULL);

    cvl_convolve3d_separable(dst, srcs, m_h, 2 * k_h + 1, m_v, 2 * k_v + 1, m_t, 2 * k_t + 1);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 *
 * Applies Minimum filtering to a frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1.
 */
void cvl_min(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    if (cvl_error())
	return;
    
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
    
    /* h */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_min_k=%d", k_h);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MIN_GLSL_STR), "$k=%d", k_h);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 1.0f / (float)cvl_frame_width(src), 0.0f);
    cvl_transform(tmpframe, src);
    /* v */
    prgname = cvl_asprintf("cvl_min_k=%d", k_v);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MIN_GLSL_STR), "$k=%d", k_v);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 0.0f, 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, tmpframe);

    cvl_frame_free(tmpframe);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * 
 * Applies Minimum filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
void cvl_min3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(k_t >= 0);
    cvl_assert(srcs[k_t] != NULL);
    cvl_assert(srcs[k_t] != dst);
    if (cvl_error())
	return;

    int t_len = 2 * k_t + 1;
    cvl_frame_t *framebuf[2 * k_t + 1];
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(srcs[k_t]);

    // Use clamping to fill the frame buffer
    int first_known = -1, last_known = -1;
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	{
	    if (first_known < 0)
		first_known = i;
	    if (last_known < i)
		last_known = i;
	}
    }
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	    framebuf[i] = srcs[i];
	else if (i < first_known)
	    framebuf[i] = srcs[first_known];
	else
	    framebuf[i] = srcs[last_known];
    }

    /* t */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_min3d_k_t=%d", k_t);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MIN3D_GLSL_STR), "$k_t=%d", k_t);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    cvl_transform_multi(&tmpframe, 1, framebuf, t_len, "textures");
    cvl_check_errors();

    /* h, v */
    cvl_min(dst, tmpframe, k_h, k_v);
    cvl_frame_free(tmpframe);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 *
 * Applies Maximum filtering to a frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1.
 */
void cvl_max(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    if (cvl_error())
	return;

    cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
    
    /* h */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_max_k=%d", k_h);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MAX_GLSL_STR), "$k=%d", k_h);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 1.0f / (float)cvl_frame_width(src), 0.0f);
    cvl_transform(tmpframe, src);
    /* v */
    prgname = cvl_asprintf("cvl_max_k=%d", k_v);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MAX_GLSL_STR), "$k=%d", k_v);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 0.0f, 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, tmpframe);

    cvl_frame_free(tmpframe);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * 
 * Applies Maximum filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
void cvl_max3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(k_t >= 0);
    cvl_assert(srcs[k_t] != NULL);
    cvl_assert(srcs[k_t] != dst);
    if (cvl_error())
	return;

    int t_len = 2 * k_t + 1;
    cvl_frame_t *framebuf[2 * k_t + 1];
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(srcs[k_t]);

    // Use clamping to fill the frame buffer
    int first_known = -1, last_known = -1;
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	{
	    if (first_known < 0)
		first_known = i;
	    if (last_known < i)
		last_known = i;
	}
    }
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	    framebuf[i] = srcs[i];
	else if (i < first_known)
	    framebuf[i] = srcs[first_known];
	else
	    framebuf[i] = srcs[last_known];
    }

    /* t */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_max3d_k_t=%d", k_t);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MAX3D_GLSL_STR), "$k_t=%d", k_t);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    cvl_transform_multi(&tmpframe, 1, framebuf, t_len, "textures");
    cvl_check_errors();

    /* h, v */
    cvl_max(dst, tmpframe, k_h, k_v);
    cvl_frame_free(tmpframe);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 *
 * Applies Median filtering to a frame.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1.
 */
void cvl_median(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    if (cvl_error())
	return;
    
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_median_k_h=%d_k_v=%d", k_h, k_v);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MEDIAN_GLSL_STR), "$k_h=%d, $k_v=%d", k_h, k_v);
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
 * \param srcs		The source frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * 
 * Applies Median filtering.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
void cvl_median3d(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(k_t >= 0);
    cvl_assert(srcs[k_t] != NULL);
    cvl_assert(srcs[k_t] != dst);
    if (cvl_error())
	return;

    int t_len = 2 * k_t + 1;
    cvl_frame_t *framebuf[t_len];

    // Use clamping to fill the frame buffer
    int first_known = -1, last_known = -1;
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	{
	    if (first_known < 0)
		first_known = i;
	    if (last_known < i)
		last_known = i;
	}
    }
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	    framebuf[i] = srcs[i];
	else if (i < first_known)
	    framebuf[i] = srcs[first_known];
	else
	    framebuf[i] = srcs[last_known];
    }

    GLuint prg;
    char *prgname = cvl_asprintf("cvl_median3d_k_h=%d_k_v=%d_k_t=%d", k_h, k_v, k_t);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MEDIAN3D_GLSL_STR), 
	    	"$k_h=%d, $k_v=%d, $k_t=%d", k_h, k_v, k_t);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)cvl_frame_width(srcs[t_len / 2]));
    glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)cvl_frame_height(srcs[t_len / 2]));
    cvl_transform_multi(&dst, 1, framebuf, t_len, "textures");
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 *
 * Applies approximated Median filtering to a frame.
 * The Median filter is separated for this purpose. The results are usually
 * close to the real median filter results.
 * The number of matrix columns will be 2k_h+1, the number of rows will
 * be 2k_v+1.
 */
void cvl_median_separated(cvl_frame_t *dst, cvl_frame_t *src, int k_h, int k_v)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    if (cvl_error())
	return;
    
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(src);
    
    /* h */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_median_separated_k=%d", k_h);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MEDIAN_SEPARATED_GLSL_STR), "$k=%d", k_h);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 1.0f / (float)cvl_frame_width(src), 0.0f);
    cvl_transform(tmpframe, src);
    /* v */
    prgname = cvl_asprintf("cvl_median_separated_k=%d", k_v);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MEDIAN_SEPARATED_GLSL_STR), "$k=%d", k_v);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 0.0f, 1.0f / (float)cvl_frame_height(src));
    cvl_transform(dst, tmpframe);

    cvl_frame_free(tmpframe);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param srcs		The source frames.
 * \param k_h		Mask size in horizontal direction.
 * \param k_v		Mask size in vertical direction.
 * \param k_t		Mask size in temporal direction.
 * 
 * Applies approximated Median filtering.
 * The Median filter is separated for this purpose. The results are usually
 * close to the real median filter results.
 * See cvl_convolve3d_separable() for a description of \a frames.
 * The kernel size will be (2k_t+1)x(2k_v+1)x(2k_h+1).
 */
void cvl_median3d_separated(cvl_frame_t *dst, cvl_frame_t **srcs, int k_h, int k_v, int k_t)
{
    cvl_assert(dst != NULL);
    cvl_assert(srcs != NULL);
    cvl_assert(k_h >= 0);
    cvl_assert(k_v >= 0);
    cvl_assert(k_t >= 0);
    cvl_assert(srcs[k_t] != NULL);
    cvl_assert(srcs[k_t] != dst);
    if (cvl_error())
	return;

    int t_len = 2 * k_t + 1;
    cvl_frame_t *framebuf[2 * k_t + 1];
    cvl_frame_t *tmpframe = cvl_frame_new_tpl(srcs[k_t]);

    // Use clamping to fill the frame buffer
    int first_known = -1, last_known = -1;
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	{
	    if (first_known < 0)
		first_known = i;
	    if (last_known < i)
		last_known = i;
	}
    }
    for (int i = 0; i < t_len; i++)
    {
	if (srcs[i])
	    framebuf[i] = srcs[i];
	else if (i < first_known)
	    framebuf[i] = srcs[first_known];
	else
	    framebuf[i] = srcs[last_known];
    }

    /* t */
    GLuint prg;
    char *prgname = cvl_asprintf("cvl_median3d_separated_k_t=%d", k_t);
    if ((prg = cvl_gl_program_cache_get(prgname)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_MEDIAN3D_SEPARATED_GLSL_STR), "$k_t=%d", k_t);
	prg = cvl_gl_program_new_src(prgname, NULL, src);
	cvl_gl_program_cache_put(prgname, prg);
	free(src);
    }
    free(prgname);
    glUseProgram(prg);
    cvl_transform_multi(&tmpframe, 1, framebuf, t_len, "textures");
    cvl_check_errors();

    /* h, v */
    cvl_median(dst, tmpframe, k_h, k_v);
    cvl_frame_free(tmpframe);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param c		The sharpness parameter, greater than or equal to zero.
 *
 * Sharpens a frame with the Laplacian operator. The parameter \a c
 * determines the strength of the sharpening effect.
 * This function works best on frames in #CVL_XYZ format.
 */
void cvl_laplace(cvl_frame_t *dst, cvl_frame_t *src, float c)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(c >= 0.0f);
    if (cvl_error())
	return;
   
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_laplace")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_laplace", NULL, CVL_LAPLACE_GLSL_STR);
	cvl_gl_program_cache_put("cvl_laplace", prg);
    }
    glUseProgram(prg);
    glUniform2f(glGetUniformLocation(prg, "step"), 
	    1.0f / (float)cvl_frame_width(src), 
	    1.0f / (float)cvl_frame_height(src));
    glUniform1f(glGetUniformLocation(prg, "c"), c);
    glUniform1i(glGetUniformLocation(prg, "clamping"), cvl_frame_format(src) == CVL_UNKNOWN ? 0 : 1);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param smoothed	A smoothed version of the frame.
 * \param c		The sharpness parameter, from (0.5, 1.0).
 *
 * Sharpens the a frame using unsharp masking. The frame \a smoothed must
 * be a smoothed version of \a src, for example produced by 3x3 Gauss filtering.
 * The parameter \a c determines the strength of the sharpening effect.
 * This function works best on frames in #CVL_XYZ format.
 */
void cvl_unsharpmask(cvl_frame_t *dst, cvl_frame_t *src, cvl_frame_t *smoothed, float c)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(smoothed != NULL);
    cvl_assert(c > 0.5f && c < 1.0f);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_unsharpmask")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_unsharpmask", NULL, CVL_UNSHARPMASK_GLSL_STR);
	cvl_gl_program_cache_put("cvl_unsharpmask", prg);
    }
    glUseProgram(prg);
    glUniform1f(glGetUniformLocation(prg, "c"), c);
    glUniform1i(glGetUniformLocation(prg, "clamping"), cvl_frame_format(src) == CVL_UNKNOWN ? 0 : 1);
    cvl_frame_t *srcs[2] = { src , smoothed };
    cvl_transform_multi(&dst, 1, srcs, 2, "textures");
    cvl_check_errors();
}
