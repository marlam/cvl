/*
 * cmd_filter.c
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
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include <cvl/cvl.h>


void cmd_filter_print_help(void)
{
    cvl_msg_fmt_req(
	    "filter average [-3|--3d] -k|--k=<k>\n"
	    "filter average [-3|--3d] -x|--k-x=<kx> -y|--k-y=<ky> [-t|--k-t=<kt>]\n"
	    "filter min [-3|--3d] -k|--k=<k>\n"
	    "filter min [-3|--3d] -x|--k-x=<kx> -y|--k-y=<ky> [-t|--k-t=<kt>]\n"
	    "filter max [-3|--3d] -k|--k=<k>\n"
	    "filter max [-3|--3d] -x|--k-x=<kx> -y|--k-y=<ky> [-t|--k-t=<kt>]\n"
	    "filter median [-3|--3d] -k|--k=<k>\n"
	    "filter median [-3|--3d] -x|--k-x=<kx> -y|--k-y=<ky> [-t|--k-t=<kt>]\n"
	    "filter gauss [-3|--3d] -k|--k=<k>\n"
	    "filter gauss [-3|--3d] -s|--sigma=<s>\n"
	    "filter gauss [-3|--3d] -x|--k-x=<kx> -y|--k-y=<ky> [-t|--k-t=<kt>]\n"
	    "filter gauss [-3|--3d] [-k|--k=<k>] [-x|--k-x=<kx>] [-y|--k-y=<ky>] [-t|--k-t=<kt>] "
	    "[-s|--sigma=<s>] [--sigma-x=<sx>] [--sigma-y=<sy>] [--sigma-t=<st>]\n"
	    "\n"
	    "Filter frames, in 2D or 3D (with the third dimension being the time). The kernel size "
	    "can be given for each dimension, or once for all. It will be (2kx+1)x(2ky+1)[x(2kt+1)]. "
	    "Different values for each direction lead to asymmetric filtering. The gauss filter "
	    "can be specified by the sigma value(s): the mask size will be computed so that "
	    "roughly 95%% of the mass lies within the resulting mask. It is also possible to "
	    "specify both sigma and k.");
}


int cmd_filter(int argc, char *argv[])
{
    typedef enum { FILTER_AVERAGE, FILTER_MIN, FILTER_MAX, FILTER_MEDIAN, FILTER_GAUSS } subcommand_t;
    subcommand_t subcommand;
    cvl_option_bool_t three_dimensional = { false, true };
    cvl_option_int_t k = { 0, 1, CVL_MASKSIZE_K_MAX };
    cvl_option_int_t kx = { 0, 1, CVL_MASKSIZE_K_MAX };
    cvl_option_int_t ky = { 0, 1, CVL_MASKSIZE_K_MAX };
    cvl_option_int_t kt = { 0, 1, CVL_MASKSIZE_K_MAX };
    cvl_option_double_t s = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_double_t sx = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_double_t sy = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_double_t st = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_t average_options[] = 
    {
	{ "3d",      '3', CVL_OPTION_BOOL,   &three_dimensional, false },
	{ "k",       'k', CVL_OPTION_INT,    &k,                 false },
	{ "k-x",     'x', CVL_OPTION_INT,    &kx,                false },
	{ "k-y",     'y', CVL_OPTION_INT,    &ky,                false },
	{ "k-t",     't', CVL_OPTION_INT,    &kt,                false },
	cvl_option_null
    };
    cvl_option_t *min_options = average_options;
    cvl_option_t *max_options = average_options;
    cvl_option_t *median_options = average_options;
    cvl_option_t gauss_options[] = 
    {
	{ "3d",      '3', CVL_OPTION_BOOL,   &three_dimensional, false },
	{ "k",       'k', CVL_OPTION_INT,    &k,                 false },
	{ "k-x",     'x', CVL_OPTION_INT,    &kx,                false },
	{ "k-y",     'y', CVL_OPTION_INT,    &ky,                false },
	{ "k-t",     't', CVL_OPTION_INT,    &kt,                false },
	{ "sigma",   's', CVL_OPTION_DOUBLE, &s,                 false },
	{ "sigma-x", 'X', CVL_OPTION_DOUBLE, &sx,                false },
	{ "sigma-y", 'Y', CVL_OPTION_DOUBLE, &sy,                false },
	{ "sigma-t", 'T', CVL_OPTION_DOUBLE, &st,                false },
	cvl_option_null
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!argv[1])
    {
	cvl_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "average") == 0)
    {
	subcommand = FILTER_AVERAGE;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), average_options, 0, 0, NULL);
	if (!error)
	{
	    if (kt.value > 0)
	    {
		three_dimensional.value = true;
	    }
	    if (k.value > 0 && (kx.value > 0 || ky.value > 0 || kt.value > 0))
	    {
		cvl_msg_err("kernel size is overdetermined");
		error = true;
	    }
	    else if (k.value <= 0)
	    {
		if (kx.value <= 0 || ky.value <= 0 || (three_dimensional.value && kt.value <= 0))
		{
		    cvl_msg_err("kernel size is underdetermined");
		    error = true;
		}
	    }
	}
    }
    else if (strcmp(argv[1], "min") == 0)
    {
	subcommand = FILTER_MIN;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), min_options, 0, 0, NULL);
	if (!error)
	{
	    if (kt.value > 0)
	    {
		three_dimensional.value = true;
	    }
	    if (k.value > 0 && (kx.value > 0 || ky.value > 0 || kt.value > 0))
	    {
		cvl_msg_err("kernel size is overdetermined");
		error = true;
	    }
	    else if (k.value <= 0)
	    {
		if (kx.value <= 0 || ky.value <= 0 || (three_dimensional.value && kt.value <= 0))
		{
		    cvl_msg_err("kernel size is underdetermined");
		    error = true;
		}
	    }
	}
    }
    else if (strcmp(argv[1], "max") == 0)
    {
	subcommand = FILTER_MAX;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), max_options, 0, 0, NULL);
	if (!error)
	{
	    if (kt.value > 0)
	    {
		three_dimensional.value = true;
	    }
	    if (k.value > 0 && (kx.value > 0 || ky.value > 0 || kt.value > 0))
	    {
		cvl_msg_err("kernel size is overdetermined");
		error = true;
	    }
	    else if (k.value <= 0)
	    {
		if (kx.value <= 0 || ky.value <= 0 || (three_dimensional.value && kt.value <= 0))
		{
		    cvl_msg_err("kernel size is underdetermined");
		    error = true;
		}
	    }
	}
    }
    else if (strcmp(argv[1], "median") == 0)
    {
	subcommand = FILTER_MEDIAN;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), median_options, 0, 0, NULL);
	if (!error)
	{
	    if (kt.value > 0)
	    {
		three_dimensional.value = true;
	    }
	    if (k.value > 0 && (kx.value > 0 || ky.value > 0 || kt.value > 0))
	    {
		cvl_msg_err("kernel size is overdetermined");
		error = true;
	    }
	    else if (k.value <= 0)
	    {
		if (kx.value <= 0 || ky.value <= 0 || (three_dimensional.value && kt.value <= 0))
		{
		    cvl_msg_err("kernel size is underdetermined");
		    error = true;
		}
	    }
	}
    }
    else if (strcmp(argv[1], "gauss") == 0)
    {
	subcommand = FILTER_GAUSS;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), gauss_options, 0, 0, NULL);
	if (!error)
	{
    	    if (kt.value > 0 || st.value > 0.0)
    	    {
    		three_dimensional.value = true;
    	    }
    	    if (k.value > 0 && (kx.value > 0 || ky.value > 0 || kt.value > 0))
    	    {
    		cvl_msg_err("kernel size is overdetermined");
    		error = true;
    	    }
    	    if (!error)
    	    {
    		if (s.value > 0.0 && (sx.value > 0.0 || sy.value > 0.0 || st.value > 0.0))
    		{
    		    cvl_msg_err("sigma is overdetermined");
    		    error = true;
    		}
    		else if (s.value <= 0.0 
    			&& (sx.value > 0.0 || sy.value > 0.0 || (three_dimensional.value && st.value > 0.0)))
    		{
    		    if (sx.value <= 0.0 || sy.value <= 0.0 || (three_dimensional.value && st.value <= 0.0))
    		    {
    			cvl_msg_err("sigma is underdetermined");
    			error = true;
    		    }
    		}
    	    }
    	    if (!error && s.value <= 0.0 && k.value <= 0)
    	    {
    		if ((sx.value <= 0.0 && kx.value <= 0) || (sy.value <= 0.0 && ky.value <= 0)
    			|| (three_dimensional.value && st.value <= 0.0 && kt.value <= 0))
    		{
    		    cvl_msg_err("parameters are underdetermined");
    		    error = true;
    		}
    	    }
       	}
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
    
    if (k.value > 0)
    {
	kx.value = k.value;
	ky.value = k.value;
	kt.value = k.value;
    }
    if (subcommand == FILTER_GAUSS)
    {
	if (s.value > 0.0)
	{
	    sx.value = s.value;
	    sy.value = s.value;
	    st.value = s.value;
	}
     	if (kx.value <= 0)
	{
	    kx.value = cvl_filter_gauss_sigma_to_k(sx.value);
	}
	if (ky.value <= 0)
	{
	    ky.value = cvl_filter_gauss_sigma_to_k(sy.value);
	}
	if (three_dimensional.value && kt.value <= 0)
	{
	    kt.value = cvl_filter_gauss_sigma_to_k(st.value);
	}
	if (sx.value <= 0.0)
	{
	    sx.value = cvl_filter_gauss_k_to_sigma(kx.value);
	}
	if (sy.value <= 0.0)
	{
	    sy.value = cvl_filter_gauss_k_to_sigma(ky.value);
	}
	if (three_dimensional.value && st.value <= 0.0)
	{
	    st.value = cvl_filter_gauss_k_to_sigma(kt.value);
	}
    }

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    if (three_dimensional.value)
    {
	cvl_frame_t *new_frame;
	int framebuflen = 2 * kt.value + 1;
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
	    if (subcommand == FILTER_AVERAGE)
	    {
		new_frame = cvl_filter3d_average(framebuf, kx.value, ky.value, kt.value);
	    }
	    else if (subcommand == FILTER_MIN)
	    {
		new_frame = cvl_filter3d_min(framebuf, kx.value, ky.value, kt.value);
	    }
	    else if (subcommand == FILTER_MAX)
	    {
		new_frame = cvl_filter3d_max(framebuf, kx.value, ky.value, kt.value);
	    }
	    else if (subcommand == FILTER_MEDIAN)
	    {
		new_frame = cvl_filter3d_median(framebuf, kx.value, ky.value, kt.value);
	    }
	    else // (subcommand == FILTER_GAUSS)
	    {
		new_frame = cvl_filter3d_gauss(framebuf, kx.value, ky.value, kt.value,
			sx.value, sy.value, st.value);
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
	    if (subcommand == FILTER_AVERAGE)
	    {
		new_frame = cvl_filter_average(frame, kx.value, ky.value);
	    }
	    else if (subcommand == FILTER_MIN)
	    {
		new_frame = cvl_filter_min(frame, kx.value, ky.value);
	    }
	    else if (subcommand == FILTER_MAX)
	    {
		new_frame = cvl_filter_max(frame, kx.value, ky.value);
	    }
	    else if (subcommand == FILTER_MEDIAN)
	    {
		new_frame = cvl_filter_median(frame, kx.value, ky.value);
	    }
	    else // (subcommand == FILTER_GAUSS)
	    {
		new_frame = cvl_filter_gauss(frame, kx.value, ky.value, sx.value, sy.value);
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

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
