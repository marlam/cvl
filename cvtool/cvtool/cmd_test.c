/*
 * cmd_test.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2006, 2007  Martin Lambers <marlam@marlam.de>
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

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <cvl/cvl.h>

#include "mh.h"


void cmd_test_print_help(void)
{
    mh_msg_fmt_req(
	    "test\n"
	    "\n"
	    "Does random things. Do not use it.");
}


int cmd_test(int argc, char *argv[])
{
    mh_option_t options[] = { mh_option_null };
    cvl_frame_t *frame = NULL;
    cvl_stream_type_t stream_type;
    mh_timer_t start, stop;

    mh_msg_set_command_name("%s", argv[0]);    
    if (!mh_getopt(argc, argv, options, 0, 0, NULL))
	return 1;

    cvl_read(stdin, &stream_type, &frame);

    mh_msg_wrn("STREAM=%s TYPE=%s FORMAT=%s WIDTH=%d HEIGHT=%d",
	    stream_type == CVL_PNM ? "pnm" : "pfs",
	    cvl_frame_type(frame) == CVL_UINT8 ? "uint8" : "float",
	    cvl_frame_format(frame) == CVL_LUM ? "lum" : "color",
	    cvl_frame_width(frame), cvl_frame_height(frame));

#if 0
     /* Test pyramids */
    int levels = 1;
    int d = mh_mini(cvl_frame_width(frame), cvl_frame_height(frame));
    while (d > 32)
    {
	d /= 2;
	levels++;
    }

    cvl_frame_t *gp[levels];
    cvl_frame_t *lp[levels - 1];
    cvl_pyramid_gaussian(frame, levels, gp);
    mh_msg_wrn("Created gaussian pyramid with %d levels", levels);
    cvl_pyramid_laplacian(gp, levels, lp);
    mh_msg_wrn("Created laplacian pyramid with %d levels", levels - 1);
    for (int l = 0; l < levels; l++)
    {
	char *gn = mh_asprintf("gauss%02d.pnm", l);
	cvl_save_pnm(gn, gp[l]);
	free(gn);
    }
    for (int l = 0; l < levels - 1; l++)
    {
	char *ln = mh_asprintf("laplace%02d.pnm", l);
	cvl_save_pnm(ln, lp[l]);
	free(ln);
    }
    return 0;

#endif


    /* Test sorting */

    mh_timer_set(MH_TIMER_REAL, &start);
    cvl_frame_t *sorted = cvl_frame_new_tpl(frame);
    cvl_sort(sorted, frame, -1);
    cvl_write(stdout, stream_type, sorted);

    mh_timer_set(MH_TIMER_REAL, &stop);
    mh_msg_wrn("Sorting took %.4f seconds", mh_timer_get(&start, &stop));

    float median[4];
    cvl_quantil(sorted, -1, 0.5, median);
    mh_msg_wrn("median: %.4f %.4f %.4f %.4f", median[0], median[1], median[2], median[3]);

    if (cvl_frame_type(frame) == CVL_UINT8)
    {
	uint8_t *ptr = cvl_frame_pointer(sorted);
	if (cvl_frame_format(frame) == CVL_LUM)
	{
	    for (int i = 1; i < cvl_frame_size(frame); i++)
	    {
		if (ptr[i] < ptr[i - 1])
		    mh_msg_err("Position %d: %d < %d!", i, (int)(ptr[i]), (int)(ptr[i - 1]));
	    }
	}
	else
	{
	    for (int i = 1; i < cvl_frame_size(frame); i++)
	    {
		if (ptr[3 * i + 0] < ptr[3 * (i - 1) + 0])
		    mh_msg_err("Position %d: R: %d < %d!", i, (int)(ptr[3 * i + 0]), (int)(ptr[3 * (i - 1) + 0]));
		if (ptr[3 * i + 1] < ptr[3 * (i - 1) + 1])
		    mh_msg_err("Position %d: G: %d < %d!", i, (int)(ptr[3 * i + 1]), (int)(ptr[3 * (i - 1) + 1]));
		if (ptr[3 * i + 2] < ptr[3 * (i - 1) + 2])
		    mh_msg_err("Position %d: B: %d < %d!", i, (int)(ptr[3 * i + 2]), (int)(ptr[3 * (i - 1) + 2]));
	    }
	}
    }
    else
    {
	float *ptr = cvl_frame_pointer(sorted);
	if (cvl_frame_format(frame) == CVL_LUM)
	{
	    for (int i = 1; i < cvl_frame_size(frame); i++)
	    {
		if (ptr[i] < ptr[i - 1])
		    mh_msg_err("Position %d: %+.6f < %+.6f!", i, ptr[i], ptr[i - 1]);
	    }
	}
	else
	{
	    for (int i = 1; i < cvl_frame_size(frame); i++)
	    {
		if (ptr[3 * i + 0] < ptr[3 * (i - 1) + 0])
		    mh_msg_err("Position %d: R: %+.6f < %+.6f!", i, ptr[3 * i + 0], ptr[3 * (i - 1) + 0]);
		if (ptr[3 * i + 1] < ptr[3 * (i - 1) + 1])
		    mh_msg_err("Position %d: G: %+.6f < %+.6f!", i, ptr[3 * i + 1], ptr[3 * (i - 1) + 1]);
		if (ptr[3 * i + 2] < ptr[3 * (i - 1) + 2])
		    mh_msg_err("Position %d: B: %+.6f < %+.6f!", i, ptr[3 * i + 2], ptr[3 * (i - 1) + 2]);
	    }
	}
    }

    cvl_frame_free(frame);
    cvl_frame_free(sorted);
    
    return 0;
}
