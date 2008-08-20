/*
 * cmd_wavelets.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_wavelets_print_help(void)
{
    mh_msg_fmt_req(
	    "wavelets -t|--task=dwt -D|--daubechies=<D> -l|--level=<l>\n"
	    "wavelets -t|--task=idwt -D|--daubechies=<D> -l|--level=<l>\n"
	    "wavelets -t|--task=hard-thresholding -l|--level=<l> -T|--threshold=<t>\n"
	    "wavelets -t|--task=soft-thresholding -l|--level=<l> -T|--threshold=<t>\n"
	    "\n"
	    "Perform Discrete Wavelet Transform (DWT), Inverse Discrete Wavelet transform (IDWT), or manipulations on "
	    "transformed data.\n"
	    "The parameter D chooses the Daubechies wavelet (D2, ..., D20; only even numbers). The level l must be at least 1. "
	    "The threshold parameter for hard and soft thresholding is applied to all input channels. "
	    "The output of this command is always of type float; it has to be manually converted if necessary.");
}


int cmd_wavelets(int argc, char *argv[])
{
    typedef enum 
    { 
	TASK_DWT, TASK_IDWT, TASK_HT, TASK_ST
    } task_t;
    const char *task_names[] = 
    { 
	"dwt", "idwt", "hard-thresholding", "soft-thresholding", NULL
    };
    mh_option_name_t task = { -1, task_names };
    mh_option_int_t D = { -1, 2, 20 };
    mh_option_int_t level = { -1, 1, INT_MAX };
    mh_option_float_t threshold = { -FLT_MAX, -FLT_MAX, false, FLT_MAX, true };
    mh_option_t options[] = 
    {
	{ "task",       't', MH_OPTION_NAME,  &task,      true  },
	{ "daubechies", 'D', MH_OPTION_INT,   &D,         false },
	{ "level",      'l', MH_OPTION_INT,   &level,     true  },
	{ "threshold",  'T', MH_OPTION_FLOAT, &threshold, false },
	mh_option_null
    };
    cvl_frame_t *inframe, *outframe, *tmpframe;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }

    if (task.value != TASK_HT && task.value != TASK_ST && D.value < 0)
    {
	mh_msg_err("Task %s requires parameter 'daubechies'", task_names[task.value]);
	return 1;
    }
    if ((task.value == TASK_HT || task.value == TASK_ST) && D.value > 0)
    {
	mh_msg_err("Invalid parameter 'daubechies' for task %s", task_names[task.value]);
	return 1;
    }
    if (task.value != TASK_HT && task.value != TASK_ST && threshold.value > -FLT_MAX)
    {
	mh_msg_err("Invalid parameter 'threshold' for task %s", task_names[task.value]);
	return 1;
    }
    if ((task.value == TASK_HT || task.value == TASK_ST) && threshold.value <= -FLT_MAX)
    {
	mh_msg_err("Task %s requires parameter 'threshold'", task_names[task.value]);
	return 1;
    }

    bool error = false;
    while (!cvl_error())
    {
	cvl_read(stdin, NULL, &inframe);
	if (!inframe)
	    break;

	outframe = cvl_frame_new(cvl_frame_width(inframe), cvl_frame_height(inframe),
		cvl_frame_channels(inframe), cvl_frame_format(inframe), CVL_FLOAT, CVL_TEXTURE);
	
	if (task.value == TASK_DWT)
	{
	    tmpframe = cvl_frame_new_tpl(outframe);
	    cvl_wavelets_dwt(outframe, inframe, tmpframe, D.value, level.value);
	    cvl_frame_free(tmpframe);
	}
	else if (task.value == TASK_IDWT)
	{
	    tmpframe = cvl_frame_new_tpl(outframe);
	    cvl_wavelets_idwt(outframe, inframe, tmpframe, D.value, level.value);
	    cvl_frame_free(tmpframe);
	}
	else if (task.value == TASK_HT)
	{
	    float T[4] = { threshold.value, threshold.value, threshold.value, threshold.value };
	    cvl_wavelets_hard_thresholding(outframe, inframe, level.value, T);
	}
	else if (task.value == TASK_ST)
	{
	    float T[4] = { threshold.value, threshold.value, threshold.value, threshold.value };
	    cvl_wavelets_soft_thresholding(outframe, inframe, level.value, T);
	}

	cvl_frame_free(inframe);
	cvl_write(stdout, CVL_PFS, outframe);
	cvl_frame_free(outframe);
    }

    return (cvl_error() || error) ? 1 : 0;
}
