/*
 * cvl_transform.c
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
 * \file cvl_transform.h
 * \brief Geometric transformations.
 *
 * Geometric transformation.
 */

#include "config.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <GL/glew.h>

#define CVL_BUILD
#include "cvl_intern.h"
#include "cvl/cvl.h"

#include "glsl/transform/bilinear.glsl.h"
#include "glsl/transform/biquadratic.glsl.h"
#include "glsl/transform/bicubic.glsl.h"


/**
 * \typedef cvl_interpolation_type_t
 * The interpolation technique.
 */
/** \var CVL_NONE
 *  No interpolation (nearest neighbor). */
/** \var CVL_BILINEAR
 *  Bilinear interpolation. */
/** \var CVL_BIQUADRATIC
 *  Biquadratic interpolation. */
/** \var CVL_BICUBIC
 *  Bicubic interpolation. */


/**
 * \param frame		The frame.
 * \param matrix	A 2x2 transformation matrix as an array of 4 floats.
 * \param interpolation_type	The interpolation type to use.
 * \param val		The values to fill resulting holes with.
 * \return		The transformed frame.
 * 
 * Applies an affine transformation to a frame. The resulting frame will be
 * large enough to hold the complete result. 
 * Holes will be filled with the 4 given values in \a val.
 */
cvl_frame_t *cvl_affine(cvl_frame_t *frame, const float *matrix, 
	cvl_interpolation_type_t interpolation_type, const float *val)
{
    cvl_assert(frame != NULL);
    cvl_assert(matrix != NULL);
    cvl_assert(val != NULL);
    if (cvl_error())
	return NULL;

    float w = (float)cvl_frame_width(frame);
    float h = (float)cvl_frame_height(frame);

    /* Determine where the four edge points will be */
    /* 1. (0,0) */
    float ep0x = 0.0f;
    float ep0y = 0.0f;
    /* 2. (w,0) */
    float ep1x = w * matrix[0];
    float ep1y = w * matrix[2];
    /* 3. (w,h) */
    float ep2x = w * matrix[0] + h * matrix[1];
    float ep2y = w * matrix[2] + h * matrix[3];
    /* 4. (0,h) */
    float ep3x = h * matrix[1];
    float ep3y = h * matrix[3];
    /* Bounding box of the frame: 
     * (minx, maxy), (maxx, maxy), (maxx, miny), (minx, miny) */
    float minx = cvl_min4f(ep0x, ep1x, ep2x, ep3x);
    float miny = cvl_min4f(ep0y, ep1y, ep2y, ep3y);
    float maxx = cvl_max4f(ep0x, ep1x, ep2x, ep3x);
    float maxy = cvl_max4f(ep0y, ep1y, ep2y, ep3y);

    int new_width = cvl_maxi(1, cvl_iroundf(maxx - minx));
    int new_height = cvl_maxi(1, cvl_iroundf(maxy - miny));
    cvl_frame_t *transformed = cvl_frame_new(new_width, new_height, cvl_frame_channels(frame),
	    cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
    if (cvl_frame_format(frame) == CVL_UNKNOWN)
    {
	for (int c = 0; c < cvl_frame_channels(frame); c++)
	{
	    cvl_frame_set_channel_name(transformed, c, cvl_frame_channel_name(frame, c));
	}
    }
    cvl_fill_rect(transformed, 0, 0, cvl_frame_width(transformed), cvl_frame_height(transformed), val);

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(transformed));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(transformed), 0);
    glViewport(0, 0, cvl_frame_width(transformed), cvl_frame_height(transformed));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
    cvl_gl_set_texture_state();
    if (interpolation_type == CVL_NONE)
    {
	glUseProgram(0);
    }
    else 
    {
	GLuint prg;
	if (interpolation_type == CVL_BILINEAR)
	{
	    if ((prg = cvl_gl_program_cache_get("cvl_bilinear")) == 0)
	    {
		prg = cvl_gl_program_new_src("cvl_bilinear", NULL, CVL_BILINEAR_GLSL_STR);
		cvl_gl_program_cache_put("cvl_bilinear", prg);
	    }
	}
	else if (interpolation_type == CVL_BIQUADRATIC)
	{
	    if ((prg = cvl_gl_program_cache_get("cvl_biquadratic")) == 0)
	    {
		prg = cvl_gl_program_new_src("cvl_biquadratic", NULL, CVL_BIQUADRATIC_GLSL_STR);
		cvl_gl_program_cache_put("cvl_biquadratic", prg);
	    }
	}
	else // A bicubic method.
	{
	    const char *bicubic_mode = "mitchell_netravali";
	    if (interpolation_type == CVL_BICUBIC_B_SPLINE)
		bicubic_mode = "b_spline";
	    else if (interpolation_type == CVL_BICUBIC_CR_SPLINE)
		bicubic_mode = "catmull_rom_spline";

	    char *prg_name = cvl_asprintf("cvl_bicubic_%s", bicubic_mode);
	    if ((prg = cvl_gl_program_cache_get(prg_name)) == 0)
	    {
		char *src = cvl_gl_srcprep(cvl_strdup(CVL_BICUBIC_GLSL_STR), "$mode=%s", bicubic_mode);
		prg = cvl_gl_program_new_src(prg_name, NULL, src);
		cvl_gl_program_cache_put(prg_name, prg);
		free(src);
	    }
	    free(prg_name);
	}
	glUseProgram(prg);
	glUniform1f(glGetUniformLocation(prg, "width"), (float)w);
	glUniform1f(glGetUniformLocation(prg, "step_h"), 1.0f / (float)w);
	glUniform1f(glGetUniformLocation(prg, "step_h_2"), 0.5f / (float)w);
	glUniform1f(glGetUniformLocation(prg, "height"), (float)h);
	glUniform1f(glGetUniformLocation(prg, "step_v"), 1.0f / (float)h);
	glUniform1f(glGetUniformLocation(prg, "step_v_2"), 0.5f / (float)h);
    }
    float nw = (float)new_width;
    float nh = (float)new_height;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(((ep0x - minx) / nw) * 2.0f - 1.0f, ((ep0y - miny) / nh) * 2.0f - 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(((ep1x - minx) / nw) * 2.0f - 1.0f, ((ep1y - miny) / nh) * 2.0f - 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(((ep2x - minx) / nw) * 2.0f - 1.0f, ((ep2y - miny) / nh) * 2.0f - 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(((ep3x - minx) / nw) * 2.0f - 1.0f, ((ep3y - miny) / nh) * 2.0f - 1.0f, 0.0f);
    glEnd();
    cvl_check_errors();

    return transformed;
}


/**
 * \param frame		The frame.
 * \param new_width	The new width.
 * \param new_height	The new height.
 * \param interpolation_type	The interpolation type to use.
 * \return		The scaled frame.
 * 
 * Scales a frame.
 */
cvl_frame_t *cvl_scale(cvl_frame_t *frame, int new_width, int new_height,
	cvl_interpolation_type_t interpolation_type)
{
    cvl_assert(frame != NULL);
    cvl_assert(new_width > 0);
    cvl_assert(new_height > 0);
    if (cvl_error())
	return NULL;

    float matrix[4] = 
    { 
	(float)new_width / (float)cvl_frame_width(frame),
	0.0f,
	0.0f,
	(float)new_height / (float)cvl_frame_height(frame)
    };
    return cvl_affine(frame, matrix, interpolation_type, matrix);
}


/**
 * \param frame		The frame.
 * \param angle		The rotation angle in radians [0,2PI].
 * \param interpolation_type	The interpolation type to use.
 * \param val		The values to fill resulting holes with.
 * \return		The transformed frame.
 * 
 * Rotates a frame counterclockwise. The resulting frame will be large enough 
 * to hold the complete result. Holes will be filled with the given \a color.
 * Holes will be filled with the 4 given values in \a val.
 */
cvl_frame_t *cvl_rotate(cvl_frame_t *frame, float angle,
	cvl_interpolation_type_t interpolation_type, const float *val)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return NULL;

    float matrix[4] = { cosf(angle), sinf(angle), -sinf(angle), cosf(angle) };
    return cvl_affine(frame, matrix, interpolation_type, val);
}


/**
 * \param frame		The frame.
 * \param shear_angle_x	The shear angle in horizontal direction, in radians.
 * \param shear_angle_y	The shear angle in vertical direction, in radians.
 * \param interpolation_type	The interpolation type to use.
 * \param val		The values to fill resulting holes with.
 * \return		The transformed frame.
 *
 * Shears a frame. The angles should be from (-PI/2, PI/2). Negative angles 
 * shear clockwise. The angles should not be to close to -PI/2 or PI/2, or the
 * resulting frame will be unreasonably wide.
 * Holes will be filled with the 4 given values in \a val.
 */
cvl_frame_t *cvl_shear(cvl_frame_t *frame, float shear_angle_x, float shear_angle_y,
	cvl_interpolation_type_t interpolation_type, const float *val)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return NULL;

    float matrix_x[4] = { 1.0f, tan(shear_angle_x), 0.0f, 1.0f };
    float matrix_y[4] = { 1.0f, 0.0f, tan(shear_angle_y), 1.0f };
    cvl_frame_t *tmpframe = cvl_affine(frame, matrix_x, interpolation_type, val);
    cvl_frame_t *transformed = cvl_affine(tmpframe, matrix_y, interpolation_type, val);
    cvl_frame_free(tmpframe);
    return transformed;
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * 
 * Flips a frame vertically.
 */
void cvl_flip(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(dst));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(dst), 0);
    glViewport(0, 0, cvl_frame_width(dst), cvl_frame_height(dst));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(src));
    cvl_gl_set_texture_state();
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glEnd();
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * 
 * Flops a frame horizontally.
 */
void cvl_flop(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(dst));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(dst), 0);
    glViewport(0, 0, cvl_frame_width(dst), cvl_frame_height(dst));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(src));
    cvl_gl_set_texture_state();
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
    cvl_check_errors();
}
