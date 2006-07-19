/*
 * cmd_mat.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2006  Martin Lambers <marlam@marlam.de>
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
#include <string.h>
#include <limits.h>

#include <cvl/cvl.h>

#include "xalloc.h"


void cmd_mat_print_help(void)
{
    cvl_msg_fmt_req(
	    "mat [-3|--3d]\n"
	    "\n"
	    "Computes the Medial Axis Transform (MAT) of the input, with the following properties:\n"
	    "- The MAT is not necessarily thin (1 pixel wide)\n"
	    "- The MAT is not necessarily connected\n"
	    "- The original shape can be recovered from the MAT without errors\n"
	    "The input must be the SEDT integer fields produced by the sedt command. "
	    "The --3d option is analogue to the same option of the sedt command.\n"
	    "The output will be in form of integer fields that have the same dimensions "
	    "as the input. Pixels that do not belong to the MAT will have the value 0. "
	    "The value of MAT pixels will be the squared euclidean distance "
	    "to the next background pixel.");
}

int cmd_mat(int argc, char *argv[])
{
    cvl_option_bool_t three_dimensional = { false, true };
    cvl_option_t options[] = 
    { 
	{ "3d", '3', CVL_OPTION_BOOL, &three_dimensional, false },
	cvl_option_null 
    };
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    error = false;
    if (three_dimensional.value)
    {
	cvl_field_t **sedts = NULL;
	cvl_field_t **mat;
	int depth = 0;

	while (!cvl_io_eof(stdin) && depth < INT_MAX)
	{
	    sedts = xnrealloc(sedts, ++depth, sizeof(cvl_field_t *));
	    if (!cvl_field_read_knowntype(stdin, &sedts[depth - 1], sizeof(int)))
	    {
		error = true;
		break;
	    }
	    if (depth > 1)
	    {
		if (cvl_field_width(sedts[depth - 1]) != cvl_field_width(sedts[0])
			|| cvl_field_height(sedts[depth - 1]) != cvl_field_height(sedts[0]))
		{
		    cvl_msg_err("all frames must have the same dimensions");
		    error = true;
		    break;
		}
	    }
	}
	if (!error && depth > 0)
	{
	    mat = cvl_mat3d(sedts, depth);
	    for (int i = 0; i < depth; i++)
	    {
		cvl_field_free(sedts[i]);
	    }
	    free(sedts);
	    for (int i = 0; i < depth; i++)
	    {
		if (!error && !cvl_field_write(stdout, mat[i]))
		{
		    error = true;
		}
		cvl_field_free(mat[i]);
	    }
	    free(mat);
	}
    }
    else
    {
	cvl_field_t *sedt;
	cvl_field_t *mat;

	while (!cvl_io_eof(stdin))
	{
	    if (!cvl_field_read_knowntype(stdin, &sedt, sizeof(int)))
	    {
		error = true;
		break;
	    }
	    mat = cvl_mat(sedt);
	    cvl_field_free(sedt);
	    if (!cvl_field_write(stdout, mat))
	    {
		cvl_field_free(mat);
		error = true;
		break;
	    }
	    cvl_field_free(mat);
	}
    }

    return error ? 1 : 0;
}
