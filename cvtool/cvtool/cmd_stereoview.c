/*
 * cmd_stereoview.c
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
#include <string.h>

#include <cvl/cvl.h>


void cmd_stereoview_print_help(void)
{
    cvl_msg_fmt_req(
	    "stereoview anaglyph [-c|--color[=on|off]] [-g|--glasses=red-cyan|red-green|red-blue]\n"
	    "stereoview 3d-display -f|--format=lr|tb|ci|ri [-w|--width=<w>] [-h|--height=<h>]\n"
	    "\n"
	    "Prepares stereoscopic image pairs for a display device.\n"
	    "anaglyph: converts stereo frames (left and right view side by side) into anaglyph images, viewable "
	    "with appropriate color 3D glasses. The default is to create graylevel anaglyphs for "
	    "red-blue glasses.\n"
	    "3d-display: convert stereo frames (left and right view side by side) into a format that can be viewed "
	    "directly on a 3D display by DTI (http://www.dti3d.com/): just play the resulting video "
	    "with any media player in fullscreen mode. The width and height parameters should be set "
	    "to the resolution of the 3D display; the default is 1280x1024. The formats are: lr = "
	    "left-right (S/S on DTI display), tb = top-bottom (T/B on DTI display), ci = "
	    "column-interleaved (Fr/S on DTI display), ri = row-interleaved (F/S on DTI display).");
}


int cmd_stereoview(int argc, char *argv[])
{
    typedef enum { ANAGLYPH, THREEDIM_DISPLAY } subcommand_t;
    subcommand_t subcommand;
    cvl_option_bool_t color = { false, true };
    const char *glasses_names[] = { "red-cyan", "red-green", "red-blue", NULL };
    cvl_option_name_t glasses = { 2, glasses_names };
    cvl_option_t anaglyph_options[] = 
    {
	{ "color",   'c', CVL_OPTION_BOOL, &color,   false },
	{ "glasses", 'g', CVL_OPTION_NAME, &glasses, false },
	cvl_option_null
    };
    enum { LEFT_RIGHT = 0, TOP_BOTTOM = 1, COLUMN_INTERLEAVED = 2, ROW_INTERLEAVED = 3 }; 
    const char *format_names[] = { "lr", "tb", "ci", "ri", NULL };
    cvl_option_name_t format = { -1, format_names };
    cvl_option_int_t width = { 1280, 1, INT_MAX };
    cvl_option_int_t height = { 1024, 1, INT_MAX };
    cvl_option_t threedim_display_options[] = 
    {
	{ "format", 'f', CVL_OPTION_NAME, &format, true },
	{ "width",  'w', CVL_OPTION_INT,  &width,  false },
	{ "height", 'h', CVL_OPTION_INT,  &height, false },
	cvl_option_null
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    cvl_frame_t *stereoframe;
    cvl_frame_t *outputframe;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!argv[1]) 
    {
	cvl_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "anaglyph") == 0)
    {
	subcommand = ANAGLYPH;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), anaglyph_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "3d-display") == 0)
    {
	subcommand = THREEDIM_DISPLAY;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), threedim_display_options, 0, 0, NULL);
	if (!error)
	{
	    if (width.value % 2 != 0 || height.value % 2 != 0)
	    {
		cvl_msg_err("both width and height must be even numbers");
		error = true;
	    }
	}		
    }
    else
    {
	cvl_msg_err("unknown subcommand");
	error = true;
    }
    if (error)
    {
	return 1;
    }
    
    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    while (!cvl_io_eof(stdin, input_info))
    {
	if (!cvl_io_read(stdin, input_info, &stereoframe))
	{
	    error = true;
	    break;
	}
	if (cvl_frame_width(stereoframe) % 2 != 0)
	{
	    cvl_msg_err("input frame is not a stereo frame: odd number of columns");
	    error = true;
	    break;
	}
	if (subcommand == ANAGLYPH)
	{
	    outputframe = cvl_anaglyph(stereoframe, color.value, glasses.value);
	}
	else // subcommand == THREEDIM_DISPLAY
	{
	    cvl_frame_t *l, *sl, *rsl, *r, *sr, *rsr;
	    // The scale parameters for both halves
	    int new_width, new_height;
	    double factor_h, factor_v;
	    // The resize-offset parameters for both halves
	    int offset_x, offset_y;
	    
	    if (format.value == LEFT_RIGHT || format.value == COLUMN_INTERLEAVED)
	    {
		new_width = width.value / 2;
		factor_h = (double)new_width / (double)(cvl_frame_width(stereoframe) / 2);
		factor_v = 2.0 * factor_h;
		new_height = cvl_maxi(1, cvl_iround(factor_v * (double)cvl_frame_height(stereoframe)));
		offset_x = 0;
		offset_y = (height.value - new_height) / 2;
		if (new_height > height.value)
		{
		    new_height = height.value;
		    factor_v = (double)new_height / (double)cvl_frame_height(stereoframe);
		    factor_h = 0.5 * factor_v;
		    new_width = cvl_maxi(1, cvl_iround(factor_h * (double)(cvl_frame_width(stereoframe) / 2)));
		    offset_x = (width.value / 2 - new_width) / 2;
		    offset_y = 0;
		}
		l = cvl_cut(stereoframe, 0, 0, 
			cvl_frame_width(stereoframe) / 2, cvl_frame_height(stereoframe));
		sl = cvl_scale(l, CVL_INTERPOLATION_BILINEAR, new_width, new_height);
		cvl_frame_free(l);
		rsl = cvl_resize(sl, CVL_COLOR_BLACK, width.value / 2, height.value, offset_x, offset_y);
		cvl_frame_free(sl);			
		r = cvl_cut(stereoframe, cvl_frame_width(stereoframe) / 2, 0, 
			cvl_frame_width(stereoframe) / 2, cvl_frame_height(stereoframe));
		sr = cvl_scale(r, CVL_INTERPOLATION_BILINEAR, new_width, new_height);
		cvl_frame_free(r);
		rsr = cvl_resize(sr, CVL_COLOR_BLACK, width.value / 2, height.value, offset_x, offset_y);
		cvl_frame_free(sr);
		outputframe = cvl_frame_new(cvl_frame_pixel_type(stereoframe), width.value, height.value);
		if (format.value == LEFT_RIGHT)
		{
		    cvl_frame_copy_rect(outputframe, 0, 0, 
			    rsl, 0, 0, width.value / 2, height.value);
		    cvl_frame_copy_rect(outputframe, width.value / 2, 0, 
			    rsr, 0, 0, width.value / 2, height.value);
		}
		else
		{
		    for (int x = 0; x < width.value; x++)
		    {
			cvl_frame_copy_rect(outputframe, x, 0, 
		    		(x % 2 == 0) ? rsl : rsr, x / 2, 0, 1, height.value);
		    }
		}
		cvl_frame_free(rsl);
		cvl_frame_free(rsr);
	    }
	    else // (format.value == TOP_BOTTOM || format.value == ROW_INTERLEAVED)
	    {
		new_height = height.value / 2;
		factor_v = (double)new_height / (double)(cvl_frame_height(stereoframe) / 2);
		factor_h = 2.0 * factor_v;
		new_width = cvl_maxi(1, cvl_iround(factor_h * (double)(cvl_frame_width(stereoframe) / 2)));
		offset_x = (width.value - new_width) / 2;
		offset_y = 0;
		if (new_width > width.value)
		{
		    new_width = width.value;
		    factor_h = (double)new_width / (double)(cvl_frame_width(stereoframe) / 2);
		    factor_v = 0.5 * factor_h;
		    new_height = cvl_maxi(1, cvl_iround(factor_v * (double)cvl_frame_height(stereoframe)));
		    offset_x = 0;
		    offset_y = (height.value / 2 - new_height) / 2;
		}		
		l = cvl_cut(stereoframe, 0, 0, 
			cvl_frame_width(stereoframe) / 2, cvl_frame_height(stereoframe));
		sl = cvl_scale(l, CVL_INTERPOLATION_BILINEAR, new_width, new_height);
		cvl_frame_free(l);
		rsl = cvl_resize(sl, CVL_COLOR_BLACK, width.value, height.value / 2, offset_x, offset_y);
		cvl_frame_free(sl);			
		r = cvl_cut(stereoframe, cvl_frame_width(stereoframe) / 2, 0, 
			cvl_frame_width(stereoframe) / 2, cvl_frame_height(stereoframe));
		sr = cvl_scale(r, CVL_INTERPOLATION_BILINEAR, new_width, new_height);
		cvl_frame_free(r);
		rsr = cvl_resize(sr, CVL_COLOR_BLACK, width.value, height.value / 2, offset_x, offset_y);
		cvl_frame_free(sr);
		outputframe = cvl_frame_new(cvl_frame_pixel_type(stereoframe), width.value, height.value);
		if (format.value == TOP_BOTTOM)
		{
		    cvl_frame_copy_rect(outputframe, 0, 0, 
			    rsl, 0, 0, width.value, height.value / 2);
		    cvl_frame_copy_rect(outputframe, 0, height.value / 2, 
			    rsr, 0, 0, width.value, height.value / 2);
		}
		else
		{
		    for (int y = 0; y < height.value; y++)
		    {
			cvl_frame_copy_rect(outputframe, 0, y, 
		    		(y % 2 == 0) ? rsl : rsr, 0, y / 2, width.value, 1);
		    }
		}
		cvl_frame_free(rsl);
		cvl_frame_free(rsr);
	    }
	}
     	cvl_frame_free(stereoframe);
	cvl_io_info_set_width(output_info, cvl_frame_width(outputframe));
	cvl_io_info_set_height(output_info, cvl_frame_height(outputframe));
	if (!cvl_io_write(stdout, output_info, outputframe))
	{
	    cvl_frame_free(outputframe);
	    error = true;
	    break;
	}
	cvl_frame_free(outputframe);
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
