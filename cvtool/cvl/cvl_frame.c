/*
 * cvl_frame.c
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
 * \file cvl_frame.h
 * \brief Frame creation and manipulation.
 *
 * Frame creation and manipulation.
 */

#include "config.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_taglist.h"
#include "cvl/cvl_gl.h"
#include "cvl/cvl_frame.h"


/**
 * \typedef cvl_format_t
 * Format of a frame.
 */
/** \var CVL_LUM
 * Luminance values. */
/** \var CVL_XYZ
 * CIE XYZ color space. */
/** \var CVL_RGB
 * RGB color space. */
/** \var CVL_HSL
 * HSL color space. H = 0/360 is red, 120/360 is green, and 240/360 is blue. */
/** \var CVL_UNKNOWN
 * Unspecified or unknown color space. */

/**
 * \typedef cvl_type_t
 * Type of a frame.
 */
/** \var CVL_UINT8
 * 8bit unsigned integer. */
/** \var CVL_FLOAT
 * 32bit IEEE floats. */
/** \var CVL_FLOAT16
 * A 16bit floating point format used by the graphics hardware.
 * This can only be used for frame data in textures. It will be handled as
 * normal floats in main memory. */

/**
 * \typedef cvl_storage_t
 * Storage area of a frame. Note that both memory and texture representations
 * may be valid at the same time.
 */
/** \var CVL_MEM
 * In-memory representation. */
/** \var CVL_TEXTURE
 * Texture representation. */

/**
 * \struct cvl_frame_t
 * \brief A frame.
 *
 * Structure that stores a frame.
 */


/**
 * \param width		Frame width.
 * \param height	Frame height.
 * \param channels	Number of channels.
 * \param format	Frame format.
 * \param type		Frame type.
 * \param storage	Frame storage.
 *
 * Creates a new frame with the given \a width and \a height. Each pixel will
 * consist of 1-4 values (depending on the \a channels parameter) of the given
 * \a type. The pixels will be interpreted in the given \a format. Note that if
 * the number of channels is less than what would be expected for the given
 * format, the results might be unexpected. A larger number of channels is
 * ok. The initial storage space for the frame can be chosen with the
 * \a storage parameter.
 */
cvl_frame_t *cvl_frame_new(int width, int height, int channels, cvl_format_t format, cvl_type_t type, cvl_storage_t storage)
{
    cvl_assert(width > 0);
    cvl_assert(height > 0);
    cvl_assert(channels >= 1 && channels <= 4);
    if (cvl_error())
	return NULL;

    cvl_context_t *ctx = cvl_context();
    cvl_frame_t *frame;
    
    if (!(frame = malloc(sizeof(cvl_frame_t))))
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	return NULL;
    }
    if (width > ctx->cvl_gl_max_tex_size || height > ctx->cvl_gl_max_tex_size)
    {
	cvl_error_set(CVL_ERROR_GL, "frame size %dx%d is too large: OpenGL implementation limit is %dx%d", 
		width, height, ctx->cvl_gl_max_tex_size, ctx->cvl_gl_max_tex_size);
	return NULL;
    }
    frame->taglist = cvl_taglist_new();
    frame->width = width;
    frame->height = height;
    frame->channels = channels;
    frame->channel_names[0] = NULL;
    frame->channel_names[1] = NULL;
    frame->channel_names[2] = NULL;
    frame->channel_names[3] = NULL;
    frame->format = format;
    frame->type = type;

    if (storage == CVL_MEM)
    {
	size_t typesize = (type == CVL_UINT8 ? sizeof(uint8_t) : sizeof(float));
	size_t memchannels = (format == CVL_LUM ? 1 : format == CVL_UNKNOWN ? 4 : 3);
	if (!(frame->ptr = malloc(width * height * memchannels * typesize)))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    return NULL;
	}
    }
    else
    {
	glGenTextures(1, &(frame->tex));
	glBindTexture(GL_TEXTURE_2D, frame->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, ctx->cvl_gl_texture_formats[type][channels - 1],
		width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	cvl_check_errors();
	frame->ptr = NULL;
    }
    return frame;
}

/**
 * \param frame		A frame.
 *
 * Creates a new frame with the same dimensions, type, channels, and format as the frame
 * \a frame. Frame tags are not copied.
 */
