/*
 * cvl_basic.c
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
 * \file cvl_basic.h
 * \brief Basic frame manipulation.
 *
 * Basic frame manipulation.
 */

#include "config.h"

#include <GL/glew.h>

#define CVL_BUILD
#include "cvl_intern.h"
#include "cvl/cvl.h"


/**
 * \param frame		The frame.
 * \param channel	The channel.
 * \param x		The left x coordinate.
 * \param y		The top y coordinate.
 * \param result	A buffer for one or four floats.
 *
 * Gets the value of the pixel at \a x, \a y from the frame \a frame and stores it
 * in \a result. If \a channel is 0, 1, 2, or 3, then a single float from the
 * corresponding channel in \a frame is stored in \a result. If \a channel is -1, 
 * then four floats, one for each channel, are stored in \a result.
 */
void cvl_get(cvl_frame_t *frame, int channel, int x, int y, float *result)
{
    cvl_assert(frame != NULL);
    cvl_assert(channel >= -1 && channel <= 3);
    cvl_assert(x >= 0 && x < cvl_frame_width(frame));
    cvl_assert(y >= 0 && y < cvl_frame_height(frame));
    cvl_assert(result != NULL);
    if (cvl_error())
	return;

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(frame), 0);
    glReadPixels(x, y, 1, 1, 
	    channel == 0 ? GL_RED
	    : channel == 1 ? GL_GREEN
	    : channel == 2 ? GL_BLUE
	    : channel == 3 ? GL_ALPHA
	    : GL_RGBA, GL_FLOAT, result);
    cvl_check_errors();
}


/**
 * \param frame	The frame.
 * \param x	The left x coordinate.
 * \param y	The top y coordinate.
 * \param w	The width of the rectangle.
 * \param h	The height of the rectangle.
 * \param val	The values to fill the rectangle with.
 *
 * Fills the rectangle given by \a x, \a y, \a w, \a h in the frame \a frame with
 * the values \a val. \a val must contain 4 values, one for each channel.
 */
void cvl_fill_rect(cvl_frame_t *frame, int x, int y, int w, int h, const float *val)
{
    cvl_assert(frame != NULL);
    cvl_assert(val != NULL);
    if (cvl_error())
	return;

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cvl_frame_texture(frame), 0);
    glViewport(0, 0, cvl_frame_width(frame), cvl_frame_height(frame));
    glDisable(GL_TEXTURE_2D);
    glColor4f(val[0], val[1], val[2], val[3]);
    float fx = (float)x / (float)cvl_frame_width(frame);
    float fy = (float)y / (float)cvl_frame_height(frame);
    float fw = (float)w / (float)cvl_frame_width(frame);
    float fh = (float)h / (float)cvl_frame_height(frame);
    float xl = fx * 2.0f - 1.0f;
    float xr = (fx + fw) * 2.0f - 1.0f;
    float yt = fy * 2.0f - 1.0f;
    float yb = (fy + fh) * 2.0f - 1.0f;
    glUseProgram(0);
    glBegin(GL_QUADS);
    glVertex3f(xl, yt, 0.0f);
    glVertex3f(xr, yt, 0.0f);
    glVertex3f(xr, yb, 0.0f);
    glVertex3f(xl, yb, 0.0f);
    glEnd();
    cvl_check_errors();
    glEnable(GL_TEXTURE_2D);
}


/**
 * \param dst           The destination frame.
 * \param dst_x         The x coordinate of the destination rectangle's upper left corner.
 * \param dst_y         The y coordinate of the destination rectangle's upper left corner.
 * \param src           The source frame.
 * \param src_x         The x coordinate of the source rectangle's upper left corner.
 * \param src_y         The y coordinate of the source rectangle's upper left corner.
 * \param rwidth        The width of the rectangle.
 * \param rheight       The height of the rectangle.
 * 
 * Copies the given rectangle from \a src to \a dst. Destination and source must
 * not be the same frame. The frame formats should be the same in most cases.
 */
