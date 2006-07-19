/*
 * cmd_opticalflow.c
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
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <cvl/cvl.h>


void cmd_opticalflow_print_help(void)
{
    cvl_msg_fmt_req(
	    "opticalflow hs [-b|--backwards] -l|--lambda=<l> -n|--iterations=<n>\n"
	    "opticalflow lk [-b|--backwards] -k|--k=<k>\n"
	    "opticalflow clg [-b|--backwards] -l|--lambda=<l> -O|--omega=<O> "
	    "-n|--iterations=<n>\n"
	    "opticalflow bm-sad [-b|--backwards] -k|--k=<k> -M|--max-distance=<m> "
	    "-D|--distance-weight=<dw> -L|--luminance-weight=<lw>\n"
	    "opticalflow bm-asw [-b|--backwards] -k|--k=<k> -M|--max-distance=<m> "
	    "-c|--gamma-c=<gc> -p|--gamma-p=<gp>\n"
	    "opticalflow cc -t|--tolerance=<t> [-w|--warning-level=<w>] [-o|--output=<file>] "
	    "-f|--verification-flow=<flow>\n"
	    "\n"
	    "Compute the optical flow between frames. For n input frames, this command "
	    "will produce n-1 fields of optical flow vectors, in plain text format.\n"
	    "If -b is given, then the optical flow is computed in the opposite direction.\n"
	    "Available methods are hs (Horn/Schunck), lk (Lukas/Kanade), clg (Combined Local/Global), "
	    "bm-sad (block matching with sum of absoulte differences), "
	    "and bm-asw (block matching using adaptive support weights).\n"
	    "The cc (Consistency Check) method does not compute optical flow from images. Instead, it "
	    "offers a verification step that improves existing optical flow fields: the flow fields "
	    "from stdin are compared against those from the given file. Each flow vector in the first "
	    "field must match its corresponding flow vector in the second field with the given tolerance. "
	    "Vector pairs that differ by more than the given tolerance are marked as unreliable, and are "
	    "replaced by interpolations of neighboring reliable vectors. If a warning level between 0 and 1 "
	    "is given and the fraction of unreliable vectors is greater than or equal to this level, "
	    "then a warning is issued for the current flow field pair. These warnings can be redirected "
	    "to a separate file with the -o option.");
}


int cmd_opticalflow(int argc, char *argv[])
{
    typedef enum { HS, LK, CLG, BM_SAD, BM_ASW, CC } subcommand_t;
    subcommand_t subcommand;
    cvl_option_bool_t backwards = { false, true };
    cvl_option_double_t lambda = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_double_t omega = { -1.0, 0.0, false, 2.0, false };
    cvl_option_int_t iterations = { -1, 1, INT_MAX };
    cvl_option_int_t k = { -1, 1, CVL_MASKSIZE_K_MAX };
    cvl_option_int_t max_distance = { -1, 0, CVL_MASKSIZE_K_MAX };
    cvl_option_double_t distance_weight = { -1.0, 0.0, true, 1.0, true };
    cvl_option_double_t luminance_weight = { -1.0, 0.0, true, 1.0, true };
    cvl_option_double_t gamma_c = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_double_t gamma_p = { -1.0, 0.0, false, DBL_MAX, true };
    cvl_option_int_t tolerance = { -1, INT_MIN, INT_MAX };
    cvl_option_double_t warninglevel = { -1.0, 0.0, true, 1.0, true };
    cvl_option_file_t output = { NULL, "w", false };
    cvl_option_file_t verificationflow = { NULL, "r", false };
    cvl_option_t hs_options[] = 
    {
	{ "backwards",         'b', CVL_OPTION_BOOL,   &backwards,        false },
	{ "lambda",            'l', CVL_OPTION_DOUBLE, &lambda,           true },
	{ "iterations",        'n', CVL_OPTION_INT,    &iterations,       true },
	cvl_option_null
    };
    cvl_option_t lk_options[] = 
    {
	{ "backwards",         'b', CVL_OPTION_BOOL,   &backwards,        false },
	{ "k",                 'k', CVL_OPTION_INT,    &k,                true },
	cvl_option_null
    };
    cvl_option_t clg_options[] = 
    {
	{ "backwards",         'b', CVL_OPTION_BOOL,   &backwards,        false },
	{ "lambda",            'l', CVL_OPTION_DOUBLE, &lambda,           true },
	{ "iterations",        'n', CVL_OPTION_INT,    &iterations,       true },
	{ "omega",             'O', CVL_OPTION_DOUBLE, &omega,            true },
	cvl_option_null
    };
    cvl_option_t bm_sad_options[] = 
    {
	{ "backwards",         'b', CVL_OPTION_BOOL,   &backwards,        false },
	{ "k",                 'k', CVL_OPTION_INT,    &k,                true },
	{ "max-distance",      'M', CVL_OPTION_INT,    &max_distance,     true },
	{ "distance-weight",   'D', CVL_OPTION_DOUBLE, &distance_weight,  true },
	{ "luminance-weight",  'L', CVL_OPTION_DOUBLE, &luminance_weight, true },
	cvl_option_null
    };
    cvl_option_t bm_asw_options[] = 
    {
	{ "backwards",         'b', CVL_OPTION_BOOL,   &backwards,        false },
	{ "k",                 'k', CVL_OPTION_INT,    &k,                true },
	{ "max-distance",      'M', CVL_OPTION_INT,    &max_distance,     true },
	{ "gamma-c",           'c', CVL_OPTION_DOUBLE, &gamma_c,          true },
	{ "gamma-p",           'p', CVL_OPTION_DOUBLE, &gamma_p,          true },
	cvl_option_null
    };
    cvl_option_t cc_options[] = 
    {
	{ "tolerance",         't', CVL_OPTION_INT,    &tolerance,        true },
	{ "warning-level",     'w', CVL_OPTION_DOUBLE, &warninglevel,     false },
	{ "output",            'o', CVL_OPTION_FILE,   &output,           false },
	{ "verification-flow", 'f', CVL_OPTION_FILE,   &verificationflow, true },
	cvl_option_null
    };
    cvl_field_t *flowfield;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    if (!argv[1])
    {
	cvl_msg_err("missing subcommand");
	error = true;
    }
    else if (strcmp(argv[1], "hs") == 0)
    {
	subcommand = HS;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), hs_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "lk") == 0)
    {
	subcommand = LK;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), lk_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "clg") == 0)
    {
	subcommand = CLG;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), clg_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "bm-sad") == 0)
    {
	subcommand = BM_SAD;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), bm_sad_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "bm-asw") == 0)
    {
	subcommand = BM_ASW;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), bm_asw_options, 0, 0, NULL);
    }
    else if (strcmp(argv[1], "cc") == 0)
    {
	subcommand = CC;
	cvl_msg_set_command_name("%s %s", argv[0], argv[1]);
	error = !cvl_getopt(argc - 1, &(argv[1]), cc_options, 0, 0, NULL);
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

    if (subcommand == CC)
    {
	cvl_field_t *field1, *field2;
	int fieldpaircounter;

	fieldpaircounter = 0;
	while (!cvl_io_eof(stdin) && !cvl_io_eof(verificationflow.value))
	{
	    double fraction;

	    if (!cvl_field_read_knowntype(stdin, &field1, 2 * sizeof(int)))
	    {
		error = true;
		break;
	    }
	    if (!cvl_field_read_knowntype(verificationflow.value, &field2, 2 * sizeof(int)))
	    {
		cvl_field_free(field1);
		error = true;
		break;
	    }
	    if (cvl_field_width(field1) != cvl_field_width(field2)
		    || cvl_field_height(field1) != cvl_field_height(field2))
	    {
		cvl_field_free(field1);
		cvl_field_free(field2);
		cvl_msg_err("fields must have the same dimensions");
		error = true;
		break;
	    }
	    flowfield = cvl_opticalflow_cc(field1, field2, tolerance.value, &fraction);
	    cvl_field_free(field1);
	    cvl_field_free(field2);
	    if (!cvl_field_write(stdout, flowfield))
	    {
		cvl_field_free(flowfield);
		error = true;
		break;
	    }
    	    cvl_field_free(flowfield);
	    if (warninglevel.value >= 0.0 && fraction >= warninglevel.value)
	    {
		cvl_msg(output.value ? output.value : stderr, CVL_MSG_WRN, 
			"field pair %d: fraction of unreliable vectors is %+.4f", 
			fieldpaircounter, fraction);
	    }
	    fieldpaircounter++;
	}

	if (output.value)
	{
	    fclose(output.value);
	}
	fclose(verificationflow.value);
    }
    else
    {
	cvl_io_info_t *frame_info;
	cvl_frame_t *frame1, *frame2;		// the real frames
	cvl_frame_t *frame1x, *frame2x;		// just pointers to the real frames

    	frame_info = cvl_io_info_new();
	
	if (!cvl_io_read(stdin, frame_info, &frame1))
	{
	    error = true;
	}
	while (!error && !cvl_io_eof(stdin))
	{
    	    if (!cvl_io_read(stdin, frame_info, &frame2))
    	    {
    		error = true;
    		break;
    	    }
    	    if (cvl_frame_width(frame1) != cvl_frame_width(frame2) 
    		    || cvl_frame_height(frame1) != cvl_frame_height(frame2))
    	    {
    		cvl_frame_free(frame2);
    		cvl_msg_err("the frames must have the same size");
    		error = true;
    		break;
    	    }
	    // swap the direction if necessary
	    if (backwards.value)
	    {
		frame1x = frame2;
		frame2x = frame1;
	    }
	    else
	    {
		frame1x = frame1;
		frame2x = frame2;
	    }
    	    switch (subcommand)
    	    {
    		case HS:
    		    flowfield = cvl_opticalflow_hs(frame1x, frame2x, 
			    lambda.value, iterations.value);
    		    break;

    		case LK:
    		    flowfield = cvl_opticalflow_lk(frame1x, frame2x, 
			    k.value);
    		    break;

    		case CLG:
    		    flowfield = cvl_opticalflow_clg(frame1x, frame2x, 
			    lambda.value, omega.value, iterations.value);
    		    break;

    		case BM_SAD:
    		    flowfield = cvl_opticalflow_bm_sad(frame1x, frame2x, 
			    k.value, max_distance.value,
    			    distance_weight.value, luminance_weight.value);
    		    break;

    		case BM_ASW:
    		default:
    		    flowfield = cvl_opticalflow_bm_asw(frame1x, frame2x, 
			    k.value, max_distance.value,
    			    gamma_c.value, gamma_p.value);
    		    break;
    	    }
	    cvl_frame_free(frame1);
    	    frame1 = frame2;
    	    if (!cvl_field_write(stdout, flowfield))
    	    {
    		cvl_field_free(flowfield);
    		error = true;
    		break;
    	    }
	    cvl_field_free(flowfield);
       	}

	cvl_frame_free(frame1);
	cvl_io_info_free(frame_info);
    }

    return error ? 1 : 0;
}