cvl_frame_t *cvl_frame_new_tpl(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return NULL;

    cvl_frame_t *newframe = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame),
	    cvl_frame_channels(frame), cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
    if (cvl_frame_format(frame) == CVL_UNKNOWN)
    {
	for (int c = 0; c < cvl_frame_channels(frame); c++)
	{
	    cvl_frame_set_channel_name(newframe, c, cvl_frame_channel_name(frame, c));
	}
    }
    return newframe;
}

/**
 * \param frame		The frame.
 *
 * Frees the frame and its associated resources.
 */
void cvl_frame_free(cvl_frame_t *frame)
{
    if (cvl_error())
	return;

    if (frame)
    {
	cvl_taglist_free(frame->taglist);
       	free(frame->ptr);
	glDeleteTextures(1, &(frame->tex));
	free(frame->channel_names[0]);
	free(frame->channel_names[1]);
	free(frame->channel_names[2]);
	free(frame->channel_names[3]);
	free(frame);
    }
}


/**
 * \param frame		The frame.
 * \return		The tag list.
 *
 * Returns the tag list of the frame.
 */
cvl_taglist_t *cvl_frame_taglist(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return NULL;

    return frame->taglist;
}

/**
 * \param frame		The frame.
 * \param taglist	The tag list.
 *
 * Sets the tag list of the current frame.
 */
void cvl_frame_set_taglist(cvl_frame_t *frame, cvl_taglist_t *taglist)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    cvl_taglist_free(frame->taglist);
    frame->taglist = taglist;
}

/**
 * \param frame		The frame.
 * \return		The frame width.
 *
 * Returns the width of the frame.
 */
int cvl_frame_width(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    return frame->width;
}

/**
 * \param frame		The frame.
 * \return		The frame height.
 *
 * Returns the height of the frame.
 */
int cvl_frame_height(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    return frame->height;
}

/**
 * \param frame		The frame.
 * \return		The frame size.
 *
 * Returns the number of pixels in the frame.
 */
int cvl_frame_size(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    return frame->width * frame->height;
}

/**
 * \param frame		The frame.
 * \return		The number of channels.
 *
 * Returns the number of channels in the frame.
 */
int cvl_frame_channels(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    return frame->channels;
}

/**
 * \param frame		The frame.
 * \param channel	The channel.
 * \return		The name of the given channel.
 *
 * Returns the name of the given \a channel. The channel must exist.\n
 * The names for known frame formats are single letters, for example
 * "R", "G", and "B" for #CVL_RGB. For #CVL_LUM, the name of the channel is 
 * "Y". If the frame format is #CVL_UNKNOWN, the channel names can be arbitrary
 * strings or NULL if there is no associated name.
 */
const char *cvl_frame_channel_name(const cvl_frame_t *frame, int channel)
{
    cvl_assert(frame != NULL);
    cvl_assert(channel >= 0);
    cvl_assert(channel < cvl_frame_channels(frame));
    if (cvl_error())
	return NULL;

    if (cvl_frame_format(frame) == CVL_LUM)
    {
	return "Y";
    }
    else if (cvl_frame_format(frame) == CVL_XYZ)
    {
	return (channel == 0 ? "X" : channel == 1 ? "Y" : "Z");
    }
    else if (cvl_frame_format(frame) == CVL_RGB)
    {
	return (channel == 0 ? "R" : channel == 1 ? "G" : "B");
    }
    else if (cvl_frame_format(frame) == CVL_HSL)
    {
	return (channel == 0 ? "H" : channel == 1 ? "S" : "L");
    }
    else // CVL_UNKNOWN
    {
	return frame->channel_names[channel];
    }
}

/**
 * \param frame		The frame.
 * \param channel	The channel.
 * \param name		The name of the given channel.
 *
 * Sets the name of the given \a channel of the frame \a frame. 
 * The channel must exist. Settings channel names is only allowed for frames of
 * format #CVL_UNKNOWN. A name that is NULL deletes a previous name.
 */
