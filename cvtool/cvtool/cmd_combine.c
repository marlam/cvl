/*
 * cmd_combine.c
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

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
extern int errno;

#include <cvl/cvl.h>

#include "xalloc.h"


void cmd_combine_print_help(void)
{
    cvl_msg_fmt_req(
	    "combine [-m|--method=(lr|leftright)|(tb|topbottom)] [-j|--justify=(left|top)|center|(right|bottom)] "
	    "[-c|--color=<color>] file...\n"
	    "\n"
	    "Combine the given files by placing the frames side by side (leftright) or one "
	    "below the other (topbottom). The default is leftright. If the frames have different sizes, "
	    "then the smaller ones have to be aligned with the biggest one. The default is to "
	    "center them. The remaining space will be filled with the given color; the default is black.");
}


int cmd_combine(int argc, char *argv[])
{
    const char *method_names[] = { "lr", "leftright", "tb", "topbottom", NULL };
    cvl_option_name_t method = { 0, method_names };
    const char *justify_names[] = { "left", "top", "center", "bottom", "right", NULL };
    cvl_option_name_t justify = { 2, justify_names };
    cvl_option_color_t color = { CVL_COLOR_BLACK };
    cvl_option_t options[] = 
    {
	{ "method",   'm', CVL_OPTION_NAME,  &method,  false },
	{ "justify",  'j', CVL_OPTION_NAME,  &justify, false },
	{ "color",    'c', CVL_OPTION_COLOR, &color,   false },
	cvl_option_null
    };
    cvl_io_info_t *combined_info;
    cvl_frame_t *combined_frame;
    int first_argument;
    int number_of_files;
    cvl_io_info_t **input_info;
    FILE **f;
    cvl_frame_t **frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 1, -1, &first_argument))
    {
	return 1;
    }

    number_of_files = argc - first_argument;
    f = xmalloc(number_of_files * sizeof(FILE *));
    input_info = xmalloc(number_of_files * sizeof(cvl_io_info_t *));
    for (int i = 0; i < number_of_files; i++)
    {
	input_info[i] = cvl_io_info_new();
	if (!(f[i] = fopen(argv[first_argument + i], "r")))
	{
	    cvl_msg_err("cannot open %s: %s", argv[first_argument + i], strerror(errno));
	    for (int j = 0; j < i; j++)
	    {
		cvl_io_info_free(input_info[j]);
		fclose(f[j]);
	    }
	    free(input_info);
	    free(f);
	    return 1;
	}
    }
    frame = xmalloc(number_of_files * sizeof(cvl_frame_t *));
    combined_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(combined_info, input_info[0]);
    
    error = false;
    for (;;)
    {
	/* check for EOF */
	bool eof = false;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_io_eof(f[i], input_info[i]))
	    {
		eof = true;
		break;
	    }
	}
	if (eof)
	{
	    break;
	}

	/* read one frame from each file */
	for (int i = 0; i < number_of_files; i++)
	{
    	    if (!cvl_io_read(f[i], input_info[i], &(frame[i])))
    	    {
		for (int j = 0; j < i; i++)
		{
		    cvl_frame_free(frame[j]);
		}
    		error = true;
    		break;
    	    }
	}
	if (error)
	{
	    break;
	}
	
	/* determine the lowest usable pixel type */
	cvl_pixel_type_t combined_pixel_type = CVL_PIXEL_GRAY;
	for (int i = 0; i < number_of_files; i++)
	{
	    if (cvl_frame_pixel_type(frame[i]) == CVL_PIXEL_YUV)
	    {
		combined_pixel_type = CVL_PIXEL_YUV;
	    }
	    else if (cvl_frame_pixel_type(frame[i]) == CVL_PIXEL_RGB)
	    {
		combined_pixel_type = CVL_PIXEL_RGB;
		break;	// worst case
	    }
	}
	/* convert the fill color to this pixel type */
	cvl_pixel_t fillpixel = cvl_color_to_pixel(color.value, combined_pixel_type);

	if (method.value < 2)
	{
	    /* leftright */
	    int combined_width = 0;
	    int combined_height = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		cvl_frame_convert(frame[i], combined_pixel_type);
		if (INT_MAX - cvl_frame_width(frame[i]) < combined_width)
		{
		    cvl_msg_err("combined frame would be too big");
		    error = true;
		    break;
		}
		combined_width += cvl_frame_width(frame[i]);
		if (cvl_frame_height(frame[i]) > combined_height)
		{
		    combined_height = cvl_frame_height(frame[i]);
		}		
	    }
	    if (error)
	    {
		break;
	    }
    	    combined_frame = cvl_frame_new(combined_pixel_type, combined_width, combined_height);
	    int x_offset = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		if (justify.value < 2)		// top
		{
		    /* the frame */
		    cvl_frame_copy_rect(combined_frame, x_offset, 0, 
			    frame[i], 0, 0, cvl_frame_width(frame[i]), cvl_frame_height(frame[i]));
		    /* the bottom rectangle */
		    if (combined_height - cvl_frame_height(frame[i]) > 0)
		    {
			cvl_frame_fill_rect(combined_frame, x_offset, cvl_frame_height(frame[i]),
				cvl_frame_width(frame[i]), combined_height - cvl_frame_height(frame[i]),
				fillpixel);
		    }
		}
		else if (justify.value == 2)	// center
		{
		    int lines_to_fill = combined_height - cvl_frame_height(frame[i]);
		    /* the top rectangle */
		    if (lines_to_fill / 2 > 0)
		    {
			cvl_frame_fill_rect(combined_frame, x_offset, 0,
				cvl_frame_width(frame[i]), lines_to_fill / 2,
				fillpixel);
		    }
		    /* the frame */
		    cvl_frame_copy_rect(combined_frame, x_offset, lines_to_fill / 2,
			    frame[i], 0, 0, cvl_frame_width(frame[i]), cvl_frame_height(frame[i]));
		    /* the bottom rectangle */
		    if (combined_height - cvl_frame_height(frame[i]) - (lines_to_fill / 2) > 0)
		    {
			cvl_frame_fill_rect(combined_frame, 
				x_offset, cvl_frame_height(frame[i]) + (lines_to_fill / 2),
				cvl_frame_width(frame[i]), 
				combined_height - cvl_frame_height(frame[i]) - (lines_to_fill / 2),
				fillpixel);
		    }
		}
		else				// bottom
		{
		    /* the top rectangle */
		    if (combined_height - cvl_frame_height(frame[i]) > 0)
		    {
			cvl_frame_fill_rect(combined_frame, x_offset, 0,
				cvl_frame_width(frame[i]), combined_height - cvl_frame_height(frame[i]),
				fillpixel);
		    }
		    /* the frame */
		    cvl_frame_copy_rect(combined_frame, x_offset, combined_height - cvl_frame_height(frame[i]), 
			    frame[i], 0, 0, cvl_frame_width(frame[i]), cvl_frame_height(frame[i]));
		}
		x_offset += cvl_frame_width(frame[i]);
	    }
	}
	else
	{
	    /* topbottom */
	    int combined_width = 0;
	    int combined_height = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		cvl_frame_convert(frame[i], combined_pixel_type);
		if (INT_MAX - cvl_frame_height(frame[i]) < combined_height)
		{
		    cvl_msg_err("combined frame would be too big");
		    error = true;
		    break;
		}
		combined_height += cvl_frame_height(frame[i]);
		if (cvl_frame_width(frame[i]) > combined_width)
		{
		    combined_width = cvl_frame_width(frame[i]);
		}		
	    }
	    if (error)
	    {
		break;
	    }
	    combined_frame = cvl_frame_new(combined_pixel_type, combined_width, combined_height);
	    int y_offset = 0;
	    for (int i = 0; i < number_of_files; i++)
	    {
		if (justify.value < 2)		// left
		{
		    /* the frame */
		    cvl_frame_copy_rect(combined_frame, 0, y_offset, 
			    frame[i], 0, 0, cvl_frame_width(frame[i]), cvl_frame_height(frame[i]));
		    /* the right rectangle */
		    if (combined_width - cvl_frame_width(frame[i]) > 0)
		    {
			cvl_frame_fill_rect(combined_frame, cvl_frame_width(frame[i]), y_offset,
				combined_width - cvl_frame_width(frame[i]), cvl_frame_height(frame[i]),
				fillpixel);
		    }
		}
		else if (justify.value == 2)	// center
		{
		    int columns_to_fill = combined_width - cvl_frame_width(frame[i]);
		    /* the left rectangle */
		    if (columns_to_fill / 2 > 0)
		    {
			cvl_frame_fill_rect(combined_frame, 0, y_offset,
				columns_to_fill / 2, cvl_frame_height(frame[i]),
				fillpixel);
		    }
		    /* the frame */
		    cvl_frame_copy_rect(combined_frame, columns_to_fill / 2, y_offset,
			    frame[i], 0, 0, cvl_frame_width(frame[i]), cvl_frame_height(frame[i]));
		    /* the right rectangle */
		    if (combined_width - cvl_frame_width(frame[i]) - (columns_to_fill / 2) > 0)
		    {
			cvl_frame_fill_rect(combined_frame, 
				cvl_frame_width(frame[i]) + (columns_to_fill / 2), y_offset,
				combined_width - cvl_frame_width(frame[i]) - (columns_to_fill / 2), 
				cvl_frame_height(frame[i]),
				fillpixel);
		    }
		}
		else				// right
		{
		    /* the left rectangle */
		    if (combined_width - cvl_frame_width(frame[i]) > 0)
		    {
			cvl_frame_fill_rect(combined_frame, 0, y_offset,
				combined_width - cvl_frame_width(frame[i]), cvl_frame_height(frame[i]),
				fillpixel);
		    }
		    /* the frame */
		    cvl_frame_copy_rect(combined_frame, combined_width - cvl_frame_width(frame[i]), y_offset, 
			    frame[i], 0, 0, cvl_frame_width(frame[i]), cvl_frame_height(frame[i]));
		}
		y_offset += cvl_frame_height(frame[i]);
	    }
	}
	for (int i = 0; i < number_of_files; i++)
	{
	    cvl_frame_free(frame[i]);
	}
	if (error)
	{
	    break;
	}

	/* output */
	cvl_io_info_set_width(combined_info, cvl_frame_width(combined_frame));
	cvl_io_info_set_height(combined_info, cvl_frame_height(combined_frame));
	if (!cvl_io_write(stdout, combined_info, combined_frame))
	{
	    cvl_frame_free(combined_frame);
	    error = true;
	    break;
	}
	cvl_frame_free(combined_frame);
    }

    free(frame);
    for (int i = 0; i < number_of_files; i++)
    {
	fclose(f[i]);
	cvl_io_info_free(input_info[i]);
    }
    free(f);
    free(input_info);
    cvl_io_info_free(combined_info);
    return error ? 1 : 0;
}
