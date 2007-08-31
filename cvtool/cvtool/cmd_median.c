/*
 * cmd_median.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006m 2007  Martin Lambers <marlam@marlam.de>
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
#include <limits.h>
#include <float.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_median_print_help(void)
{
    mh_msg_fmt_req(
	    "median [-a|--approximated] [-3|--3d] -k|--k=<k>\n"
	    "median [-a|--approximated] [-3|--3d] -x|--k-x=<kx> -y|--k-y=<ky> [-t|--k-t=<kt>]\n"
	    "\n"
	    "Filter frames, in 2D or 3D (with the third dimension being the time). The kernel size "
	    "can be given for each dimension, or once for all. It will be (2kx+1)x(2ky+1)[x(2kt+1)]. "
	    "Different values for each direction lead to asymmetric filtering.");
}

static bool has_more_data(FILE *f)
{
    int c;
    return ((c = fgetc(f)) != EOF && ungetc(c, f) != EOF);
}

int cmd_median(int argc, char *argv[])
{
    mh_option_bool_t approximated = { false, true };
    mh_option_bool_t three_dimensional = { false, true };
    mh_option_int_t k = { -1, 0, MH_MASKSIZE_K_MAX };
    mh_option_int_t kx = { -1, 0, MH_MASKSIZE_K_MAX };
    mh_option_int_t ky = { -1, 0, MH_MASKSIZE_K_MAX };
    mh_option_int_t kt = { -1, 0, MH_MASKSIZE_K_MAX };
    mh_option_t options[] = 
    {
	{ "approximated", 'a', MH_OPTION_BOOL, &approximated,      false },
	{ "3d",           '3', MH_OPTION_BOOL, &three_dimensional, false },
	{ "k",            'k', MH_OPTION_INT,  &k,                 false },
	{ "k-x",          'x', MH_OPTION_INT,  &kx,                false },
	{ "k-y",          'y', MH_OPTION_INT,  &ky,                false },
	{ "k-t",          't', MH_OPTION_INT,  &kt,                false },
	mh_option_null
    };
    cvl_stream_type_t stream_type;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    error = !mh_getopt(argc, argv, options, 0, 0, NULL);
    if (!error)
    {
	if (kt.value >= 0)
	{
	    three_dimensional.value = true;
	}
	if (k.value >= 0 && (kx.value >= 0 || ky.value >= 0 || kt.value >= 0))
	{
	    mh_msg_err("Kernel size is overdetermined");
	    error = true;
	}
	else if (k.value < 0)
	{
	    if (kx.value < 0 || ky.value < 0 || (three_dimensional.value && kt.value < 0))
	    {
		mh_msg_err("Kernel size is underdetermined");
		error = true;
	    }
	}
    }
    if (error)
    {
	return 1;
    }
    
    if (k.value >= 0)
    {
	kx.value = k.value;
	ky.value = k.value;
	kt.value = k.value;
    }

    if (three_dimensional.value)
    {
	cvl_frame_t *new_frame;
	int framebuflen = 2 * kt.value + 1;
	cvl_frame_t *framebuf[framebuflen];
	int future_frames = 0;
	
	for (int i = 0; i < framebuflen; i++)
	    framebuf[i] = NULL;

	for (;;)
	{
	    // Read the next frame, or take it from the future.
	    if (future_frames == 0 && has_more_data(stdin))
	    {
		cvl_read(stdin, &stream_type, &(framebuf[framebuflen / 2]));
		if (!framebuf[framebuflen / 2])
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

	    // While (has_more_data && future not complete): Read more frames
	    while (has_more_data(stdin) && future_frames < framebuflen / 2)
	    {
		cvl_read(stdin, &stream_type, &(framebuf[framebuflen / 2 + future_frames + 1]));
		if (!framebuf[framebuflen / 2 + future_frames + 1])
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
	    new_frame = cvl_frame_new_tpl(framebuf[framebuflen / 2]);
	    cvl_frame_set_taglist(new_frame, cvl_taglist_copy(cvl_frame_taglist(framebuf[framebuflen / 2])));
	    if (approximated.value)
		cvl_median3d_separated(new_frame, framebuf, kx.value, ky.value, kt.value);
	    else
		cvl_median3d(new_frame, framebuf, kx.value, ky.value, kt.value);
	    cvl_write(stdout, stream_type, new_frame);
	    cvl_frame_free(new_frame);
	    error = cvl_error();
	    if (error)
	    {
		break;
	    }

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
	    if (approximated.value)
		cvl_median_separated(new_frame, frame, kx.value, ky.value);
	    else
		cvl_median(new_frame, frame, kx.value, ky.value);
	    cvl_frame_free(frame);
	    cvl_write(stdout, stream_type, new_frame);
	    cvl_frame_free(new_frame);
	}
	error = cvl_error();
    }

    return error ? 1 : 0;
}
