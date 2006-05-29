/*
 * cmd_dibr.c
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
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>

#include "xalloc.h"

#include <cvl/cvl.h>


void cmd_dibr_print_help(void)
{
    cvl_msg_fmt_req(
	    "dibr -d|--depth=<depthsource> -b|--b=<b> [-p|--position=<p>] "
	    "[-z|--zps=<z>] [-h|--hole-filling=none|average|near|far|linear]\n"
	    "\n"
	    "Builds stereo images from a source view and corresponding depth maps. b is "
	    "the \"eye\" distance in pixels. The position of the source view must be in "
	    "[-1,+1], where -1 means left view, +1 means right view, and 0 means "
	    "intermediate view (this is the default). zps is the zero parallax setting. "
	    "It must be in [0,1]; the default is 0. "
	    "The hole filling method can be none, average color (default), color of "
	    "nearer/farther neighbor pixel, or linear color gradient.");
}


int cmd_dibr(int argc, char *argv[])
{
    cvl_option_file_t depth = { NULL, "r", false };
    cvl_option_int_t b = { -1, 0, INT_MAX };
    cvl_option_double_t position = { 0.0, -1.0, true, +1.0, true };
    cvl_option_double_t zps = { 0.0, 0.0, true, 1.0, true };
    const char *hole_filling_names[] = { "none", "average", "near", "far", "linear", NULL };
    cvl_option_name_t hole_filling = { 1, hole_filling_names };
    cvl_option_t options[] =
    {
	{ "depth",        'd', CVL_OPTION_FILE,   &depth,        true },
	{ "b",            'b', CVL_OPTION_INT,    &b,            true },
	{ "position",     'p', CVL_OPTION_DOUBLE, &position,     false },
	{ "zps",          'z', CVL_OPTION_DOUBLE, &zps,          false },
	{ "hole-filling", 'h', CVL_OPTION_NAME,   &hole_filling, false },
	cvl_option_null
    };
    cvl_io_info_t *frame_info;    
    cvl_io_info_t *depthmap_info;
    cvl_frame_t *frame;
    cvl_frame_t *depthmap;
    cvl_io_info_t *stereo_frame_info;
    cvl_frame_t *stereoframe;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    frame_info = cvl_io_info_new();
    depthmap_info = cvl_io_info_new();
    stereo_frame_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(stereo_frame_info, frame_info);

    error = false;
    while (!cvl_io_eof(stdin, frame_info) && !cvl_io_eof(depth.value, depthmap_info))
    {
	if (!cvl_io_read(stdin, frame_info, &frame))
	{
	    error = true;
	    break;
	}
	if (!cvl_io_read(depth.value, depthmap_info, &depthmap))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	if (cvl_frame_width(frame) != cvl_frame_width(depthmap) || cvl_frame_height(frame) != cvl_frame_height(depthmap))
	{
	    cvl_msg_err("frames and depth maps must have the same dimensions");
	    cvl_frame_free(frame);
	    cvl_frame_free(depthmap);
	    error = true;
	    break;
	}
	if (!cvl_product_fits_in_int(cvl_frame_width(frame), 2))
	{
	    cvl_msg_err("frame too wide");
	    cvl_frame_free(frame);
	    cvl_frame_free(depthmap);
	    error = true;
	    break;
	}
	cvl_frame_to_gray(depthmap);
	stereoframe = cvl_dibr(frame, depthmap, position.value, zps.value, 
		b.value, 1.0, hole_filling.value);
	cvl_frame_free(frame);
	cvl_frame_free(depthmap);
	cvl_io_info_set_width(stereo_frame_info, cvl_frame_width(stereoframe));
	cvl_io_info_set_height(stereo_frame_info, cvl_frame_height(stereoframe));
	if (!cvl_io_write(stdout, stereo_frame_info, stereoframe))
	{
	    cvl_frame_free(stereoframe);
	    error = true;
	    break;
	}
	cvl_frame_free(stereoframe);
    }

    fclose(depth.value);
    cvl_io_info_free(frame_info);
    cvl_io_info_free(depthmap_info);
    cvl_io_info_free(stereo_frame_info);

    return error ? 1 : 0;
}
