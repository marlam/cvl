/*
 * cmd_vectors.c
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

#include <limits.h>
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <cvl/cvl.h>


void cmd_vectors_print_help(void)
{
    cvl_msg_fmt_req(
	    "vectors visualize -t|--type=2i|2|3 [-x|--sample-x=<x>] [-y|--sample-y=<y>] "
	    "[-X|--dist-x=<dx>] [-Y|--dist-y=<dy>] [-f|--factor=<f>]\n"
	    "\n"
	    "Reads vector fields in plain text formats, as produced by other commands "
	    "such as opticalflow, and visualizes them as a needle diagram. The type of "
	    "the vectors must be known; it can be \"2i\" for vectors with two integer "
	    "components, \"2\" for vectors with two floating point components, and \"3\" "
	    "for vectors with three floating point components.\n"
	    "Every x-th vector in horizontal direction and every y-th vector in vertical "
	    "direction will be represented by a needle. The needles will have a distance of "
	    "dx pixels in horizontal and dy pixels in vertical direction. The needle length "
	    "is the length of the vector after it was scaled with the factor f. "
	    "The default values are x=y=dx=dy=10, f=1.0.");
}


int cmd_vectors(int argc, char *argv[])
{
    enum subcommand_t { VISUALIZE };
    int subcommand;
    const char *type_names[] = { "2i", "2", "3", NULL };
    cvl_option_name_t type = { -1, type_names };
    cvl_option_int_t sample_x = { 10, 1, INT_MAX };
    cvl_option_int_t sample_y = { 10, 1, INT_MAX };
    cvl_option_int_t dist_x = { 10, 1, INT_MAX };
    cvl_option_int_t dist_y = { 10, 1, INT_MAX };
    cvl_option_double_t factor = { 1.0, -DBL_MAX, true, DBL_MAX, true };
    cvl_option_t visualize_options[] = 
    {
	{ "type",     't', CVL_OPTION_NAME,   &type,     true },
	{ "sample-x", 'x', CVL_OPTION_INT,    &sample_x, false },
	{ "sample-y", 'y', CVL_OPTION_INT,    &sample_y, false },
	{ "dist-x",   'X', CVL_OPTION_INT,    &dist_x,   false },
	{ "dist-y",   'Y', CVL_OPTION_INT,    &dist_y,   false },
	{ "factor",   'f', CVL_OPTION_DOUBLE, &factor,   false },
	cvl_option_null
    };
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_field_t *field;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!argv[1])
    {
	cvl_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "visualize") == 0)
    {
	subcommand = VISUALIZE;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), visualize_options, 0, 0, NULL);
    }
    else
    {
	cvl_msg_err("unknown subcommand");
	error = true;
    }
    if (error)
    {
	return 1;
    }

    output_info = cvl_io_info_new();
    cvl_io_info_set_stream_type(output_info, CVL_IO_STREAM_PNM);
    error = false;
    for (;;)
    {
	if (cvl_io_eof(stdin))
	{
	    break;
	}
	if (type.value == 0)
	{
	    error = !cvl_field_read_knowntype(stdin, &field, 2 * sizeof(int));
	}
	else if (type.value == 1)
	{
	    error = !cvl_field_read_knowntype(stdin, &field, 2 * sizeof(float));
	}
	else
	{
	    error = !cvl_field_read_knowntype(stdin, &field, 3 * sizeof(float));
	}
	if (error)
	{
	    break;
	}
	if (type.value == 0)
	{
	    frame = cvl_vector2i_visualize(field, sample_x.value, sample_y.value, 
		    dist_x.value, dist_y.value, factor.value);
	}
	else if (type.value == 1)
	{
	    frame = cvl_vector2_visualize(field, sample_x.value, sample_y.value, 
		    dist_x.value, dist_y.value, factor.value);
	}
	else
	{
	    frame = cvl_vector3_visualize(field, sample_x.value, sample_y.value, 
		    dist_x.value, dist_y.value, factor.value);
	}
	cvl_field_free(field);
	cvl_io_info_set_width(output_info, cvl_frame_width(frame));
	cvl_io_info_set_height(output_info, cvl_frame_height(frame));
	if (!(cvl_io_write(stdout, output_info, frame)))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_frame_free(frame);
    }
    
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
