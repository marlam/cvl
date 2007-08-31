/*
 * cmd_blend.c
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

#include <cvl/cvl.h>

#include "mh.h"


void cmd_blend_print_help(void)
{
    mh_msg_fmt_req(
	    "blend -s|--source=<file> [-a|--alpha=<file>] [-S|--single] [-x|--x=<x>] [-y|--y=<y>]\n"
	    "\n"
	    "Blends the source into the frame stream, using the alpha map a. "
	    "With no alpha map, the source is simply copied into the frames. "
	    "x and y specify the position that the source should be copied to. "
	    "The default is (0,0). Positions outside of the frames are possible: "
	    "parts of the source that do not fit into the "
	    "frames will be ignored. When --single is used, only the first "
	    "frame of the source will be used; this frame will be copied into "
	    "all frames of the stream.");
}


int cmd_blend(int argc, char *argv[])
{
    mh_option_file_t source = { NULL, "r", false };
    mh_option_file_t alpha = { NULL, "r", false };
    mh_option_bool_t single = { false, true };
    mh_option_int_t x = { 0, INT_MIN, INT_MAX };
    mh_option_int_t y = { 0, INT_MIN, INT_MAX };
    mh_option_t options[] =
    {
	{ "source", 's', MH_OPTION_FILE, &source, true },
	{ "alpha",  'a', MH_OPTION_FILE, &alpha,  false },
	{ "single", 'S', MH_OPTION_BOOL, &single, false },
	{ "x",      'x', MH_OPTION_INT,  &x,      false },
	{ "y",      'y', MH_OPTION_INT,  &y,      false },
	mh_option_null
    };
    cvl_frame_t *frame;
    cvl_frame_t *source_frame;
    cvl_frame_t *source_frame_bak;
    cvl_frame_t *alpha_frame;
    cvl_stream_type_t stream_type;
    bool error;
    bool eof;

    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    error = false;
    eof = false;
    alpha_frame = NULL;
    source_frame = NULL;
    source_frame_bak = NULL;
    while (!cvl_error())
    {
	/* Read the necessary frames */
	cvl_read(stdin, &stream_type, &frame);
	error = cvl_error();
	eof = !error && !frame;
	if (!error && !eof && (!single.value || !source_frame))
	{
	    cvl_read(source.value, NULL, &source_frame);
	    error = cvl_error();
	    eof = !error && !source_frame;
	    if (error || eof)
	    {
		cvl_frame_free(frame);
	    }
	}
	if (!error && !eof && alpha.value && (!single.value || !alpha_frame))
	{
	    cvl_read(alpha.value, NULL, &alpha_frame);
	    error = cvl_error();
	    eof = !error && !alpha_frame;
	    if (error || eof)
	    {
		cvl_frame_free(frame);
		cvl_frame_free(source_frame);
	    }
	    cvl_convert_format_inplace(alpha_frame, CVL_LUM);
	    if (cvl_frame_width(alpha_frame) != cvl_frame_width(source_frame)
		    || cvl_frame_height(alpha_frame) != cvl_frame_height(source_frame))
	    {
		mh_msg_err("Alpha and source frames must have the same dimensions");
		cvl_frame_free(frame);
		cvl_frame_free(source_frame);
		error = true;
	    }
	}
	if (error || eof)
	    break;

	/* Convert the format if necessary. If only a single block frame is
	 * used, convert from the original, backed up copy of this, because
	 * multiple conversions are not reversible: RGB->LUM->RGB won't work. */
	if (cvl_frame_format(source_frame) != cvl_frame_format(frame))
	{
	    if (single.value)
	    {
		if (!source_frame_bak)
		{
		    source_frame_bak = cvl_frame_new_tpl(source_frame);
		    cvl_copy(source_frame_bak, source_frame);
		}
		else
		{
		    cvl_frame_free(source_frame);
		    source_frame = cvl_frame_new_tpl(source_frame_bak);
		    cvl_copy(source_frame, source_frame_bak);
		}
	    }
	    cvl_convert_format_inplace(source_frame, cvl_frame_format(frame));
	}

	/* Cut the block if it does not fit entirely into the frame, or ignore
	 * the block if it is completely outside of the frame, or blend/copy it
	 * as-is into the frame. */
	if (x.value < 0 || x.value + cvl_frame_width(source_frame) > cvl_frame_width(frame)
		|| y.value < 0 || y.value + cvl_frame_height(source_frame) > cvl_frame_height(frame))
	{
	    int x_offset = (x.value < 0) ? - x.value : 0;
	    int y_offset = (y.value < 0) ? - y.value : 0;
	    int new_width = mh_mini(cvl_frame_width(frame) - x.value, 
		    cvl_frame_width(source_frame) - x_offset);
	    int new_height = mh_mini(cvl_frame_height(frame) - y.value, 
		    cvl_frame_height(source_frame) - y_offset);
	    
	    if (new_width > 0 && new_height > 0)
	    {
		cvl_frame_t *source_tmp = cvl_frame_new(new_width, new_height, 
			cvl_frame_channels(source_frame), cvl_frame_format(source_frame), 
			cvl_frame_type(source_frame), CVL_TEXTURE);
		cvl_cut_rect(source_tmp, source_frame, x_offset, y_offset);
		if (alpha.value)
		{
		    cvl_frame_t *alpha_tmp = cvl_frame_new(new_width, new_height,
			    cvl_frame_channels(alpha_frame), cvl_frame_format(alpha_frame),
			    cvl_frame_type(alpha_frame), CVL_TEXTURE);
		    cvl_cut_rect(alpha_tmp, alpha_frame, x_offset, y_offset);
		    cvl_blend(frame, mh_maxi(x.value, 0), mh_maxi(y.value, 0),
			    source_tmp, alpha_tmp);
		    cvl_frame_free(alpha_tmp);
		}
		else
		{
		    cvl_copy_rect(frame, mh_maxi(x.value, 0), mh_maxi(y.value, 0), 
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
		cvl_copy_rect(frame, x.value, y.value, source_frame, 0, 0, 
			cvl_frame_width(source_frame), cvl_frame_height(source_frame));
	    }
	}
	if (!single.value)
	{
	    cvl_frame_free(source_frame);
	    if (alpha.value)
		cvl_frame_free(alpha_frame);
	}
	/* Output */
	cvl_write(stdout, stream_type, frame);
	cvl_frame_free(frame);
    }
    
    /* Cleanup */
    cvl_frame_free(source_frame_bak);
    fclose(source.value);
    if (alpha.value)
	fclose(alpha.value);
    
    return (error || cvl_error()) ? 1 : 0;
}
