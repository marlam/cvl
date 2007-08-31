/*
 * cvl_mix.c
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

/** 
 * \file cvl_mix.h
 * \brief Mixing frames.
 *
 * Mixing frames.
 */

#include "config.h"

#include <GL/glew.h>

#include "mh.h"

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_basic.h"
#include "cvl/cvl_mix.h"

#include "glsl/mix/blend.glsl.h"
#include "glsl/mix/layer.glsl.h"


/**
 * \typedef cvl_layer_mode_t
 * The layering mode.
 */
/** \var CVL_LAYER_MIN
 *  Use minimum value. */
/** \var CVL_LAYER_MAX
 *  Use maximum value. */
/** \var CVL_LAYER_MEDIAN
 *  Use median value. */
/** \var CVL_LAYER_OR
 *  Bitwise or. */
/** \var CVL_LAYER_AND
 *  Bitwise and. */
/** \var CVL_LAYER_XOR
 *  Bitwise xor. */
/** \var CVL_LAYER_DIFF
 *  Use difference between maximum and minimum value. */
/** \var CVL_LAYER_ADD
 *  Use sum of values. */
/** \var CVL_LAYER_XADD
 *  Use sum of values. The ranges are transformed so that the results fit in
 *  [0,1]. Example for two layers: X = (L0 / 2) + (L1 / 2). */
/** \var CVL_LAYER_SUB
 *  Subtract values from the first value. */
/** \var CVL_LAYER_XSUB
 *  Subtract values from the first value. The ranges are transformed so that the results fit in
 *  [0,1]. Example for two layers: X = (L0 / 2) - (L1 / 2) + 1/2. */
/** \var CVL_LAYER_MUL
 *  Multiply values. */
/** \var CVL_LAYER_DIV
 *  Divide values. */

/**
 * \param frame		The destination frame.
 * \param mode		The layering mode.
 * \param layers	The source frames.
 * \param number_of_layers	The number of source frames.
 *
 * Layers the given source frames on top of each other, using the given \a mode.
 * Layering is done for each channel separately. 
 * At least one layer must be present.
 * The layers will be implicitly scaled to the size of the destination frame.
 */
void cvl_layer(cvl_frame_t *frame, cvl_frame_t **layers, int number_of_layers, cvl_layer_mode_t mode)
{
    cvl_assert(frame != NULL);
    cvl_assert(layers != NULL);
    cvl_assert(layers[0] != NULL);
    cvl_assert(number_of_layers >= 1);
    if (cvl_error())
	return;

    const char *cvl_layer_mode_names[] = 
    {
	"mode_min",
	"mode_max",
	"mode_median",
	"mode_or",
	"mode_and",
	"mode_xor",
	"mode_diff",
	"mode_add",
	"mode_xadd",
	"mode_sub",
	"mode_xsub",
	"mode_mul",
	"mode_div"
    };

    GLuint prg;
    char *prg_name = cvl_asprintf("cvl_layer_%s_n%d", cvl_layer_mode_names[mode], number_of_layers);
    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
    {
	char *src = cvl_gl_srcprep(cvl_strdup(CVL_LAYER_GLSL_STR), 
		"$layer_mode=%s, $n=%d", cvl_layer_mode_names[mode], number_of_layers);
	prg = cvl_gl_program_new_src(prg_name, NULL, src);
	cvl_gl_program_cache_put(prg_name, prg);
	free(src);
    }
    free(prg_name);
    glUseProgram(prg);

    cvl_transform_multi(&frame, 1, layers, number_of_layers, "layers");
    cvl_check_errors();
}

/**
 * \param frame		The frame.
 * \param dst_x         The x coordinate of the block's destination in the frame.
 * \param dst_y         The y coordinate of the block's destination in the frame.
 * \param block         The block to be copied into the frame.
 * \param alpha		The alpha map for the block.
 * 
 * Copies \a block into \a frame at the position specified by \a dst_x
 * and \a dst_y. The transparency of the block is read from \a alpha,
 * which must be of format #CVL_LUM. The block and the frame must have the
 * same pixel type.
 */
void cvl_blend(cvl_frame_t *frame, int dst_x, int dst_y,
	        cvl_frame_t *block, cvl_frame_t *alpha)
{
    cvl_assert(frame != NULL);
    cvl_assert(block != NULL);
    cvl_assert(alpha != NULL);
    cvl_assert(cvl_frame_width(block) == cvl_frame_width(alpha)
	    && cvl_frame_height(block) == cvl_frame_height(alpha));
    if (cvl_error())
	return;

    cvl_frame_t *orig = cvl_frame_new(cvl_frame_width(block), cvl_frame_height(block),
	    cvl_frame_channels(frame), cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
    cvl_cut_rect(orig, frame, dst_x, dst_y);

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cvl_frame_texture(frame), 0);
    glViewport(0, 0, cvl_frame_width(frame), cvl_frame_height(frame));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(orig));
    cvl_gl_set_texture_state();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(block));
    cvl_gl_set_texture_state();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(alpha));
    cvl_gl_set_texture_state();
    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("cvl_blend")) == 0)
    {
	prg = cvl_gl_program_new_src("cvl_blend", NULL, CVL_BLEND_GLSL_STR);
	cvl_gl_program_cache_put("cvl_blend", prg);
    }
    glUseProgram(prg);
    glUniform1i(glGetUniformLocation(prg, "orig"), 0);
    glUniform1i(glGetUniformLocation(prg, "block"), 1);
    glUniform1i(glGetUniformLocation(prg, "alpha"), 2);
    float dst_xf = (float)dst_x / (float)cvl_frame_width(frame);
    float dst_yf = (float)dst_y / (float)cvl_frame_height(frame);
    float dst_wf = (float)cvl_frame_width(block) / (float)cvl_frame_width(frame);
    float dst_hf = (float)cvl_frame_height(block) / (float)cvl_frame_height(frame);
    float dst_xlf = dst_xf * 2.0f - 1.0f;
    float dst_xrf = (dst_xf + dst_wf) * 2.0f - 1.0f;
    float dst_ytf = dst_yf * 2.0f - 1.0f;
    float dst_ybf = (dst_yf + dst_hf) * 2.0f - 1.0f;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(dst_xlf, dst_ytf, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(dst_xrf, dst_ytf, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(dst_xrf, dst_ybf, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(dst_xlf, dst_ybf, 0.0f);
    glEnd();
    glActiveTexture(GL_TEXTURE0);
    cvl_check_errors();
    cvl_frame_free(orig);
}
