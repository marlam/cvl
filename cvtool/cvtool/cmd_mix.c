/*
 * cmd_mix.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_mix_print_help(void)
{
    mh_msg_fmt_req(
	    "mix -w|--weight=weight... file...\n"
	    "\n"
	    "Mixes the given sources into a single stream using the given weights. "
	    "Each input file is assigned exactly one weight. The weights must be floating point numbers "
	    "greater than or equal to zero.\n"
	    "The input frames may differ in size. In this case, they will be implicitly scaled to a common size.");
}


int cmd_mix(int argc, char *argv[])
{
    mh_option_float_array_t w = { NULL, 1, 0, 1, NULL };
    mh_option_t options[] = 
    { 
	{ "weight", 'w', MH_OPTION_FLOAT_ARRAY, &w, true },
	mh_option_null 
    };
    int first_argument;
    int number_of_files;
    FILE **f;
    cvl_stream_type_t stream_type;
    cvl_frame_t **input_frames;
    cvl_frame_t *frame;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }
    number_of_files = argc - first_argument;
    if (w.value_sizes[0] != number_of_files)
    {
	mh_msg_err("Need exactly one weight for each input file");
	return 1;
    }
    for (int i = 0; i < number_of_files; i++)
    {
	if (w.value[i] < 0.0f)
	{
	    mh_msg_err("Weight must be greater than or equal to zero");
	    return 1;
	}
    }

    f = mh_alloc(number_of_files * sizeof(FILE *));
    error = false;
    for (int i = 0; i < number_of_files; i++)
    {
	if (!(f[i] = fopen(argv[first_argument + i], "r")))
	{
	    mh_msg_err("Cannot open %s: %s", argv[first_argument + i], strerror(errno));
	    error = true;
	}
    }
    input_frames = mh_alloc(number_of_files * sizeof(cvl_frame_t *));
    
    while (!error && !cvl_error())
    {
	// Read frames, determine max width and height.
	int maxwidth = 0;
	int maxheight = 0;
	bool stop = false;
	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_read(f[i], i == 0 ? &stream_type : NULL, &(input_frames[i]));
	    if (!input_frames[i])
	    {
		for (int j = 0; j < i; j++)
		    cvl_frame_free(input_frames[i]);
		stop = true;
		break;
	    }
	    if (cvl_frame_width(input_frames[i]) > maxwidth)
		maxwidth = cvl_frame_width(input_frames[i]);
	    if (cvl_frame_height(input_frames[i]) > maxheight)
		maxheight = cvl_frame_height(input_frames[i]);
	}
	if (stop)
	    break;
	
	// Layer the frames
	frame = cvl_frame_new(maxwidth, maxheight, cvl_frame_channels(input_frames[0]),
		cvl_frame_format(input_frames[0]), cvl_frame_type(input_frames[0]), CVL_TEXTURE);
	cvl_mix(frame, input_frames, w.value, number_of_files);
	for (int i = 0; i < number_of_files; i++)
	    cvl_frame_free(input_frames[i]);

	// Output
	cvl_write(stdout, stream_type, frame);
	cvl_frame_free(frame);
    }

    for (int i = 0; i < number_of_files; i++)
	if (f[i])
	    fclose(f[i]);
    free(f);
    free(input_frames);
    return error || cvl_error() ? 1 : 0;
}
