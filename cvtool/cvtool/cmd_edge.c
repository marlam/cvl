/*
 * cmd_edge.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#include <stdbool.h>
#include <stdio.h>
#include <float.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_edge_print_help(void)
{
    mh_msg_fmt_req(
	    "edge -m|--method=sobel\n"
	    "edge -m|--method=canny -s|--sigma=<sigma> -l|--low=<tl> -h|--high=<th>\n"
	    "\n"
	    "Detect edges. Sobel will generate graylevel frames: the brighter a point, "
	    "the stronger the edge. Canny will generate binary frames. The sigma "
	    "parameter is for Gauss smoothing. tl and th are used for Hysterese "
	    "thresholding; both must be from [0,1].\n"
	    "If the input is PFS, then the output will be PFS too and will contain both "
	    "a channel containing the edge strengths and a channel containing the edge "
	    "directions. If the input is PNM, then the output will be graylevel frames "
	    "containing only the strength information.");
}


int cmd_edge(int argc, char *argv[])
{
    typedef enum { EDGE_SOBEL = 0, EDGE_CANNY = 1 } method_t;
    const char *method_names[] = { "sobel", "canny", NULL };
    mh_option_name_t method = { -1, method_names };
    mh_option_float_t sigma = { -1.0f, 0.0f, false, FLT_MAX, true };
    mh_option_float_t tl = { -1.0f, 0.0f, true, 1.0, true };
    mh_option_float_t th = { -1.0f, 0.0f, true, 1.0, true };
    mh_option_t options[] = 
    {
	{ "method", 'm', MH_OPTION_NAME,  &method, true  },
	{ "sigma",  's', MH_OPTION_FLOAT, &sigma,  false },
	{ "low",    'l', MH_OPTION_FLOAT, &tl,     false },
	{ "high",   'h', MH_OPTION_FLOAT, &th,     false },
	mh_option_null
    };
    cvl_stream_type_t stream_type;
    cvl_frame_t *frame, *edge_frame;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    if (!(error = !mh_getopt(argc, argv, options, 0, 0, NULL)))
    {
	if (method.value == EDGE_SOBEL
		&& (sigma.value > 0.0f || tl.value >= 0.0f || th.value >= 0.0f))
	{
	    mh_msg_err("The sobel edge detector does not support any options");
	    error = true;
	}
	else if (method.value == EDGE_CANNY 
		&& (sigma.value < 0.0f || tl.value < 0.0f || th.value < 0.0f))
	{
	    mh_msg_err("The canny edge detector needs options --sigma, --low, and -high");
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	cvl_convert_format_inplace(frame, CVL_LUM);

	edge_frame = cvl_frame_new(cvl_frame_width(frame), cvl_frame_height(frame),
		2, CVL_UNKNOWN, CVL_FLOAT, CVL_TEXTURE);
	if (method.value == EDGE_SOBEL)
	{
	    cvl_edge_sobel(edge_frame, frame, 0);
	}
	else // (method.value == EDGE_CANNY)
	{
	    cvl_edge_canny(edge_frame, frame, 0, sigma.value, tl.value, th.value);
	}
	cvl_frame_free(frame);
	if (stream_type == CVL_PFS)
	{
    	    cvl_write(stdout, stream_type, edge_frame);
	}
	else
	{
	    cvl_frame_t *tmp_frame = cvl_frame_new(cvl_frame_width(edge_frame), 
		    cvl_frame_height(edge_frame), 1, CVL_LUM, CVL_UINT8, CVL_TEXTURE);
	    cvl_channel_extract(tmp_frame, edge_frame, 0);
	    cvl_write(stdout, stream_type, tmp_frame);
	    cvl_frame_free(tmp_frame);
	}
	cvl_frame_free(edge_frame);
    }

    return cvl_error() ? 1 : 0;
}
