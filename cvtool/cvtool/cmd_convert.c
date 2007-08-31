/*
 * cmd_convert.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_convert_print_help(void)
{
    mh_msg_fmt_req(
	    "convert [-t|--type=uint8|float] [-f|--format=lum|color]\n"
	    "\n"
	    "Converts the input frames to another type and format. The default is to keep the "
	    "input type and format. The output will be PNM for type uint8 "
	    "and PFS for type float.");
}


int cmd_convert(int argc, char *argv[])
{
    const char *type_names[] = { "uint8", "float", NULL };
    mh_option_name_t t = { -1, type_names };
    const char *format_names[] = { "lum", "color", NULL };
    mh_option_name_t f = { -1, format_names };
    mh_option_t options[] = 
    {
	{ "type",   't', MH_OPTION_NAME, &t, false },
	{ "format", 'f', MH_OPTION_NAME, &f, false },
	mh_option_null
    };
    cvl_frame_t *frame;

    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    
    while (!cvl_error())
    {
	cvl_type_t oldtype, newtype;
	cvl_format_t oldformat, newformat;

	cvl_read(stdin, NULL, &frame);
	if (!frame)
	    break;
	// FIXME: This is necessary because of some bug:
	//(void)cvl_frame_pointer(frame);

	oldtype = cvl_frame_type(frame);
	if (t.value < 0)
	{
	    newtype = oldtype;
	}
	else if (t.value == 0)
	{
	    newtype = CVL_UINT8;
	}
	else
	{
	    newtype = CVL_FLOAT;
	}

	oldformat = cvl_frame_format(frame);
	if (f.value < 0)
	{
	    if (oldformat == CVL_LUM)
		newformat = oldformat;
	    else
	     	newformat = (newtype == CVL_UINT8 ? CVL_RGB : CVL_XYZ);
	}
	else if (f.value == 0)
	{
	    newformat = CVL_LUM;
	}
	else
	{
	    newformat = (newtype == CVL_UINT8 ? CVL_RGB : CVL_XYZ);
	}

	if (newtype == CVL_FLOAT)
	    cvl_frame_set_type(frame, newtype);

	cvl_convert_format_inplace(frame, newformat);

	if (newtype != CVL_FLOAT)
	    cvl_frame_set_type(frame, newtype);
	
	cvl_write(stdout, newtype == CVL_UINT8 ? CVL_PNM : CVL_PFS, frame);
    }

    return cvl_error() ? 1 : 0;
}
