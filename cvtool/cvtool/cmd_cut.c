/*
 * cmd_cut.c
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

void cmd_cut_print_help(void)
{
    mh_msg_fmt_req(
	    "cut -l|--left=<l> -t|--top=<t> -w|--width=<w> -h|--height=<h>\n"
	    "\n"
	    "Only let the given rectangle through; cut the rest of each frame.");
}


int cmd_cut(int argc, char *argv[])
{
    mh_option_int_t l = { 0, 0, INT_MAX };
    mh_option_int_t t = { 0, 0, INT_MAX };
    mh_option_int_t w = { 0, 1, INT_MAX };
    mh_option_int_t h = { 0, 1, INT_MAX };
    mh_option_t options[] =
    {
	{ "left",   'l', MH_OPTION_INT, &l, true },
	{ "top",    't', MH_OPTION_INT, &t, true },
	{ "width",  'w', MH_OPTION_INT, &w, true },
	{ "height", 'h', MH_OPTION_INT, &h, true },
	mh_option_null
    };
    cvl_stream_type_t stream_type;
    cvl_frame_t *frame;
    cvl_frame_t *newframe;
    bool error = false;

    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    while (!error && !cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	if (l.value + w.value > cvl_frame_width(frame) || t.value + h.value > cvl_frame_height(frame))
	{
	    mh_msg_err("Rectangle does not fit into frame");
	    error = true;
	    break;
	}
	newframe = cvl_frame_new(w.value, h.value, 
		cvl_frame_channels(frame), cvl_frame_format(frame), cvl_frame_type(frame), CVL_TEXTURE);
	cvl_frame_set_taglist(newframe, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_cut_rect(newframe, frame, l.value, t.value);
	cvl_frame_free(frame);
	cvl_write(stdout, stream_type, newframe);
	cvl_frame_free(newframe);
    }

    return error || cvl_error() ? 1 : 0;
}
