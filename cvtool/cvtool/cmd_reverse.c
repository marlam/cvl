/*
 * cmd_reverse.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_reverse_print_help(void)
{
    mh_msg_fmt_req(
	    "reverse\n"
	    "\n"
	    "Reverses the order of the frames in the stream.");
}


int cmd_reverse(int argc, char *argv[])
{
    mh_option_t options[] = { mh_option_null };
    cvl_stream_type_t stream_type;
    FILE *tmpf;
    char *tmpf_name;
    cvl_frame_t *frame;
    fpos_t *positions;
    const int positions_growby = 1000;
    int positions_size;
    int positions_index;
    bool error;

    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    positions = mh_alloc(positions_growby * sizeof(fpos_t));
    positions_size = positions_growby;
    positions_index = 0;
    
    error = false;
    if (!(tmpf = mh_mktempfile(PACKAGE_NAME "-reverse-", &tmpf_name)))
    {
	mh_msg_err("Cannot create temporary file: %s", strerror(errno));
	error = true;
    }

    while (!error)
    {
	cvl_read(stdin, &stream_type, &frame);
	if (!frame)
	    break;
	/* Store the position for the frame */
	if (positions_index == positions_size)
	{
	    positions = mh_realloc(positions, mh_alloc_mul((positions_size + positions_growby), sizeof(fpos_t)));
	    positions_size += positions_growby;
	}
	if (fgetpos(tmpf, &(positions[positions_index])) != 0)
	{
	    mh_msg_err("Cannot get file position indicator for %s: %s", tmpf_name, strerror(errno));
	    cvl_frame_free(frame);
	    error = true;
	    break;
	}
	positions_index++;
	/* Write the frame to the temporary file */
	cvl_write(tmpf, stream_type, frame);
	cvl_frame_free(frame);
	error = cvl_error();
    }

    if (!error)
    {
	if (fflush(tmpf) != 0)
	{
	    mh_msg_err("Cannot flush %s: %s", tmpf_name, strerror(errno));
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
		mh_msg_err("Cannot set file position indicator for %s: %s", tmpf_name, strerror(errno));
		error = true;
		break;
	    }
	    /* Read one frame */
	    cvl_read(tmpf, &stream_type, &frame);
	    /* Write the frame to stdout */
	    cvl_write(stdout, stream_type, frame);
    	    cvl_frame_free(frame);
	    error = cvl_error();
	    if (error)
		break;
	}
    }
    
    fclose(tmpf);
    remove(tmpf_name);
    free(tmpf_name);
    free(positions);
    return error ? 1 : 0;
}
