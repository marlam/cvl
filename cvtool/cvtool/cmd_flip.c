/*
 * cmd_flip.c
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


void cmd_flip_print_help(void)
{
    mh_msg_fmt_req(
	    "flip\n"
	    "\n"
	    "Flip frames vertically.");
}


int cmd_flip(int argc, char *argv[] UNUSED)
{
    mh_option_t options[] = { mh_option_null };
    cvl_stream_type_t stream_type;
    cvl_frame_t *frame, *flipped_frame;

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
	flipped_frame = cvl_frame_new_tpl(frame);
	cvl_frame_set_taglist(flipped_frame, cvl_taglist_copy(cvl_frame_taglist(frame)));
	cvl_flip(flipped_frame, frame);
	cvl_frame_free(frame);
	cvl_write(stdout, stream_type, flipped_frame);
	cvl_frame_free(flipped_frame);
    }

    return cvl_error() ? 1 : 0;
}
