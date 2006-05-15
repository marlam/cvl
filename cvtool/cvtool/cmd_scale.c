/*
 * cmd_scale.c
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


void cmd_scale_print_help(void)
{
    cvl_msg_fmt_req(
	    "scale [-w|--width=<width>] [-h|--height=<height>] [-i|--interpolation=none|bilinear]\n"
	    "scale -x|--factor-x=<factor-x> -y|--factor-y=<factor-y> "
	    "[-i|--interpolation=none|bilinear]\n"
	    "scale -f|--factor=<factor> [-i|--interpolation=none|bilinear]\n"
	    "\n"
	    "Scale frames to new size. The default interpolation type is bilinear.\n"
	    "First form: Give new width and/or height. If one value is missing, it is "
	    "computed from the other so that the aspect ratio remains the same.\n"
	    "Second form: Give scale factors for width and height.\n"
	    "Third form: Give one scale factor for both width and height.");
}


int cmd_scale(int argc, char *argv[])
{
    option_int_t width = { 0, 1, INT_MAX };
    option_int_t height = { 0, 1, INT_MAX };
    option_double_t factor_x = { -1.0, 0.0, false, DBL_MAX, true };
    option_double_t factor_y = { -1.0, 0.0, false, DBL_MAX, true };
    option_double_t factor = { -1.0, 0.0, false, DBL_MAX, true };
    const char *interpolation_names[] = { "none", "bilinear", NULL };
    option_name_t interpolation = { 1, interpolation_names };
    option_t options[] = 
    {
        { "width",         'w', OPTION_INT,    &width,         false },
	{ "height",        'h', OPTION_INT,    &height,        false },
	{ "factor-x",      'x', OPTION_DOUBLE, &factor_x,      false },
	{ "factor-y",      'y', OPTION_DOUBLE, &factor_y,      false },
	{ "factor",        'f', OPTION_DOUBLE, &factor,        false },
	{ "interpolation", 'i', OPTION_NAME,   &interpolation, false },
	null_option
    };
    bool absolute_size = false;
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *scaled_frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    error = !cvtool_getopt(argc, argv, options, 0, 0, NULL);
    if (!error)
    {
	if (width.value > 0 || height.value > 0)
	{
	    absolute_size = true;
	    if (factor_x.value > 0.0 || factor_y.value > 0.0 || factor.value > 0.0)
	    {
		cvl_msg_err("invalid mix of options");
		error = true;
	    }
	}
	else if (factor_x.value > 0.0 && factor_y.value > 0.0)
	{
	    absolute_size = false;
	    if (width.value > 0 || height.value > 0 || factor.value > 0.0)
	    {
		cvl_msg_err("invalid mix of options");
		error = true;
	    }
	}
	else if (factor.value > 0.0)
	{
	    absolute_size = false;
	    if (width.value > 0 || height.value > 0 
		    || factor_x.value > 0.0 || factor_y.value > 0.0)
	    {
		cvl_msg_err("invalid mix of options");
		error = true;
	    }
	    factor_x.value = factor.value;
	    factor_y.value = factor.value;
	}
	else
	{
	    cvl_msg_err("incomplete information");
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }
	

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    error = false;
    while (!cvl_io_eof(stdin, input_info))
    {
	int new_width, new_height;

	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	if (!absolute_size)
	{
	    new_width = cvl_maxi(1, cvl_iround((double)cvl_frame_width(frame) * factor_x.value));
	    new_height = cvl_maxi(1, cvl_iround((double)cvl_frame_height(frame) * factor_y.value));
	}
	else
	{
	    new_width = width.value;
	    new_height = height.value;
	    if (new_width <= 0)
	    {
		double factor = (double)new_height / (double)cvl_frame_height(frame);
		new_width = cvl_maxi(1, cvl_iround(factor * (double)cvl_frame_width(frame)));
	    }
	    else if (new_height <= 0)
	    {
		double factor = (double)new_width / (double)cvl_frame_width(frame);
		new_height = cvl_maxi(1, cvl_iround(factor * (double)cvl_frame_height(frame)));
	    }
	}
	scaled_frame = cvl_scale(frame, interpolation.value, new_width, new_height);
	cvl_frame_free(frame);
	cvl_io_info_set_width(output_info, cvl_frame_width(scaled_frame));
	cvl_io_info_set_height(output_info, cvl_frame_height(scaled_frame));
	if (!cvl_io_write(stdout, output_info, scaled_frame))
	{
	    cvl_frame_free(scaled_frame);
	    error = true;
	    break;
	}
	cvl_frame_free(scaled_frame);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
