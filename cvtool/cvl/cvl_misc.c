/*
 * cvl_misc.c
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
 * \file cvl_misc.h
 * \brief Miscellaneous functions.
 *
 * Miscellaneous functions.
 */

#include "config.h"

#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>

#include "mh.h"

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_basic.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_transform.h"
#include "cvl/cvl_mix.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_misc.h"

#include "glsl/misc/resize_seq.glsl.h"
#include "glsl/misc/reduce.glsl.h"
#include "glsl/misc/sort.glsl.h"
#include "glsl/misc/pyramid_gaussian.glsl.h"


/**
 * \typedef cvl_reduce_mode_t
 * The condense mode.
 */
/** \var CVL_REDUCE_MIN
 * Reduce to the minimum value. */
/** \var CVL_REDUCE_MIN_GREATER_ZERO
 * Reduce to the minimum value that is greater than zero. */
/** \var CVL_REDUCE_ABSMIN
 * Reduce to the minimum absolute value. */
/** \var CVL_REDUCE_ABSMIN_GREATER_ZERO
 * Reduce to the minimum absolute value that is greater than zero. */
/** \var CVL_REDUCE_MAX
 * Reduce to the maximum value. */
/** \var CVL_REDUCE_ABSMAX
 * Reduce to the maximum absolute value. */
/** \var CVL_REDUCE_SUM
 * Reduce to the sum of all values. */


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param fillcolor	Color to fill empty areas with.
 *
 * Resizes the frame \a src to the new dimensions of the frame \a dst.
 * The frame data is treated as a sequential array with cvl_frame_size(src)
 * elements. When resizing to a larger size, the new empty areas at the end of
 * the array are filled with \a fillcolor, which must have 4 components.
 * When resizing to a smaller size, the \a fillcolor parameter is not used.\n
 * The typical use of this function is to resize a frame to power-of-two
 * dimensions, then process the resized frame with an algorithm that requires
 * power-of-two dimensions, and afterwards resize the frame back to its original
 * size.
 */
void cvl_resize_seq(cvl_frame_t *dst, cvl_frame_t *src, const float *fillcolor)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_size(dst) <= cvl_frame_size(src) || fillcolor != NULL);
    if (cvl_error())
	return;

    if (cvl_frame_size(dst) == cvl_frame_size(src))
    {
	glUseProgram(0);
	cvl_transform(dst, src);
	return;
    }

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_resize_seq")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_resize_seq", NULL, CVL_RESIZE_SEQ_GLSL_STR);
	cvl_gl_program_cache_put("cvl_resize_seq", prg);
    }
    glUseProgram(prg);
    glUniform1i(glGetUniformLocation(prg, "src_width"), cvl_frame_width(src));
    glUniform1i(glGetUniformLocation(prg, "src_height"), cvl_frame_height(src));
    glUniform1i(glGetUniformLocation(prg, "dst_width"), cvl_frame_width(dst));
    glUniform1i(glGetUniformLocation(prg, "dst_height"), cvl_frame_height(dst));
    if (cvl_frame_size(dst) > cvl_frame_size(src))
	glUniform4fv(glGetUniformLocation(prg, "fill_color"), 1, fillcolor);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param frame		The frame.
 * \param mode		The mode.
 * \param channel	The channel(s).
 * \param result	An array to store the result in.
 *
 * Reduces the frame \a frame to a single value in the mode(s) given in \a mode.
 * The \a channel parameter determines which channel will be used for reduction:
 * 0, 1, 2, or 3. The special value -1 means to process all channels
 * independently.\n
 * For example, channel 0 in mode #CVL_REDUCE_MIN means that the 
 * minimum value in channel 0 will be computed. The other channel values will 
 * not be minimized. They will contain the values that the pixel with the minimum
 * channel 0 value contains. All channels are minimized independently if 
 * channel -1 is chosen.\n 
 * In #CVL_REDUCE_SUM mode, all channels are always summed up
 * independently.\n
 * The results are stored in \a result, which must be large enough to hold one
 * or four floats, depending on \a channel.
 */
