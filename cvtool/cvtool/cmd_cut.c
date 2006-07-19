/*
 * cmd_cut.c
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
#include <limits.h>

#include <cvl/cvl.h>


void cmd_cut_print_help(void)
{
    cvl_msg_fmt_req(
	    "cut -l|--left=<l> -t|--top=<t> -w|--width=<w> -h|--height=<h>\n"
	    "\n"
	    "Only let the given rectangle through; cut the rest of each frame.");
}


int cmd_cut(int argc, char *argv[])
{
    cvl_option_int_t l = { 0, 0, INT_MAX };
    cvl_option_int_t t = { 0, 0, INT_MAX };
    cvl_option_int_t w = { 0, 1, INT_MAX };
    cvl_option_int_t h = { 0, 1, INT_MAX };
    cvl_option_t options[] =
    {
	{ "left",   'l', CVL_OPTION_INT, &l, true },
	{ "top",    't', CVL_OPTION_INT, &t, true },
	{ "width",  'w', CVL_OPTION_INT, &w, true },
	{ "height", 'h', CVL_OPTION_INT, &h, true },
	cvl_option_null
    };
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *newframe;
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
	if (l.value + w.value > cvl_frame_width(frame) || t.value + h.value > cvl_frame_height(frame))
	{
	    cvl_msg_err("rectangle does not fit into frame");
	    error = true;
	    break;
	}
	newframe = cvl_cut(frame, l.value, t.value, w.value, h.value);
	cvl_frame_free(frame);
	cvl_io_info_set_width(output_info, cvl_frame_width(newframe));
	cvl_io_info_set_height(output_info, cvl_frame_height(newframe));
	if (!cvl_io_write(stdout, output_info, newframe))
	{
	    cvl_frame_free(newframe);
	    error = true;
	    break;
	}
	cvl_frame_free(newframe);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
