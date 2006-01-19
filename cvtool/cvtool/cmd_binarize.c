/*
 * cmd_binarize.c
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

#include "options.h"


void cmd_binarize_print_help(void)    
{
    cvl_msg_fmt_req(
	    "binarize global -t|--threshold=<t>\n"
	    "binarize iterative\n"
	    "binarize otsu\n"
	    "binarize hysterese -l|--low=<tl> -h|--high=<th>\n"
	    "binarize local -T|--type=mean|median|minmax -k|--k=<k> -C|--constant=<C>\n"
	    "\n"
	    "Convert input to grayscale (if necessary), then binarize it using the given method.\n"
	    "The global method turns every pixel to black whose value is lower than "
	    "the given threshold; all over pixels are turned to white. The threshold must be "
	    "from [0, 256].\n"
	    "The iterative and otsu methods are global methods, too, but they "
	    "compute the threshold automatically.\n"
	    "The hysterese method is not adequate for general images; it is mainly used by the "
	    "Canny edge detector. It uses a low and a high threshold; both must be in [0,255], "
	    "and should be chosen so that 2tl <= th <= 4tl.\n"
	    "The local method uses a local threshold (T-C) for a neighborhood of size "
	    "(2k+1)x(2k+1), where T is the mean of all pixel values in the neighborhood, the median "
	    "of the pixel values, or the mean of the minimum and maximum of the pixel values. "
	    "C can be negative. The mean type is much faster than median and minmax.");
}


int cmd_binarize(int argc, char *argv[])
{
    typedef enum { GLOBAL, ITERATIVE, OTSU, HYSTERESE, LOCAL } subcommand_t;
    subcommand_t subcommand;
    option_int_t global_threshold = { -1, 0, 256 };
    option_t global_options[] = 
    {
	{ "threshold", 't', OPTION_INT, &global_threshold, true },
	null_option 
    };
    option_t iterative_options[] = { null_option };
    option_t otsu_options[] = { null_option };
    option_int_t hysterese_low = { -1, 0, 255 };
    option_int_t hysterese_high = { -1, 0, 255 };
    option_t hysterese_options[] = 
    {
	{ "low",  'l', OPTION_INT, &hysterese_low,  true },
	{ "high", 'h', OPTION_INT, &hysterese_high, true },
	null_option 
    };
    const char *local_type_names[] = { "mean", "median", "minmax", NULL };
    option_name_t local_type = { -1, local_type_names };
    option_int_t local_k = { -1, 1, MASKSIZE_K_MAX };
    option_int_t local_C = { INT_MIN, INT_MIN + 1, INT_MAX };
    option_t local_options[] = 
    {
	{ "type",     'T', OPTION_NAME, &local_type, true },
	{ "k",        'k', OPTION_INT,  &local_k,    true },
	{ "constant", 'C', OPTION_INT,  &local_C,    true },
	null_option 
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    cvl_frame_t *frame;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!argv[1]) 
    {
	cvl_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "global") == 0)
    {
	subcommand = GLOBAL;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvtool_getopt(argc - 1, &(argv[1]), global_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "iterative") == 0)
    {
	subcommand = ITERATIVE;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvtool_getopt(argc - 1, &(argv[1]), iterative_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "otsu") == 0)
    {
	subcommand = OTSU;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvtool_getopt(argc - 1, &(argv[1]), otsu_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "hysterese") == 0)
    {
	subcommand = HYSTERESE;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvtool_getopt(argc - 1, &(argv[1]), hysterese_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "local") == 0)
    {
	subcommand = LOCAL;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvtool_getopt(argc - 1, &(argv[1]), local_options, 0, 0, NULL);
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
	if (!cvl_io_read(stdin, input_info, &frame))
	{
	    error = true;
	    break;
	}
	if (subcommand == GLOBAL)
	{
	    cvl_frame_binarize_global_threshold(frame, global_threshold.value);
	}
	else if (subcommand == ITERATIVE)
	{
	    cvl_frame_binarize_global_threshold(frame, cvl_frame_binarize_get_threshold_iterative(frame));
	}
	else if (subcommand == OTSU)
	{
	    cvl_frame_binarize_global_threshold(frame, cvl_frame_binarize_get_threshold_otsu(frame));
	}
	else if (subcommand == HYSTERESE)
	{
	    cvl_frame_binarize_hysterese(frame, hysterese_low.value, hysterese_high.value);
	}
	else // (subcommand == LOCAL)
	{
	    cvl_frame_binarize_local_threshold(frame, local_type.value, local_k.value, local_C.value);
	}
	error = !cvl_io_write(stdout, output_info, frame);
	cvl_frame_free(frame);
	if (error)
	{
	    break;
	}
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