void cvl_reduce(cvl_frame_t *frame, cvl_reduce_mode_t mode, int channel, float *result)
{
    cvl_assert(frame != NULL);
    cvl_assert(channel >= -1 && channel <= 3);
    cvl_assert(result != NULL);
    if (cvl_error())
	return;

    const char *mode_names[] = { "min", "mingz", "absmin", "absmingz", "max", "absmax", "sum" };
    const char *channel_names[] = { "r", "g", "b", "a" };
    cvl_frame_t *input_frame;

    GLint internalformat;
    if (mode == CVL_REDUCE_SUM || cvl_frame_type(frame) == CVL_FLOAT)
	internalformat = GL_RGBA32F_ARB;
    else
	internalformat = GL_RGBA;
    
    int src_w = mh_next_power_of_two(cvl_frame_width(frame));
    int src_h = mh_next_power_of_two(cvl_frame_height(frame));
    if (src_w != cvl_frame_width(frame) || src_h != cvl_frame_height(frame))
    {
	const float neutral_min[4] = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
	const float neutral_max[4] = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
	const float neutral_sum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	const float *neutral_element;
	if (mode == CVL_REDUCE_MIN || mode == CVL_REDUCE_MIN_GREATER_ZERO 
		|| mode == CVL_REDUCE_ABSMIN || mode == CVL_REDUCE_ABSMIN_GREATER_ZERO)
	    neutral_element = neutral_min;
	else if (mode == CVL_REDUCE_MAX || mode == CVL_REDUCE_ABSMAX)
	    neutral_element = neutral_max;
	else
	    neutral_element = neutral_sum;
	input_frame = cvl_frame_new(src_w, src_h, cvl_frame_channels(frame), 
		cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
	cvl_resize_seq(input_frame, frame, neutral_element);
    }
    else
    {
	input_frame = frame;
    }
    GLuint src_tex = cvl_frame_texture(input_frame);

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_reduce_mode=%s_channel=%s", 
	    mode_names[mode], channel == -1 ? "all" : channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_REDUCE_GLSL_STR), 
		"$mode_absmingz=%d, $mode_absmin=%d, $mode_mingz=%d, $mode_min=%d, "
		"$mode_absmax=%d, $mode_max=%d, $mode_sum=%d, $all_channels=%d, $channel=%s", 
		(mode == CVL_REDUCE_ABSMIN_GREATER_ZERO ? 1 : 0),
		(mode == CVL_REDUCE_ABSMIN ? 1 : 0),
		(mode == CVL_REDUCE_MIN_GREATER_ZERO ? 1 : 0),
		(mode == CVL_REDUCE_MIN ? 1 : 0),
		(mode == CVL_REDUCE_ABSMAX ? 1 : 0),
		(mode == CVL_REDUCE_MAX ? 1 : 0),
		(mode == CVL_REDUCE_SUM ? 1 : 0),
		(channel == -1 ? 1 : 0),
		channel == -1 ? "all" : channel_names[channel]);
	prg = cvl_gl_program_new_src(prg_name, NULL, src);
	cvl_gl_program_cache_put(prg_name, prg);
	free(src);
    }
    free(prg_name);
    glUseProgram(prg);

    while (src_w > 1 || src_h > 1)
    {
	int dst_w = mh_max(1, src_w / 2);
	int dst_h = mh_max(1, src_h / 2);
	GLuint dst_tex;
	glGenTextures(1, &dst_tex);
	glBindTexture(GL_TEXTURE_2D, dst_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, dst_w, dst_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	cvl_gl_set_texture_state();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dst_tex, 0);
	glViewport(0, 0, dst_w, dst_h);
	glBindTexture(GL_TEXTURE_2D, src_tex);
	cvl_gl_set_texture_state();
	glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)src_w);
	glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)src_h);
	glUniform1i(glGetUniformLocation(prg, "reduce_case"), 
		(dst_w < src_w && dst_h < src_h) ? 0 : (dst_w < src_w) ? 1 : 2);
	glDrawArrays(GL_QUADS, 0, 4);
	if (src_tex != cvl_frame_texture(input_frame))
	    glDeleteTextures(1, &src_tex);
	src_tex = dst_tex;
	src_w = dst_w;
	src_h = dst_h;
    }

    glBindTexture(GL_TEXTURE_2D, src_tex);
    glGetTexImage(GL_TEXTURE_2D, 0, 
	    channel == 0 ? GL_RED 
	    : channel == 1 ? GL_GREEN 
	    : channel == 2 ? GL_BLUE 
	    : channel == 3 ? GL_ALPHA 
	    : GL_RGBA, GL_FLOAT, result);

    if (src_tex != cvl_frame_texture(frame))
	glDeleteTextures(1, &src_tex);
    if (input_frame != frame)
	cvl_frame_free(input_frame);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param channel	The sorting key.
 *
 * Sorts the frame \a src: The top left pixel will contain the smallest
 * values, and the bottom right pixel will contain the largest values.\n
 * The given \a channel (0-3) sets the sorting key. For \a channel -1, all 
 * channels are sorted separately.\n
 * Source frames with format #CVL_FLOAT must only contain finite floats
 * that are smaller than FLT_MAX.
 */
