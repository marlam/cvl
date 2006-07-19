/*
 * cmd_trackdepth.c
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
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include <cvl/cvl.h>

#include "xalloc.h"

#include "tempfile.h"


void cmd_trackdepth_print_help(void)
{
    cvl_msg_fmt_req(
	    "trackdepth -n|--n=<n> -d|--depthmap-list=<d0>,<d1>,... "
	    "-f|--flow-forward=<flow-fw> -F|--flow-backward=<flow-bw> <depthfile0> <depthfile1> ...\n"
	    "\n"
	    "Creates n depth maps by using depth tracking with the given flow information on the "
	    "given list of depth maps. The depth map list contains the numbers of the frames for which "
	    "a depth map is available. It must be in ascending order. It is not necessary "
	    "to give a depth map for the first and last frame (0 and n-1), but it may improve the results. "
	    "Exactly one depth map file must be given for each entry in the list.");
}


int cmd_trackdepth(int argc, char *argv[])
{
    cvl_option_int_t N = { -1, 1, INT_MAX };
    cvl_option_int_array_t depthmaplist = { NULL, 0, NULL, 1, NULL };
    cvl_option_file_t flow_fw_file = { NULL, "r", false };
    cvl_option_file_t flow_bw_file = { NULL, "r", false };
    cvl_option_t options[] = 
    {
	{ "n",             'n', CVL_OPTION_INT,       &N,            true },
	{ "depthmap-list", 'd', CVL_OPTION_INT_ARRAY, &depthmaplist, true },
	{ "flow-forward",  'f', CVL_OPTION_FILE,      &flow_fw_file, true },
	{ "flow-backward", 'F', CVL_OPTION_FILE,      &flow_bw_file, true },
      	cvl_option_null
    };
    int first_argument;
    /* Forward and backward flow fields */
    cvl_field_t *flow_fw = NULL;
    cvl_field_t *flow_bw = NULL;
    /* Temporary file to reverse the order of the forward flow fields */
    FILE *tmp_flow_file = NULL;
    char *tmp_flow_filename = NULL;
    /* The depth data for the current A (=first) and O (=last) frame */
    FILE *depth_A_file = NULL;
    FILE *depth_O_file = NULL;
    cvl_frame_t *depth_A = NULL;
    cvl_frame_t *depth_O = NULL;
    /* The output depthmap */
    cvl_frame_t *depthmap = NULL;
    /* I/O info */
    cvl_io_info_t *first_depth_info = NULL;    
    cvl_io_info_t *depth_info = NULL;    
    cvl_io_info_t *output_info = cvl_io_info_new();
    /* dA and dO files and fields for depth tracking */
    FILE *dA_file = NULL;
    char *dA_filename = NULL;
    cvl_field_t *dA_field = NULL;
    cvl_field_t *dA_prev_field = NULL;
    FILE *dO_file = NULL;
    char *dO_filename = NULL;
    cvl_field_t *dO_field = NULL;
    cvl_field_t *dO_prev_field = NULL;
    /* Common frame width and height */
    int width = -1;
    int height = -1;
    /* Error */
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);
    error = !cvl_getopt(argc, argv, options, 1, -1, &first_argument);
    if (!error)
    {
	for (int i = 0; i < depthmaplist.value_sizes[0]; i++)
	{
	    if (depthmaplist.value[i] < 0)
	    {
		cvl_msg_err("negative number %d in depthmap list", depthmaplist.value[i]);
		error = true;
		break;
	    }
	    if (i > 0 && depthmaplist.value[i] <= depthmaplist.value[i - 1])
	    {
		cvl_msg_err("depthmap list must be in ascending order and must not contain duplicates");
		error = true;
		break;
	    }
	    if (depthmaplist.value[i] >= N.value)
	    {
		cvl_msg_err("number %d in depthmap list is greater than or equal to n=%d", 
			depthmaplist.value[i], N.value);
		error = true;
		break;
	    }
	}
	if (argc - first_argument != depthmaplist.value_sizes[0])
	{
	    cvl_msg_err("exactly one depthmap file for each entry in the depthmap list is required");
    	    error = true;
	}
    }
    if (error)
    {
	return 1;
    }
    

    int last_part_A_index;
    int last_part_O_index;
    int current_part_A_index = -1;
    int current_part_O_index = -1;
    int current_part_N;	
    /* This loop splits the task into multiple parts. In each part, a depth map
     * for the first and last frame of the part is known, so that cvl_trackmap()
     * can be called. The first and last part are allowed to lack a first/last
     * depth map as a special exception. If the first depth map is 0, then the
     * first part nevertheless starts with an unknown first depth map and ends
     * with the depth map 0.
     * We must make sure that output_info is hooked only to the io info of the
     * first depth map. */    
    for (int next_known_depthmap_index = 0; 
	    next_known_depthmap_index <= depthmaplist.value_sizes[0]; 
	    next_known_depthmap_index++)
    {
	last_part_A_index = current_part_A_index;
	last_part_O_index = current_part_O_index;
	
	/* 
	 * Determine the first and last depth map for the current part 
	 * -1 means "not available"
	 */
	
	current_part_A_index = last_part_O_index;
	if (next_known_depthmap_index < depthmaplist.value_sizes[0])
	{
	    current_part_O_index = next_known_depthmap_index;
	}
	else
	{
	    current_part_O_index = -1;
	}
	if (current_part_A_index == -1)
	{
	    current_part_N = depthmaplist.value[current_part_O_index] + 1;
	}
	else if (current_part_O_index == -1)
	{
	    current_part_N = N.value - depthmaplist.value[current_part_A_index];
	}
	else
	{
	    current_part_N = depthmaplist.value[current_part_O_index] 
		- depthmaplist.value[current_part_A_index] + 1;
	}
	if (current_part_O_index == -1 && current_part_N == 1)
	{
	    break;
	}
	cvl_msg_dbg("current part: A = %d, O = %d, N = %d", 
		current_part_A_index == -1 ? -1 : depthmaplist.value[current_part_A_index],
		current_part_O_index == -1 ? -1 : depthmaplist.value[current_part_O_index],
		current_part_N);

	/* 
	 * Read depth maps 
	 */

	if (current_part_A_index != -1)
	{
	    cvl_frame_free(depth_A);
	    depth_A = NULL;
	    if (current_part_A_index == last_part_O_index)
	    {
		cvl_msg_dbg("reusing old depth map O as new depth map A...");
		depth_A = depth_O;
		depth_O = NULL;
	    }
	    else
	    {
		depth_info = cvl_io_info_new();
		cvl_msg_dbg("reading current part depth map A: %s...", 
		     	argv[first_argument + current_part_A_index]);
		if (!(depth_A_file = fopen(argv[first_argument + current_part_A_index], "r"))
			|| cvl_io_eof(depth_A_file)
			|| !cvl_io_read(depth_A_file, depth_info, &depth_A))
		{
		    cvl_msg_err("cannot read depthmap %s", argv[first_argument + current_part_A_index]);
		    error = true;
		    goto exit;
		}
		fclose(depth_A_file);
		depth_A_file = NULL;
		cvl_io_info_free(depth_info);
		depth_info = NULL;
		if (width == -1)
		{
		    width = cvl_frame_width(depth_A);
		    height = cvl_frame_height(depth_A);
		}
		else
		{
		    if (cvl_frame_width(depth_A) != width || cvl_frame_height(depth_A) != height)
		    {
			cvl_msg_err("the depth maps must have the same size");
			error = true;
			goto exit;
		    }			
		}
		cvl_frame_to_gray(depth_A);
	    }
	}
	if (current_part_O_index != -1)
	{
	    cvl_frame_free(depth_O);
	    depth_O = NULL;
	    cvl_msg_dbg("reading current part depth map O: %s...", 
		    argv[first_argument + current_part_O_index]);
	    // the first depthmap that we read is always used as depth_O
	    if (!first_depth_info)
	    {
		first_depth_info = cvl_io_info_new();
		cvl_io_info_link_output_to_input(output_info, first_depth_info);
		depth_info = first_depth_info;
	    }
	    else
	    {
		depth_info = cvl_io_info_new();
	    }
	    if (!(depth_O_file = fopen(argv[first_argument + current_part_O_index], "r"))
		    || cvl_io_eof(depth_O_file)
		    || !cvl_io_read(depth_O_file, depth_info, &depth_O))
	    {
		cvl_msg_err("cannot read depthmap %s", argv[first_argument + current_part_O_index]);
		error = true;
		goto exit;
	    }
	    fclose(depth_O_file);
	    depth_O_file = NULL;
	    if (depth_info != first_depth_info)
	    {
		cvl_io_info_free(depth_info);
		depth_info = NULL;
	    }
	    if (width == -1)
	    {
		width = cvl_frame_width(depth_O);
		height = cvl_frame_height(depth_O);
	    }
	    else
	    {
		if (cvl_frame_width(depth_O) != width || cvl_frame_height(depth_O) != height)
		{
		    cvl_msg_err("the depth maps must have the same size");
		    error = true;
		    goto exit;
		}			
	    }
	    cvl_frame_to_gray(depth_O);
	}

	/* 
	 * Compute the depth tracking depth maps, and output them 
	 */
	
	if (current_part_N > 1 && 
		!(current_part_N == 2 && current_part_A_index != -1 && current_part_O_index != -1))
	{
	    /* 
	     * 1. Create the depth-O file. We need the forward flow in reversed
	     *    order, so we have to use a temporary file. 
	     */
	    cvl_msg_dbg("reversing forward flow field stream...");
	    if (!(tmp_flow_file = tempfile(PROGRAM_NAME "-trackdepth-ffw-", &tmp_flow_filename)))
	    {
		cvl_msg_err("cannot create temporary file: %s", strerror(errno));
		error = true;
		goto exit;
	    }
	    if (setvbuf(tmp_flow_file, NULL, _IONBF, 0) != 0)
	    {
		cvl_msg_err("cannot disable buffering for temporary file: %s", strerror(errno));
		error = true;
		goto exit;
	    }
	    for (int i = 0; i < current_part_N - 1; i++)
	    {
		/* read forward flow field */
		flow_fw = cvl_field_new(2 * sizeof(int), width, height);
	    	if (cvl_io_eof(flow_fw_file.value) 
    			|| !cvl_field_read_known(flow_fw_file.value, flow_fw))
		{
		    cvl_msg_err("cannot read forward flow");
		    error = true;
		    goto exit;
		}
		/* store forward flow field in temporary file */
		cvl_msg_dbg("writing forward flow %d to temporary file", i);
		if (!cvl_field_write(tmp_flow_file, flow_fw))
		{
		    error = true;
		    goto exit;
		}
		cvl_field_free(flow_fw);
		flow_fw = NULL;
	    }
	    /* Write the depth-O file */
	    cvl_msg_dbg("creating depth-O file...");
	    if (!(dO_file = tempfile(PROGRAM_NAME "-trackdepth-dO-", &dO_filename)))
	    {
		cvl_msg_err("cannot create temporary file: %s", strerror(errno));
		error = true;
		goto exit;
	    }
	    if (setvbuf(dO_file, NULL, _IONBF, 0) != 0)
	    {
		cvl_msg_err("cannot disable buffering for temporary file: %s", strerror(errno));
		error = true;
		goto exit;
	    }
	    if (!dO_field)
	    {
		dO_field = cvl_field_new(sizeof(int16_t), width, height);
	    }
	    cvl_trackdepth_init_first_trackmap(dO_field, (current_part_O_index == -1) ? NULL : depth_O);
	    cvl_msg_dbg("writing depth-O map %d", current_part_N - 1);
	    if (!cvl_field_seek(dO_file, sizeof(int16_t), width, height, current_part_N - 1)
		    || !cvl_field_write(dO_file, dO_field))
	    {
	     	error = true;
    		goto exit;
	    }
	    if (!dO_prev_field)
	    {
		dO_prev_field = cvl_field_new(sizeof(int16_t), width, height);
	    }
	    for (int i = current_part_N - 2; i >= 0; i--)
	    {
		cvl_field_copy(dO_prev_field, dO_field);
		/* read forward flow field */
		if (!cvl_field_seek(tmp_flow_file, 2 * sizeof(int), width, height, i))
		{
		    error = true;
		    goto exit;
		}
		flow_fw = cvl_field_new(2 * sizeof(int), width, height);
		if (!cvl_field_read_known(tmp_flow_file, flow_fw))
		{
		    error = true;
		    goto exit;
		}
		cvl_trackdepth_init_next_trackmap(dO_field, dO_prev_field, flow_fw);
		cvl_field_free(flow_fw);
		flow_fw = NULL;
		cvl_msg_dbg("writing depth-O map %d", i);
	     	if (!cvl_field_seek(dO_file, sizeof(int16_t), width, height, i)
			|| !cvl_field_write(dO_file, dO_field))
		{
		    error = true;
		    goto exit;
		}		
	    }
	    if (!cvl_field_seek(dO_file, sizeof(int16_t), width, height, 0))
    	    {
		error = true;
		goto exit;
	    }
	    fclose(tmp_flow_file);
	    tmp_flow_file = NULL;
	    remove(tmp_flow_filename);
	    free(tmp_flow_filename);
	    tmp_flow_filename = NULL;
	    
	    /* 
	     * 2. Create the depth-A file.
	     */
	    cvl_msg_dbg("creating depth-A file...");
	    if (!(dA_file = tempfile(PROGRAM_NAME "-trackdepth-dA-", &dA_filename)))
	    {
		cvl_msg_err("cannot create temporary file: %s", strerror(errno));
		error = true;
		goto exit;
	    }
	    if (setvbuf(dA_file, NULL, _IONBF, 0) != 0)
	    {
		cvl_msg_err("cannot disable buffering for temporary file: %s", strerror(errno));
		error = true;
		goto exit;
	    }
	    if (!dA_field)
	    {
		dA_field = cvl_field_new(sizeof(int16_t), width, height);
	    }
	    cvl_trackdepth_init_first_trackmap(dA_field, (current_part_A_index == -1) ? NULL : depth_A);
	    cvl_msg_dbg("writing depth-A map %d", 0);
	    if (!cvl_field_write(dA_file, dA_field))
	    {
		error = true;
		goto exit;
	    }
	    if (!dA_prev_field)
	    {
		dA_prev_field = cvl_field_new(sizeof(int16_t), width, height);
	    }
	    for (int i = 1; i < current_part_N; i++)
	    {
		cvl_field_copy(dA_prev_field, dA_field);
		flow_bw = cvl_field_new(2 * sizeof(int), width, height);
		/* read backward flow field */
	    	if (cvl_io_eof(flow_bw_file.value) 
    			|| !cvl_field_read_known(flow_bw_file.value, flow_bw))
		{
		    error = true;
		    goto exit;
		}
		cvl_trackdepth_init_next_trackmap(dA_field, dA_prev_field, flow_bw);
		cvl_field_free(flow_bw);
		flow_bw = NULL;
		cvl_msg_dbg("writing depth-A map %d", i);
		if (!cvl_field_write(dA_file, dA_field))
		{
		    error = true;
		    goto exit;
		}
	    }
	    if (!cvl_field_seek(dA_file, sizeof(int16_t), width, height, 0))
    	    {
		error = true;
		goto exit;
	    }

	    /*
	     * 3. Compute and write the output depth maps
	     */
	    cvl_msg_dbg("computing depth maps...");
	    for (int i = 0; i < current_part_N; i++)
	    {
		if (!cvl_field_read_known(dA_file, dA_field) || !cvl_field_read_known(dO_file, dO_field))
		{
		    error = true;
		    goto exit;
		}
		if (i == 0 && current_part_A_index != -1)
		{
		    /* this depthmap was already written in the last step */
		    continue;
		}
		depthmap = cvl_trackdepth(current_part_N, i, dA_field, dO_field);
    		error = !cvl_io_write(stdout, output_info, depthmap);
		cvl_frame_free(depthmap);
		if (error)
		{
	    	    goto exit;
	       	}
	    }
    	    fclose(dA_file);
	    dA_file = NULL;
	    remove(dA_filename);
	    free(dA_filename);
	    dA_filename = NULL;
    	    fclose(dO_file);
	    dO_file = NULL;
	    remove(dO_filename);
	    free(dO_filename);
	    dO_filename = NULL;
	}
	else
	{
	    /* Output of depthmap O */
	    error = !cvl_io_write(stdout, output_info, depth_O);
	    if (error)
	    {
		goto exit;
	    }
	}
    }