void cvl_frame_set_channel_name(cvl_frame_t *frame, int channel, const char *name)
{
    cvl_assert(frame != NULL);
    cvl_assert(cvl_frame_format(frame) == CVL_UNKNOWN);
    cvl_assert(channel >= 0);
    cvl_assert(channel < cvl_frame_channels(frame));
    if (cvl_error())
	return;

    free(frame->channel_names[channel]);
    frame->channel_names[channel] = NULL;
    if (name)
	frame->channel_names[channel] = cvl_strdup(name);
}

/**
 * \param frame		The frame.
 * \return		The frame format.
 *
 * Returns the format of the frame.
 */
cvl_format_t cvl_frame_format(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    return frame->format;
}

/**
 * \param frame		The frame.
 * \param format	The frame format.
 *
 * Sets the format of the frame.
 */
void cvl_frame_set_format(cvl_frame_t *frame, cvl_format_t format)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    (void)cvl_frame_texture(frame);
    frame->format = format;
    if (format == CVL_LUM)
	frame->channels = 1;
    else if (format != CVL_UNKNOWN)
	frame->channels = 3;
}

/**
 * \param frame		The frame.
 * \return		The frame type.
 *
 * Returns the type of the frame.
 */
cvl_type_t cvl_frame_type(const cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    return frame->type;
}

/**
 * \param frame		The frame.
 * \param type		The new type.
 *
 * Converts the frame \a frame to the new type \a type.
 */
void cvl_frame_set_type(cvl_frame_t *frame, cvl_type_t type)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    if (type != frame->type)
    {
	cvl_frame_t *tmpframe = cvl_frame_new(
		cvl_frame_width(frame), cvl_frame_height(frame), cvl_frame_channels(frame),
		cvl_frame_format(frame), type, CVL_TEXTURE);
	glUseProgram(0);
	cvl_transform(tmpframe, frame);
	frame->type = type;
	GLuint tmptex = frame->tex;
	frame->tex = tmpframe->tex;
	tmpframe->tex = tmptex;
	cvl_frame_free(tmpframe);
    }
}

/**
 * \param frame		The frame.
 * \return		A pointer to the memory representation.
 *
 * Returns a pointer to the memory representation of the frame.
 * The pointer is only valid until the next call to a CVL function.
 * For #CVL_LUM frames, the returned data is in GL_LUMINANCE format. For
 * #CVL_UNKNOWN, it is GL_RGBA. For other formats, it is GL_RGB.\n
 * Note that you will never get a two-channel memory
 * representation, and that you will always get a four-channel representation
 * for #CVL_UNKNOWN frames!
 */
void *cvl_frame_pointer(cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return NULL;

    if (!(frame->ptr))
    {
	int channels = (cvl_frame_format(frame) == CVL_LUM ? 1 
		: cvl_frame_format(frame) == CVL_UNKNOWN ? 4 : 3);
	int typesize = (cvl_frame_type(frame) == CVL_UINT8 ? sizeof(uint8_t) : sizeof(float));
	if (!(frame->ptr = malloc(cvl_frame_size(frame) * channels * typesize)))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    return NULL;
	}
	GLint glformat = (cvl_frame_format(frame) == CVL_LUM ? GL_LUMINANCE 
		: cvl_frame_format(frame) == CVL_UNKNOWN ? GL_RGBA : GL_RGB);
	GLint gltype = (cvl_frame_type(frame) == CVL_UINT8 ? GL_UNSIGNED_BYTE : GL_FLOAT);
	glBindTexture(GL_TEXTURE_2D, frame->tex);
	glGetTexImage(GL_TEXTURE_2D, 0, glformat, gltype, frame->ptr);
	glDeleteTextures(1, &(frame->tex));
	cvl_check_errors();
    }

    return frame->ptr;
}

/**
 * \param frame		The frame.
 * \return		The frame texture.
 *
 * Returns the GL texture of the frame. The texture handle is only valid until
 * the next call to a CVL function.
 */
