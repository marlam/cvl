/*
 * cmd_split.c
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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
extern int errno;

#include "inttostr.h"
#include "xalloc.h"

#include <cvl/cvl.h>

#include "options.h"


void cmd_split_print_help(void)
{
    cvl_msg_fmt_req(
	    "split [-n|--n=<n>] [-t|--template=<template>] [-b|--backwards] [-s|--start=<i>]\n"
	    "\n"
	    "Split the input stream into multiple files, each containing n frames (default "
	    "is n = 1). The filename will be generated from the template: the template must "
	    "contain exactly one appearance of the character %%. This character must be "
	    "followed by one of the digits 1 through 9. The digit must be followed by the "
	    "uppercase character N. This special string %%xN will be replaced by the number "
	    "of the first frame of the stream contained in this file. The number will be "
	    "left-padded with zeros until its width is at least x characters. The default "
	    "template is \"frame-%%6N\".\n"
	    "A start number i for the first frame can be given, and the frames can be counted "
	    "backwards. If the frames are counted backwards, a start number is required, "
	    "because negative frame numbers are not accepted.");
}


bool template_is_ok(const char *t)
{
    char *p;
    
    return ((p = strchr(t, '%')) 
       	&& strrchr(t, '%') == p 
	&& isdigit((unsigned char)(*(p + 1))) 
	&& *(p + 1) != '0'
	&& *(p + 2) == 'N');
}


int cmd_split(int argc, char *argv[])
{
    option_int_t n = { 1, 1, INT_MAX };
    option_string_t t = { (char *)"frame-%6N", template_is_ok };
    option_bool_t b = { false, true };
    option_int_t s = { 0, 0, INT_MAX };
    option_t options[] = 
    { 
	{ "n",         'n', OPTION_INT,    &n, false },
	{ "template",  't', OPTION_STRING, &t, false },
	{ "backwards", 'b', OPTION_BOOL,   &b, false },
	{ "start",     's', OPTION_INT,    &s, false },
	null_option 
    };
    cvl_io_info_t *input_info;    
    int frame_number_index;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    frame_number_index = strchr(t.value, '%') - t.value;
    input_info = cvl_io_info_new();

    error = false;
    while (!cvl_io_eof(stdin, input_info))
    {
	FILE *outfile = NULL;
	cvl_frame_t *frame = NULL;
	cvl_io_info_t *output_info = NULL;

	for (int i = 0; i < n.value && !cvl_io_eof(stdin, input_info); i++)
	{
	    cvl_frame_free(frame);
	    if (!cvl_io_read(stdin, input_info, &frame))
	    {
		error = true;
		break;
	    }
	    if (!outfile)
	    {
		int frame_number;
		char frame_number_str[INT_BUFSIZE_BOUND(intmax_t)];
		size_t frame_number_len;
		char padded_frame_number_str[cvl_maxi(INT_BUFSIZE_BOUND(intmax_t), 10)];
		char *outfile_name = NULL;
		size_t outfile_name_len;
		int i;
		char *p;
		
		frame_number = s.value;
		if (b.value)
		{
		    frame_number -= cvl_io_info_frame_counter(input_info) - 1;
		}
		else
		{
		    frame_number += cvl_io_info_frame_counter(input_info) - 1;
		}
		if (frame_number < 0)
		{
		    cvl_msg_err("input frame %d: output frame number %d is negative", 
			    cvl_io_info_frame_counter(input_info) - 1, frame_number);
		    error = true;
		    break;
		}
		p = imaxtostr(frame_number, frame_number_str);
		frame_number_len = strlen(p);
		for (i = 0; i < (int)(t.value[frame_number_index + 1] - '0' - frame_number_len); i++)
		{
		    padded_frame_number_str[i] = '0';
		}
		strcpy(padded_frame_number_str + i, p);
		outfile_name_len = strlen(t.value) - 3 + strlen(padded_frame_number_str);
		outfile_name = xmalloc((outfile_name_len + 1) * sizeof(char));
		strncpy(outfile_name, t.value, frame_number_index);
		strcpy(outfile_name + frame_number_index, padded_frame_number_str);
		strcpy(outfile_name + frame_number_index + strlen(padded_frame_number_str),
			t.value + frame_number_index + 3);
		if (!(outfile = fopen(outfile_name, "w")))
		{
		    cvl_msg_err("cannot open %s: %s", outfile_name, strerror(errno));
		    free(outfile_name);
		    error = true;
		    break;
		}
		free(outfile_name);
		output_info = cvl_io_info_new();
		cvl_io_info_link_output_to_input(output_info, input_info);
	    }
	    if (!cvl_io_write(outfile, output_info, frame))
	    {
		error = true;
		break;
	    }
	}
	cvl_frame_free(frame);
	if (error)
	{
	    break;
	}
	if (fclose(outfile) != 0)
	{
	    cvl_msg_err("output error: %s", strerror(errno));
	    error = true;
	    break;
	}
	cvl_io_info_free(output_info);
    }

    cvl_io_info_free(input_info);
    return error ? 1 : 0;
}
