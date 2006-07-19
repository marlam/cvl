/*
 * cmd_create.c
 * 
 * This file is part of cvtool, a computer vision tool.
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

#include "config.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#include <cvl/cvl.h>


void cmd_create_print_help(void)
{
    cvl_msg_fmt_req(
	    "create [-t|--type=gray|rgb|yuv] [-n|--n=<n>] -w|--width=<w> -h|--height=<h> "
	    "[-c|--color=<color>] "
	    "[-C|--chroma=420jpeg|444] [-F|--framerate=<f1>:<f2>] [-A|--aspect-ratio=<a1>:<a2>]\n"
	    "\n"
	    "Create n (default 1) frames of pixel type gray, rgb or yuv (default "
	    "rgb), with the given width and height, filled with the given color (default "
	    "black). The resulting stream type will be pnm for gray and rgb frames, and "
	    "y4m for yuv frames. The chroma subsampling, frame rate, and aspect ratio "
	    "information is only relevant for the yuv type; it will be silently ignored "
	    "for the other types.");
}


int cmd_create(int argc, char *argv[])
{
    const char *type_names[] = { "gray", "rgb", "yuv", NULL };
    cvl_option_name_t t = { 1, type_names };
    cvl_option_int_t n = { 1, 1, INT_MAX };
    cvl_option_int_t w = { 0, 1, INT_MAX };
    cvl_option_int_t h = { 0, 1, INT_MAX };
    cvl_option_color_t c = { CVL_COLOR_BLACK };
    const char *chroma_names[] = { "420jpeg", "444", NULL };
    cvl_option_name_t C = { 0, chroma_names };
    cvl_option_ratio_t F = { 0, 0 };
    cvl_option_ratio_t A = { 0, 0 };
    cvl_option_t options[] =
    {
	{ "type",         't', CVL_OPTION_NAME,  &t, false },
	{ "n",            'n', CVL_OPTION_INT,   &n, false },
	{ "width",        'w', CVL_OPTION_INT,   &w, true },
	{ "height",       'h', CVL_OPTION_INT,   &h, true },
	{ "color",        'c', CVL_OPTION_COLOR, &c, false },
	{ "chroma",       'C', CVL_OPTION_NAME,  &C, false },
	{ "framerate",    'F', CVL_OPTION_RATIO, &F, false },
	{ "aspect-ratio", 'A', CVL_OPTION_RATIO, &A, false },
	cvl_option_null
    };
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_pixel_t p;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    
    output_info = cvl_io_info_new();
    cvl_io_info_set_stream_type(output_info, 
	    t.value == CVL_PIXEL_YUV ? CVL_IO_STREAM_Y4M : CVL_IO_STREAM_PNM);
    cvl_io_info_set_width(output_info, w.value);
    cvl_io_info_set_height(output_info, h.value);
    cvl_io_info_set_chroma(output_info, C.value == 0 ? CVL_Y4M_CHROMA_420JPEG : CVL_Y4M_CHROMA_444);
    cvl_io_info_set_framerate(output_info, F.value1, F.value2);
    cvl_io_info_set_aspectratio(output_info, A.value1, A.value2);
    p = cvl_color_to_pixel(c.value, t.value);
    
    error = false;
    for (int i = 0; i < n.value; i++)
    {
	frame = cvl_frame_new(t.value, w.value, h.value);
	cvl_frame_fill_rect(frame, 0, 0, w.value, h.value, p);	
	if (!cvl_io_write(stdout, output_info, frame))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_frame_free(frame);
    }

    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
