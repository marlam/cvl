/*
 * cmd_affine.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <errno.h>
extern int errno;

#include <cvl/cvl.h>


void cmd_affine_print_help(void)
{
    cvl_msg_fmt_req(
	    "affine -m|--matrix=<2x2-matrix> [-c|--color=<color>] [-i|--interpolation=none|bilinear]\n"
	    "\n"
	    "Apply the affine tranformation defined by the given matrix (4 floating point values "
	    "separated by commas) to the frames. "
	    "The frame dimensions will be adapted so that the resulting image will fit. "
	    "Possible holes will be filled with the given color; the default is black. "
	    "The default interpolation type is bilinear.");
}

int cmd_affine(int argc, char *argv[])
{
    int matrix_sizes[2] = { 2, 2 };
    cvl_option_double_array_t matrix = { NULL, 0, NULL, 2, matrix_sizes };
    cvl_option_color_t color = { CVL_COLOR_BLACK };
    const char *interpolation_names[] = { "none", "bilinear", NULL };
    cvl_option_name_t interpolation = { 1, interpolation_names };
    cvl_option_t options[] = 
    {
        { "matrix",        'm', CVL_OPTION_DOUBLE_ARRAY, &matrix,        true },
	{ "color",         'c', CVL_OPTION_COLOR,        &color,         false },
	{ "interpolation", 'i', CVL_OPTION_NAME,         &interpolation, false },
	cvl_option_null
    };
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *new_frame;
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
    while (!cvl_io_eof(stdin, input_info))
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	new_frame = cvl_affine(frame, interpolation.value, color.value, matrix.value);
	cvl_frame_free(frame);
	cvl_io_info_set_width(output_info, cvl_frame_width(new_frame));
	cvl_io_info_set_height(output_info, cvl_frame_height(new_frame));
	if (!cvl_io_write(stdout, output_info, new_frame))
	{
	    cvl_frame_free(new_frame);
	    error = true;
	    break;
	}
	cvl_frame_free(new_frame);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    free(matrix.value);
    free(matrix.value_sizes);
    return error ? 1 : 0;
}