GLuint cvl_frame_texture(cvl_frame_t *frame)
{
    cvl_assert(frame != NULL);
    if (cvl_error())
	return 0;

    if (frame->ptr)
    {
	cvl_context_t *ctx = cvl_context();
	GLint glformat = (cvl_frame_format(frame) == CVL_LUM ? GL_LUMINANCE 
		: cvl_frame_format(frame) == CVL_UNKNOWN ? GL_RGBA : GL_RGB);
	int channels = (cvl_frame_format(frame) == CVL_LUM ? 1 
		: cvl_frame_format(frame) == CVL_UNKNOWN ? 4 : 3);
	GLint gltype = (cvl_frame_type(frame) == CVL_UINT8 ? GL_UNSIGNED_BYTE : GL_FLOAT);
	cvl_type_t type = cvl_frame_type(frame);

	glGenTextures(1, &(frame->tex));
	glBindTexture(GL_TEXTURE_2D, frame->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, ctx->cvl_gl_texture_formats[type][channels - 1],
		cvl_frame_width(frame), cvl_frame_height(frame), 0,
		glformat, gltype, frame->ptr);
	cvl_check_errors();
	free(frame->ptr);
	frame->ptr = NULL;
    }

    return frame->tex;
}

/**
 * \param dst		The destination frame.
 * \param src		The source frame.
 *
 * Renders the frame \a src 1:1 into the frame \a dst.
 * This is used for many kinds of frame manipulations that can be done with a
 * fragment shader. After activating the GL program with the chosen fragment 
 * shader, this function does the necessary quad rendering. 
 */
void cvl_transform(cvl_frame_t *dst, cvl_frame_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    if (cvl_error())
	return;

    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(dst));
    cvl_gl_set_texture_state();
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
	    GL_TEXTURE_2D, cvl_frame_texture(dst), 0);
    glViewport(0, 0, cvl_frame_width(dst), cvl_frame_height(dst));
    glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(src));
    cvl_gl_set_texture_state();
    glDrawArrays(GL_QUADS, 0, 4);

    cvl_check_errors();
}

/**
 * \param dsts		The destination frames.
 * \param ndsts		The number of destination frames.
 * \param srcs		The source frames.
 * \param nsrcs		The number of source frames.
 * \param textures_name	Name of the textures uniform variable array.
 *
 * Renders the frames \a srcs 1:1 into the frames \a dsts, using Multitexturing
 * and Multiple Render Targets.\n
 * The active program must be able to handle this situation. The name of the
 * uniform variable array that stores the textures must be given in \a
 * textures_names, so that this function can fill the right values in.\n
 * See also cvl_frame_transform().
 */
void cvl_transform_multi(cvl_frame_t **dsts, int ndsts, cvl_frame_t **srcs, int nsrcs, const char *textures_name)
{
    cvl_assert(dsts != NULL);
    cvl_assert(ndsts > 0);
    cvl_assert(srcs != NULL);
    cvl_assert(nsrcs > 0);
    cvl_assert(textures_name != NULL);
    if (cvl_error())
	return;

    cvl_context_t *ctx = cvl_context();

    // Check if we can perform the operation
    if (ndsts > ctx->cvl_gl_max_render_targets)
    {
	cvl_error_set(CVL_ERROR_GL, "%d render targets needed, but only %d available",
		ndsts, ctx->cvl_gl_max_render_targets);
    }
    if (nsrcs > ctx->cvl_gl_max_texture_units)
    {
	cvl_error_set(CVL_ERROR_GL, "%d texture units needed, but only %d available",
		nsrcs, ctx->cvl_gl_max_texture_units);
    }

    // Setup destinations
    GLuint draw_buffers[ndsts];
    for (int i = 0; i < ndsts; i++)
    {
	glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(dsts[i]));
	cvl_gl_set_texture_state();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, 
		GL_TEXTURE_2D, cvl_frame_texture(dsts[i]), 0);
	draw_buffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
    }
    glDrawBuffers(ndsts, draw_buffers);

    // Setup sources
    GLint textures[nsrcs];
    for (int i = 0; i < nsrcs; i++)
    {
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(srcs[i]));
	cvl_gl_set_texture_state();
	textures[i] = i;
    }
    GLint prg;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prg);
    glUniform1iv(glGetUniformLocation(prg, textures_name), nsrcs, textures);

    // Render
    glViewport(0, 0, cvl_frame_width(dsts[0]), cvl_frame_height(dsts[0]));
    glDrawArrays(GL_QUADS, 0, 4);

    // Cleanup
    cvl_check_errors();
    glActiveTexture(GL_TEXTURE0);
    glDrawBuffers(1, draw_buffers);
}
