/*
 * cmd_convert.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cvl/cvl.h>


void cmd_convert_print_help(void)
{
    cvl_msg_fmt_req(
	    "convert [-o|--output-type=pnm|y4m] [-g|--gray] [-C|--chroma=420jpeg|444] "
	    "[-F|--framerate=<f1>:<f2>] [-A|--aspect-ratio=<a1>:<a2>]\n"
	    "\n"
	    "Convert input to pnm or y4m format. The default is to keep the input format.\n"
	    "The --gray option converts each frame to graylevels. For the pnm output type, this is the only "
	    "option that has any effect; the others are silently ignored.\n"
	    "For y4m, the chroma subsampling, frame rate, and aspect ratio can be given. The "
	    "default is to incur them from the input stream. If the input stream is not a y4m "
	    "stream, the defaults are 0:0 (\"unknown\") for frame rate and aspect ratio, and 420jpeg "
	    "for chroma subsampling.");
}


int cmd_convert(int argc, char *argv[])
{
    const char *output_type_names[] = { "y4m", "pnm", NULL };
    cvl_option_name_t output_type = { -1, output_type_names };
    cvl_option_bool_t gray = { false, true };
    const char *chroma_names[] = { "420jpeg", "444", NULL };
    cvl_option_name_t chroma = { -1, chroma_names };
    cvl_option_ratio_t framerate = { -1, -1 };
    cvl_option_ratio_t aspectratio = { -1, -1 };
    cvl_option_t options[] = 
    {
	{ "output-type",  'o', CVL_OPTION_NAME,  &output_type, false },
	{ "gray",         'g', CVL_OPTION_BOOL,  &gray,        false },
	{ "chroma",       'C', CVL_OPTION_NAME,  &chroma,      false },
	{ "framerate",    'F', CVL_OPTION_RATIO, &framerate,   false },
	{ "aspect-ratio", 'A', CVL_OPTION_RATIO, &aspectratio, false },
	cvl_option_null
    };
    cvl_frame_t *frame;
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    
    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);
    if (output_type.value >= 0)
    {
	cvl_io_info_set_stream_type(output_info, 
		output_type.value == 0 ? CVL_IO_STREAM_Y4M : CVL_IO_STREAM_PNM);
    }
    if (chroma.value >= 0)
    {
	cvl_io_info_set_chroma(output_info, 
		chroma.value == 0 ? CVL_Y4M_CHROMA_420JPEG : CVL_Y4M_CHROMA_444);
    }
    if (framerate.value1 >= 0)
    {
	cvl_io_info_set_framerate(output_info, framerate.value1, framerate.value2);
    }
    if (aspectratio.value1 >= 0)
    {
	cvl_io_info_set_aspectratio(output_info, aspectratio.value1, aspectratio.value2);
    }

    error = false;
    while (!cvl_io_eof(stdin))
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	if (gray.value)
	{
	    cvl_frame_to_gray(frame);
	}
	else if (output_type.value == 1 || (output_type.value == -1 
		    && cvl_io_info_stream_type(input_info) == CVL_IO_STREAM_PNM))
	{
	    cvl_frame_to_rgb(frame);
	}
	if (!cvl_io_write(stdout, output_info, frame))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_frame_free(frame);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
