/*
 * cmd_laplace.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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


void cmd_laplace_print_help(void)
{
    mh_msg_fmt_req(
	    "laplace [-c|--c=<c>]\n"
	    "\n"
	    "Sharpens the input frames using the Laplace operator. The sharpness factor c must be greater than "
	    "or equal to zero. Larger values increase the effect. The default is 0.5.");
}

int cmd_laplace(int argc, char *argv[])
{
    mh_option_float_t c = { 0.5f, 0.0f, true, FLT_MAX, true };
    mh_option_t options[] = 
    {
	{ "c", 'c', MH_OPTION_FLOAT, &c, false },
	mh_option_null
    };
    cvl_frame_t *frame_in, *frame_out;
    cvl_stream_type_t stream_type;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame_in);
	if (!frame_in)
	    break;

	frame_out = cvl_frame_new_tpl(frame_in);
	cvl_frame_set_taglist(frame_out, cvl_taglist_copy(cvl_frame_taglist(frame_in)));
	cvl_laplace(frame_out, frame_in, c.value);
	cvl_frame_free(frame_in);
	cvl_write(stdout, stream_type, frame_out);
	cvl_frame_free(frame_out);
    }

    return cvl_error() ? 1 : 0;
}
