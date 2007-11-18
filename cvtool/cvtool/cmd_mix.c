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
#include <limits.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_mix_print_help(void)
{
    mh_msg_fmt_req(
	    "mix -w|--firstweight=fw... [-W|--lastweight=lw...] [-s|--steps=s] [-b|--bias=b] file...\n"
	    "\n"
	    "Mixes the given sources into a single stream using the given weights.\n"
	    "The default is to produce a single step, i.e. one output frame for each set of input frames. "
	    "If more steps are requested, the weights are interpolated between the set of first weights and the "
	    "set of last weights. By default, this interpolation is done linearly, which corresponds to a bias setting of "
	    "0.5. Smaller bias values will give more attention to the first weights, larger values more to the last weigths. "
	    "The bias must be from (0,1).\n"
	    "The input frames may differ in size. In this case, they will be implicitly scaled to a common size.");
}


int cmd_mix(int argc, char *argv[])
{
    mh_option_float_array_t w = { NULL, 1, 0, 1, NULL };
    mh_option_float_array_t W = { NULL, 1, 0, 1, NULL };
    mh_option_int_t s = { 1, 1, INT_MAX };
    mh_option_float_t b = { 0.5f, 0.0f, false, 1.0f, false };
    mh_option_t options[] = 
    { 
	{ "firstweight", 'w', MH_OPTION_FLOAT_ARRAY, &w, true },
	{ "lastweight",  'W', MH_OPTION_FLOAT_ARRAY, &W, false },
	{ "steps",       's', MH_OPTION_INT,         &s, false },
	{ "bias",        'b', MH_OPTION_FLOAT,       &b, false },
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
	mh_msg_err("Need exactly one first weight for each input file");
	return 1;
    }
    if (W.value && W.value_sizes[0] != number_of_files)
    {
	mh_msg_err("Need exactly one last weight for each input file");
	return 1;
    }
    if (!W.value)
    {
	W.value = w.value;
    }
    for (int i = 0; i < number_of_files; i++)
    {
	if (w.value[i] < 0.0f || W.value[i] < 0.0f)
	{
	    mh_msg_err("Weights must be greater than or equal to zero");
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
		{
		    cvl_frame_free(input_frames[i]);
		}
		stop = true;
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
	if (stop)
	{
	    break;
	}
	
	// Mix the frames
	frame = cvl_frame_new(maxwidth, maxheight, cvl_frame_channels(input_frames[0]),
		cvl_frame_format(input_frames[0]), cvl_frame_type(input_frames[0]), CVL_TEXTURE);
	for (int step = 0; step < s.value; step++)
	{
	    float weights[number_of_files];
	    float x = (float)step / (float)(s.value - 1);
    	    float y = mh_clampf(powf(x, b.value >= 0.5f ? 2.0f * (1.0f - b.value) : 1.0f / (2.0f * b.value)), 0.0f, 1.0f);
	    for (int i = 0; i < number_of_files; i++)
	    {
		weights[i] = (1.0f - y) * w.value[i] + y * W.value[i];
	    }
	    cvl_mix(frame, input_frames, weights, number_of_files);
	    cvl_write(stdout, stream_type, frame);
	}
	cvl_frame_free(frame);

	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_frame_free(input_frames[i]);
	}
    }

    for (int i = 0; i < number_of_files; i++)
    {
	if (f[i])
	{
	    fclose(f[i]);
	}
    }
    free(f);
    free(input_frames);
    free(w.value);
    if (W.value != w.value)
    {
	free(W.value);
    }
    return error || cvl_error() ? 1 : 0;
}
