/*
 * cvl_visualization.c
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
 * \file cvl_visualization.h
 * \brief Functions to visualize things.
 *
 * Functions to visualize things.
 */

#include "config.h"

#include <stdlib.h>

#include <GL/glew.h>

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_basic.h"
#include "cvl/cvl_visualization.h"

#include "glsl/visualization/vector2_color.glsl.h"


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Transform the x,y coordinates of the vector data in \a src to the color
 * components R,G. Each component of the normalized vector will be transformed
 * from [-1,1] to [0,1] for this purpose. The frame \a dst must be in #CVL_RGB
 * format.
 */
void cvl_visualize_vector2_color(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(cvl_frame_format(dst) == CVL_RGB);
    if (cvl_error())
	return;

    GLuint prg;
    if ((prg = cvl_gl_program_cache_get("visualize_vector2_color")) == 0)
    {
	prg = cvl_gl_program_new_src("visualize_vector2_color", NULL, CVL_VECTOR2_COLOR_GLSL_STR);
	cvl_gl_program_cache_put("visualize_vector2_color", prg);
    }
    glUseProgram(prg);
    cvl_transform(dst, src);
    cvl_check_errors();
}


/**
 * \param frame		The frame with the vector data.
 * \param sample_x	Visualize every sample_x -th vector in horizontal direction.
 * \param sample_y	Visualize every sample_y -th vector in vertical direction.
 * \param dist_x	Horizontal needle distance.
 * \param dist_y	Vertical needle distance.
 * \param factor	Factor for the needle length.
 * \return		The needle diagram.
 *
 * Every \a sample_x -th vector in horizontal direction and every \a sample_y -th 
 * vector in vertical direction will be represented by a needle. The needles 
 * will have a distance of \a dist_x pixels in horizontal and \a dist_y pixels in
 * vertical direction. The needle length is the length of the vector after it
 * was scaled with \a factor.
 */
cvl_frame_t *cvl_visualize_vector2_needle(cvl_frame_t *frame, 
	int sample_x, int sample_y, int dist_x, int dist_y, float factor)
{
    cvl_assert(frame != NULL);
    cvl_assert(sample_x > 0);
    cvl_assert(sample_y > 0);
    cvl_assert(dist_x > 0);
    cvl_assert(dist_y > 0);
    if (cvl_error())
	return NULL;
    
    cvl_format_t format = cvl_frame_format(frame);
    cvl_frame_set_format(frame, CVL_UNKNOWN);
    float *ptr = cvl_frame_pointer(frame);
    cvl_frame_set_format(frame, format);

    int width = cvl_frame_width(frame) / sample_x * dist_x;
    int height = cvl_frame_height(frame) / sample_y * dist_y;
    float step_h = 1.0f / (float)width;
    float step_v = 1.0f / (float)height;
    cvl_frame_t *vis = cvl_frame_new(width, height, 1, CVL_LUM, CVL_UINT8, CVL_TEXTURE);
    float fillval[4];
    cvl_color_to_float(0xffffff, CVL_RGB, fillval);
    cvl_fill_rect(vis, 0, 0, width, height, fillval);
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(vis));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 
	    cvl_frame_texture(vis), 0);
    glViewport(0, 0, width, height);
    glDisable(GL_TEXTURE_2D);

    glLineWidth(1.0f);
    for (int y = 0; y < cvl_frame_height(frame); y += sample_y)
    {
	for (int x = 0; x < cvl_frame_width(frame); x += sample_x)
	{
	    float *v = ptr + 4 * (y * cvl_frame_width(frame) + x);
	    float needle_pos_x = 2.0f * (((float)x / (float)sample_x * (float)dist_x) / width + 0.5f * step_h) - 1.0f;
	    float needle_pos_y = 2.0f * (((float)y / (float)sample_y * (float)dist_y) / height + 0.5f * step_v) - 1.0f;
	    float delta_x = 2.0f * v[0] * factor;
	    float delta_y = 2.0f * v[1] * factor;
	    glBegin(GL_LINES);
	    glColor3f(0.63f, 0.63f, 0.63f);
	    glVertex3f(needle_pos_x - 2.0f * step_h, needle_pos_y, 0.0f);
	    glVertex3f(needle_pos_x + 3.0f * step_h, needle_pos_y, 0.0f);
	    glVertex3f(needle_pos_x, needle_pos_y - 2.0f * step_v, 0.0f);
	    glVertex3f(needle_pos_x, needle_pos_y + 3.0f * step_v, 0.0f);
	    glColor3f(0.0f, 0.0f, 0.0f);
	    glVertex3f(needle_pos_x, needle_pos_y, 0.0f);
	    glVertex3f(needle_pos_x + delta_x, needle_pos_y - delta_y, 0.0f);
	    glEnd();
	}
    }

    cvl_check_errors();
    glEnable(GL_TEXTURE_2D);

    return vis;
}