void cvl_sort(cvl_frame_t *dst, cvl_frame_t *src, int channel)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(channel >= -1 && channel <= 3);
    if (cvl_error())
	return;

    if (cvl_frame_size(src) == 1)
    {
	cvl_copy(dst, src);
	return;
    }

    cvl_frame_t *input_frame;
    const char *channel_names[] = { "r", "g", "b", "a" };
    GLuint srctex, dsttex;
    cvl_frame_t *buf1, *buf2;

    int width = mh_next_power_of_two(cvl_frame_width(src));
    int height = mh_next_power_of_two(cvl_frame_height(src));
    /* TODO: Do not resize if width/height were already powers of two */
    float fltmax[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
    input_frame = cvl_frame_new(width, height, cvl_frame_channels(src),
	    cvl_frame_format(src), cvl_frame_type(src), CVL_TEXTURE);
    cvl_resize_seq(input_frame, src, fltmax);
    buf1 = cvl_frame_new_tpl(input_frame);
    buf2 = cvl_frame_new_tpl(input_frame);
    srctex = cvl_frame_texture(input_frame);
    dsttex = cvl_frame_texture(buf2);

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_sort_channel=%s", 
	    channel == -1 ? "all" : channel_names[channel]);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_SORT_GLSL_STR),
		"$all_channels=%d, $channel=%s", 
		(channel == -1 ? 1 : 0), 
		channel == -1 ? "all" : channel_names[channel]);
	prg = cvl_gl_program_new_src(prg_name, NULL, src);
	cvl_gl_program_cache_put(prg_name, prg);
	free(src);
    }
    free(prg_name);
    glUseProgram(prg);
    glUniform1i(glGetUniformLocation(prg, "width"), width);
    glUniform1i(glGetUniformLocation(prg, "height"), height);
    glUniform1f(glGetUniformLocation(prg, "hstep"), 1.0f / (float)width);
    glUniform1f(glGetUniformLocation(prg, "vstep"), 1.0f / (float)height);
    glViewport(0, 0, width, height);

    int log2_n = mh_log2(width * height);
    for (int step = 1, stepno = 2; step <= log2_n; step++, stepno *= 2)
    {
	for (int stage = step, stageno = stepno; stage >= 1; stage--, stageno /= 2)
	{
	    glUniform1i(glGetUniformLocation(prg, "stageno"), stageno);
	    glUniform1i(glGetUniformLocation(prg, "stepno"), stepno);
	    glUniform1i(glGetUniformLocation(prg, "offset"), stageno / 2);
	    glBindTexture(GL_TEXTURE_2D, dsttex);
	    cvl_gl_set_texture_state();
	    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, dsttex, 0);
	    glBindTexture(GL_TEXTURE_2D, srctex);
	    cvl_gl_set_texture_state();
	    glDrawArrays(GL_QUADS, 0, 4);
	    dsttex = (dsttex == cvl_frame_texture(buf1) ? cvl_frame_texture(buf2) : cvl_frame_texture(buf1));
	    srctex = (dsttex == cvl_frame_texture(buf1) ? cvl_frame_texture(buf2) : cvl_frame_texture(buf1));
	}
    }

    cvl_resize_seq(dst, dsttex == cvl_frame_texture(buf1) ? buf2 : buf1, NULL);
    cvl_frame_free(input_frame);
    cvl_frame_free(buf1);
    cvl_frame_free(buf2);

    cvl_check_errors();
}


