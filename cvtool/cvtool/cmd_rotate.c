/*
 * cmd_rotate.c
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
#include <limits.h>
#include <float.h>

#include <cvl/cvl.h>

#include "options.h"


void cmd_rotate_print_help(void)
{
    cvl_msg_fmt_req(
	    "rotate -a|--angle=<angle> [-c|--color=<color>] [-i|--interpolation=none|bilinear]\n"
	    "\n"
	    "Rotate frames with the given angle (in degrees), counterclockwise. "
	    "The dimensions of the rotated frame will be big enough to hold all "
	    "informations from the source. Holes will be filled with the given "
	    "color; the default is black. The default interpolation type is bilinear "
	    "(simple rotations (90, 180, or 270) do not need interpolation).");
}


int cmd_rotate(int argc, char *argv[])
{
    option_double_t angle = { 0.0, -DBL_MAX, true, DBL_MAX, true };
    option_color_t color = { CVL_COLOR_BLACK };
    const char *interpolation_names[] = { "none", "bilinear", NULL };
    option_name_t interpolation = { 1, interpolation_names };
    option_t options[] = 
    {
	{ "angle",         'a', OPTION_DOUBLE, &angle,         true },
	{ "color",         'c', OPTION_COLOR, &color,         false },
	{ "interpolation", 'i', OPTION_NAME,  &interpolation, false },
	null_option
    };
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *rotated_frame;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    angle.value = cvl_angle_normalize_1(cvl_deg_to_rad(angle.value));
	
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
    	rotated_frame = cvl_rotate(frame, interpolation.value, color.value, angle.value); 
	cvl_frame_free(frame);
	cvl_io_info_set_width(output_info, cvl_frame_width(rotated_frame));
	cvl_io_info_set_height(output_info, cvl_frame_height(rotated_frame));
	if (!cvl_io_write(stdout, output_info, rotated_frame))
	{
	    cvl_frame_free(rotated_frame);
	    error = true;
	    break;
	}
	cvl_frame_free(rotated_frame);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
