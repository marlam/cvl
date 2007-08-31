/*
 * cmd_diff.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007  Martin Lambers <marlam@marlam.de>
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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_diff_print_help(void)
{
    mh_msg_fmt_req(
	    "diff [-o|--output=<file>] [-s|--statistics] <file-1> <file-2>\n"
	    "\n"
	    "Shows the differences between the two sources. The sources must have the same "
	    "pixel type, width, and height. This command produces frames of the same "
	    "dimensions and of the same pixel type. Each pixel will be the absolute value "
	    "of the difference of the corresponding pixels in the two sources. "
	    "For RGB frames, the values will be computed for each channel separately.\n"
	    "If --statistics is used, the command will also compute the minimum, maximum, and mean "
	    "error, and the standard deviation. For multichannel frames, these values will be "
	    "computed for each channel separately.\n"
	    "The output will be printed to stderr, unless it is redirected with the --output "
	    "option. If the output is redirected to stdout (-), then only the statistics "
	    "and no frames will be written to stdout.");
}


int cmd_diff(int argc, char *argv[])
{
    mh_option_file_t output = { NULL, "w", true };
    mh_option_bool_t statistics = { false, true };
    mh_option_t options[] = 
    {
	{ "output",     'o', MH_OPTION_FILE, &output,     false },
	{ "statistics", 's', MH_OPTION_BOOL, &statistics, false },
	mh_option_null
    };
    int first_argument;
    FILE *src1, *src2;
    cvl_stream_type_t stream_type;
    cvl_frame_t *src1_frame, *src2_frame;
    cvl_frame_t *diff_frame;
    float min[4], max[4], median[4], mean[4], stddev[4];
    int frame_counter;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    error = !mh_getopt(argc, argv, options, 2, 2, &first_argument);
    if (!error)
    {
	if (output.value && !statistics.value)
	{
	    mh_msg_err("Cannot use --output without --statistics");
	    error = true;
	}
	else if (!(src1 = fopen(argv[first_argument], "r")))
	{
	    mh_msg_err("Cannot open %s: %s", argv[first_argument], strerror(errno));
	    error = true;
	}
	else if (!(src2 = fopen(argv[first_argument + 1], "r")))
	{
	    fclose(src1);
	    mh_msg_err("Cannot open %s: %s", argv[first_argument + 1], strerror(errno));
	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }

    frame_counter = 0;
    while (!error && !(error = cvl_error()))
    {
	cvl_read(src1, &stream_type, &src1_frame);
	if (!src1_frame)
	{
	    break;
	}
	cvl_read(src2, NULL, &src2_frame);
	if (!src2_frame)
	{
	    cvl_frame_free(src1_frame);
	    break;
	}
	if (cvl_frame_width(src1_frame) != cvl_frame_width(src2_frame) 
		|| cvl_frame_height(src1_frame) != cvl_frame_height(src2_frame))
	{
	    cvl_frame_free(src1_frame);
	    cvl_frame_free(src2_frame);
	    mh_msg_err("The sources differ in size!");
	    error = true;
	    break;
	}
	if (cvl_frame_format(src1_frame) != cvl_frame_format(src2_frame)
		|| cvl_frame_type(src1_frame) != cvl_frame_type(src2_frame))
	{
	    cvl_frame_free(src1_frame);
	    cvl_frame_free(src2_frame);
	    mh_msg_err("The sources differ in format and/or type!");
	    error = true;
	    break;
	}
	diff_frame = cvl_frame_new_tpl(src1_frame);
	cvl_diff(diff_frame, src1_frame, src2_frame);
	cvl_frame_free(src1_frame);
	cvl_frame_free(src2_frame);
	if (output.value != stdout)
	{
	    cvl_write(stdout, stream_type, diff_frame);
	}
	if (statistics.value)
	{
	    FILE *outstream = output.value ? output.value : stderr;
	    cvl_statistics(diff_frame, min, max, median, mean, stddev, NULL);

	    if (cvl_frame_channels(diff_frame) == 1)
	    {
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: minimum error      = %.4f",
			frame_counter, min[0]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: maximum error      = %.4f",
			frame_counter, max[0]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: median error       = %.4f",
			frame_counter, median[0]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: mean error         = %.4f",
			frame_counter, mean[0]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: standard deviation = %.4f",
			frame_counter, stddev[0]);
	    }
	    else if (cvl_frame_channels(diff_frame) == 2)
	    {
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: minimum error      = %.4f %.4f",
			frame_counter, min[0], min[1]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: maximum error      = %.4f %.4f",
			frame_counter, max[0], max[1]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: median error       = %.4f %.4f",
			frame_counter, median[0], median[1]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: mean error         = %.4f %.4f",
			frame_counter, mean[0], mean[1]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: standard deviation = %.4f %.4f",
			frame_counter, stddev[0], stddev[1]);
	    }
	    else if (cvl_frame_channels(diff_frame) == 3)
	    {
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: minimum error      = %.4f %.4f %.4f",
			frame_counter, min[0], min[1], min[2]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: maximum error      = %.4f %.4f %.4f",
			frame_counter, max[0], max[1], max[2]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: median error       = %.4f %.4f %.4f",
			frame_counter, median[0], median[1], median[2]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: mean error         = %.4f %.4f %.4f",
			frame_counter, mean[0], mean[1], mean[2]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: standard deviation = %.4f %.4f %.4f",
			frame_counter, stddev[0], stddev[1], stddev[2]);
	    }
	    else
	    {
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: minimum error      = %.4f %.4f %.4f %.4f",
			frame_counter, min[0], min[1], min[2], min[3]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: maximum error      = %.4f %.4f %.4f %.4f",
			frame_counter, max[0], max[1], max[2], max[3]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: median error       = %.4f %.4f %.4f %.4f",
			frame_counter, median[0], median[1], median[2], median[3]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: mean error         = %.4f %.4f %.4f %.4f",
			frame_counter, mean[0], mean[1], mean[2], mean[3]);
		mh_msg(outstream, MH_MSG_REQ, "frame pair %d: standard deviation = %.4f %.4f %.4f %.4f",
			frame_counter, stddev[0], stddev[1], stddev[2], stddev[3]);
	    }
	    frame_counter++;
	}
	cvl_frame_free(diff_frame);
    }
    error = error || cvl_error();
    
    fclose(src1);
    fclose(src2);
    if (output.value && output.value != stdout)
    {
	fclose(output.value);
    }
    return error ? 1 : 0;
}
