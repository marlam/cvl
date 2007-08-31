/*
 * cmd_rotate.c
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


void cmd_rotate_print_help(void)
{
    mh_msg_fmt_req(
	    "rotate -a|--angle=<angle> [-c|--color=<color>] "
	    "[-i|--interpolation=none|bilinear|biquadratic|bicubic|bicubic-b-spline|bicubic-cr-spline]\n"
	    "\n"
	    "Rotate frames with the given angle (in degrees), counterclockwise. "
	    "The dimensions of the rotated frame will be big enough to hold all "
	    "informations from the source. Holes will be filled with the given "
	    "color; the default is black. The default interpolation type is bilinear.");
}

static bool check_color(const char *s)
{
    return (cvl_color_from_string(s) != CVL_COLOR_INVALID);
}

int cmd_rotate(int argc, char *argv[])
{
    mh_option_float_t angle = { 0.0, -FLT_MAX, true, FLT_MAX, true };
    mh_option_string_t c = { (char *)"black", check_color };
    const char *interpolation_names[] = { "none", "bilinear", "biquadratic", 
	"bicubic", "bicubic-b-spline", "bicubic-cr-spline", NULL };
    mh_option_name_t interpolation = { 1, interpolation_names };
    mh_option_t options[] = 
    {
	{ "angle",         'a', MH_OPTION_FLOAT,  &angle,         true  },
	{ "color",         'c', MH_OPTION_STRING, &c,             false },
	{ "interpolation", 'i', MH_OPTION_NAME,   &interpolation, false },
	mh_option_null
    };
    cvl_stream_type_t stream_type;
    cvl_color_t color;
    cvl_frame_t *frame;
    cvl_frame_t *rotated_frame;
    float fillval[4];

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    color = cvl_color_from_string(c.value);
    angle.value = mh_angle_0_to_2pi(mh_deg_to_rad(angle.value));
	
    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	cvl_color_to_float(color, cvl_frame_format(frame), fillval);
    	rotated_frame = cvl_rotate(frame, angle.value, interpolation.value, fillval); 
	cvl_frame_set_taglist(rotated_frame, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_frame_free(frame);
	cvl_write(stdout, stream_type, rotated_frame);
	cvl_frame_free(rotated_frame);
    }

    return cvl_error() ? 1 : 0;
}
