/*
 * cmd_resize.c
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
#include <limits.h>

#include <cvl/cvl.h>

#include "mh.h"

void cmd_resize_print_help(void)
{
    mh_msg_fmt_req(
	    "resize -w|--width=<w> -h|--height=<h> [-x|--x-offset=<x>] [-y|--y-offset=<y>] "
	    "[-c|--color=<color>]\n"
	    "\n"
	    "Resizes the frames to the given new width and height. Places the original frame "
	    "contents at the position (x,y) relative to the new frame (these offsets may be "
	    "negative). If no or an incomplete position is given, the missing part(s) are computed "
	    "so that the old contents are centered on the new frame. "
	    "Holes are filled with the given color (default is black).");
}

static bool check_color(const char *s)
{
    return (cvl_color_from_string(s) != CVL_COLOR_INVALID);
}

int cmd_resize(int argc, char *argv[])
{
    mh_option_int_t w = { 0, 1, INT_MAX };
    mh_option_int_t h = { 0, 1, INT_MAX };
    mh_option_int_t x = { INT_MIN, INT_MIN + 1, INT_MAX };
    mh_option_int_t y = { INT_MIN, INT_MIN + 1, INT_MAX };
    mh_option_string_t c = { (char *)"black", check_color };
    mh_option_t options[] =
    {
	{ "width",      'w', MH_OPTION_INT,    &w, true  },
	{ "height",     'h', MH_OPTION_INT,    &h, true  },
	{ "x-offset",   'x', MH_OPTION_INT,    &x, false },
	{ "y-offset",   'y', MH_OPTION_INT,    &y, false },
	{ "color",      'c', MH_OPTION_STRING, &c, false },
	mh_option_null
    };
    bool compute_x;
    bool compute_y;
    cvl_stream_type_t stream_type;
    cvl_frame_t *frame;
    cvl_frame_t *newframe;
    cvl_color_t color;
    float fillval[4];
    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    compute_x = (x.value == INT_MIN);
    compute_y = (y.value == INT_MIN);
    color = cvl_color_from_string(c.value);

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	if (compute_x)
	{
	    x.value = (w.value - cvl_frame_width(frame)) / 2;
	}
	if (compute_y)
	{
	    y.value = (h.value - cvl_frame_height(frame)) / 2;
	}
	newframe = cvl_frame_new(w.value, h.value, cvl_frame_channels(frame), 
		cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
	cvl_frame_set_taglist(newframe, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_color_to_float(color, cvl_frame_format(frame), fillval);
	cvl_resize(newframe, frame, x.value, y.value, fillval);
	cvl_frame_free(frame);
	cvl_write(stdout, stream_type, newframe);
	cvl_frame_free(newframe);
    }

    return cvl_error() ? 1 : 0;
}
