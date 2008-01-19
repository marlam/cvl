/*
 * cmd_combine.c
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

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_combine_print_help(void)
{
    mh_msg_fmt_req(
	    "combine [-m|--method=(lr|leftright)|(tb|topbottom)] [-j|--justify=(left|top)|center|(right|bottom)] "
	    "[-c|--color=<color>] file...\n"
	    "\n"
	    "Combine the given files by placing the frames side by side (leftright) or one "
	    "below the other (topbottom). The default is leftright. If the frames have different sizes, "
	    "then the smaller ones have to be aligned with the biggest one. The default is to "
	    "center them. The remaining space will be filled with the given color; the default is black.");
}

static bool check_color(const char *s)
{
    return (cvl_color_from_string(s) != CVL_COLOR_INVALID);
}

int cmd_combine(int argc, char *argv[])
{
    const char *method_names[] = { "lr", "leftright", "tb", "topbottom", NULL };
    mh_option_name_t method = { 0, method_names };
    const char *justify_names[] = { "left", "top", "center", "bottom", "right", NULL };
    mh_option_name_t justify = { 2, justify_names };
    mh_option_string_t color_string = { (char *)"black", check_color };
    mh_option_t options[] = 
    {
	{ "method",   'm', MH_OPTION_NAME,   &method,       false },
	{ "justify",  'j', MH_OPTION_NAME,   &justify,      false },
	{ "color",    'c', MH_OPTION_STRING, &color_string, false },
	mh_option_null
    };
    cvl_color_t color;
    int first_argument;
    int number_of_files;
    cvl_stream_type_t stream_type;
    bool stream_type_known;
    FILE **f;
    cvl_frame_t **frames;
    cvl_frame_t *combined_frame;
    bool error;
    bool eof;
    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }

    color = cvl_color_from_string(color_string.value);

    number_of_files = argc - first_argument;
    f = mh_alloc(number_of_files * sizeof(FILE *));
    for (int i = 0; i < number_of_files; i++)
    {
	if (!(f[i] = fopen(argv[first_argument + i], "r")))
	{
	    mh_msg_err("cannot open %s: %s", argv[first_argument + i], strerror(errno));
	    for (int j = 0; j < i; j++)
		fclose(f[j]);
	    free(f);
	    return 1;
	}
    }
    frames = mh_alloc(number_of_files * sizeof(cvl_frame_t *));
    
    stream_type_known = false;
    error = false;
    eof = false;
    for (;;)
    {
	/* Read one frame from each file */
	for (int i = 0; i < number_of_files; i++)
	{
	    if (!stream_type_known)
	    {
		cvl_read(f[i], &stream_type, &(frames[i]));
		stream_type_known = true;
	    }
	    else
	    {
		cvl_read(f[i], NULL, &(frames[i]));
	    }
	    error = cvl_error();
    	    eof = !error && !frames[i];
	    if (error || eof)
	    {
		for (int j = 0; j < i; i++)
		    cvl_frame_free(frames[j]);
		break;
	    }
	}
	if (error || eof)
	{
	    break;
	}
	
	/* Determine the lowest usable channels, format, and type */
	int combined_channels = 1;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_frame_channels(frames[i]) >= combined_channels)
	    {
		combined_channels = cvl_frame_channels(frames[i]);
	    }
	}
	cvl_format_t combined_format = CVL_LUM;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_frame_format(frames[i]) == CVL_RGB)
	    {
		combined_format = CVL_RGB;
	    }
	    else if (cvl_frame_format(frames[i]) == CVL_UNKNOWN)
	    {
		combined_format = CVL_UNKNOWN;
		break;	// worst case
	    }
	    else // other color formats
	    {
		combined_format = CVL_XYZ;
	    }
	}
	cvl_type_t combined_type = CVL_UINT8;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_frame_type(frames[i]) != CVL_UINT8)
	    {
		combined_type = CVL_FLOAT;
		break;	// worst case
	    }
	}

	float fillval[4];
	cvl_color_to_float(color, combined_format, fillval);
	if (method.value < 2)
	{
	    /* leftright */
	    int combined_width = 0;
	    int combined_height = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		cvl_convert_format_inplace(frames[i], combined_format);
		if (INT_MAX - cvl_frame_width(frames[i]) < combined_width)
		{
		    mh_msg_err("Combined frame would be too big");
		    error = true;
		    break;
		}
		combined_width += cvl_frame_width(frames[i]);
		if (cvl_frame_height(frames[i]) > combined_height)
		{
		    combined_height = cvl_frame_height(frames[i]);
		}		
	    }
	    if (error)
	    {
		break;
	    }
    	    combined_frame = cvl_frame_new(combined_width, combined_height, combined_channels,
		    combined_format, combined_type, CVL_TEXTURE);
	    int x_offset = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		if (justify.value < 2)		// top
		{
		    /* The frame */
		    cvl_copy_rect(combined_frame, x_offset, 0, 
			    frames[i], 0, 0, cvl_frame_width(frames[i]), cvl_frame_height(frames[i]));
		    /* The bottom rectangle */
		    if (combined_height - cvl_frame_height(frames[i]) > 0)
		    {
			cvl_fill_rect(combined_frame, x_offset, cvl_frame_height(frames[i]),
				cvl_frame_width(frames[i]), combined_height - cvl_frame_height(frames[i]),
				fillval);
		    }
		}
		else if (justify.value == 2)	// center
		{
		    int lines_to_fill = combined_height - cvl_frame_height(frames[i]);
		    /* The top rectangle */
		    if (lines_to_fill / 2 > 0)
		    {
			cvl_fill_rect(combined_frame, x_offset, 0,
				cvl_frame_width(frames[i]), lines_to_fill / 2,
				fillval);
		    }
		    /* The frame */
		    cvl_copy_rect(combined_frame, x_offset, lines_to_fill / 2,
			    frames[i], 0, 0, cvl_frame_width(frames[i]), cvl_frame_height(frames[i]));
		    /* The bottom rectangle */
		    if (combined_height - cvl_frame_height(frames[i]) - (lines_to_fill / 2) > 0)
		    {
			cvl_fill_rect(combined_frame, 
				x_offset, cvl_frame_height(frames[i]) + (lines_to_fill / 2),
				cvl_frame_width(frames[i]), 
				combined_height - cvl_frame_height(frames[i]) - (lines_to_fill / 2),
				fillval);
		    }
		}
		else				// bottom
		{
		    /* The top rectangle */
		    if (combined_height - cvl_frame_height(frames[i]) > 0)
		    {
			cvl_fill_rect(combined_frame, x_offset, 0,
				cvl_frame_width(frames[i]), combined_height - cvl_frame_height(frames[i]),
				fillval);
		    }
		    /* The frame */
		    cvl_copy_rect(combined_frame, x_offset, combined_height - cvl_frame_height(frames[i]), 
			    frames[i], 0, 0, cvl_frame_width(frames[i]), cvl_frame_height(frames[i]));
		}
		x_offset += cvl_frame_width(frames[i]);
	    }
	}
	else
	{
	    /* topbottom */
	    int combined_width = 0;
	    int combined_height = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		cvl_convert_format_inplace(frames[i], combined_format);
		if (INT_MAX - cvl_frame_height(frames[i]) < combined_height)
		{
		    mh_msg_err("Combined frame would be too big");
		    error = true;
		    break;
		}
		combined_height += cvl_frame_height(frames[i]);
		if (cvl_frame_width(frames[i]) > combined_width)
		{
		    combined_width = cvl_frame_width(frames[i]);
		}		
	    }
	    if (error)
	    {
		break;
	    }
    	    combined_frame = cvl_frame_new(combined_width, combined_height, combined_channels, 
		    combined_format, combined_type, CVL_TEXTURE);
	    int y_offset = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		if (justify.value < 2)		// left
		{
		    /* The frame */
		    cvl_copy_rect(combined_frame, 0, y_offset, 
			    frames[i], 0, 0, cvl_frame_width(frames[i]), cvl_frame_height(frames[i]));
		    /* The right rectangle */
		    if (combined_width - cvl_frame_width(frames[i]) > 0)
		    {
			cvl_fill_rect(combined_frame, cvl_frame_width(frames[i]), y_offset,
				combined_width - cvl_frame_width(frames[i]), cvl_frame_height(frames[i]),
				fillval);
		    }
		}
		else if (justify.value == 2)	// center
		{
		    int columns_to_fill = combined_width - cvl_frame_width(frames[i]);
		    /* The left rectangle */
		    if (columns_to_fill / 2 > 0)
		    {
			cvl_fill_rect(combined_frame, 0, y_offset,
				columns_to_fill / 2, cvl_frame_height(frames[i]),
				fillval);
		    }
		    /* The frame */
		    cvl_copy_rect(combined_frame, columns_to_fill / 2, y_offset,
			    frames[i], 0, 0, cvl_frame_width(frames[i]), cvl_frame_height(frames[i]));
		    /* The right rectangle */
		    if (combined_width - cvl_frame_width(frames[i]) - (columns_to_fill / 2) > 0)
		    {
			cvl_fill_rect(combined_frame, 
				cvl_frame_width(frames[i]) + (columns_to_fill / 2), y_offset,
				combined_width - cvl_frame_width(frames[i]) - (columns_to_fill / 2), 
				cvl_frame_height(frames[i]),
				fillval);
		    }
		}
		else				// right
		{
		    /* The left rectangle */
		    if (combined_width - cvl_frame_width(frames[i]) > 0)
		    {
			cvl_fill_rect(combined_frame, 0, y_offset,
				combined_width - cvl_frame_width(frames[i]), cvl_frame_height(frames[i]),
				fillval);
		    }
		    /* The frame */
		    cvl_copy_rect(combined_frame, combined_width - cvl_frame_width(frames[i]), y_offset, 
			    frames[i], 0, 0, cvl_frame_width(frames[i]), cvl_frame_height(frames[i]));
		}
		y_offset += cvl_frame_height(frames[i]);
	    }
	}
	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_frame_free(frames[i]);
	}
	if (error)
	{
	    break;
	}

	/* Output */
	cvl_write(stdout, stream_type, combined_frame);
	cvl_frame_free(combined_frame);
	error = cvl_error();
	if (error)
	{
	    break;
	}
    }

    free(frames);
    for (int i = 0; i < number_of_files; i++)
	fclose(f[i]);
    free(f);
    return error ? 1 : 0;
}
