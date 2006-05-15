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
#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include <cvl/cvl.h>

#include "options.h"


void cmd_diff_print_help(void)
{
    cvl_msg_fmt_req(
	    "diff [-o|--output=<file>] <file-1> <file-2>\n"
	    "\n"
	    "Shows the differences between the two sources. The sources must have the same "
	    "pixel type, width, and height. This command produces frames of the same "
	    "dimensions and of the same pixel type. Each pixel will be the absolute value "
	    "of the difference of the corresponding pixels in the two sources. The difference "
	    "is computed separated by color channels.\n"
	    "In addition to the difference frames, the command will compute the "
	    "sum of the absolute differences of all pixel values, divided by "
	    "the number of pixels. The value will be printed to stderr, unless the output is redirected "
	    "with the -o option. If the output is redirected to stdout (-), then only the difference values "
	    "and no frames will be written to stdout. "
	    "RGB and YUV frames produce three error values (one per channel), printed on "
	    "a single line.");
}


int cmd_diff(int argc, char *argv[])
{
    option_file_t output = { NULL, "w", true };
    option_t options[] = 
    {
	{ "output",  'o', OPTION_FILE, &output,  false },
	null_option
    };
    int first_argument;
    FILE *src1, *src2;
    cvl_io_info_t *src1_info, *src2_info;
    cvl_frame_t *src1_frame, *src2_frame;
    cvl_io_info_t *diff_info;
    cvl_frame_t *diff_frame;
    int size;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    error = !cvtool_getopt(argc, argv, options, 2, 2, &first_argument);
    if (!error)
    {
	if (!(src1 = fopen(argv[first_argument], "r")))
	{
	    cvl_msg_err("cannot open %s: %s", argv[first_argument], strerror(errno));
	    error = true;
	}
	else if (!(src2 = fopen(argv[first_argument + 1], "r")))
	{
	    fclose(src1);
	    cvl_msg_err("cannot open %s: %s", argv[first_argument], strerror(errno));
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
    
    while (!cvl_io_eof(src1, src1_info) && !cvl_io_eof(src2, src2_info))
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
	diff_frame = cvl_diff(src1_frame, src2_frame);
	size = cvl_frame_height(src1_frame) * cvl_frame_width(src1_frame);
	if (cvl_frame_pixel_type(src1_frame) == CVL_PIXEL_GRAY)
	{
	    unsigned long long errorsum = 0;
	    for (int i = 0; i < size; i++)
	    {
		unsigned long long error = cvl_frame_get_i(diff_frame, i);
		if (ULLONG_MAX - errorsum < error)
		{
		    cvl_msg_err("cannot compute average error: arithmetic overflow");
		    error = true;
		    break;
		}			
		errorsum += error;
	    }
	    cvl_msg(output.value ? output.value : stderr, CVL_MSG_REQ, 
		    "%.20g", (double)errorsum / (double)size);
	}
	else if (cvl_frame_pixel_type(src1_frame) == CVL_PIXEL_RGB)
	{
	    unsigned long long errorsum0 = 0;
	    unsigned long long errorsum1 = 0;
	    unsigned long long errorsum2 = 0;
	    for (int i = 0; i < size; i++)
	    {
		cvl_pixel_t diff = cvl_frame_get_i(diff_frame, i);
		unsigned long long error0 = cvl_pixel_rgb_to_r(diff);
		unsigned long long error1 = cvl_pixel_rgb_to_g(diff);
		unsigned long long error2 = cvl_pixel_rgb_to_b(diff);
		if (ULLONG_MAX - errorsum0 < error0
			|| ULLONG_MAX - errorsum1 < error1
			|| ULLONG_MAX - errorsum2 < error2)
		{
		    cvl_msg_err("cannot compute average error: arithmetic overflow");
		    error = true;
		    break;
		}			
		errorsum0 += error0;
		errorsum1 += error1;
		errorsum2 += error2;
	    }
	    cvl_msg(output.value ? output.value : stderr, CVL_MSG_REQ, 
		    "%.20g %.20g %.20g", 
		    (double)errorsum0 / (double)size,
		    (double)errorsum1 / (double)size,
		    (double)errorsum2 / (double)size);
	}
	else // (cvl_frame_pixel_type(src1_frame) == CVL_PIXEL_YUV)
	{
	    unsigned long long errorsum0 = 0;
	    unsigned long long errorsum1 = 0;
	    unsigned long long errorsum2 = 0;
	    for (int i = 0; i < size; i++)
	    {
		cvl_pixel_t diff = cvl_frame_get_i(diff_frame, i);
		unsigned long long error0 = cvl_pixel_yuv_to_y(diff);
		unsigned long long error1 = cvl_pixel_yuv_to_u(diff);
		unsigned long long error2 = cvl_pixel_yuv_to_v(diff);
		if (ULLONG_MAX - errorsum0 < error0
			|| ULLONG_MAX - errorsum1 < error1
			|| ULLONG_MAX - errorsum2 < error2)
		{
		    cvl_msg_err("cannot compute average error: arithmetic overflow");
		    error = true;
		    break;
		}			
		errorsum0 += error0;
		errorsum1 += error1;
		errorsum2 += error2;
	    }
	    cvl_msg(output.value ? output.value : stderr, CVL_MSG_REQ, 
		    "%.20g %.20g %.20g", 
		    (double)errorsum0 / (double)size,
		    (double)errorsum1 / (double)size,
		    (double)errorsum2 / (double)size);
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
