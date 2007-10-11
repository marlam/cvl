/*
 * cmd_info.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2006, 2007  Martin Lambers <marlam@marlam.de>
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
#include <math.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_info_print_help(void)
{
    mh_msg_fmt_req(
	    "info [-s|--statistics] [-S|--single] [-o|--output=<file>]\n"
	    "\n"
	    "Print information about frames in the input stream.\n"
	    "If --single is used, the command exits after the first frame has been processed.\n"
	    "If --statistics is used, additional statistics about the frame contents are printed.\n"
	    "The output can be redirected to a file or to standard output (-) using the --output option.\n"
	    "The following information will be printed: STREAM (pfs or pnm), CHANNELS (0-4), "
	    "FORMAT (luminance or color), TYPE (uint8 or float), WIDTH, HEIGHT.\n"
	    "Statistics are computed for each available channel c: "
	    "CHc_MIN, CHc_MAX, CHc_MEAN, CHc_MEDIAN, CHc_STDDEVIATION.");
}


int cmd_info(int argc, char *argv[])
{
    mh_option_bool_t statistics = { false, true };
    mh_option_bool_t single = { false, true };
    mh_option_file_t output = { NULL, "w", true };
    mh_option_t options[] = 
    {
	{ "statistics", 's', MH_OPTION_BOOL, &statistics, false },
	{ "single",     'S', MH_OPTION_BOOL, &single,     false },
	{ "output",     'o', MH_OPTION_FILE, &output,     false },
	mh_option_null
    };
    cvl_frame_t *frame = NULL;
    cvl_stream_type_t stream_type;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
	return 1;

    while (!cvl_error())
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    return 0;

	mh_msg(output.value ? output.value : stderr, MH_MSG_REQ,
		"STREAM=%s CHANNELS=%d FORMAT=%s TYPE=%s WIDTH=%d HEIGHT=%d",
		stream_type == CVL_PNM ? "pnm" : "pfs",
		cvl_frame_channels(frame),
		cvl_frame_format(frame) == CVL_LUM ? "luminance" : "color",
		cvl_frame_type(frame) == CVL_UINT8 ? "uint8" : "float",
		cvl_frame_width(frame), cvl_frame_height(frame));

	if (statistics.value)
	{
	    float min[4], max[4], median[4], mean[4], stddev[4];
	    cvl_statistics(frame, min, max, median, mean, stddev, NULL);
	    for (int c = 0; c < cvl_frame_channels(frame); c++)
	    {
		mh_msg(output.value ? output.value : stderr, MH_MSG_REQ,
			"CH%d_MIN=%.6g CH%d_MAX=%.6g CH%d_MEAN=%.6g CH%d_MEDIAN=%.6g CH%d_STDDEVIATION=%.6g",
			c, min[c], c, max[c], c, mean[c], c, median[c], c, stddev[c]);
	    }
	}

	cvl_frame_free(frame);

	if (single.value)
	    break;
    }

    if (output.value && output.value != stdout)
	fclose(output.value);
    
    return cvl_error() ? 1 : 0;
}
