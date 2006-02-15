/*
 * cvl_io.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006  Martin Lambers <marlam@marlam.de>
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
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CVL_IO_H
#define CVL_IO_H

#include <stdio.h>
#include <stdbool.h>

#include "cvl_frame.h"

typedef enum
{
    CVL_IO_STREAM_Y4M = 0,
    CVL_IO_STREAM_PNM = 1
} cvl_io_stream_t;

typedef enum
{
    CVL_Y4M_CHROMA_UNKNOWN = 0,
    CVL_Y4M_CHROMA_420JPEG = 1,
    CVL_Y4M_CHROMA_444 = 2
} cvl_y4m_chroma_t;

typedef enum
{
    CVL_Y4M_INTERLACING_UNKNOWN = 0,
    CVL_Y4M_INTERLACING_PROGRESSIVE = 1,
} cvl_y4m_interlacing_t;

typedef struct cvl_io_info_t
{
    cvl_io_stream_t _stream_type;
    int _frame_counter;
    int _width;
    int _height;
    cvl_y4m_chroma_t _chroma;
    cvl_y4m_interlacing_t _interlacing;
    int _framerate1;
    int _framerate2;
    int _aspectratio1;
    int _aspectratio2;
    /* only for input: */
    bool _input_info_known;
    /* only for output: */
    const struct cvl_io_info_t *_input_info;
    bool _output_info_written;
} cvl_io_info_t;

cvl_io_info_t *cvl_io_info_new(void);
void cvl_io_info_free(cvl_io_info_t *info);

int cvl_io_info_frame_counter(cvl_io_info_t *info);
cvl_io_stream_t cvl_io_info_stream_type(cvl_io_info_t *info);
void cvl_io_info_set_stream_type(cvl_io_info_t *output_info, cvl_io_stream_t stream_type);
int cvl_io_info_width(cvl_io_info_t *info);
void cvl_io_info_set_width(cvl_io_info_t *output_info, int width);
int cvl_io_info_height(cvl_io_info_t *info);
void cvl_io_info_set_height(cvl_io_info_t *output_info, int height);
cvl_y4m_chroma_t cvl_io_info_chroma(cvl_io_info_t *info);
void cvl_io_info_set_chroma(cvl_io_info_t *output_info, cvl_y4m_chroma_t chroma);
cvl_y4m_interlacing_t cvl_io_info_interlacing(cvl_io_info_t *info);
void cvl_io_info_set_interlacing(cvl_io_info_t *output_info, cvl_y4m_interlacing_t interlacing);
void cvl_io_info_framerate(cvl_io_info_t *info, int *framerate1, int *framerate2);
void cvl_io_info_set_framerate(cvl_io_info_t *output_info, int framerate1, int framerate2);
void cvl_io_info_aspectratio(cvl_io_info_t *info, int *aspectratio1, int *aspectratio2);
void cvl_io_info_set_aspectratio(cvl_io_info_t *output_info, int aspectratio1, int aspectratio2);
void cvl_io_info_link_output_to_input(cvl_io_info_t *output_info, const cvl_io_info_t *input_info);

bool cvl_io_eof(FILE *f, const cvl_io_info_t *input_info);
bool cvl_io_read(FILE *f, cvl_io_info_t *input_info, cvl_frame_t **frame);
bool cvl_io_write(FILE *f, cvl_io_info_t *output_info, const cvl_frame_t *frame);

bool cvl_io_read_pnm(FILE *f, cvl_frame_t **frame);
bool cvl_io_write_pnm(FILE *f, const cvl_frame_t *frame);
bool cvl_io_load_pnm(const char *filename, cvl_frame_t **frame);
bool cvl_io_save_pnm(const char *filename, const cvl_frame_t *frame);

#endif
