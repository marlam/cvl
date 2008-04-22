/*
 * cmd_channelextract.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
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


void cmd_channelextract_print_help(void)
{
    mh_msg_fmt_req(
	    "channelextract -c|--channel=0|1|2|3|r|g|b|lum\n"
	    "\n"
	    "Extract the given channel from the input. If channel is 0, 1, 2, or 3, then the "
	    "data is copied unmodified. If the channel is r, g, b, or lum, then the input is "
	    "first converted to the red, gree, blue, or luminance form.");
}


int cmd_channelextract(int argc, char *argv[])
{
    const char *channel_names[] = { "0", "1", "2", "3", "r", "g", "b", "lum", NULL };
    mh_option_name_t c = { -1, channel_names };
    mh_option_t options[] = 
    { 
	{ "channel", 'c', MH_OPTION_NAME, &c, true },
	mh_option_null 
    };
    cvl_frame_t *frame, *newframe;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, NULL, &frame);
	if (!frame)
	{
	    break;
	}
	newframe = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame), 1,
		cvl_frame_format(frame) == CVL_UNKNOWN ? CVL_UNKNOWN : CVL_LUM,
		cvl_frame_type(frame), CVL_TEXTURE);
	cvl_frame_set_taglist(newframe, cvl_taglist_copy(cvl_frame_taglist(frame)));
	if (c.value >= 4 && c.value <= 6)
	{
	    if (cvl_frame_format(frame) != CVL_UNKNOWN)
	    {
		cvl_convert_format_inplace(frame, CVL_RGB);
	    }
	    cvl_channel_extract(newframe, frame, c.value - 4);
	}
	else if (c.value == 7)
	{
	    if (cvl_frame_format(frame) != CVL_UNKNOWN)
	    {
		cvl_convert_format_inplace(frame, CVL_LUM);
	    }
	    cvl_channel_extract(newframe, frame, 0);
	}
	else
	{
	    cvl_channel_extract(newframe, frame, c.value);
	}
	cvl_frame_free(frame);
	cvl_write(stdout, cvl_frame_type(frame) == CVL_UINT8 ? CVL_PNM : CVL_PFS, newframe);
	cvl_frame_free(newframe);
    }

    return cvl_error() ? 1 : 0;
}
