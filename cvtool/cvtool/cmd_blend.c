/*
 * cmd_blend.c
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

#include <cvl/cvl.h>


void cmd_blend_print_help(void)
{
    cvl_msg_fmt_req(
	    "blend -s|--source=<file> [-a|--alpha=<file>] [-S|--single] [-x|--x=<x>] [-y|--y=<y>]\n"
	    "\n"
	    "Blends the source into the image stream, using the alpha map a. "
	    "With no alpha map, the source is simply copied into the images. "
	    "x and y specify the position that the source should be copied to. "
	    "The default is (0,0). Positions outside of the images are possible: "
	    "parts of the source that do not fit into the "
	    "images will be ignored. When --single is used, only the first "
	    "frame of the source will be used; this frame will be copied into "
	    "all images of the stream.");
}


int cmd_blend(int argc, char *argv[])
{
    cvl_option_file_t source = { NULL, "r", false };
    cvl_option_file_t alpha = { NULL, "r", false };
    cvl_option_bool_t single = { false, true };
    cvl_option_int_t x = { 0, INT_MIN, INT_MAX };
    cvl_option_int_t y = { 0, INT_MIN, INT_MAX };
    cvl_option_t options[] =
    {
	{ "source", 's', CVL_OPTION_FILE, &source, true },
	{ "alpha",  'a', CVL_OPTION_FILE, &alpha,  false },
	{ "single", 'S', CVL_OPTION_BOOL, &single, false },
	{ "x",      'x', CVL_OPTION_INT,  &x,      false },
	{ "y",      'y', CVL_OPTION_INT,  &y,      false },
	cvl_option_null
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *source_info;
    cvl_io_info_t *alpha_info;
    cvl_io_info_t *output_info;    
    cvl_frame_t *frame;
    cvl_frame_t *source_frame;
    cvl_frame_t *source_frame_bak;
    cvl_frame_t *alpha_frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    input_info = cvl_io_info_new();
    source_info = cvl_io_info_new();
    alpha_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    error = false;
    source_frame = NULL;
    source_frame_bak = NULL;
    /* While none of the needed streams is at EOF */
    while (!error && !cvl_io_eof(stdin, input_info)
	    && (single.value || (!single.value && !cvl_io_eof(source.value, source_info)
		    && (!alpha.value || (alpha.value && !cvl_io_eof(alpha.value, alpha_info))))))
    {
	/* Read frame */
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	/* Read block to be blended into the frame, if necessary */
	if (!single.value || !source_frame)
	{
	    if (!cvl_io_read(source.value, source_info, &source_frame))
	    {
		cvl_frame_free(frame);
		error = true;
		break;
	    }
	    if (alpha.value)
	    {
		if (!cvl_io_read(alpha.value, alpha_info, &alpha_frame))
		{
		    cvl_frame_free(frame);
		    cvl_frame_free(source_frame);
		    error = true;
		    break;
		}
		else
		{
		    cvl_frame_to_gray(alpha_frame);
		    if (cvl_frame_width(alpha_frame) != cvl_frame_width(source_frame)
			    || cvl_frame_height(alpha_frame) != cvl_frame_height(source_frame))
		    {
			cvl_msg_err("alpha and source frames must have the same dimensions");
			error = true;
			break;
		    }
		}
	    }
	}
	/* Convert the pixel type if necessary. If only a single block frame is
	 * used, convert from the original, backed up copy of this, because
	 * multiple conversions are not reversible: RGB->GRAY->RGB won't work. */
	if (cvl_frame_pixel_type(source_frame) != cvl_frame_pixel_type(frame))
	{
	    if (single.value)
	    {
		if (!source_frame_bak)
		{
		    source_frame_bak = cvl_frame_clone(source_frame);
		}
		else
		{
		    cvl_frame_free(source_frame);
		    source_frame = cvl_frame_clone(source_frame_bak);
		}
	    }
    	    cvl_frame_convert(source_frame, cvl_frame_pixel_type(frame));
	}
	/* Cut the block if it does not fit entirely into the frame, or ignore
	 * the block if it is completely outside of the frame, or blend/copy it
	 * as-is into the frame. */
	if (x.value < 0 || x.value + cvl_frame_width(source_frame) > cvl_frame_width(frame)
		|| y.value < 0 || y.value + cvl_frame_height(source_frame) > cvl_frame_height(frame))
	{
	    int x_offset = (x.value < 0) ? - x.value : 0;
	    int y_offset = (y.value < 0) ? - y.value : 0;
	    int new_width = cvl_mini(cvl_frame_width(frame) - x.value, 
		    cvl_frame_width(source_frame) - x_offset);
	    int new_height = cvl_mini(cvl_frame_height(frame) - y.value, 
		    cvl_frame_height(source_frame) - y_offset);
	    
	    if (new_width > 0 && new_height > 0)
	    {
		cvl_frame_t *source_tmp = cvl_cut(source_frame, 
			x_offset, y_offset, new_width, new_height);
		if (alpha.value)
		{
		    cvl_frame_t *alpha_tmp = cvl_cut(alpha_frame, 
			    x_offset, y_offset, new_width, new_height);
		    cvl_blend(frame, cvl_maxi(x.value, 0), cvl_maxi(y.value, 0),
			    source_tmp, alpha_tmp);
		    cvl_frame_free(alpha_tmp);
		}
		else
		{
		    cvl_frame_copy_rect(frame, cvl_maxi(x.value, 0), cvl_maxi(y.value, 0), 
			    source_tmp, 0, 0, new_width, new_height);
		}
		cvl_frame_free(source_tmp);
	    }
	}
	else
	{
	    if (alpha.value)
	    {
    		cvl_blend(frame, x.value, y.value, source_frame, alpha_frame);
    	    }
    	    else
    	    {
		cvl_frame_copy_rect(frame, x.value, y.value, source_frame, 0, 0, 
			cvl_frame_width(source_frame), cvl_frame_height(source_frame));
	    }
	}
	if (!single.value)
	{
	    cvl_frame_free(source_frame);
	    if (alpha.value)
	    {
		cvl_frame_free(alpha_frame);
	    }
	}
	/* Output */
	if (!cvl_io_write(stdout, output_info, frame))
	{
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_frame_free(frame);
    }
    
    /* Cleanup */
    cvl_frame_free(source_frame_bak);
    if (single.value)
    {
	cvl_frame_free(source_frame);
	if (alpha.value)
	{
	    cvl_frame_free(alpha_frame);
	}
    }
    fclose(source.value);
    if (alpha.value)
    {
	fclose(alpha.value);
    }
    cvl_io_info_free(input_info);
    cvl_io_info_free(source_info);
    cvl_io_info_free(alpha_info);
    cvl_io_info_free(output_info);
    
    return error ? 1 : 0;
}
