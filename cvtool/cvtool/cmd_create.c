/*
 * cmd_create.c
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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_create_print_help(void)
{
    mh_msg_fmt_req(
	    "create [-t|--type=uint8|float] [-f|--format=lum|color] [-n|--n=<n>] -w|--width=<w> -h|--height=<h> [-c|--color=<color>]\n"
	    "\n"
	    "Create n (default 1) frames with the given format (default color) and the given type (default uint8). "
	    "The frames will have the given width and height, and they will be filled with the given color (default black). "
	    "The resulting stream type will be pnm if the type is uint8, and pfs otherwise.");
}

static bool check_color(const char *s)
{
    return (cvl_color_from_string(s) != CVL_COLOR_INVALID);
}

int cmd_create(int argc, char *argv[])
{
    const char *type_names[] = { "uint8", "float", NULL };
    mh_option_name_t t = { 0, type_names };
    const char *format_names[] = { "lum", "color", NULL };
    mh_option_name_t f = { 1, format_names };
    mh_option_int_t n = { 1, 1, INT_MAX };
    mh_option_int_t w = { 0, 1, INT_MAX };
    mh_option_int_t h = { 0, 1, INT_MAX };
    mh_option_string_t c = { (char *)"black", check_color };
    mh_option_t options[] =
    {
	{ "type",   't', MH_OPTION_NAME,   &t, false },
	{ "format", 'f', MH_OPTION_NAME,   &f, false },
	{ "n",      'n', MH_OPTION_INT,    &n, false },
	{ "width",  'w', MH_OPTION_INT,    &w, true  },
	{ "height", 'h', MH_OPTION_INT,    &h, true  },
	{ "color",  'c', MH_OPTION_STRING, &c, false },
	mh_option_null
    };
    cvl_frame_t *frame;

    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    
    cvl_stream_type_t stream_type = (t.value == 0 ? CVL_PNM : CVL_PFS);
    cvl_type_t type = (t.value == 0 ? CVL_UINT8 : CVL_FLOAT);
    cvl_format_t format = (f.value == 0 ? CVL_LUM : (t.value == 0 ? CVL_RGB : CVL_XYZ));
    float fillval[4];
    cvl_color_to_float(cvl_color_from_string(c.value), format, fillval);
    for (int i = 0; !cvl_error() && i < n.value; i++)
    {
	frame = cvl_frame_new(w.value, h.value, format == CVL_LUM ? 1 : 3, format, type, CVL_TEXTURE);
	cvl_fill_rect(frame, 0, 0, w.value, h.value, fillval);
	cvl_write(stdout, stream_type, frame);
	cvl_frame_free(frame);
    }

    return cvl_error() ? 1 : 0;
}
