/*
 * cmd_layer.c
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

#include <cvl/cvl.h>

#include "xalloc.h"


void cmd_layer_print_help(void)
{
    cvl_msg_fmt_req(
	    "layer -m|--mode=min|max|median|or|and|xor|diff|add|xadd|sub|xsub|mul|div file...\n"
	    "\n"
	    "Layers the frames from the given files on top of each other, using the given mode. "
	    "Layering will be done for each channel separately. Graylevel frames have only one "
	    "channel. Color frames have the channels R, G, and B. Graylevel layering is only done "
	    "if all input frames are graylevel. The input frames may differ in size. In this case, "
	    "the result will be big enough to hold all input frames, and the input frames are centered "
	    "on the result.");
}


int cmd_layer(int argc, char *argv[])
{
    /* these names correspond to the cvl_layer_mode_t values */
    const char *mode_names[] = { "min", "max", "median", "or", "and", "xor", 
	"diff", "add", "xadd", "sub", "xsub", "mul", "div", NULL };
    cvl_option_name_t mode = { -1, mode_names };
    cvl_option_t options[] = 
    { 
	{ "mode", 'm', CVL_OPTION_NAME, &mode, true },
	cvl_option_null 
    };
    int first_argument;
    int number_of_files;
    FILE **f;
    cvl_io_info_t **input_infos;
    cvl_io_info_t *output_info;
    cvl_frame_t **input_frames;
    int *offset_x;
    int *offset_y;
    cvl_frame_t *frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }
    error = false;
    number_of_files = argc - first_argument;
    
    f = xmalloc(number_of_files * sizeof(FILE *));
    input_infos = xmalloc(number_of_files * sizeof(cvl_io_info_t *));
    for (int i = 0; i < number_of_files; i++)
    {
	if (!(f[i] = fopen(argv[first_argument + i], "r")))
	{
	    cvl_msg_err("cannot open %s: %s", argv[first_argument + i], strerror(errno));
	    error = true;
	}
	input_infos[i] = cvl_io_info_new();
    }
    input_frames = xmalloc(number_of_files * sizeof(cvl_frame_t *));
    offset_x = xmalloc(number_of_files * sizeof(int));
    offset_y = xmalloc(number_of_files * sizeof(int));
    
    output_info = cvl_io_info_new();
    if (!error)
    {
	cvl_io_info_link_output_to_input(output_info, input_infos[0]);
    }

    while (!error)
    {
	// Stop on EOF
	bool eof = false;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_io_eof(f[i]))
	    {
		eof = true;
		break;
	    }
	}
	if (eof)
	{
	    break;
	}
	
	// Read frames, determine max width and height.
	int maxwidth = 0;
	int maxheight = 0;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (!cvl_io_read(f[i], input_infos[i], &(input_frames[i])))
	    {
		error = true;
		break;
	    }
	    if (cvl_frame_width(input_frames[i]) > maxwidth)
	    {
		maxwidth = cvl_frame_width(input_frames[i]);
	    }
	    if (cvl_frame_height(input_frames[i]) > maxheight)
	    {
		maxheight = cvl_frame_height(input_frames[i]);
	    }
	}
	if (error)
	{
	    break;
	}
	
	/* Determine the output pixel type */
	cvl_pixel_type_t output_pixel_type = CVL_PIXEL_GRAY;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_frame_pixel_type(input_frames[i]) != CVL_PIXEL_GRAY)
	    {
		output_pixel_type = CVL_PIXEL_RGB;
		break;
	    }
	}
	if (output_pixel_type != CVL_PIXEL_GRAY)
	{
	    for (int i = 0; i < number_of_files; i++)
	    {
		cvl_frame_to_rgb(input_frames[i]);
	    }
	}

	// Layer the frames
	frame = cvl_frame_new(output_pixel_type, maxwidth, maxheight);
	for (int i = 0; i < number_of_files; i++)
	{
	    offset_x[i] = maxwidth / 2 - cvl_frame_width(input_frames[i]) / 2;
	    offset_y[i] = maxheight / 2 - cvl_frame_height(input_frames[i]) / 2;
	}
	cvl_layer(frame, mode.value, input_frames, offset_x, offset_y, number_of_files);	
	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_frame_free(input_frames[i]);
	}

	// Output
	if (!cvl_io_write(stdout, output_info, frame))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_frame_free(frame);
    }

    for (int i = 0; i < number_of_files; i++)
    {
	if (f[i])
	{
	    fclose(f[i]);
	}
	cvl_io_info_free(input_infos[i]);
    }
    free(f);
    free(input_infos);
    free(input_frames);
    free(offset_x);
    free(offset_y);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