/**
 * \param frame		The sorted frame.
 * \param channel	The channel.
 * \param q		The quantil.
 * \param result	A buffer.
 *
 * Gets the quantil defined by \a q from the sorted frame \a frame. \a q must be
 * from [0,1].
 * If \a channel is 0, 1, 2, or 3, then a single float from the
 * corresponding channel in \a frame is stored in \a result. If \a channel is -1, 
 * then four floats, one for each channel, are stored in \a result.
 */
void cvl_quantil(cvl_frame_t *frame, int channel, float q, float *result)
{
    cvl_assert(frame != NULL);
    cvl_assert(channel >= -1 && channel <= 3);
    cvl_assert(q >= 0.0f && q <= 1.0f);
    cvl_assert(result != NULL);
    if (cvl_error())
	return;

    int i = mh_iroundf(q * (float)(cvl_frame_size(frame) - 1));
    GLint x = i % cvl_frame_width(frame);
    GLint y = i / cvl_frame_width(frame);
    cvl_get(frame, channel, x, y, result);
}


/**
 * \param frame		The frame.
 * \param min		Pointer to 4 floats.
 * \param max		Pointer to 4 floats.
 * \param median	Pointer to 4 floats.
 * \param mean		Pointer to 4 floats.
 * \param stddev	Pointer to 4 floats.
 * \param dynrange	Pointer to 4 floats.
 *
 * Computes some simple statistics for the frame \a frame.
 * The minimum, maximum, median, and mean values are stored in \a min, \a max,
 * \a median, \a mean respectively. The standard deviation is stored in \a stddev.\n
 * The dynamic range of each channel is stored in \a dynrange.
 * If any of \a min, \a max, \a median, \a mean, \a stddev, \a dynrange 
 * is NULL, then the corresponding computation will not be executed.
 */
void cvl_statistics(cvl_frame_t *frame, float *min, float *max, float *median, 
	float *mean, float *stddev, float *dynrange)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    float sum[4];

    if (min)
    {
	cvl_reduce(frame, CVL_REDUCE_MIN, -1, min);
    }
    if (max)
    {
	cvl_reduce(frame, CVL_REDUCE_MAX, -1, max);
    }
    if (median)
    {
	cvl_frame_t *sorted = cvl_frame_new_tpl(frame);
	cvl_sort(sorted, frame, -1);
	cvl_quantil(sorted, -1, 0.5, median);
	cvl_frame_free(sorted);
    }
    if (mean || stddev)
    {
	cvl_reduce(frame, CVL_REDUCE_SUM, -1, sum);
    }
    if (mean)
    {
	for (int c = 0; c < 4; c++)
	{
	    mean[c] = sum[c] / (float)cvl_frame_size(frame);
	}
    }
    if (stddev)
    {
	if (cvl_frame_size(frame) == 1)
	{
	    for (int c = 0; c < 4; c++)
	    {
		stddev[c] = 0.0f;
	    }
	}
	else
	{
	    float sumsq[4];
	    cvl_frame_t *layers[2] = { frame, frame };
	    cvl_frame_t *squared = cvl_frame_new_tpl(frame);
	    cvl_layer(squared, layers, 2, CVL_LAYER_MUL);
	    cvl_reduce(squared, CVL_REDUCE_SUM, -1, sumsq);
	    cvl_frame_free(squared);

	    for (int c = 0; c < 4; c++)
	    {
		float radicand = (sumsq[c] - sum[c] * sum[c] / (float)cvl_frame_size(frame)) / (float)(cvl_frame_size(frame) - 1);
		stddev[c] = (radicand > 0.0f) ? sqrtf(radicand) : 0.0f;
	    }
	}
    }
    if (dynrange)
    {
	float drmin[4], summand[4], drmax[4], mingz[4];
	cvl_frame_t *tmpframe;

	if (min)
	{
	    for (int c = 0; c < 4; c++)
		drmin[c] = min[c];
	}
     	else
	{
    	    cvl_reduce(frame, CVL_REDUCE_MIN, -1, drmin);
	}
	for (int c = 0; c < 4; c++)
	    summand[c] = - min[c];
	tmpframe = cvl_frame_new_tpl(frame);
	cvl_add(tmpframe, frame, summand);
	cvl_reduce(tmpframe, CVL_REDUCE_MIN_GREATER_ZERO, -1, mingz);
	cvl_reduce(tmpframe, CVL_REDUCE_MAX, -1, drmax);
	cvl_frame_free(tmpframe);
	for (int c = 0; c < 4; c++)
	    dynrange[c] = drmax[c] / mingz[c];
    }
}


