/*
 * cmd_skeleton.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2006  Martin Lambers <marlam@marlam.de>
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
#include <limits.h>

#include <cvl/cvl.h>

#include "xalloc.h"


void cmd_skeleton_print_help(void)
{
    cvl_msg_fmt_req(
	    "skeleton [-3|--3d]\n"
	    "\n"
	    "Computes the skeletons of the input, with the following properties:\n"
	    "- The skeleton is thin (1 pixel wide)\n"
	    "- The skeleton is connected\n"
	    "- The original shape can be recovered from the skeleton with a tolerance "
	    "of 1 pixel\n"
	    "The input will be converted to graylevel if necessary. Black pixels in the "
	    "input will be interpreted as background, all others as foreground.\n"
	    "The output will be in form of integer fields that have the same dimensions "
	    "as the input. Pixels that do not belong to the skeleton will have the value 0. "
	    "The value of skeleton pixels will be the chessboard distance "
	    "to the next background pixel.\n"
	    "If --3d is used, then the complete input will be interpreted as a 3D cuboid and "
	    "the 3D skeleton of it will be computed.\n"
	    "Note that a 1-pixel-border of the input is ignored.");
}

int cmd_skeleton(int argc, char *argv[])
{
    cvl_option_bool_t three_dimensional = { false, true };
    cvl_option_t options[] = 
    { 
	{ "3d", '3', CVL_OPTION_BOOL, &three_dimensional, false },
	cvl_option_null 
    };
    cvl_io_info_t *input_info;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    input_info = cvl_io_info_new();
    
    error = false;
    if (three_dimensional.value)
    {
	cvl_frame_t **frames = NULL;
	cvl_field_t **skeleton;
	int depth = 0;

	while (!cvl_io_eof(stdin) && depth < INT_MAX)
	{
	    frames = xnrealloc(frames, ++depth, sizeof(cvl_frame_t *));
	    if (!cvl_io_read(stdin, input_info, &frames[depth - 1]))
	    {
		error = true;
		break;
	    }
	    cvl_frame_to_gray(frames[depth - 1]);
	    if (depth > 1)
	    {
		if (cvl_frame_width(frames[depth - 1]) != cvl_frame_width(frames[0])
			|| cvl_frame_height(frames[depth - 1]) != cvl_frame_height(frames[0]))
		{
		    cvl_msg_err("all frames must have the same dimensions");
		    error = true;
		    break;
		}
	    }
	}
	if (!error && depth > 0)
	{
	    skeleton = cvl_skeleton3d(frames, depth);
	    for (int i = 0; i < depth; i++)
	    {
		cvl_frame_free(frames[i]);
	    }
	    free(frames);
	    for (int i = 0; i < depth; i++)
	    {
		if (!error && !cvl_field_write(stdout, skeleton[i]))
		{
		    error = true;
		}
		cvl_field_free(skeleton[i]);
	    }
	    free(skeleton);
	}
    }
    else
    {
	cvl_frame_t *frame;
	cvl_field_t *skel;

	while (!cvl_io_eof(stdin))
	{
	    if (!cvl_io_read(stdin, input_info, &frame))
	    {
		error = true;
		break;
	    }
	    cvl_frame_to_gray(frame);
	    skel = cvl_skeleton(frame);
	    cvl_frame_free(frame);
	    if (!cvl_field_write(stdout, skel))
	    {
		cvl_field_free(skel);
		error = true;
		break;
	    }
	    cvl_field_free(skel);
	}
    }

    cvl_io_info_free(input_info);
    return error ? 1 : 0;
}
