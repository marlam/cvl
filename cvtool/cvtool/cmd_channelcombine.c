/*
 * cmd_channelcombine.c
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
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_channelcombine_print_help(void)
{
    mh_msg_fmt_req(
	    "channelcombine file0 [file1 [file2 [file3]]]\n"
	    "\n"
	    "Extract the first channel from the given files and combine them into multichannel output data.");
}


int cmd_channelcombine(int argc, char *argv[])
{
    mh_option_t options[] = { mh_option_null };
    int first_argument;
    int number_of_files;
    FILE *f[4];
    cvl_frame_t *ch[4], *newframe;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 1, 4, &first_argument))
    {
	return 1;
    }
    number_of_files = argc - first_argument;
    error = false;
    for (int i = 0; i < number_of_files; i++)
    {
	if (!(f[i] = fopen(argv[first_argument + i], "r")))
	{
	    mh_msg_err("cannot open %s: %s", argv[first_argument + i], strerror(errno));
	    error = true;
	}
    }
    if (error)
    {
	for (int i = 0; i < number_of_files; i++)
	{
	    if (f[i])
	    {
		fclose(f[i]);
	    }
	}
	return 1;
    }

    while (!cvl_error())
    {
	bool quit = false;
	bool use_float = false;
	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_frame_t *tmpframe;
	    cvl_read(f[i], NULL, &tmpframe);
	    if (!tmpframe)
	    {
		quit = true;
		break;
	    }
	    ch[i] = cvl_frame_new(cvl_frame_width(tmpframe), cvl_frame_height(tmpframe),
		    1, CVL_UNKNOWN, cvl_frame_type(tmpframe), CVL_TEXTURE);
	    cvl_channel_extract(ch[i], tmpframe, 0);
	    if (cvl_frame_type(tmpframe) == CVL_FLOAT)
	    {
		use_float = true;
	    }
	    cvl_frame_free(tmpframe);
	}
	if (quit)
	{
	    break;
	}

	newframe = cvl_frame_new(cvl_frame_width(ch[0]), cvl_frame_height(ch[0]),
		number_of_files, CVL_UNKNOWN, use_float ? CVL_FLOAT : CVL_UINT8, CVL_TEXTURE);
	cvl_channel_combine(newframe, ch[0],
		number_of_files > 1 ? ch[1] : NULL,
		number_of_files > 2 ? ch[2] : NULL,
		number_of_files > 3 ? ch[3] : NULL);
	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_frame_free(ch[i]);
	}

	cvl_write(stdout, cvl_frame_type(newframe) == CVL_UINT8 ? CVL_PNM : CVL_PFS, newframe);
	cvl_frame_free(newframe);
    }

    for (int i = 0; i < number_of_files; i++)
    {
	fclose(f[i]);
    }

    return cvl_error() ? 1 : 0;
}