/**
 * \param dst		The destination frame.
 * \param src1		The first source frame.
 * \param src2		The second source frame.
 *
 * Visualizes the differences between two frames.
 * The frames \a dst, \a src1, and \a src2 must have the same dimensions and the same
 * format. The pixel values in the resulting frame will be the absolute of the
 * difference of the corresponding pixels in the two sources.\n
 * This function is a convenience function for cvl_layer() with #CVL_LAYER_DIFF.
 */
void cvl_diff(cvl_frame_t *dst, cvl_frame_t *src1, cvl_frame_t *src2)
{
    cvl_assert(dst != NULL);
    cvl_assert(src1 != NULL);
    cvl_assert(src2 != NULL);
    cvl_assert(dst != src1);
    cvl_assert(dst != src2);
    cvl_assert(cvl_frame_width(src1) == cvl_frame_width(src2));
    cvl_assert(cvl_frame_height(src1) == cvl_frame_height(src2));
    cvl_assert(cvl_frame_format(src1) == cvl_frame_format(src2));
    if (cvl_error())
	return;

    cvl_frame_t *layers[2] = { src1, src2 };
    cvl_layer(dst, layers, 2, CVL_LAYER_DIFF);
}


/**
 * \param frame		The frame.
 * \param n		Number of pyramid levels.
 * \param pyramid	Buffer for n frames.
 *
 * Computes a gaussian pyramid with \a n levels from the frame \a frame.
 * Level 0 will be a copy of \a frame.
 */
void cvl_pyramid_gaussian(cvl_frame_t *frame, int n, cvl_frame_t **pyramid)
{
    cvl_assert(frame != NULL);
    cvl_assert(n > 0);
    cvl_assert(pyramid != NULL);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_pyramid_gaussian")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_pyramid_gaussian", NULL, CVL_PYRAMID_GAUSSIAN_GLSL_STR);
	cvl_gl_program_cache_put("cvl_pyramid_gaussian", prg);
    }
    glUseProgram(prg);

    pyramid[0] = cvl_frame_new_tpl(frame);
    cvl_copy(pyramid[0], frame);
    int width = cvl_frame_width(frame);
    int height = cvl_frame_height(frame);
    for (int l = 1; l < n; l++)
    {
	glUseProgram(prg);	// The GL complains if this is moved out of the loop.
	glUniform1f(glGetUniformLocation(prg, "width"), (float)width);
	glUniform1f(glGetUniformLocation(prg, "height"), (float)height);
	width = mh_max(1, width / 2);
	height = mh_max(1, height / 2);
	pyramid[l] = cvl_frame_new(width, height, cvl_frame_channels(frame),
		cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
	cvl_transform(pyramid[l], pyramid[l - 1]);
    }
}


/**
 * \param gaussian_pyramid	A gaussian pyramid.
 * \param n			Number of pyramid levels.
 * \param laplacian_pyramid	Buffer for n-1 frames.
 *
 * Computes a laplacian pyramid from the gaussian pyramid \a gaussian_pyramid
 * which has \a n levels. The resulting laplacian pyramid will have \a n - 1
 * levels.\n
 * The values in the laplacian pyramid levels will be fitted to [0,1] for easier
 * handling. Transform them using the formula (2 * (diff - 0.5)) to get the real
 * differences.
 */
