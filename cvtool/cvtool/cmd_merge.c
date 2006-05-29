/*
 * cmd_merge.c
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include <gsl/gsl_randist.h>

#include <cvl/cvl.h>

#include "xalloc.h"


void cmd_merge_print_help(void)
{
    cvl_msg_fmt_req(
	    "merge [-s|--shuffle] [-o|--output=<file>] file...\n"
	    "\n"
	    "Merges files into one stream, in the given order. "
	    "If --shuffle is used, the order will be randomized. "
	    "The file names will be printed to stderr in the order they are merged. "
	    "If --output is used, the file names will be written to the given file instead.");
}


int cmd_merge(int argc, char *argv[])
{
    cvl_option_bool_t shuffle = { false, true };
    cvl_option_file_t output = { NULL, "w", false };
    cvl_option_t options[] = 
    {
	{ "shuffle", 's', CVL_OPTION_BOOL, &shuffle, false },
	{ "output",  'o', CVL_OPTION_FILE, &output,  false },
	cvl_option_null 
    };
    int first_argument;
    int number_of_files;
    char **filenames;
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;    
    bool error;
    FILE *f;
    cvl_frame_t *frame;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvl_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }

    number_of_files = argc - first_argument;
    filenames = xmalloc(number_of_files * sizeof(char *));
    for (int i = 0; i < number_of_files; i++)
    {
	filenames[i] = argv[first_argument + i];
    }
    if (shuffle.value)
    {
	cvl_rng_init(0);
	gsl_ran_shuffle(cvl_rng, filenames, number_of_files, sizeof(char *));
    }
    
    output_info = cvl_io_info_new();
    error = false;
    for (int i = 0; !error && i < number_of_files; i++)
    {
	if (!(f = fopen(filenames[i], "r")))
	{
	    cvl_msg_err("cannot open %s: %s", filenames[i], strerror(errno));
	    error = true;
	}
	else
	{
	    cvl_msg(output.value ? output.value : stderr, CVL_MSG_REQ, "%s", filenames[i]);
	    input_info = cvl_io_info_new();
	    cvl_io_info_link_output_to_input(output_info, input_info);
	    while (!error && !cvl_io_eof(f, input_info))
	    {
		frame = NULL;
		error = (!cvl_io_read(f, input_info, &frame)
			|| !cvl_io_write(stdout, output_info, frame));
		cvl_frame_free(frame);
	    }
	    cvl_io_info_free(input_info);
	    fclose(f);
	}
    }
    cvl_io_info_free(output_info);
    if (output.value)
    {
	fclose(output.value);
    }
    free(filenames);
    return error ? 1 : 0;
}
