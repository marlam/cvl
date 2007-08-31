/*
 * cmd_color.c
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
#include <float.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_color_print_help(void)
{
    mh_msg_fmt_req(
	    "color [-h|--hue=<h>] [-s|--saturation=<s>] [-l|--lightness=<l>] [-c|--contrast=<c>]\n"
	    "\n"
	    "Color adjustment.\n"
	    "Hue, saturation, lightness, and contrast are manipulated in the HSL "
	    "(Hue, Saturation, Lightness) color space. h is an additive constant to "
	    "the hue angle, in degrees. s, l, c measure the relative change in "
	    "saturation, lightness, and contrast: -1 means the result will be zero, 0 means "
	    "the result will be the same as the original, and +1 means that the result "
	    "will be two times as high as the original. Values greater than +1 are possible. "
	    "For example, s = -1 will convert the input frames to graylevels.");
}


int cmd_color(int argc, char *argv[])
{
    mh_option_float_t h = { 0.0f, -FLT_MAX, true, FLT_MAX, true };
    mh_option_float_t s = { 0.0f, -1.0f, true, FLT_MAX, true };
    mh_option_float_t l = { 0.0f, -1.0f, true, FLT_MAX, true };
    mh_option_float_t c = { 0.0f, -1.0f, true, FLT_MAX, true };
    mh_option_t options[] = 
    { 
	{ "hue",        'h', MH_OPTION_FLOAT, &h, false },
	{ "saturation", 's', MH_OPTION_FLOAT, &s, false },
	{ "lightness",  'l', MH_OPTION_FLOAT, &l, false },
	{ "contrast",   'c', MH_OPTION_FLOAT, &c, false },
	mh_option_null 
    };
    cvl_stream_type_t stream_type;
    cvl_format_t format;
    cvl_frame_t *frame;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    h.value = mh_angle_0_to_2pi(mh_deg_to_rad(h.value));

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	if (stream_type == CVL_PNM)
	    cvl_frame_set_type(frame, CVL_FLOAT);
	format = cvl_frame_format(frame);
	cvl_convert_format_inplace(frame, CVL_HSL);
	cvl_frame_t *tmpframe = cvl_frame_new_tpl(frame);
	cvl_frame_set_taglist(tmpframe, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_color_adjust(tmpframe, frame, h.value, s.value, l.value, c.value);
	cvl_frame_free(frame);
	cvl_convert_format_inplace(tmpframe, format);
	if (stream_type == CVL_PNM)
	    cvl_frame_set_type(tmpframe, CVL_UINT8);
	cvl_write(stdout, stream_type, tmpframe);
	cvl_frame_free(tmpframe);
    }

    return cvl_error() ? 1 : 0;
}
