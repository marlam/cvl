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
