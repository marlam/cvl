/*
 * cmd_convolve.c
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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <cvl/cvl.h>

#include "options.h"


void cmd_convolve_print_help(void)
{
    cvl_msg_fmt_req(
	    "convolve -K|--kernel=<K>\n"
	    "convolve -X|--vector-x=<X> -Y|--vector-y=<Y> [-T|--vector-t=<T>]\n"
	    "\n"
	    "Convolve frames with the given convolution kernel. Both 2D and 3D kernels are accepted "
	    "(the third dimension is the time). If the kernel is separable, the vectors "
	    "that generate it can be given instead. "
	    "All kernel elements must be integers. The size of the kernel must be an odd number in each "
	    "dimension.");
}

int cmd_convolve(int argc, char *argv[])
{
    option_int_array_t K = { NULL, 0, NULL };
    option_int_array_t X = { NULL, 1, NULL };
    option_int_array_t Y = { NULL, 1, NULL };
    option_int_array_t T = { NULL, 1, NULL };
    option_t options[] = 
    {
	{ "kernel",   'K', OPTION_INT_ARRAY, &K, false },
	{ "vector-x", 'X', OPTION_INT_ARRAY, &X, false },
	{ "vector-y", 'Y', OPTION_INT_ARRAY, &Y, false },
	{ "vector-t", 'T', OPTION_INT_ARRAY, &T, false },
	null_option
    };
    bool three_dimensional;
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    error = !cvtool_getopt(argc, argv, options, 0, 0, NULL);
    if (!error && (K.value && (X.value || Y.value || T.value)))
    {
	cvl_msg_err("cannot use kernel and vectors at the same time");
	error = true;
    }
    if (!error && K.value)
    {
	if ((K.dimensions != 2 && K.dimensions != 3) 
		|| K.sizes[0] % 2 != 1 || K.sizes[1] % 2 != 1
		|| (K.dimensions == 3 && K.sizes[2] % 2 != 1))
	{
	    cvl_msg_err("invalid convolution kernel");
	    error = true;
	}
    }
    if (!error && !K.value)
    {
	if (!X.value || !Y.value)
	{
	    cvl_msg_err("incomplete kernel information");
	    error = true;
	}
	else if (X.sizes[0] % 2 != 1 || Y.sizes[0] % 2 != 1 || (T.value && T.sizes[0] % 2 != 1))
	{
	    cvl_msg_err("invalid convolution kernel");
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }

    three_dimensional = ((K.value && K.dimensions == 3) || T.value);
    
    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    if (three_dimensional)
    {
	cvl_frame_t *new_frame;
	int framebuflen = (K.value ? K.sizes[0] : T.sizes[0]);
	cvl_frame_t *framebuf[framebuflen];
	int future_frames = 0;
	
	for (int i = 0; i < framebuflen; i++)
	{
	    framebuf[i] = NULL;
	}
	for (;;)
	{
	    // Read the next frame, or take it from the future.
	    if (future_frames == 0 && !cvl_io_eof(stdin, input_info))
	    {
		if (!cvl_io_read(stdin, input_info, &(framebuf[framebuflen / 2])))
		{
		    error = true;
		    break;
		}
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

	    // While (!eof && future not complete): Read more frames
	    while (!cvl_io_eof(stdin, input_info) && future_frames < framebuflen / 2)
	    {
		if (!cvl_io_read(stdin, input_info, &(framebuf[framebuflen / 2 + future_frames + 1])))
		{
		    error = true;
		    break;
		}
		future_frames++;
	    }
	    if (error)
	    {
		break;
	    }
		
	    // Process the present frame
	    if (K.value)
	    {
		new_frame = cvl_frame_convolve3d((const cvl_frame_t **)framebuf, 
			K.value, K.sizes[0], K.sizes[1], K.sizes[2]);
	    }
	    else
	    {
		new_frame = cvl_frame_convolve3d_separable((const cvl_frame_t **)framebuf, 
		     	X.value, X.sizes[0], Y.value, Y.sizes[0], T.value, T.sizes[0]);
	    }
	    if (!cvl_io_write(stdout, output_info, new_frame))
	    {
		cvl_frame_free(new_frame);
		error = true;
		break;
	    }
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
	
    	while (!cvl_io_eof(stdin, input_info))
	{
	    if (!cvl_io_read(stdin, input_info, &frame))
	    {
		error = true;
		break;
	    }
    	    if (K.value)
	    {
		new_frame = cvl_frame_convolve(frame, K.value, K.sizes[0], K.sizes[1]);
	    }
	    else
	    {
		new_frame = cvl_frame_convolve_separable(frame, X.value, X.sizes[0], Y.value, Y.sizes[0]);
	    }
	    cvl_frame_free(frame);
	    if (!cvl_io_write(stdout, output_info, new_frame))
	    {
		cvl_frame_free(new_frame);
		error = true;
		break;
	    }
	    cvl_frame_free(new_frame);
	}
    }
	
    free(K.value);
    free(K.sizes);
    free(X.value);
    free(X.sizes);
    free(Y.value);
    free(Y.sizes);
    free(T.value);
    free(T.sizes);
    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
