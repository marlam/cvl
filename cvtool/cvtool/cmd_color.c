/*
 * cmd_color.c
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
#include <math.h>
#include <float.h>

#include <cvl/cvl.h>

#include "options.h"


void cmd_color_print_help(void)
{
    cvl_msg_fmt_req(
	    "color [-h|--hue=<h>] [-s|--saturation=<s>] [-l|--lightness=<l>] "
	    "[-c|--contrast=<c>] [-g|--gamma=(<g>|<gr>,<gg>,<gb>)]\n"
	    "\n"
	    "Color adjustment.\n"
	    "Hue, saturation, lightness, and constrast are manipulated in the HSL "
	    "(Hue, Saturation, Lightness) color space. h is an additive constant to "
	    "the hue angle, in degrees. s, l, c measure the relative change in "
	    "saturation, lightness, contrast: -1 means the result will be zero, 0 means "
	    "the result will be the same as the original, and +1 means that the result "
	    "will be two times as high as the original. Values greater than +1 are possible. "
	    "For example, s = -1 will convert the input images to graylevels.\n"
	    "Gamma correction (--gamma) is applied to the gray channel for gray frames, "
	    "to the Y channel for YUV frames, and to the R,G,B channels for RGB frames. "
	    "If three gamma values are given, the frames are always converted to RGB, "
	    "then gamma corrected for each channel separately, and then converted back "
	    "to their original type. All gamma values must be greater than zero.");
}


int cmd_color(int argc, char *argv[])
{
    option_double_t h = { 0.0, -DBL_MAX, true, DBL_MAX, true };
    option_double_t s = { 0.0, -1.0, true, DBL_MAX, true };
    option_double_t l = { 0.0, -1.0, true, DBL_MAX, true };
    option_double_t c = { 0.0, -1.0, true, DBL_MAX, true };
    option_double_array_t g = { NULL, 1, NULL };
    option_t options[] = 
    { 
	{ "hue",        'h', OPTION_DOUBLE,        &h, false },
	{ "saturation", 's', OPTION_DOUBLE,        &s, false },
	{ "lightness",  'l', OPTION_DOUBLE,        &l, false },
	{ "contrast",   'c', OPTION_DOUBLE,        &c, false },
	{ "gamma",      'g', OPTION_DOUBLE_ARRAY,  &g, false },
	null_option 
    };
    bool do_color, do_gamma;
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    error = !cvtool_getopt(argc, argv, options, 0, 0, NULL);
    if (!error)
    {
	if (g.value)
	{
	    if (g.sizes[0] != 1 && g.sizes[0] != 3)
	    {
		cvl_msg_err("need either 1 or 3 values for gamma correction");
		error = true;
	    }
	    else
	    {
		do_gamma = false;
		for (int i = 0; i < g.sizes[0]; i++)
		{
		    if (g.value[i] <= 0.0)
		    {
			cvl_msg_err("gamma values must be > 0.0");
			error = true;
			break;
		    }
		    if (fabs(g.value[i] - 1.0) > 0.0001)
		    {
			do_gamma = true;
		    }
		}
	    }
	}
	else
	{
	    do_gamma = false;
	}
    }
    if (error)
    {
	return 1;
    }
    h.value = cvl_angle_normalize_2(cvl_deg_to_rad(h.value));
    do_color = (fabs(h.value) > 0.0001 
	    || fabs(s.value) > 0.0001 
	    || fabs(l.value) > 0.0001 
	    || fabs(c.value) > 0.0001);

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
	if (do_gamma)
	{
	    if (g.sizes[0] == 1)
	    {
		cvl_frame_gamma_correct(frame, g.value[0]);
	    }
	    else // g.sizes[0] == 3
	    {
		cvl_frame_gamma_correct_rgb(frame, g.value[0], g.value[1], g.value[2]);
	    }
	}
	if (do_color)
	{
	    cvl_frame_color_adjust(frame, h.value, s.value, l.value, c.value);
	}
	if (!cvl_io_write(stdout, output_info, frame))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_frame_free(frame);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
