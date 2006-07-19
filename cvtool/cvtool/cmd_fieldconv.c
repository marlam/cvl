/*
 * cmd_fieldconv.c
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
#include <limits.h>
#include <float.h>

#include <cvl/cvl.h>


void cmd_fieldconv_print_help(void)
{
    cvl_msg_fmt_req(
	    "fieldconv -i|--input=frame|intfield|floatfield -o|--output=<outputtype> "
	    "[-l|--low-int=<l>] [-h|--high-int=<h>] [-L|--low-float=<L>] [-H|--high-float=<H>] "
	    "\n"
	    "Converts between fields and frames.\n"
	    "Supported input types are frames, fields with integer values, and fields with "
	    "float values.\n"
	    "Supported output types are:\n"
	    "gray:   Graylevel PNM frames.\n"
	    "rgb:    Color PNM frames.\n"
	    "int:    Fields with 1 integer per element.\n"
	    "int3:   Fields with 3 integers per element.\n"
	    "float:  Fields with 1 float per element.\n"
	    "float3: Fields with 3 floats per element.\n"
	    "If the input data is fields, then the float or integer values are clamped to "
	    "their lower and upper bounds (default is [0,1] for floats and [0,255] for integers) "
	    "and then transformed linearly to their destination range ([0,1] for floats and "
	    "[0,255] for integers).");
}

inline int clamp_int(int x, int l, int h)
{
    if (x < l)
    {
	x = l;
    }
    else if (x > h)
    {
	x = h;
    }
    return x;
}

inline float clamp_float(float x, float l, float h)
{
    if (x < l)
    {
	x = l;
    }
    else if (x > h)
    {
	x = h;
    }
    return x;
}

int cmd_fieldconv(int argc, char *argv[])
{
    typedef enum { INTYPE_FRAME, INTYPE_INTFIELD, INTYPE_FLOATFIELD } intype_t;
    const char *intype_names[] = { "frame", "intfield", "floatfield", NULL };
    cvl_option_name_t intype = { -1, intype_names };
    typedef enum { OUTTYPE_GRAY, OUTTYPE_RGB, OUTTYPE_INT, OUTTYPE_INT3,
    OUTTYPE_FLOAT, OUTTYPE_FLOAT3 } outtype_t;
    const char *outtype_names[] = { "gray", "rgb", "int", "int3", "float", "float3", NULL };
    cvl_option_name_t outtype = { -1, outtype_names };
    cvl_option_int_t l = { 0, INT_MIN, INT_MAX };
    cvl_option_int_t h = { 255, INT_MIN, INT_MAX };
    cvl_option_double_t L = { 0.0, FLT_MIN, true, FLT_MAX, true };
    cvl_option_double_t H = { 1.0, FLT_MIN, true, FLT_MAX, true };
    cvl_option_t options[] = 
    { 
	{ "input",      'i', CVL_OPTION_NAME,   &intype,  true  },
	{ "output",     'o', CVL_OPTION_NAME,   &outtype, true  },
	{ "low-int",    'l', CVL_OPTION_INT,    &l,       false },
	{ "high-int",   'h', CVL_OPTION_INT,    &h,       false },
	{ "low-float",  'L', CVL_OPTION_DOUBLE, &L,       false },
	{ "high-float", 'H', CVL_OPTION_DOUBLE, &H,       false },
	cvl_option_null 
    };
    cvl_io_info_t *input_info;    
    cvl_io_info_t *output_info;
    bool error;

    cvl_msg_set_command_name("%s", argv[0]);    
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    if (l.value > h.value)
    {
	cvl_msg_err("l must be less than or equal to h");
	return 1;
    }
    if (L.value > H.value)
    {
	cvl_msg_err("L must be less than or equal to H");
	return 1;
    }

    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);
    
    error = false;
    while (!error)
    {
	cvl_frame_t *inframe;
	cvl_frame_t *outframe = NULL;	// shut up compiler warning
	int width, height;
	bool use_float;
	cvl_field_t *tmpfield;
	cvl_field_t *infield;
	cvl_field_t *outfield = NULL;	// shut up compiler warning
	cvl_pixel_t valp[3];
	float valf[3];
	int vali[3];
	
	/* Read. */
	if (intype.value == INTYPE_FRAME)
	{
	    if (cvl_io_eof(stdin))
	    {
		break;
	    }
	    if (!cvl_io_read(stdin, input_info, &inframe))
	    {
		error = true;
		break;
	    }
	    width = cvl_frame_width(inframe);
	    height = cvl_frame_height(inframe);
	}
	else
	{
	    if (cvl_io_eof(stdin))
	    {
		break;
	    }
	    if (!cvl_field_read(stdin, &infield))
	    {
		error = true;
		break;
	    }
	    width = cvl_field_width(infield);
	    height = cvl_field_height(infield);
	}

	/* Convert to intermediate format. */
	use_float = (outtype.value == OUTTYPE_FLOAT || outtype.value == OUTTYPE_FLOAT3);
	tmpfield = cvl_field_new(3 * (use_float ? sizeof(float) : sizeof(int)), width, height);
	for (int i = 0; i < width * height; i++)
	{
	    if (intype.value == INTYPE_FRAME)
	    {
		if (cvl_frame_pixel_type(inframe) == CVL_PIXEL_GRAY)
		{
		    valp[0] = cvl_frame_get_i(inframe, i);
	    	    valp[1] = valp[0];
    		    valp[2] = valp[0];
		}
		else if (cvl_frame_pixel_type(inframe) == CVL_PIXEL_RGB)
		{
		    cvl_pixel_t p = cvl_frame_get_i(inframe, i);
		    valp[0] = cvl_pixel_rgb_to_r(p);
	    	    valp[1] = cvl_pixel_rgb_to_g(p);
    		    valp[2] = cvl_pixel_rgb_to_b(p);
		}
		else
		{
		    cvl_pixel_t p = cvl_pixel_yuv_to_rgb(cvl_frame_get_i(inframe, i));
		    valp[0] = cvl_pixel_rgb_to_r(p);
	    	    valp[1] = cvl_pixel_rgb_to_g(p);
    		    valp[2] = cvl_pixel_rgb_to_b(p);
		}
		if (use_float)
	    	{
		    valf[0] = (float)valp[0] / (float)255.0;
		    valf[1] = (float)valp[1] / (float)255.0;
		    valf[2] = (float)valp[1] / (float)255.0;
		    cvl_field_set_i(tmpfield, i, valf);
    		}
		else
		{
		    vali[0] = valp[0];
		    vali[1] = valp[1];
		    vali[2] = valp[1];
		    cvl_field_set_i(tmpfield, i, vali);
		}
	    }
	    else
	    {
		if (intype.value == INTYPE_INTFIELD)
		{
		    if (cvl_field_element_size(infield) / sizeof(int) == 1)
		    {
			if (use_float)
			{
			    int p = *cvl_field_i_get_i(infield, i);
			    p = clamp_int(p, l.value, h.value);
			    float P = (((double)p + (double)l.value)) / (double)(h.value - l.value);
			    valf[0] = P;
			    valf[1] = P;
			    valf[2] = P;
			    cvl_field_set_i(tmpfield, i, valf);
			}
			else
			{
			    int p = *cvl_field_i_get_i(infield, i);
			    p = clamp_int(p, l.value, h.value);
			    p = cvl_iround((255.0 * ((double)p + (double)l.value)) / (double)(h.value - l.value));
			    vali[0] = p;
			    vali[1] = p;
			    vali[2] = p;
			    cvl_field_set_i(tmpfield, i, vali);
			}
		    }
		    else
		    {
			if (use_float)
			{
			    int p = cvl_field_i_get_i(infield, i)[0];
			    p = clamp_int(p, l.value, h.value);
			    float P;
			    P = (((double)p + (double)l.value)) / (double)(h.value - l.value);
			    valf[0] = P;
			    p = cvl_field_f_get_i(infield, i)[1];
			    p = clamp_int(p, l.value, h.value);
			    P = (((double)p + (double)l.value)) / (double)(h.value - l.value);
			    valf[1] = P;
			    p = cvl_field_f_get_i(infield, i)[2];
			    p = clamp_int(p, l.value, h.value);
			    P = (((double)p + (double)l.value)) / (double)(h.value - l.value);
			    valf[2] = P;
			    cvl_field_set_i(tmpfield, i, valf);
			}
			else
			{
			    int p = cvl_field_i_get_i(infield, i)[0];
			    p = clamp_int(p, l.value, h.value);
			    p = cvl_iround((255.0 * ((double)p + (double)l.value)) / (double)(h.value - l.value));
			    vali[0] = p;
			    p = cvl_field_i_get_i(infield, i)[1];
			    p = clamp_int(p, l.value, h.value);
			    p = cvl_iround((255.0 * ((double)p + (double)l.value)) / (double)(h.value - l.value));
			    vali[1] = p;
			    p = cvl_field_i_get_i(infield, i)[2];
			    p = clamp_int(p, l.value, h.value);
			    p = cvl_iround((255.0 * ((double)p + (double)l.value)) / (double)(h.value - l.value));
			    vali[2] = p;
			    cvl_field_set_i(tmpfield, i, vali);
			}
		    }
		}
		else
		{
		    if (cvl_field_element_size(infield) / sizeof(float) == 1)
		    {
			if (use_float)
			{
			    float p = *cvl_field_f_get_i(infield, i);
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    valf[0] = p;
			    valf[1] = p;
			    valf[2] = p;
			    cvl_field_set_i(tmpfield, i, valf);
			}
			else
			{
			    float p = *cvl_field_f_get_i(infield, i);
			    p = clamp_float(p, L.value, H.value);
			    int P;
			    p = (p + L.value) / (H.value - L.value);
			    P = cvl_iround(255.0 * p);
			    vali[0] = P;
			    vali[1] = P;
			    vali[2] = P;
			    cvl_field_set_i(tmpfield, i, vali);
			}
		    }
		    else
		    {
			if (use_float)
			{
			    float p = cvl_field_f_get_i(infield, i)[0];
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    valf[0] = p;
			    p = cvl_field_f_get_i(infield, i)[1];
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    valf[1] = p;
			    p = cvl_field_f_get_i(infield, i)[2];
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    valf[2] = p;
			    cvl_field_set_i(tmpfield, i, valf);
			}
			else
			{
			    int P;
			    float p = cvl_field_f_get_i(infield, i)[0];
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    P = cvl_iround(255.0 * p);
			    vali[0] = P;
			    p = cvl_field_f_get_i(infield, i)[1];
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    P = cvl_iround(255.0 * p);
			    vali[1] = P;
			    p = cvl_field_f_get_i(infield, i)[2];
			    p = clamp_float(p, L.value, H.value);
			    p = (p + L.value) / (H.value - L.value);
			    P = cvl_iround(255.0 * p);
			    vali[2] = P;
			    cvl_field_set_i(tmpfield, i, vali);
			}
		    }
		}
	    }
	}

	/* Convert to output format. */
	if (use_float)
	{
	    if (outtype.value == OUTTYPE_FLOAT)
	    {
		outfield = cvl_field_new(sizeof(float), width, height);
		for (int i = 0; i < width * height; i++)
		{
		    float p = (cvl_field_f_get_i(tmpfield, i)[0] 
			    + cvl_field_f_get_i(tmpfield, i)[1] 
			    + cvl_field_f_get_i(tmpfield, i)[2]) / (float)3.0;
		    cvl_field_set_i(outfield, i, &p);
		}
	    }
	    else
	    {
		outfield = cvl_field_clone(tmpfield);
	    }
     	}
	else
	{
	    if (outtype.value == OUTTYPE_INT)
	    {
		outfield = cvl_field_new(sizeof(int), width, height);
		for (int i = 0; i < width * height; i++)
		{
		    int p = (cvl_field_i_get_i(tmpfield, i)[0] 
			    + cvl_field_i_get_i(tmpfield, i)[1] 
			    + cvl_field_i_get_i(tmpfield, i)[2]) / 3;
		    cvl_field_set_i(outfield, i, &p);
		}
	    }
	    else if (outtype.value == OUTTYPE_INT3)
	    {
		outfield = cvl_field_clone(tmpfield);
	    }
	    else if (outtype.value == OUTTYPE_GRAY)
	    {
		outframe = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
		for (int i = 0; i < width * height; i++)
		{
		    cvl_pixel_t p = (cvl_field_i_get_i(tmpfield, i)[0] 
			    + cvl_field_i_get_i(tmpfield, i)[1] 
			    + cvl_field_i_get_i(tmpfield, i)[2]) / 3;
		    cvl_frame_set_i(outframe, i, p);
		}
	    }
	    else if (outtype.value == OUTTYPE_RGB)
	    {
		outframe = cvl_frame_new(CVL_PIXEL_RGB, width, height);
		for (int i = 0; i < width * height; i++)
		{
		    cvl_pixel_t p = cvl_pixel_rgb(
			    cvl_field_i_get_i(tmpfield, i)[0],
			    cvl_field_i_get_i(tmpfield, i)[1],
			    cvl_field_i_get_i(tmpfield, i)[2]);
		    cvl_frame_set_i(outframe, i, p);
		}
	    }
	}
	cvl_field_free(tmpfield);

	/* Write. */
	if (outtype.value == OUTTYPE_GRAY || outtype.value == OUTTYPE_RGB)
	{
	    if (!cvl_io_write(stdout, output_info, outframe))
	    {
		cvl_frame_free(outframe);
		error = true;
		break;
	    }
	    cvl_frame_free(outframe);
	}
	else
	{
	    if (!cvl_field_write(stdout, outfield))
	    {
		cvl_field_free(outfield);
		error = true;
		break;
	    }
	    cvl_field_free(outfield);
	}
    }

    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
