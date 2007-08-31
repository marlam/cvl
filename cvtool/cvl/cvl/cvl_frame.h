/*
 * cvl_frame.h
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

#ifndef CVL_FRAME_H
#define CVL_FRAME_H

#include <GL/gl.h>

#include "cvl_taglist.h"

typedef enum
{
    CVL_LUM	= 0,
    CVL_XYZ	= 1,
    CVL_RGB	= 2,
    CVL_HSL 	= 3,
    CVL_UNKNOWN	= 4
} cvl_format_t;

typedef enum
{
    CVL_UINT8 = 0,
    CVL_FLOAT = 1
} cvl_type_t;

typedef enum
{
    CVL_MEM = 0,
    CVL_TEXTURE = 1
} cvl_storage_t;

typedef struct
{
    cvl_taglist_t *taglist;
    int width;
    int height;
    int channels;
    char *channel_names[4];
    cvl_format_t format;
    cvl_type_t type;
    void *ptr;
    GLuint tex;
} cvl_frame_t;

cvl_frame_t *cvl_frame_new(int width, int height, int channels, cvl_format_t format, cvl_type_t type, cvl_storage_t storage);
cvl_frame_t *cvl_frame_new_tpl(const cvl_frame_t *tpl);
void cvl_frame_free(cvl_frame_t *frame);

cvl_taglist_t *cvl_frame_taglist(const cvl_frame_t *frame);
void cvl_frame_set_taglist(cvl_frame_t *frame, cvl_taglist_t *taglist);

int cvl_frame_width(const cvl_frame_t *frame);
int cvl_frame_height(const cvl_frame_t *frame);
int cvl_frame_size(const cvl_frame_t *frame);
int cvl_frame_channels(const cvl_frame_t *frame);
const char *cvl_frame_channel_name(const cvl_frame_t *frame, int channel);
void cvl_frame_set_channel_name(cvl_frame_t *frame, int channel, const char *name);
cvl_format_t cvl_frame_format(const cvl_frame_t *frame);
void cvl_frame_set_format(cvl_frame_t *frame, cvl_format_t format);
cvl_type_t cvl_frame_type(const cvl_frame_t *frame);
void cvl_frame_set_type(cvl_frame_t *frame, cvl_type_t type);
void *cvl_frame_pointer(cvl_frame_t *frame);
GLuint cvl_frame_texture(cvl_frame_t *frame);

void cvl_transform(cvl_frame_t *dst, cvl_frame_t *src);
void cvl_transform_multi(cvl_frame_t **dsts, int ndsts, cvl_frame_t **srcs, int nsrcs, const char *textures_name);

#endif
