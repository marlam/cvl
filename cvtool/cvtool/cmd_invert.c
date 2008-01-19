/*
 * cmd_invert.c
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


void cmd_invert_print_help(void)
{
    mh_msg_fmt_req(
	    "invert\n"
	    "\n"
	    "Invert input frames.");
}


int cmd_invert(int argc, char *argv[])
{
    mh_option_t options[] = { mh_option_null };
    cvl_frame_t *frame;
    cvl_stream_type_t stream_type;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	cvl_format_t format = cvl_frame_format(frame);
	if (format != CVL_LUM)
	    cvl_convert_format_inplace(frame, CVL_RGB);
	cvl_frame_t *inverse = cvl_frame_new_tpl(frame);
	cvl_frame_set_taglist(inverse, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_invert(inverse, frame);
	cvl_frame_free(frame);
	if (format != CVL_LUM)
	    cvl_convert_format_inplace(inverse, format);
	cvl_write(stdout, stream_type, inverse);
    	cvl_frame_free(inverse);
    }

    return cvl_error() ? 1 : 0;
}
