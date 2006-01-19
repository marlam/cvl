/*
 * cmd_reverse.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
extern int errno;

#include "xalloc.h"

#include "tempfile.h"

#include <cvl/cvl.h>

#include "options.h"


void cmd_reverse_print_help(void)
{
    cvl_msg_fmt_req(
	    "reverse\n"
	    "\n"
	    "Reverses the order of the frames in the video stream.");
}



int cmd_reverse(int argc, char *argv[])
{
    option_t options[] = { null_option };
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    cvl_io_info_t *tmp_input_info;
    cvl_io_info_t *tmp_output_info;
    FILE *tmpf;
    char *tmpf_name;
    cvl_frame_t *frame;
    fpos_t *positions;
    const int positions_growby = 1000;
    int positions_size;
    int positions_index;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvtool_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    positions = xmalloc(positions_growby * sizeof(fpos_t));
    positions_size = positions_growby;
    positions_index = 0;
    
    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);
    
    error = false;
    if (!(tmpf = tempfile(PROGRAM_NAME "-reverse-", &tmpf_name)))
    {
	cvl_msg_err("cannot create temporary file: %s", strerror(errno));
	error = true;
    }

    while (!error && !cvl_io_eof(stdin, input_info))
    {
	/* Read one frame */
	if (!cvl_io_read(stdin, input_info, &frame))
    	{
	    error = true;
	    break;
	}
	/* Store the position for the frame */
	if (positions_index == positions_size)
	{
	    positions = xrealloc(positions, (positions_size + positions_growby) * sizeof(fpos_t));
	    positions_size += positions_growby;
	}
	if (fgetpos(tmpf, &(positions[positions_index])) != 0)
	{
	    cvl_msg_err("cannot get file position indicator for %s: %s", tmpf_name, strerror(errno));
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	positions_index++;
	/* Write the frame to the temporary file */
	tmp_output_info = cvl_io_info_new();
	cvl_io_info_link_output_to_input(tmp_output_info, input_info);
	if (!cvl_io_write(tmpf, tmp_output_info, frame))
    	{
	    cvl_io_info_free(tmp_output_info);
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	cvl_io_info_free(tmp_output_info);
	cvl_frame_free(frame);
    }

    if (!error)
    {
	if (fflush(tmpf) != 0)
	{
	    cvl_msg_err("cannot flush %s: %s", tmpf_name, strerror(errno));
	    error = true;
	}
    }

    if (!error)
    {
    	for (int i = positions_index - 1; i >= 0; i--)
	{
	    /* Set the file position */
	    if (fsetpos(tmpf, &(positions[i])) != 0)
	    {
		cvl_msg_err("cannot set file position indicator for %s: %s", tmpf_name, strerror(errno));
		error = true;
		break;
	    }
	    /* Read one frame */
	    tmp_input_info = cvl_io_info_new();
	    if (!cvl_io_read(tmpf, tmp_input_info, &frame))
	    {
		cvl_io_info_free(tmp_input_info);
		error = true;
		break;
	    }
	    cvl_io_info_free(tmp_input_info);
	    /* Write the frame to stdout */
	    if (!cvl_io_write(stdout, output_info, frame))
	    {
		cvl_frame_free(frame);
		error = true;
		break;
	    }
	    cvl_frame_free(frame);	    
	}
    }
    
    fclose(tmpf);
    remove(tmpf_name);
    free(tmpf_name);
    free(positions);
    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
