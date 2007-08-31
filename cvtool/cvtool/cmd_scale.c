/*
 * cmd_scale.c
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


void cmd_scale_print_help(void)
{
    mh_msg_fmt_req(
	    "scale [-w|--width=<width>] [-h|--height=<height>] "
	    "[-i|--interpolation=none|bilinear|biquadratic|bicubic|bicubic-b-spline|bicubic-cr-spline]\n"
	    "scale -x|--factor-x=<factor-x> -y|--factor-y=<factor-y> "
	    "[-i|--interpolation=none|bilinear|biquadratic|bicubic|bicubic-b-spline|bicubic-cr-spline]\n"
	    "scale -f|--factor=<factor> "
	    "[-i|--interpolation=none|bilinear|biquadratic|bicubic|bicubic-b-spline|bicubic-cr-spline]\n"
	    "\n"
	    "Scale frames to new size. The default interpolation type is bilinear.\n"
	    "First form: Give new width and/or height. If one value is missing, it is "
	    "computed from the other so that the aspect ratio remains the same.\n"
	    "Second form: Give scale factors for width and height.\n"
	    "Third form: Give one scale factor for both width and height.");
}


int cmd_scale(int argc, char *argv[])
{
    mh_option_int_t width = { 0, 1, INT_MAX };
    mh_option_int_t height = { 0, 1, INT_MAX };
    mh_option_float_t factor_x = { -1.0, 0.0, false, FLT_MAX, true };
    mh_option_float_t factor_y = { -1.0, 0.0, false, FLT_MAX, true };
    mh_option_float_t factor = { -1.0, 0.0, false, FLT_MAX, true };
    const char *interpolation_names[] = { "none", "bilinear", "biquadratic", 
	"bicubic", "bicubic-b-spline", "bicubic-cr-spline", NULL };
    mh_option_name_t interpolation = { 1, interpolation_names };
    mh_option_t options[] = 
    {
        { "width",         'w', MH_OPTION_INT,   &width,         false },
	{ "height",        'h', MH_OPTION_INT,   &height,        false },
	{ "factor-x",      'x', MH_OPTION_FLOAT, &factor_x,      false },
	{ "factor-y",      'y', MH_OPTION_FLOAT, &factor_y,      false },
	{ "factor",        'f', MH_OPTION_FLOAT, &factor,        false },
	{ "interpolation", 'i', MH_OPTION_NAME,  &interpolation, false },
	mh_option_null
    };
    bool absolute_size = false;
    cvl_stream_type_t stream_type;
    cvl_frame_t *frame;
    cvl_frame_t *scaled_frame;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    error = !mh_getopt(argc, argv, options, 0, 0, NULL);
    if (!error)
    {
	if (width.value > 0 || height.value > 0)
	{
	    absolute_size = true;
	    if (factor_x.value > 0.0 || factor_y.value > 0.0 || factor.value > 0.0)
	    {
		mh_msg_err("Invalid mix of options");
		error = true;
	    }
	}
	else if (factor_x.value > 0.0 && factor_y.value > 0.0)
	{
	    absolute_size = false;
	    if (width.value > 0 || height.value > 0 || factor.value > 0.0)
	    {
		mh_msg_err("Invalid mix of options");
		error = true;
	    }
	}
	else if (factor.value > 0.0)
	{
	    absolute_size = false;
	    if (width.value > 0 || height.value > 0 
		    || factor_x.value > 0.0 || factor_y.value > 0.0)
	    {
		mh_msg_err("Invalid mix of options");
		error = true;
	    }
	    factor_x.value = factor.value;
	    factor_y.value = factor.value;
	}
	else
	{
	    mh_msg_err("Incomplete information");
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }
	
    error = false;
    while (!error)
    {
	int new_width, new_height;

	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;

	if (!absolute_size)
	{
	    new_width = mh_maxi(1, mh_iroundf((float)cvl_frame_width(frame) * factor_x.value));
	    new_height = mh_maxi(1, mh_iroundf((float)cvl_frame_height(frame) * factor_y.value));
	}
	else
	{
	    new_width = width.value;
	    new_height = height.value;
	    if (new_width <= 0)
	    {
		float factor = (float)new_height / (float)cvl_frame_height(frame);
		new_width = mh_maxi(1, mh_iroundf(factor * (float)cvl_frame_width(frame)));
	    }
	    else if (new_height <= 0)
	    {
		float factor = (float)new_width / (float)cvl_frame_width(frame);
		new_height = mh_maxi(1, mh_iroundf(factor * (float)cvl_frame_height(frame)));
	    }
	}
	scaled_frame = cvl_scale(frame, new_width, new_height, interpolation.value);
	cvl_frame_set_taglist(scaled_frame, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_frame_free(frame);
	cvl_write(stdout, stream_type, scaled_frame);
	cvl_frame_free(scaled_frame);
	error = cvl_error();
    }

    return error ? 1 : 0;
}