void cvl_copy_rect(cvl_frame_t *dst, int dst_x, int dst_y,
	cvl_frame_t *src, int src_x, int src_y, int rwidth, int rheight)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;
    
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(dst));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cvl_frame_texture(dst), 0);
    glViewport(0, 0, cvl_frame_width(dst), cvl_frame_height(dst));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(src));
    cvl_gl_set_texture_state();
    float src_xlf = (float)src_x / (float)cvl_frame_width(src);
    float src_ytf = (float)src_y / (float)cvl_frame_height(src);
    float src_xrf = src_xlf + (float)rwidth / (float)cvl_frame_width(src);
    float src_ybf = src_ytf + (float)rheight / (float)cvl_frame_height(src);
    float dst_xf = (float)dst_x / (float)cvl_frame_width(dst);
    float dst_yf = (float)dst_y / (float)cvl_frame_height(dst);
    float dst_wf = (float)rwidth / (float)cvl_frame_width(dst);
    float dst_hf = (float)rheight / (float)cvl_frame_height(dst);
    float dst_xlf = dst_xf * 2.0f - 1.0f;
    float dst_xrf = (dst_xf + dst_wf) * 2.0f - 1.0f;
    float dst_ytf = dst_yf * 2.0f - 1.0f;
    float dst_ybf = (dst_yf + dst_hf) * 2.0f - 1.0f;
    glUseProgram(0);
    glBegin(GL_QUADS);
    glTexCoord2f(src_xlf, src_ytf);
    glVertex3f(dst_xlf, dst_ytf, 0.0f);
    glTexCoord2f(src_xrf, src_ytf);
    glVertex3f(dst_xrf, dst_ytf, 0.0f);
    glTexCoord2f(src_xrf, src_ybf);
    glVertex3f(dst_xrf, dst_ybf, 0.0f);
    glTexCoord2f(src_xlf, src_ybf);
    glVertex3f(dst_xlf, dst_ybf, 0.0f);
    glEnd();
    cvl_check_errors();
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * 
 * Copies the contents of \a src to \a dst.
 */
void cvl_copy(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;

    glUseProgram(0);
    cvl_transform(dst, src);
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param x             The x coordinate of the rectangle.
 * \param y             The y coordinate of the rectangle.
 * 
 * Cuts a rectangle out of frame \a src and stores it in \a dst. The rectangle
 * will have the size of \a dst.
 */
void cvl_cut_rect(cvl_frame_t *dst, cvl_frame_t *src, int x, int y)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    if (cvl_error())
	return;

    cvl_copy_rect(dst, 0, 0, src, x, y, cvl_frame_width(dst), cvl_frame_height(dst));
}


/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 * \param x_offset	The x position of the original frame contents in the resized frame.
 * \param y_offset	The y position of the original frame contents in the resized frame.
 * \param val		The value that resulting holes will be filled with.
 * \return		The resized frame.
 * 
 * Resizes the frame \a src to \a dst. Position the contents of \a src at \a
 * x_offset, \a y_offset. Resulting holes will be filled with \a val, which must
 * contain 4 values, one for each channel.
 */
void cvl_resize(cvl_frame_t *dst, cvl_frame_t *src,
	int x_offset, int y_offset, const float *val)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst != src);
    cvl_assert(val != NULL);
    if (cvl_error())
	return;

    cvl_fill_rect(dst, 0, 0, cvl_frame_width(dst), cvl_frame_height(dst), val);
    int srx, sry, drx, dry, rw, rh;
    /* Compute the rectangle from 'src' that must be copied to 'dst' */
    srx = x_offset < 0 ? - x_offset : 0;
    sry = y_offset < 0 ? - y_offset : 0;
    drx = x_offset < 0 ? 0 : x_offset;
    dry = y_offset < 0 ? 0 : y_offset;
    rw = cvl_frame_width(src) - srx;
    if (drx + rw - 1 >= cvl_frame_width(dst))
    {
	rw = cvl_frame_width(dst) - drx;
    }
    rh = cvl_frame_height(src) - sry;
    if (dry + rh - 1 >= cvl_frame_height(dst))
    {
	rh = cvl_frame_height(dst) - dry;
    }
    if (srx < cvl_frame_width(src) && sry < cvl_frame_height(src) 
	    && drx < cvl_frame_width(dst) && dry < cvl_frame_height(dst)
	    && rw > 0 && rh > 0)
    {
	cvl_copy_rect(dst, drx, dry, src, srx, sry, rw, rh);
    }
}