void cvl_pyramid_laplacian(cvl_frame_t **gaussian_pyramid, int n, cvl_frame_t **laplacian_pyramid)
{
    cvl_assert(gaussian_pyramid != NULL);
    cvl_assert(n > 0);
    cvl_assert(laplacian_pyramid != NULL);
    if (cvl_error())
	return;

    for (int l = 0; l < n - 1; l++)
    {
	cvl_frame_t *layers[2];
	int width, height;

	width = mh_maxi(1, cvl_frame_width(gaussian_pyramid[l]) / 2);
	height = mh_maxi(1, cvl_frame_height(gaussian_pyramid[l]) / 2);
	layers[0] = gaussian_pyramid[l];
	layers[1] = cvl_scale(gaussian_pyramid[l + 1], width, height, CVL_BILINEAR);
	laplacian_pyramid[l] = cvl_frame_new(width, height, cvl_frame_channels(gaussian_pyramid[l]),
		cvl_frame_format(gaussian_pyramid[l]), cvl_frame_type(gaussian_pyramid[l]), CVL_TEXTURE);
	cvl_layer(laplacian_pyramid[l], layers, 2, CVL_LAYER_XSUB);
	cvl_frame_free(layers[1]);
    }
}


/**
 * \param frame		The frame.
 * \param channel	The channel.
 * \param bins		The number of bins.
 * \param min		The minimum value(s).
 * \param max		The maximum value(s).
 * \param histogram	The histogram.
 *
 * Computes the histogram of the given \a channel in \a frame. The channel must be
 * 0-3, or -1 to compute the histogram of all channels. The histogram will be stored in 
 * \a histogram, which must have enough space for the requested bins and channels.
 * Frame values outside of [\a min, \a max] will be ignored. These values must be
 * given for all four channels (if \a channel is -1), or for the selected
 * channel.
 */
void cvl_histogram(cvl_frame_t *frame, int channel, int bins, const float *min, const float *max, int *histogram)
{
    cvl_assert(frame != NULL);
    cvl_assert(channel >= -1 && channel <= 3);
    cvl_assert(min != NULL);
    cvl_assert(max != NULL);
    cvl_assert(histogram != NULL);
    if (cvl_error())
	return;

    int s = cvl_frame_size(frame);
    if (channel == -1)
    {
	float *ptr;
	if (!(ptr = malloc(s * 4 * sizeof(float))))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    return;
	}
	glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, ptr);
	memset(histogram, 0, 4 * bins * sizeof(int));
	for (int i = 0; i < s; i++)
	{
	    float *p = ptr + 4 * i;
	    for (int c = 0; c < 4; c++)
	    {
		if (!isfinite(p[c]) || p[c] < min[c] || p[c] > max[c])
		    continue;
		int hi = mh_clampi((p[c] - min[c]) / (max[c] - min[c]) * (float)(bins - 1), 0, bins - 1);
		histogram[c * bins + hi]++;
	    }
	}
	free(ptr);
    }
    else
    {
	float *ptr;
	if (!(ptr = malloc(s * sizeof(float))))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    return;
	}
	glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
	glGetTexImage(GL_TEXTURE_2D, 0, 
		channel == 0 ? GL_RED : channel == 1 ? GL_GREEN : channel == 2 ? GL_BLUE : GL_ALPHA, 
		GL_FLOAT, ptr);
	memset(histogram, 0, bins * sizeof(int));
	for (int i = 0; i < s; i++)
	{
	    float *p = ptr + i;
	    if (!isfinite(p[0]) || p[0] < min[0] || p[0] > max[0])
    		continue;
	    int hi = mh_clampi((p[0] - min[0]) / (max[0] - min[0]) * (float)(bins - 1), 0, bins - 1);
	    histogram[hi]++;
	}
	free(ptr);
    }
}
