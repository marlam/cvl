/*
 * cmd_unsharpmask.c
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


void cmd_unsharpmask_print_help(void)
{
    mh_msg_fmt_req(
	    "unsharpmask -u|--unsharp=<file> [-c|--c=<c>]\n"
	    "\n"
	    "Sharpens the input frames using unsharp masking. The unsharp version of the input frames "
	    "must be given using the --unsharp option. It can be produced using e.g. a 3x3 Gauss filter. "
	    "The sharpness parameter c must be from (0.5, 1.0). The default is 0.7.");
}

int cmd_unsharpmask(int argc, char *argv[])
{
    mh_option_file_t u = { NULL, "r", false };
    mh_option_float_t c = { 0.7f, 0.5f, false, 1.0f, false };
    mh_option_t options[] = 
    {
	{ "unsharp", 'u', MH_OPTION_FILE, &u, true },
	{ "c",       'c', MH_OPTION_FLOAT, &c, false },
	mh_option_null
    };
    cvl_frame_t *frame_in, *frame_unsharp, *frame_out;
    cvl_stream_type_t stream_type;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame_in);
	cvl_read(u.value, NULL, &frame_unsharp);
	if (!frame_in || !frame_unsharp)
	    break;

	frame_out = cvl_frame_new_tpl(frame_in);
	cvl_frame_set_taglist(frame_out, cvl_taglist_copy(cvl_frame_taglist(frame_in)));
	cvl_unsharpmask(frame_out, frame_in, frame_unsharp, c.value);
	cvl_frame_free(frame_in);
	cvl_frame_free(frame_unsharp);
	cvl_write(stdout, stream_type, frame_out);
	cvl_frame_free(frame_out);
    }

    return cvl_error() ? 1 : 0;
}
