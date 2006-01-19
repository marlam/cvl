/*
 * cmd_grid.c
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
#include <string.h>
#include <float.h>
#include <limits.h>

#include <cvl/cvl.h>

#include "options.h"


void cmd_grid_print_help(void)
{
    cvl_msg_fmt_req(
	    "grid -f|--factor=<factor> -w|--width=<w> -h|--height=<h>\n"
	    "\n"
	    "Compute grid images from the input images, which are converted to gray "
	    "and interpreted as depth maps. <w> and <h> are the horizontal and vertical "
	    "grid distances in pixels. The minimum depth (gray value 255) will result in "
	    "a vertical shift of (factor * inputwidth) pixels; the output images will be "
	    "this much higher than the input images.");
}


int cmd_grid(int argc, char *argv[])
{
    option_double_t factor = { -1.0, 0.0, false, DBL_MAX, true };
    option_int_t width = { -1, 1, INT_MAX };
    option_int_t height = { -1, 1, INT_MAX };
    option_t options[] = 
    {
	{ "factor", 'f', OPTION_DOUBLE, &factor, true },
	{ "width",  'w', OPTION_INT,   &width,  true },
	{ "height", 'h', OPTION_INT,   &height, true },
	null_option
    };	
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *gridframe;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    
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
	cvl_frame_to_gray(frame);
	gridframe = cvl_frame_grid(frame, factor.value, width.value, height.value);
	cvl_frame_free(frame);
	cvl_io_info_set_width(output_info, cvl_frame_width(gridframe));
	cvl_io_info_set_height(output_info, cvl_frame_height(gridframe));
	if (!cvl_io_write(stdout, output_info, gridframe))
	{
	    cvl_frame_free(gridframe);
	    error = true;
	    break;
	}
	cvl_frame_free(gridframe);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
