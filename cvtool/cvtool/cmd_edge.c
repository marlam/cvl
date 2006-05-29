/*
 * cmd_edge.c
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
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "xalloc.h"

#include <cvl/cvl.h>


void cmd_edge_print_help(void)
{
    cvl_msg_fmt_req(
	    "edge sobel\n"
	    "edge canny -s|--sigma=<sigma> -l|--low=<tl> -h|--high=<th>\n"
	    "\n"
	    "Detect edges. Sobel will generate graylevel images: the brighter a point, "
	    "the stronger the edge. Canny will generate binary images. The sigma "
	    "parameter is for Gauss smoothing. tl and th are used for Hysterese "
	    "thresholding; both must be from [0,255].");
}


int cmd_edge(int argc, char *argv[])
{
    typedef enum { SOBEL, CANNY } subcommand_t;
    subcommand_t subcommand;
    cvl_option_t sobel_options[] = { cvl_option_null };
    cvl_option_double_t canny_sigma = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_int_t canny_tl = { -1, 0, 255 };
    cvl_option_int_t canny_th = { -1, 0, 255 };
    cvl_option_t canny_options[] =
    {
	{ "sigma",  's', CVL_OPTION_DOUBLE, &canny_sigma, true },
	{ "low",    'l', CVL_OPTION_INT,    &canny_tl,    true },
	{ "high",   'h', CVL_OPTION_INT,    &canny_th,    true },
	cvl_option_null
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    cvl_frame_t *edge_frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!argv[1])
    {
	cvl_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "sobel") == 0)
    {
	subcommand = SOBEL;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), sobel_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "canny") == 0)
    {
	subcommand = CANNY;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), canny_options, 0, 0, NULL);
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

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    while (!cvl_io_eof(stdin, input_info))
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	cvl_frame_to_gray(frame);
	if (subcommand == SOBEL)
	{
	    edge_frame = cvl_edge_sobel(frame, NULL);
	}
	else // (subcommand == CANNY)
	{
	    cvl_field_t *edge_dir_field = cvl_field_new(sizeof(double), 
		    cvl_frame_width(frame), cvl_frame_height(frame));
	    edge_frame = cvl_edge_canny(frame, canny_sigma.value, canny_tl.value, canny_th.value, 
		    edge_dir_field);
	    cvl_field_free(edge_dir_field);
	}
	cvl_frame_free(frame);
	error = !cvl_io_write(stdout, output_info, edge_frame);
	cvl_frame_free(edge_frame);
	if (error)
	{
	    break;
	}
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
