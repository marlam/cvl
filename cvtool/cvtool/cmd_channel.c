/*
 * cmd_channel.c
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

#include <cvl/cvl.h>

#include "mh.h"


void cmd_channel_print_help(void)
{
    mh_msg_fmt_req(
	    "channel [-r|--reverse] -c|--channel=0|1|2|r|g|b\n"
	    "\n"
	    "Extract the given channel from the input, and write it into luminance "
	    "output. When channel is r, g, or b, then the input is converted to RGB format first. "
	    "When --reverse is used, the luminance of the input is written into the given channel "
	    "of an RGB frame, and the other channels are set to zero.");
}


int cmd_channel(int argc, char *argv[])
{
    mh_option_bool_t r = { false, true };
    const char *channel_names[] = { "0", "1", "2", "3", "r", "g", "b", NULL };
    mh_option_name_t c = { -1, channel_names };
    mh_option_t options[] = 
    { 
	{ "reverse", 'r', MH_OPTION_BOOL, &r, false },
	{ "channel", 'c', MH_OPTION_NAME, &c, true },
	mh_option_null 
    };
    cvl_frame_t *frame;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, NULL, &frame);
	if (!frame)
	    break;
	cvl_frame_t *newframe;
	if (r.value)
	{
	    cvl_frame_t *r = (c.value == 0 || c.value == 4 ? frame : NULL);
	    cvl_frame_t *g = (c.value == 1 || c.value == 5 ? frame : NULL);
	    cvl_frame_t *b = (c.value == 2 || c.value == 6 ? frame : NULL);
	    newframe = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame),
		    3, CVL_RGB, cvl_frame_format(frame), CVL_TEXTURE);
	    cvl_frame_set_taglist(newframe, cvl_taglist_copy(cvl_frame_taglist(frame)));
	    cvl_channel_combine(newframe, r, g, b, NULL);
	}
	else
	{
	    newframe = cvl_frame_new_tpl(frame);
	    cvl_frame_set_taglist(newframe, cvl_taglist_copy(cvl_frame_taglist(frame)));
	    cvl_frame_set_format(newframe, CVL_LUM);
	    if (c.value >= 4)
	    {
		cvl_convert_format_inplace(frame, CVL_RGB);
    		cvl_channel_extract(newframe, frame, c.value - 4);
	    }
	    else
	    {
		cvl_channel_extract(newframe, frame, c.value);
	    }
	}
	cvl_frame_free(frame);
	cvl_write(stdout, cvl_frame_type(frame) == CVL_UINT8 ? CVL_PNM : CVL_PFS, newframe);
	cvl_frame_free(newframe);
    }

    return cvl_error() ? 1 : 0;
}
