/*
 * cmd_visualize.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007  Martin Lambers <marlam@marlam.de>
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

#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_visualize_print_help(void)
{
    mh_msg_fmt_req(
	    "visualize scalar [-p|--pseudo-color] [-m|--min=<m>] [-M|--max=<M>] [-l|--log=<base>]\n"
	    "visualize vector2 -m|--mode=color\n"
	    "visualize vector2 -m|--mode=needle [-x|--sample-x=<x>] [-y|--sample-y=<y>] "
	    "[-X|--dist-x=<dx>] [-Y|--dist-y=<dy>] [-f|--factor=<f>]\n"
	    "\n"
	    "visualize scalar: Visualizes scalar values by transforming values from [m,M] to [0,1] and "
	    "writing the result as graylevel frames. M and m are automatically determined from the input "
	    "if they are not given. By default, the transformation is linear. If --log is given, then "
	    "the transformation will use the logarithm with the given base. If --pseudo-color is given, then "
	    "pseudo colors are used instead of gray levels.\n"
	    "visualize vector2: "
	    "Reads vector fields as produced by other commands "
	    "such as opticalflow, and visualizes them.\n"
	    "Visualization as colors: Each of the x,y,z components, which range from -1 to 1, are "
	    "transformed to R,G,B values that range from 0 to 1.\n"
	    "Visualization as needle diagrams:\n"
	    "Every x-th vector in horizontal direction and every y-th vector in vertical "
	    "direction will be represented by a needle. The needles will have a distance of "
	    "dx pixels in horizontal and dy pixels in vertical direction. The needle length "
	    "is the length of the vector after it was scaled with the factor f. "
	    "The default values are x=y=dx=dy=10, f=1.0.");
}


int cmd_visualize(int argc, char *argv[])
{
    enum subcommand_t { VIS_SCALAR, VIS_VECTOR2 };
    int subcommand;
    mh_option_float_t scalar_m = { FLT_MAX, -FLT_MAX, true, FLT_MAX, false };
    mh_option_float_t scalar_M = { FLT_MAX, -FLT_MAX, true, FLT_MAX, false };
    mh_option_float_t scalar_l = { -1.0, 2.0, true, FLT_MAX, true };
    mh_option_bool_t scalar_p = { false, true };
    mh_option_t scalar_options[] =
    {
	{ "pseudo-color", 'p', MH_OPTION_BOOL,  &scalar_p, false },
	{ "min",          'm', MH_OPTION_FLOAT, &scalar_m, false },
	{ "max",          'M', MH_OPTION_FLOAT, &scalar_M, false },
	{ "log",          'l', MH_OPTION_FLOAT, &scalar_l, false },
	mh_option_null
    };
    const char *mode_names[] = { "color", "needle", NULL };
    mh_option_name_t mode = { -1, mode_names };
    mh_option_int_t sample_x = { 10, 1, INT_MAX };
    mh_option_int_t sample_y = { 10, 1, INT_MAX };
    mh_option_int_t dist_x = { 10, 1, INT_MAX };
    mh_option_int_t dist_y = { 10, 1, INT_MAX };
    mh_option_float_t factor = { 1.0, -FLT_MAX, true, FLT_MAX, true };
    mh_option_t vector2_options[] = 
    {
	{ "mode",     'm', MH_OPTION_NAME,  &mode,     false },
	{ "sample-x", 'x', MH_OPTION_INT,   &sample_x, false },
	{ "sample-y", 'y', MH_OPTION_INT,   &sample_y, false },
	{ "dist-x",   'X', MH_OPTION_INT,   &dist_x,   false },
	{ "dist-y",   'Y', MH_OPTION_INT,   &dist_y,   false },
	{ "factor",   'f', MH_OPTION_FLOAT, &factor,   false },
	mh_option_null
    };
    cvl_frame_t *frame, *vis;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!argv[1])
    {
	mh_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "scalar") == 0)
    {
	subcommand = VIS_SCALAR;
	mh_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !mh_getopt(argc - 1, &(argv[1]), scalar_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "vector2") == 0)
    {
	subcommand = VIS_VECTOR2;
	mh_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !mh_getopt(argc - 1, &(argv[1]), vector2_options, 0, 0, NULL);
    }
    else
    {
	mh_msg_err("unknown subcommand");
	error = true;
    }
    if (error)
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, NULL, &frame);
	if (!frame)
	    break;
	if (subcommand == VIS_SCALAR)
	{
	    float min = scalar_m.value;
	    if (min >= FLT_MAX)
	    {
		cvl_reduce(frame, CVL_REDUCE_MIN, 0, &min);
	    }
	    float max = scalar_M.value;
	    if (max >= FLT_MAX)
	    {
		cvl_reduce(frame, CVL_REDUCE_MAX, 0, &max);
	    }

	    mh_msg_dbg("min = %+.4f, max = %+.4f", min, max);

	    vis = cvl_frame_new_tpl(frame);
	    if (scalar_l.value < 0.0)
	    {
		cvl_transform_linear(vis, frame, -1, min, max);
	    }
	    else
	    {
		cvl_transform_log(vis, frame, -1, min, max, scalar_l.value);
	    }
	    if (scalar_p.value)
	    {
		cvl_frame_t *tmp = cvl_frame_new(cvl_frame_width(vis), cvl_frame_height(vis), 
			3, CVL_HSL, CVL_UINT8, CVL_TEXTURE);
		cvl_pseudo_color(tmp, vis, 0, false, 0.0f, 0.0f, 0.0f, 1.0f);
		cvl_frame_free(vis);
		vis = tmp;
	    }
	    cvl_frame_set_type(vis, CVL_UINT8);
	}
	else // subcommand == VIS_VECTOR2
	{
	    if (mode.value == 0)
	    {
		vis = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame),
			3, CVL_RGB, CVL_UINT8, CVL_TEXTURE);
    		cvl_visualize_vector2_color(vis, frame);
	    }
	    else
	    {
		vis = cvl_visualize_vector2_needle(frame, sample_x.value, sample_y.value, 
		     	dist_x.value, dist_y.value, factor.value);
	    }
	}
	cvl_frame_free(frame);
	cvl_write(stdout, CVL_PNM, vis);
	cvl_frame_free(vis);
    }

    return cvl_error() ? 1 : 0;
}
