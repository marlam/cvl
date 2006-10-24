/*
 * cmd_diff.c
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include <cvl/cvl.h>


void cmd_diff_print_help(void)
{
    cvl_msg_fmt_req(
	    "diff [-o|--output=<file>] [-s|--statistics] <file-1> <file-2>\n"
	    "\n"
	    "Shows the differences between the two sources. The sources must have the same "
	    "pixel type, width, and height. This command produces frames of the same "
	    "dimensions and of the same pixel type. Each pixel will be the absolute value "
	    "of the difference of the corresponding pixels in the two sources. "
	    "For RGB frames, the values will be computed for each channel separately.\n"
	    "If --statistics is used, the command will also compute the minimum, maximum, mean, and "
	    "median error, and the standard deviation. For RGB frames, these values will be "
	    "computed for each channel separately. For YUV frames, only the Y channel is considered.\n"
	    "The output will be printed to stderr, unless it is redirected with the --output "
	    "option. If the output is redirected to stdout (-), then only the statistics "
	    "and no frames will be written to stdout.");
}


int cmd_diff(int argc, char *argv[])
{
    cvl_option_file_t output = { NULL, "w", true };
    cvl_option_bool_t statistics = { false, true };
    cvl_option_t options[] = 
    {
	{ "output",     'o', CVL_OPTION_FILE, &output,     false },
	{ "statistics", 's', CVL_OPTION_BOOL, &statistics, false },
	cvl_option_null
    };
    int first_argument;
    FILE *src1, *src2;
    cvl_io_info_t *src1_info, *src2_info;
    cvl_frame_t *src1_frame, *src2_frame;
    cvl_io_info_t *diff_info;
    cvl_frame_t *diff_frame;
    uint8_t min[3], max[3], med[3];
    double mean[3], dev[3];    
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    error = !cvl_getopt(argc, argv, options, 2, 2, &first_argument);
    if (!error)
    {
	if (output.value && !statistics.value)
	{
	    cvl_msg_err("cannot use --output without --statistics");
	    error = true;
	}
	else if (!(src1 = fopen(argv[first_argument], "r")))
	{
	    cvl_msg_err("cannot open %s: %s", argv[first_argument], strerror(errno));
	    error = true;
	}
	else if (!(src2 = fopen(argv[first_argument + 1], "r")))
	{
	    fclose(src1);
	    cvl_msg_err("cannot open %s: %s", argv[first_argument + 1], strerror(errno));
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }

    src1_info = cvl_io_info_new();
    src2_info = cvl_io_info_new();
    diff_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(diff_info, src1_info);
    
    while (!cvl_io_eof(src1) && !cvl_io_eof(src2))
    {
	if (!cvl_io_read(src1, src1_info, &src1_frame))
	{
	    error = true;
	    break;
	}
	if (!cvl_io_read(src2, src2_info, &src2_frame))
	{
	    cvl_frame_free(src1_frame);
	    error = true;
	    break;
	}
	if (cvl_frame_width(src1_frame) != cvl_frame_width(src2_frame) 
		|| cvl_frame_height(src1_frame) != cvl_frame_height(src2_frame))
	{
	    cvl_frame_free(src1_frame);
	    cvl_frame_free(src2_frame);
	    cvl_msg_err("the sources differ in size!");
	    error = true;
	    break;
	}
	if (cvl_frame_pixel_type(src1_frame) != cvl_frame_pixel_type(src2_frame))
	{
	    cvl_frame_free(src1_frame);
	    cvl_frame_free(src2_frame);
	    cvl_msg_err("the sources differ in pixel type!");
	    error = true;
	    break;
	}
	if (statistics.value)
	{
	    FILE *outstream = output.value ? output.value : stderr;
	    int frameno = cvl_io_info_frame_counter(src1_info); 

	    diff_frame = NULL;
	    cvl_diffstat(src1_frame, src2_frame, 
		    (output.value == stdout ? NULL : &diff_frame),
		    min, max, med, mean, dev);
	    if (cvl_frame_pixel_type(src1_frame) == CVL_PIXEL_RGB)
	    {
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: minimum error      = %3d %3d %3d",
			frameno, (int)min[0], (int)min[1], (int)min[2]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: maximum error      = %3d %3d %3d",
			frameno, (int)max[0], (int)max[1], (int)max[2]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: median error       = %3d %3d %3d",
			frameno, (int)med[0], (int)med[1], (int)med[2]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: mean error         = %.10g %.10g %.10g",
			frameno, mean[0], mean[1], mean[2]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: standard deviation = %.10g %.10g %.10g",
			frameno, dev[0], dev[1], dev[2]);
	    }
	    else
	    {
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: minimum error      = %3d",
			frameno, (int)min[0]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: maximum error      = %3d",
			frameno, (int)max[0]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: median error       = %3d",
			frameno, (int)med[0]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: mean error         = %.10g",
			frameno, mean[0]);
		cvl_msg(outstream, CVL_MSG_REQ, "frame pair %d: standard deviation = %.10g",
			frameno, dev[0]);
	    }
	}
	else
	{		
	    diff_frame = cvl_diff(src1_frame, src2_frame);
	}
	cvl_frame_free(src1_frame);
	cvl_frame_free(src2_frame);
	if (error)
	{
	    break;
	}
	if (output.value != stdout)
	{
	    if (!cvl_io_write(stdout, diff_info, diff_frame))
	    {
		cvl_frame_free(diff_frame);
		error = true;
		break;
	    }
	}
	cvl_frame_free(diff_frame);
    }
    
    fclose(src1);
    fclose(src2);
    if (output.value && output.value != stdout)
    {
	fclose(output.value);
    }
    cvl_io_info_free(src1_info);
    cvl_io_info_free(src2_info);
    cvl_io_info_free(diff_info);
    return error ? 1 : 0;
}
