/*
 * cmd_split.c
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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#include "inttostr.h"

#include <cvl/cvl.h>

#include "mh.h"


void cmd_split_print_help(void)
{
    mh_msg_fmt_req(
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


static bool template_is_ok(const char *t)
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
    mh_option_int_t n = { 1, 1, INT_MAX };
    mh_option_string_t t = { (char *)"frame-%6N", template_is_ok };
    mh_option_bool_t b = { false, true };
    mh_option_int_t s = { 0, 0, INT_MAX };
    mh_option_t options[] = 
    { 
	{ "n",         'n', MH_OPTION_INT,    &n, false },
	{ "template",  't', MH_OPTION_STRING, &t, false },
	{ "backwards", 'b', MH_OPTION_BOOL,   &b, false },
	{ "start",     's', MH_OPTION_INT,    &s, false },
	mh_option_null 
    };
    cvl_stream_type_t stream_type;
    int frame_number_index;
    int frame_counter;
    bool error;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    frame_number_index = strchr(t.value, '%') - t.value;
    frame_counter = 0;
    error = false;
    for (;;)
    {
	FILE *outfile = NULL;
	cvl_frame_t *frame;

	for (int i = 0; i < n.value; i++)
	{
	    cvl_read(stdin, &stream_type, &frame);
	    if (!frame)
		break;
	    frame_counter++;
	    if (!outfile)
	    {
		int frame_number;
		char frame_number_str[INT_BUFSIZE_BOUND(intmax_t)];
		size_t frame_number_len;
		char padded_frame_number_str[mh_maxi(INT_BUFSIZE_BOUND(intmax_t), 10)];
		char *outfile_name = NULL;
		size_t outfile_name_len;
		int i;
		char *p;
		
		frame_number = s.value;
		if (b.value)
		    frame_number -= frame_counter - 1;
		else
		    frame_number += frame_counter - 1;
		if (frame_number < 0)
		{
		    mh_msg_err("Input frame %d: output frame number %d is negative", 
			    frame_counter - 1, frame_number);
		    cvl_frame_free(frame);
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
		outfile_name = mh_alloc((outfile_name_len + 1) * sizeof(char));
		strncpy(outfile_name, t.value, frame_number_index);
		strcpy(outfile_name + frame_number_index, padded_frame_number_str);
		strcpy(outfile_name + frame_number_index + strlen(padded_frame_number_str),
			t.value + frame_number_index + 3);
		if (!(outfile = fopen(outfile_name, "w")))
		{
		    mh_msg_err("Cannot open %s: %s", outfile_name, strerror(errno));
		    free(outfile_name);
		    cvl_frame_free(frame);
		    error = true;
		    break;
		}
		free(outfile_name);
	    }
	    cvl_write(outfile, stream_type, frame);
    	    cvl_frame_free(frame);
	    error = cvl_error();
	    if (error)
	    {
		break;
	    }
	}
	if (ferror(stdin))
	{
	    error = true;
	}
	if (error || feof(stdin))
	{
	    break;
	}
	if (fclose(outfile) != 0)
	{
	    mh_msg_err("Output error: %s", strerror(errno));
	    error = true;
	    break;
	}
    }

    return error ? 1 : 0;
}
