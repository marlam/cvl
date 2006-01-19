/*
 * cmd_resize.c
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

#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#include <cvl/cvl.h>

#include "options.h"


void cmd_resize_print_help(void)
{
    cvl_msg_fmt_req(
	    "resize -w|--width=<w> -h|--height=<h> [-x|--x-offset=<x>] [-y|--y-offset=<y>] "
	    "[-c|--color=<color>]\n"
	    "\n"
	    "Resizes the frames to the given new width and height. Places the original frame "
	    "contents at the position (x,y) relative to the new frame (these offsets may be "
	    "negative). If no or an incomplete position is given, the missing part(s) are computed "
	    "so that the old contents are centered on the new frame. "
	    "Holes are filled with the given color (default is black).");
}


int cmd_resize(int argc, char *argv[])
{
    option_int_t w = { 0, 1, INT_MAX };
    option_int_t h = { 0, 1, INT_MAX };
    option_int_t x = { INT_MIN, INT_MIN + 1, INT_MAX };
    option_int_t y = { INT_MIN, INT_MIN + 1, INT_MAX };
    option_color_t color = { CVL_COLOR_BLACK };
    option_t options[] =
    {
	{ "width",      'w', OPTION_INT,   &w,     true },
	{ "height",     'h', OPTION_INT,   &h,     true },
	{ "x-offset",   'x', OPTION_INT,   &x,     false },
	{ "y-offset",   'y', OPTION_INT,   &y,     false },
	{ "color",      'c', OPTION_COLOR, &color, false },
	null_option
    };
    bool compute_x;
    bool compute_y;
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *newframe;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    compute_x = (x.value == INT_MIN);
    compute_y = (y.value == INT_MIN);

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    error = false;
    while (!cvl_io_eof(stdin, input_info))
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	if (compute_x)
	{
	    x.value = (w.value - cvl_frame_width(frame)) / 2;
	}
	if (compute_y)
	{
	    y.value = (h.value - cvl_frame_height(frame)) / 2;
	}
	newframe = cvl_frame_resize(frame, color.value, w.value, h.value, x.value, y.value);
	cvl_frame_free(frame);
	cvl_io_info_set_width(output_info, cvl_frame_width(newframe));
	cvl_io_info_set_height(output_info, cvl_frame_height(newframe));
	if (!cvl_io_write(stdout, output_info, newframe))
	{
	    cvl_frame_free(newframe);
	    error = true;
	    break;
	}
	cvl_frame_free(newframe);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
