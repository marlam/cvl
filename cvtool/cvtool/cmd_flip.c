/*
 * cmd_flip.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006  Martin Lambers <marlam@marlam.de>
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
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <cvl/cvl.h>


void cmd_flip_print_help(void)
{
    cvl_msg_fmt_req(
	    "flip\n"
	    "\n"
	    "Flip frames (left/right).");
}


int cmd_flip(int argc, char *argv[] UNUSED)
{
    cvl_option_t options[] = { cvl_option_null };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    cvl_frame_t *frame, *flipped_frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    error = false;
    while (!cvl_io_eof(stdin))
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	flipped_frame = cvl_flip(frame);
	cvl_frame_free(frame);
	if (!cvl_io_write(stdout, output_info, flipped_frame))
	{
	    cvl_frame_free(flipped_frame);
	    error = true;
	    break;
	}
	cvl_frame_free(flipped_frame);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
