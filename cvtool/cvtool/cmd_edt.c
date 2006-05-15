/*
 * cmd_edt.c
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

#include "options.h"


void cmd_edt_print_help(void)
{
    cvl_msg_fmt_req(
	    "edt [-3|--3d]\n"
	    "\n"
	    "Computes the Euclidean Distance Transform (EDT) of the input frames. The "
	    "result will be stored in integer fields with the same dimensions as the "
	    "input. If the pixel at position (x,y[,z]) in the input is a background "
	    "pixel (its value is zero), then its entry in the distance map will be zero. "
	    "If the pixel is an object pixel (its value is not zero), then its entry in "
	    "the distance map will be its squared euclidean distance to the next background "
	    "pixel.\n"
	    "If the option --3d is used, then the input frames are interpreted as a 3D cuboid, "
	    "and the 3D EDT will be computed. In this case, all input frames must have the same "
	    "dimensions. Their number determines the depth of the cuboid.\n"
	    "The result can only be guaranteed to be correct if the width/height/depth of "
	    "the input are smaller than 2*sqrt(INT_MAX/2) pixels, or if it is known "
	    "that the maximum distance between object pixel and background pixels in the "
	    "input is less than sqrt(INT_MAX/2).");
}

bool write_int(char *s, size_t s_size, const int *i)
{
    return (snprintf(s, s_size, "%d", *i) < (int)s_size);
}

int cmd_edt(int argc, char *argv[])
{
    option_bool_t three_dimensional = { false, true };
    option_t options[] = 
    { 
	{ "3d", '3', OPTION_BOOL, &three_dimensional, false },
	null_option 
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    error = false;
    if (three_dimensional.value)
    {
	cvl_frame_t **frames = NULL;
	cvl_field_t **edt;
	int depth = 0;

	while (!cvl_io_eof(stdin, input_info) && depth < INT_MAX)
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
	    edt = cvl_edt3d(frames, depth);
	    for (int i = 0; i < depth; i++)
	    {
		cvl_frame_free(frames[i]);
	    }
	    free(frames);
	    for (int i = 0; i < depth; i++)
	    {
		if (!error && !cvl_field_write(stdout, edt[i],
			    (bool (*)(char *, size_t, const void *))write_int))
		{
		    error = true;
		}
		cvl_field_free(edt[i]);
	    }
	    free(edt);
	}
    }
    else
    {
	cvl_frame_t *frame;
	cvl_field_t *edt;

	while (!cvl_io_eof(stdin, input_info))
	{
	    if (!cvl_io_read(stdin, input_info, &frame))
	    {
		error = true;
		break;
	    }
	    cvl_frame_to_gray(frame);
	    edt = cvl_edt(frame);
	    cvl_frame_free(frame);
	    if (!cvl_field_write(stdout, edt,
			(bool (*)(char *, size_t, const void *))write_int))
	    {
		cvl_field_free(edt);
		error = true;
		break;
	    }
	    cvl_field_free(edt);
	}
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
