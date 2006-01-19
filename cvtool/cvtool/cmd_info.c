/*
 * cmd_info.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2006  Martin Lambers <marlam@marlam.de>
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

#include <cvl/cvl.h>

#include "options.h"


void cmd_info_print_help(void)
{
    cvl_msg_fmt_req(
	    "info [-o|--output=<file>]\n"
	    "\n"
	    "Print information about the first frame in a stream to stderr or to the given file "
	    "('-' means stdout). "
	    "The following information will be printed: STREAMTYPE (y4m or pnm), "
	    "PIXELTYPE (yuv for y4m streams, rgb or gray for pnm streams), WIDTH, HEIGHT, "
	    "and, if STREAMTYPE is y4m, CHROMA, INTERLACING, FRAMERATE, ASPECTRATIO.");
}


int cmd_info(int argc, char *argv[])
{
    option_file_t output = { NULL, "w", true };
    option_t options[] = 
    {
	{ "output", 'o', OPTION_FILE, &output, false },
	null_option
    };
    cvl_io_info_t *input_info;    
    cvl_frame_t *frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    input_info = cvl_io_info_new();

    if (cvl_io_eof(stdin, input_info))
    {
	cvl_msg_err("empty stream");
	error = true;
    }
    else
    {
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	}
	else
	{
	    if (cvl_io_info_stream_type(input_info) == CVL_IO_STREAM_Y4M)
	    {
		const char *interlacing;
		const char *chroma;
		int framerate1, framerate2;
		int aspectratio1, aspectratio2;
		
		if (cvl_io_info_interlacing(input_info) == CVL_Y4M_INTERLACING_PROGRESSIVE)
		{
		    interlacing = "p";
		}
		else
		{
		    interlacing = "unknown";
		}
		if (cvl_io_info_chroma(input_info) ==  CVL_Y4M_CHROMA_420JPEG)
		{
		    chroma = "420jpeg";
		}
		else if (cvl_io_info_chroma(input_info) ==  CVL_Y4M_CHROMA_444)
		{
		    chroma = "444";
		}
		else
		{
		    chroma = "unknown";
		}
		cvl_io_info_framerate(input_info, &framerate1, &framerate2);
		cvl_io_info_aspectratio(input_info, &aspectratio1, &aspectratio2);
		cvl_msg(output.value ? output.value : stderr, CVL_MSG_REQ,
		     	"STREAMTYPE=y4m PIXELTYPE=yuv WIDTH=%d HEIGHT=%d "
			"INTERLACING=%s CHROMA=%s "
			"FRAMERATE=%d:%d ASPECTRATIO=%d:%d",
			cvl_io_info_width(input_info), cvl_io_info_height(input_info),
			interlacing, chroma, 
			framerate1, framerate2, aspectratio1, aspectratio2);
	    }
	    else // CVL_IO_STREAM_PNM
	    {
		cvl_msg(output.value ? output.value : stderr, CVL_MSG_REQ,
			"STREAMTYPE=pnm PIXELTYPE=%s WIDTH=%d HEIGHT=%d",
			cvl_frame_pixel_type(frame) == CVL_PIXEL_GRAY ? "gray" : "rgb",
			cvl_io_info_width(input_info), cvl_io_info_height(input_info));
	    }
	    cvl_frame_free(frame);
	    error = false;
	}
    }
    
    cvl_io_info_free(input_info);
    if (output.value && output.value != stdout)
    {
	fclose(output.value);
    }
    return error ? 1 : 0;
}
