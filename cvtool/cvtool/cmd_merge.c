/*
 * cmd_merge.c
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
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_merge_print_help(void)
{
    mh_msg_fmt_req(
	    "merge [-s|--shuffle] [-o|--output=<file>] file...\n"
	    "\n"
	    "Merges files into one stream, in the given order. "
	    "If --shuffle is used, the order will be randomized. "
	    "The file names will be printed to stderr in the order they are merged. "
	    "If --output is used, the file names will be written to the given file instead.");
}


int cmd_merge(int argc, char *argv[])
{
    mh_option_bool_t shuffle = { false, true };
    mh_option_file_t output = { NULL, "w", false };
    mh_option_t options[] = 
    {
	{ "shuffle", 's', MH_OPTION_BOOL, &shuffle, false },
	{ "output",  'o', MH_OPTION_FILE, &output,  false },
	mh_option_null 
    };
    int first_argument;
    int number_of_files;
    char **filenames;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }

    number_of_files = argc - first_argument;
    filenames = mh_alloc(number_of_files * sizeof(char *));
    for (int i = 0; i < number_of_files; i++)
    {
	filenames[i] = argv[first_argument + i];
    }
    if (shuffle.value)
    {
	srand(time(NULL));
	mh_shuffle(filenames, sizeof(char *), number_of_files);
    }
    
    error = false;
    for (int i = 0; !error && i < number_of_files; i++)
    {
	FILE *f;
	if (!(f = fopen(filenames[i], "r")))
	{
	    mh_msg_err("cannot open %s: %s", filenames[i], strerror(errno));
	    error = true;
	}
	else
	{
	    mh_msg(output.value ? output.value : stderr, MH_MSG_REQ, "%s", filenames[i]);
	    cvl_stream_type_t stream_type;
	    cvl_frame_t *frame;
	    while (!cvl_error())
	    {
		cvl_read(f, &stream_type, &frame);
		if (!frame)
		    break;
		cvl_write(stdout, stream_type, frame);
		cvl_frame_free(frame);
	    }
	    error = cvl_error();
	    fclose(f);
	}
    }
    if (output.value)
	fclose(output.value);
    free(filenames);
    return error ? 1 : 0;
}
