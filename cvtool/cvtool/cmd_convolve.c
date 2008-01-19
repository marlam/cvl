/*
 * cmd_convolve.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2008  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_convolve_print_help(void)
{
    mh_msg_fmt_req(
	    "convolve -K|--kernel=<K>\n"
	    "convolve -X|--vector-x=<X> -Y|--vector-y=<Y> [-T|--vector-t=<T>]\n"
	    "\n"
	    "Convolve frames with the given convolution kernel. Both 2D and 3D kernels are accepted "
	    "(the third dimension is the time). If the kernel is separable, the vectors "
	    "that generate it can be given instead. "
	    "The size of the kernel must be an odd number in each "
	    "dimension.");
}

static bool has_more_data(FILE *f)
{
    int c;
    return ((c = fgetc(f)) != EOF && ungetc(c, f) != EOF);
}

int cmd_convolve(int argc, char *argv[])
{
    mh_option_float_array_t K = { NULL, 0, NULL, 0, NULL };
    mh_option_float_array_t X = { NULL, 0, NULL, 1, NULL };
    mh_option_float_array_t Y = { NULL, 0, NULL, 1, NULL };
    mh_option_float_array_t T = { NULL, 0, NULL, 1, NULL };
    mh_option_t options[] = 
    {
	{ "kernel",   'K', MH_OPTION_FLOAT_ARRAY, &K, false },
	{ "vector-x", 'X', MH_OPTION_FLOAT_ARRAY, &X, false },
	{ "vector-y", 'Y', MH_OPTION_FLOAT_ARRAY, &Y, false },
	{ "vector-t", 'T', MH_OPTION_FLOAT_ARRAY, &T, false },
	mh_option_null
    };
    bool three_dimensional;
    cvl_stream_type_t stream_type;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    error = !mh_getopt(argc, argv, options, 0, 0, NULL);
    if (!error && (K.value && (X.value || Y.value || T.value)))
    {
	mh_msg_err("Cannot use kernel and vectors at the same time");
	error = true;
    }
    if (!error && K.value)
    {
	if ((K.value_dimensions != 2 && K.value_dimensions != 3) 
		|| K.value_sizes[0] % 2 != 1 || K.value_sizes[1] % 2 != 1
		|| (K.value_dimensions == 3 && K.value_sizes[2] % 2 != 1))
	{
	    mh_msg_err("Invalid convolution kernel");
	    error = true;
	}
    }
    if (!error && !K.value)
    {
	if (!X.value || !Y.value)
	{
	    mh_msg_err("Incomplete kernel information");
	    error = true;
	}
	else if (X.value_sizes[0] % 2 != 1 
		|| Y.value_sizes[0] % 2 != 1 
		|| (T.value && T.value_sizes[0] % 2 != 1))
	{
	    mh_msg_err("Invalid convolution kernel");
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }

    three_dimensional = ((K.value && K.value_dimensions == 3) || T.value);
    
    if (three_dimensional)
    {
	cvl_frame_t *new_frame;
	int framebuflen = (K.value ? K.value_sizes[0] : T.value_sizes[0]);
	cvl_frame_t *framebuf[framebuflen];
	int future_frames = 0;
	
	for (int i = 0; i < framebuflen; i++)
	    framebuf[i] = NULL;

	while (!cvl_error())
	{
	    // Read the next frame, or take it from the future.
	    if (future_frames == 0 && has_more_data(stdin))
	    {
		cvl_read(stdin, &stream_type, &(framebuf[framebuflen / 2]));
		if (!framebuf[framebuflen / 2])
		    break;
	    }
	    else
	    {
		if (future_frames > 0)
		{
		    for (int i = 0; i < future_frames; i++)
		    {
			framebuf[i + framebuflen / 2] = framebuf[i + framebuflen / 2 + 1];
		    }
		    framebuf[framebuflen / 2 + future_frames] = NULL;
		    future_frames--;
		}
		else
		{
		    break;
		}
	    }

	    // While (has_more_data && future not complete): Read more frames
	    while (has_more_data(stdin) && future_frames < framebuflen / 2)
	    {
		cvl_read(stdin, &stream_type, &(framebuf[framebuflen / 2 + future_frames + 1]));
		if (!framebuf[framebuflen / 2 + future_frames + 1])
		    break;
		future_frames++;
	    }

	    // Process the present frame
	    new_frame = cvl_frame_new_tpl(framebuf[framebuflen / 2]);
	    cvl_frame_set_taglist(new_frame, cvl_taglist_copy(cvl_frame_taglist(framebuf[framebuflen / 2])));
	    if (K.value)
	    {
		cvl_convolve3d(new_frame, framebuf, 
			K.value, K.value_sizes[0], K.value_sizes[1], K.value_sizes[2]);
	    }
	    else
	    {
		cvl_convolve3d_separable(new_frame, framebuf, 
			X.value, X.value_sizes[0], Y.value, Y.value_sizes[0], T.value, T.value_sizes[0]);
	    }
	    cvl_write(stdout, stream_type, new_frame);
	    cvl_frame_free(new_frame);

	    // Move present into past
    	    cvl_frame_free(framebuf[0]);
	    for (int i = 0; i < framebuflen / 2; i++)
	    {
		framebuf[i] = framebuf[i + 1];
	    }
	}
	for (int i = 0; i < framebuflen / 2; i++)
	{
	    cvl_frame_free(framebuf[i]);
	}
    }
    else
    {
	cvl_frame_t *frame, *new_frame;
	
    	while (!cvl_error())
	{
	    cvl_read(stdin, &stream_type, &frame);
	    if (!frame)
		break;
	    new_frame = cvl_frame_new_tpl(frame);
	    cvl_frame_set_taglist(new_frame, cvl_taglist_copy(cvl_frame_taglist(frame)));
    	    if (K.value)
	    {
		cvl_convolve(new_frame, frame, K.value, K.value_sizes[0], K.value_sizes[1]);
	    }
	    else
	    {
		cvl_convolve_separable(new_frame, frame, X.value, X.value_sizes[0], 
			Y.value, Y.value_sizes[0]);
	    }
	    cvl_frame_free(frame);
	    cvl_write(stdout, stream_type, new_frame);
	    cvl_frame_free(new_frame);
	}
    }
	
    free(K.value);
    free(K.value_sizes);
    free(X.value);
    free(X.value_sizes);
    free(Y.value);
    free(Y.value_sizes);
    free(T.value);
    free(T.value_sizes);
    return cvl_error() ? 1 : 0;
}