exit:

    /*
     * Clean up 
     */

    cvl_msg_dbg("cleaning up");
    if (dA_file)
    {
	fclose(dA_file);
    }
    if (dA_filename)
    {
	remove(dA_filename);
	free(dA_filename);
    }
    cvl_field_free(dA_field);
    cvl_field_free(dA_prev_field);
    if (dO_file)
    {
	fclose(dO_file);
    }
    if (dO_filename)
    {
	remove(dO_filename);
	free(dO_filename);
    }
    free(dO_field);
    free(dO_prev_field);
    if (tmp_flow_file)
    {
	fclose(tmp_flow_file);
    }
    if (tmp_flow_filename)
    {
	remove(tmp_flow_filename);
	free(tmp_flow_filename);
    }
    cvl_field_free(flow_fw);
    cvl_field_free(flow_bw);
    cvl_frame_free(depth_A);
    cvl_frame_free(depth_O);
    cvl_io_info_free(first_depth_info);
    if (depth_info != first_depth_info)
    {
	cvl_io_info_free(depth_info);
    }
    cvl_io_info_free(output_info);
    fclose(flow_fw_file.value);
    fclose(flow_bw_file.value);
    if (depth_A_file)
    {
	fclose(depth_A_file);
    }
    if (depth_O_file)
    {
	fclose(depth_O_file);
    }
    free(depthmaplist.value);
    free(depthmaplist.value_sizes);

    return error ? 1 : 0;
}
