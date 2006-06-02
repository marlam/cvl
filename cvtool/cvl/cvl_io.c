/*
 * cvl_io.c
 * 
 * This file is part of CVL, a computer vision library.
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

/** 
 * \file cvl_io.h
 * \brief Input and output.
 * 
 * Handling input and output streams of NetPBM images or YUV4MPEG2 frames:
 * Manage stream information, read and write frames.
 */

#include "config.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>
extern int errno;

#include "xalloc.h"

#include "cvl/cvl_msg.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_pixel.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_io.h"


/**
 * \typedef cvl_io_stream_t
 * Type of an input or output stream.
 */
/** \var CVL_IO_STREAM_Y4M
 * A YUV4MPEG2 stream. */
/** \var CVL_IO_STREAM_PNM
 * A NetPBM stream. */

/**
 * \typedef cvl_y4m_chroma_t
 * Type of chroma data.
 */
/** \var CVL_Y4M_CHROMA_UNKNOWN
 * Unknown chroma subsampling. */
/** \var CVL_Y4M_CHROMA_420JPEG
 * 4:2:0 with JPEG/MPEG-1 siting. */
/** \var CVL_Y4M_CHROMA_444
 * 4:4:4 (no subsampling).
 */

/**
 * \typedef cvl_y4m_interlacing_t
 * Type of interlacing.
 */
/** \var CVL_Y4M_INTERLACING_UNKNOWN
 * Unknown interlacing. Treated the same as no interlacing. */
/** \var CVL_Y4M_INTERLACING_PROGRESSIVE
 * Progressive interlacing (= no interlacing). */

/**
 * \struct cvl_io_info_t
 * \brief Input / Output information.
 *
 * Structure that stores information about input and output streams. Use the
 * functions from \link cvl_io.h cvl_io.h \endlink to manipulate this structure.
 */     

/******************************************************************************
 * 
 * YUV4MPEG format 
 *
 *****************************************************************************/

static bool cvl_io_read_input_info_yuv(FILE *f, cvl_io_info_t *input_info)
{
    char magic[10];
    char chroma_str[9];

    if (fgets(magic, 10, f) == NULL || strcmp(magic, "YUV4MPEG2") != 0)
    {
	cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
		"missing YUV4MPEG2 magic");
	return false;
    }
    
    input_info->_chroma = CVL_Y4M_CHROMA_420JPEG;		// default value
    input_info->_interlacing = CVL_Y4M_INTERLACING_UNKNOWN;	// default value
    for (;;)
    {
	bool ok;
	int c;
	
	c = fgetc(f);
	if (c == '\n')
	{
	    break;
	}
	if (c != ' ' || c == EOF)
	{
	    cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
	    	    "tag not starting with space");
	    return false;
	}
	c = fgetc(f);
	switch (c)
	{
	    case 'W':
		ok = (fscanf(f, "%d", &(input_info->_width)) == 1 && input_info->_width > 0);
		break;

	    case 'H':
		ok = (fscanf(f, "%d", &(input_info->_height)) == 1 && input_info->_height > 0);
		break;

	    case 'I':
		ok = ((c = fgetc(f)) != EOF);
		if (c == 'p')
		{
		    input_info->_interlacing = CVL_Y4M_INTERLACING_PROGRESSIVE;
		}
		else if (c == '?')
		{
		    input_info->_interlacing = CVL_Y4M_INTERLACING_UNKNOWN;
		}
		else
		{
		    if (c == 't' || c == 'b' || c == 'm')
		    {
			/* TODO */
			cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
				"interlacing not supported yet");
		    }
		    else
		    {
			cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
				"unknown interlacing type");
		    }
		    ok = false;
		}
		break;

	    case 'C':
		chroma_str[0] = '\0';
		ok = (fscanf(f, "%8s", chroma_str) == 1);
		if (strcmp(chroma_str, "420jpeg") == 0)
		{
		    input_info->_chroma = CVL_Y4M_CHROMA_420JPEG;
		}
		else if (strcmp(chroma_str, "444") == 0)
		{
		    input_info->_chroma = CVL_Y4M_CHROMA_444;
		}
		else if (strcmp(chroma_str, "420mpeg2") == 0
			|| strcmp(chroma_str, "420paldv") == 0
			|| strcmp(chroma_str, "411") == 0
			|| strcmp(chroma_str, "422") == 0
			|| strcmp(chroma_str, "444alpha") == 0
			|| strcmp(chroma_str, "mono") == 0)
		{
		    cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
	    		    "unsupported chroma type");
		    ok = false;
		}
		else
		{
		    cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
	    		    "unknown chroma type");
		    ok = false;
		}
		break;

	    case 'F':
		ok = (fscanf(f, "%d:%d", &(input_info->_framerate1), &(input_info->_framerate2)) == 2
			&& input_info->_framerate1 >= 0 && input_info->_framerate2 >= 0);
		break;

	    case 'A':
		ok = (fscanf(f, "%d:%d", &(input_info->_aspectratio1), &(input_info->_aspectratio2)) == 2
			&& input_info->_aspectratio1 >= 0 && input_info->_aspectratio2 >= 0);
		break;

	    case 'X':
		/* TODO */
		cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
			"cannot handle X tags yet");
		ok = false;
		break;
		
	    default:
		ok = false;
		break;
	}
	if (!ok)
	{
	    cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
    		    "some tag is invalid");
	    return false;
	}
    }
    if (input_info->_chroma == CVL_Y4M_CHROMA_420JPEG 
	    && (input_info->_width % 2 != 0 || input_info->_height % 2 != 0))
    {
	cvl_msg_err("cannot read YUV4MPEG2 stream info: %s", 
		"chroma type 420jpeg requires that width and height are even numbers");
	return false;
    }
    input_info->_input_info_known = true;
    cvl_msg_dbg("input stream:  YUV4MPEG2 W%d H%d C%s I%c F%d:%d A%d:%d", 
	    input_info->_width, input_info->_height, 
	    (input_info->_chroma == CVL_Y4M_CHROMA_420JPEG ? "420jpeg" : "444"),
	    (input_info->_interlacing == CVL_Y4M_INTERLACING_UNKNOWN ? '?' : 'p'),
	    input_info->_framerate1, input_info->_framerate2, 
	    input_info->_aspectratio1, input_info->_aspectratio2);
    return true;
}

static bool cvl_io_write_output_info_yuv(FILE *f, cvl_io_info_t *output_info)
{
    /* 420jpeg is the default chroma subsampling, and older YUV4MPEG2 variants
     * did not have a C tag to specify a different one, so we use a C tag only
     * if we have 444 chroma subsampling. */
    fprintf(f, "YUV4MPEG2 W%d H%d %sI%c F%d:%d A%d:%d\n", 
	    output_info->_width, output_info->_height, 
	    (output_info->_chroma == CVL_Y4M_CHROMA_444 ? "C444 " : ""),
	    (output_info->_interlacing == CVL_Y4M_INTERLACING_UNKNOWN ? '?' : 'p'),
	    output_info->_framerate1, output_info->_framerate2, 
	    output_info->_aspectratio1, output_info->_aspectratio2);
    if (ferror(f))
    {
	cvl_msg_err("cannot write YUV4MPEG2 stream info: output error");
	return false;
    }
    else
    {
	/* Use a C tag in the debug output, to make things clearer */
	cvl_msg_dbg("output stream: YUV4MPEG2 W%d H%d C%s I%c F%d:%d A%d:%d",
	 	output_info->_width, output_info->_height,
		(output_info->_chroma == CVL_Y4M_CHROMA_444 ? "444" : "420jpeg"),
		(output_info->_interlacing == CVL_Y4M_INTERLACING_UNKNOWN ? '?' : 'p'),
	 	output_info->_framerate1, output_info->_framerate2,
	  	output_info->_aspectratio1, output_info->_aspectratio2);
	output_info->_output_info_written = true;
	return true;
    }
}

/* Make sure that every value in array is between 'low' and 'high' (inclusive). */
static inline void cvl_io_clip_values(uint8_t *array, size_t len, uint8_t low, uint8_t high)
{
    for (size_t i = 0; i < len; i++)
    {
	if (array[i] < low)
	{
	    array[i] = low;
	}
	else if (array[i] > high)
	{
	    array[i] = high;
	}
    }
}

static bool cvl_io_read_frame_yuv(FILE *f, cvl_io_info_t *input_info, cvl_frame_t **frame)
{
    char magic[7];
    size_t rawsize;
    uint8_t *rawdata;
    uint8_t *Y, *U, *V;
    size_t Y_len, UV_len;

    *frame = cvl_frame_new(CVL_PIXEL_YUV, input_info->_width, input_info->_height);
    /* Now we know that (width * height * sizeof(cvl_pixel_t)) fits in size_t, so that rawsize
     * cannot overflow, because sizeof(cvl_pixel_t) is at least 4 */
    Y_len = input_info->_width * input_info->_height;
    UV_len = (input_info->_chroma == CVL_Y4M_CHROMA_420JPEG ? Y_len / 4 : Y_len);
    rawsize = Y_len + UV_len * 2;
    rawdata = xmalloc(rawsize * sizeof(uint8_t));
    Y = rawdata;
    U = Y + Y_len;
    V = U + UV_len;
    
    cvl_msg_dbg("reading YUV4MPEG2 frame %d", input_info->_frame_counter++);
    if (fgets(magic, 7, f) == NULL || strcmp(magic, "FRAME\n") != 0)
    {
	cvl_msg_err("cannot read YUV4MPEG2 frame: %s", "invalid or unsupported header");
	cvl_frame_free(*frame);
	free(rawdata);
	return false;
    }
    if (fread(rawdata, rawsize, 1, f) != 1)
    {
	cvl_msg_err("cannot read YUV4MPEG2 frame: %s", "input error");
	cvl_frame_free(*frame);
	free(rawdata);
	return false;
    }
    
    /* correct the values if they are outside the valid range */
    cvl_io_clip_values(Y, Y_len, 16, 235);
    cvl_io_clip_values(U, UV_len, 16, 240);
    cvl_io_clip_values(V, UV_len, 16, 240);
    
    for (int y = 0; y < input_info->_height; y++)
    {
	for (int x = 0; x < input_info->_width; x++)
	{
	    if (input_info->_chroma == CVL_Y4M_CHROMA_420JPEG)
	    {
		cvl_frame_set(*frame, x, y, cvl_pixel_yuv(
		    	    Y[y * input_info->_width + x],
		    	    U[y / 2 * input_info->_width / 2 + x / 2],
		    	    V[y / 2 * input_info->_width / 2 + x / 2]));
	    }
	    else // (input_info->_chroma == CVL_Y4M_CHROMA_444)
	    {
		cvl_frame_set(*frame, x, y, cvl_pixel_yuv(
			    Y[y * input_info->_width + x],
			    U[y * input_info->_width + x],
			    V[y * input_info->_width + x]));
	    }
	}
    }
    
    free(rawdata);
    return true;
}

static bool cvl_io_write_frame_yuv(FILE *f, cvl_io_info_t *output_info, const cvl_frame_t *frame)
{
    size_t rawsize;
    uint8_t *rawdata;
    uint8_t *Y, *U, *V;
    size_t Y_len, UV_len;
    cvl_frame_t *tmpframe;

    cvl_msg_dbg("writing YUV4MPEG2 frame %d", output_info->_frame_counter++);
    if (output_info->_width != cvl_frame_width(frame) || output_info->_height != cvl_frame_height(frame))
    {
	cvl_msg_err("cannot write YUV4MPEG2 frame: %s", 
		"it has not the same dimensions as the prior frames");
	return false;
    }
    if (output_info->_chroma == CVL_Y4M_CHROMA_420JPEG)
    {
	if (output_info->_width % 2 != 0 || output_info->_height % 2 != 0)
	{
	    cvl_msg_err("cannot write YUV4MPEG2 frame: %s",
		    "chroma type 420jpeg requires that width and height are even numbers");
	    return false;
	}
    }
    if (!output_info->_output_info_written)
    {
	if (!cvl_io_write_output_info_yuv(f, output_info))
	{
	    return false;
	}
    }
    
    Y_len = output_info->_width * output_info->_height;
    UV_len = (output_info->_chroma == CVL_Y4M_CHROMA_420JPEG ? Y_len / 4 : Y_len);
    /* This cannot overflow because (width * height * sizeof(cvl_pixel_t)) fits
     * in size_t, and sizeof(cvl_pixel_t) is at least 4 */
    rawsize = Y_len + UV_len * 2;
    rawdata = xmalloc(rawsize * sizeof(uint8_t));
    Y = rawdata;
    U = Y + Y_len;
    V = U + UV_len;
    
    if (cvl_frame_pixel_type(frame) != CVL_PIXEL_YUV)
    {
	tmpframe = cvl_frame_clone(frame);
	cvl_frame_to_yuv(tmpframe);
    }
    else
    {
	tmpframe = (cvl_frame_t *)frame;
    }
    for (int y = 0; y < cvl_frame_height(tmpframe); y++)
    {
	for (int x = 0; x < cvl_frame_width(tmpframe); x++)
	{
	    if (output_info->_chroma == CVL_Y4M_CHROMA_420JPEG)
	    {
		Y[y * cvl_frame_width(tmpframe) + x] = cvl_pixel_yuv_to_y(cvl_frame_get(tmpframe, x, y));
		if (y % 2 == 0 && x % 2 == 0)
		{
		    U[y / 2 * cvl_frame_width(tmpframe) / 2 + x / 2] = 
			cvl_iround((double)(cvl_pixel_yuv_to_u(cvl_frame_get(tmpframe, x, y))
			    	    + cvl_pixel_yuv_to_u(cvl_frame_get(tmpframe, x + 1, y))
			    	    + cvl_pixel_yuv_to_u(cvl_frame_get(tmpframe, x, y + 1))
			    	    + cvl_pixel_yuv_to_u(cvl_frame_get(tmpframe, x + 1, y + 1))) / 4.0);
		    V[y / 2 * cvl_frame_width(tmpframe) / 2 + x / 2] = 
			cvl_iround((double)(cvl_pixel_yuv_to_v(cvl_frame_get(tmpframe, x, y))
				    + cvl_pixel_yuv_to_v(cvl_frame_get(tmpframe, x + 1, y))
				    + cvl_pixel_yuv_to_v(cvl_frame_get(tmpframe, x, y + 1))
				    + cvl_pixel_yuv_to_v(cvl_frame_get(tmpframe, x + 1, y + 1))) / 4.0);
		}
	    }
	    else // (output_info->_chroma == CVL_Y4M_CHROMA_444)
	    {
		cvl_pixel_t yuv = cvl_frame_get(tmpframe, x, y);
		Y[y * cvl_frame_width(tmpframe) + x] = cvl_pixel_yuv_to_y(yuv);
		U[y * cvl_frame_width(tmpframe) + x] = cvl_pixel_yuv_to_u(yuv);
		V[y * cvl_frame_width(tmpframe) + x] = cvl_pixel_yuv_to_v(yuv);
	    }
	}
    }
    if (tmpframe != frame)
    {
	cvl_frame_free(tmpframe);
    }

    /* correct the values if they are outside the valid range */
    cvl_io_clip_values(Y, Y_len, 16, 235);
    cvl_io_clip_values(U, UV_len, 16, 240);
    cvl_io_clip_values(V, UV_len, 16, 240);
    
    /* output */
    if (fprintf(f, "FRAME\n") != 6 || fwrite(rawdata, 1, rawsize, f) != rawsize)
    {
	cvl_msg_err("cannot write YUV4MPEG2 frame: output error");
	free(rawdata);
	return false;
    }
    
    free(rawdata);
    return true;
}


/******************************************************************************
 * 
 * The NetPBM formats
 *
 *****************************************************************************/

static bool cvl_io_read_input_info_pnm(FILE *f UNUSED, cvl_io_info_t *input_info)
{
    cvl_msg_dbg("input stream:  NetPBM");
    input_info->_input_info_known = true;
    return true;
}

static bool cvl_io_write_output_info_pnm(FILE *f UNUSED, cvl_io_info_t *output_info)
{
    cvl_msg_dbg("output stream: NetPBM");
    output_info->_output_info_written = true;
    return true;
}

/* Skip whitespace, and optional comments, too */
static bool cvl_netpbm_skip(FILE *f)
{
    int c;
    bool in_comment = false;

    for (;;)
    {
     	c = fgetc(f);
	if (c == EOF)
	{
	    if (ferror(f))
	    {
		return false;
	    }
	    else
	    {
		return true;
	    }
	}
	else
	{
	    if (in_comment)
	    {
		if (c == '\n')
		{
		    in_comment = false;
		}
	    }
	    else
	    {
		if (c == '#')
		{
		    in_comment = true;
		}
		else if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n'))
		{
		    if (ungetc(c, f) == EOF)
		    {
			return false;
		    }
		    return true;
		}
	    }
	}
    }
}

static bool cvl_io_read_frame_pnm(FILE *f, cvl_io_info_t *input_info, cvl_frame_t **frame)
{
    const char errmsg[] = "cannot read NetPBM frame";
    typedef enum { NETPBM_PBM, NETPBM_PGM, NETPBM_PPM } netpbm_subformat_t;
    netpbm_subformat_t subformat;
    cvl_pixel_type_t pixel_type;
    int width, height, size, maxval;
    int c;
    
    if ((c = fgetc(f)) != 'P')
    {
	if (c == EOF)
	{
	    if (ferror(f))
	    {
		cvl_msg_err("%s: input error", errmsg);
	    }
	    else
	    {
		cvl_msg_err("%s: EOF", errmsg);
	    }
	}
	else
	{
	    cvl_msg_err("%s: no valid PNM header", errmsg);
	}
	return false;
    }
    c = fgetc(f);
    if (c == '4')
    {
	subformat = NETPBM_PBM;
	pixel_type = CVL_PIXEL_GRAY;
	maxval = 1;
	if (!cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &width) != 1 || width < 1 || !cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &height) != 1 || height < 1 || fgetc(f) == EOF)
	{
	    cvl_msg_err("%s: PBM header invalid", errmsg);
	    return false;
	}
    }
    else if (c == '5')
    {
	subformat = NETPBM_PGM;
	pixel_type = CVL_PIXEL_GRAY;
	if (!cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &width) != 1 || width < 1 || !cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &height) != 1 || height < 1 || !cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &maxval) != 1 || maxval < 1 || maxval > 65535 || fgetc(f) == EOF)
	{
	    cvl_msg_err("%s: PGM header invalid", errmsg);
	    return false;
	}
    }
    else if (c == '6')
    {
	subformat = NETPBM_PPM;
	pixel_type = CVL_PIXEL_RGB;
	if (!cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &width) != 1 || width < 1 || !cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &height) != 1 || height < 1 || !cvl_netpbm_skip(f)
		|| fscanf(f, "%d", &maxval) != 1 || maxval < 1 || maxval > 65535 || fgetc(f) == EOF)
	{
	    cvl_msg_err("%s: PPM header invalid", errmsg);
	    return false;
	}
    }
    else if (c == '7')
    {
	bool ok;
	bool header_end;
	int depth;
	char tupletype[14];
	
	tupletype[0] = '\0';
	ok = true;
	header_end = false;
	while (ok && !header_end)
	{
	    if (!cvl_netpbm_skip(f) || (c = fgetc(f)) == EOF || ungetc(c, f) == EOF)
	    {
		cvl_msg_err("%s: PAM header invalid, error 1", errmsg);
		return false;
	    }
	    switch (c)
	    {
		case '#':
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		    ok = cvl_netpbm_skip(f);
		    break;
		    
		case 'W':
		    ok = (fscanf(f, "WIDTH %d", &width) == 1 && width > 0 && cvl_netpbm_skip(f));
		    break;
		    
		case 'H':
		    ok = (fscanf(f, "HEIGHT %d", &height) == 1 && height > 0 && cvl_netpbm_skip(f));
		    break;
		    
		case 'D':
		    ok = (fscanf(f, "DEPTH %d", &depth) == 1 && depth == 1 && cvl_netpbm_skip(f));
		    break;
		    
		case 'M':
		    ok = (fscanf(f, "MAXVAL %d", &maxval) == 1 
			    && maxval > 0 && maxval < 65536 && cvl_netpbm_skip(f));
		    break;
		    
		case 'T':
		    ok = (fscanf(f, "TUPLETYPE %13s", tupletype) == 1 && cvl_netpbm_skip(f));
		    break;
		    
		case 'E':
		    ok = (fgetc(f) == 'E' && fgetc(f) == 'N' && fgetc(f) == 'D' 
			    && fgetc(f) == 'H' && fgetc(f) == 'D' && fgetc(f) == 'R' && fgetc(f) == EOF);
		    header_end = true;
		    break;
		    
		default:
		    ok = false;
		    break;
	    }
	}
	if (!header_end || !ok)
	{
	    cvl_msg_err("%s: PAM header invalid, error 2", errmsg);
	    return false;
	}
	if (strcmp(tupletype, "BLACKANDWHITE") == 0)
	{
	    subformat = NETPBM_PBM;
	    pixel_type = CVL_PIXEL_GRAY;
	}
	else if (strcmp(tupletype, "GRAYSCALE") == 0)
	{
	    subformat = NETPBM_PGM;
	    pixel_type = CVL_PIXEL_GRAY;
	}
	else if (strcmp(tupletype, "RGB") == 0)
	{
	    subformat = NETPBM_PPM;
	    pixel_type = CVL_PIXEL_RGB;
	}
	else
	{
	    cvl_msg_err("%s: unknown tuple type in PAM header", errmsg);
	    return false;
	}	    
    }
    else
    {
	cvl_msg_err("%s: unknown PNM subtype: '%c'", errmsg, c);
	return false;
    }
    
    cvl_msg_dbg("reading NetPBM frame %d: %s, width = %d, height = %d", 
	    input_info->_frame_counter++,
	    subformat == NETPBM_PBM ? "pbm" : (subformat == NETPBM_PGM ? "pgm" : "ppm"),
	    width, height);
    input_info->_width = width;
    input_info->_height = height;
    *frame = cvl_frame_new(pixel_type, width, height);
    size = width * height;
    if (subformat == NETPBM_PBM)
    {
	/* cannot overflow here because else frame_new() would have failed */
	size_t rawsize = cvl_frame_height(*frame) * ((cvl_frame_width(*frame) - 1) / 8 + 1);
	uint8_t *pbmdata = xmalloc(rawsize * sizeof(uint8_t));
	size_t rawindex = 0;

	if (fread(pbmdata, sizeof(uint8_t), rawsize, f) != rawsize)
	{
	    cvl_frame_free(*frame);
	    cvl_msg_err("%s: EOF or input error in PBM data", errmsg);
	    free(pbmdata);
	    return false;
	}
	for (int y = 0; y < cvl_frame_height(*frame); y++)
	{
	    for (int x = 0; x < cvl_frame_width(*frame); x += 8)
	    {
		uint8_t d = pbmdata[rawindex++];
		cvl_frame_set(*frame, x, y, (cvl_pixel_t)(d & 0x80 ? 0 : 255));
		if (x + 1 < width)
		    cvl_frame_set(*frame, x + 1, y, (cvl_pixel_t)(d & 0x40 ? 0 : 255));
		if (x + 2 < width)
		    cvl_frame_set(*frame, x + 2, y, (cvl_pixel_t)(d & 0x20 ? 0 : 255));
		if (x + 3 < width)
		    cvl_frame_set(*frame, x + 3, y, (cvl_pixel_t)(d & 0x10 ? 0 : 255));
		if (x + 4 < width)
		    cvl_frame_set(*frame, x + 4, y, (cvl_pixel_t)(d & 0x08 ? 0 : 255));
		if (x + 5 < width)
		    cvl_frame_set(*frame, x + 5, y, (cvl_pixel_t)(d & 0x04 ? 0 : 255));
		if (x + 6 < width)
		    cvl_frame_set(*frame, x + 6, y, (cvl_pixel_t)(d & 0x02 ? 0 : 255));
		if (x + 7 < width)
		    cvl_frame_set(*frame, x + 7, y, (cvl_pixel_t)(d & 0x01 ? 0 : 255));
	    }
	}
	free(pbmdata);
    }
    else if (subformat == NETPBM_PGM)
    {
	/* cannot overflow because (size * sizeof(cvl_pixel_t)) fits in size_t */
	size_t rawsize = (size_t)size * (maxval < 256 ? 1 : 2);	
	uint8_t *pgmdata = xmalloc(rawsize * sizeof(uint8_t));

	if (fread(pgmdata, sizeof(uint8_t), rawsize, f) != rawsize)
	{
	    cvl_frame_free(*frame);
	    cvl_msg_err("%s: EOF or input error in PGM data", errmsg);
	    free(pgmdata);
	    return false;
	}
	if (maxval == 255)
	{
	    for (int i = 0; i < size; i++)
	    {
		cvl_frame_set_i(*frame, i, pgmdata[i]);
	    }
	}
     	else 
	{
	    cvl_msg_wrn("converting PGM data from maxval %d to maxval 255", maxval);
	    if (maxval < 255)
	    {
    		for (int i = 0; i < size; i++)
		{
		    cvl_frame_set_i(*frame, i, cvl_pixel_clamp(0, maxval, pgmdata[i]));
		}
	    }
	    else
	    {
		for (size_t i = 0; i < (size_t)size; i++)
		{
		    cvl_frame_set_i(*frame, i, cvl_pixel_clamp(0, maxval, 
				((int)pgmdata[i * 2] << 8) | (int)pgmdata[i * 2 + 1]));
		}
	    }
	}
	free(pgmdata);
    }
    else if (subformat == NETPBM_PPM)
    {
	/* For many-color PPM images, we need (size * 6) uint8_t's. */
	if (maxval >= 256 && !cvl_product_fits_in_size_t(size, 6))
	{
	    cvl_frame_free(*frame);
	    cvl_msg_err("%s: PPM data too big", errmsg); 
	    return NULL;
	}
	size_t rawsize = (size_t)size * 3 * (maxval < 256 ? 1 : 2);
	uint8_t *ppmdata = xmalloc(rawsize * sizeof(uint8_t));

	if (fread(ppmdata, sizeof(uint8_t), rawsize, f) != rawsize)
	{
	    cvl_frame_free(*frame);
	    cvl_msg_err("%s: EOF or input error in PPM data", errmsg);
	    free(ppmdata);
	    return NULL;
	}
	if (maxval == 255)
	{
	    for (size_t i = 0; i < (size_t)size; i++)
	    {
		cvl_frame_set_i(*frame, i, cvl_pixel_rgb(
			    ppmdata[i * 3    ], 
			    ppmdata[i * 3 + 1], 
			    ppmdata[i * 3 + 2]));
	    }
	}
	else 
	{
	    cvl_msg_wrn("converting PPM data from maxval %d to maxval 255", maxval);
	    if (maxval < 255)
	    {
		for (size_t i = 0; i < (size_t)size; i++)
		{
		    cvl_frame_set_i(*frame, i, cvl_pixel_rgb(
		    		cvl_pixel_clamp(0, maxval, ppmdata[i * 3    ]),
		    		cvl_pixel_clamp(0, maxval, ppmdata[i * 3 + 1]),
		    		cvl_pixel_clamp(0, maxval, ppmdata[i * 3 + 2])));
		}
	    }
	    else
	    {
		for (size_t i = 0; i < (size_t)size; i++)
		{
		    cvl_frame_set_i(*frame, i, cvl_pixel_rgb(
		    		cvl_pixel_clamp(0, maxval, 
		    		    ((int)ppmdata[i * 6    ] << 8) | (int)ppmdata[i * 6 + 1]),
		    		cvl_pixel_clamp(0, maxval, 
		    		    ((int)ppmdata[i * 6 + 2] << 8) | (int)ppmdata[i * 6 + 3]),
		    		cvl_pixel_clamp(0, maxval, 
		    		    ((int)ppmdata[i * 6 + 4] << 8) | (int)ppmdata[i * 6 + 5])));
		}
	    }
	}
	free(ppmdata);
    }
    return true;
}

static bool cvl_io_write_frame_pnm(FILE *f, cvl_io_info_t *output_info, const cvl_frame_t *frame)
{
    cvl_frame_t *tmpframe;
    int size;
    bool error;
    
    if (cvl_frame_pixel_type(frame) == CVL_PIXEL_YUV)
    {
	tmpframe = cvl_frame_clone(frame);
	cvl_frame_to_rgb(tmpframe);
    }
    else
    {
	tmpframe = (cvl_frame_t *)frame;
    }
    size = cvl_frame_width(tmpframe) * cvl_frame_height(tmpframe);
    cvl_msg_dbg("writing NetPBM frame %d: %s, width = %d, height = %d", 
	    output_info->_frame_counter++,
	    cvl_frame_pixel_type(tmpframe) == CVL_PIXEL_GRAY ? "pgm" : "ppm", 
	    cvl_frame_width(tmpframe), cvl_frame_height(tmpframe));
    
    if (cvl_frame_pixel_type(tmpframe) == CVL_PIXEL_GRAY)
    {
	uint8_t *pgmdata = xmalloc(size * sizeof(uint8_t));
	for (int i = 0; i < size; i++)
	{
	    pgmdata[i] = cvl_frame_get_i(tmpframe, i);
	}
	error = (fprintf(f, "P5\n%d %d\n255\n", cvl_frame_width(frame), cvl_frame_height(frame)) < 0
		|| fwrite(pgmdata, size * sizeof(uint8_t), 1, f) != 1);
	free(pgmdata);
    }
    else
    {
	/* cannot overflow because (size * sizeof(cvl_pixel_t)) fits in size_t */
	uint8_t *ppmdata = xmalloc((size_t)size * 3 * sizeof(uint8_t));
	for (size_t i = 0; i < (size_t)size; i++)
	{
	    cvl_pixel_t p = cvl_frame_get_i(tmpframe, i);
	    ppmdata[i * 3    ] = cvl_pixel_rgb_to_r(p);
	    ppmdata[i * 3 + 1] = cvl_pixel_rgb_to_g(p);
	    ppmdata[i * 3 + 2] = cvl_pixel_rgb_to_b(p);
	}
	error = (fprintf(f, "P6\n%d %d\n255\n", cvl_frame_width(frame), cvl_frame_height(frame)) < 0
		|| fwrite(ppmdata, (size_t)size * 3 * sizeof(uint8_t), 1, f) != 1);
	free(ppmdata);
    }

    if (tmpframe != frame)
    {
	cvl_frame_free(tmpframe);
    }

    if (error)
    {
	cvl_msg_err("cannot write NetPBM frame: output error");
    }
    
    return !error;
}    


/******************************************************************************
 *
 *  Stream independent functions (user interface)
 *
 *****************************************************************************/


/**
 * \return		A new input/output information structure, with default values filled in.
 *
 * Creates a new input/output information sructure.
 */
cvl_io_info_t *cvl_io_info_new(void)
{
    cvl_io_info_t *info = xmalloc(sizeof(cvl_io_info_t));
    info->_stream_type = INT_MAX;
    info->_frame_counter = 0;
    info->_width = -1;
    info->_height = -1;
    info->_chroma = CVL_Y4M_CHROMA_UNKNOWN;
    info->_interlacing = CVL_Y4M_INTERLACING_UNKNOWN;
    info->_framerate1 = 0;
    info->_framerate2 = 0;
    info->_aspectratio1 = 0;
    info->_aspectratio2 = 0;
    info->_input_info_known = false;
    info->_input_info = NULL;
    info->_output_info_written = false;
    return info;
}

/**
 * \param info		The information structure.
 * 
 * Frees an input/output information structure.
 * NULL pointers are accepted.
 */
void cvl_io_info_free(cvl_io_info_t *info)
{
    if (info)
    {
	free(info);
    }
}

/**
 * \param info		The io stream information.
 * \return		The frame counter.
 *
 * Returns the number of frames that were read from or written to the stream
 * assossiated with \a info.
 */
int cvl_io_info_frame_counter(cvl_io_info_t *info)
{
    cvl_assert(info != NULL);
    
    return info->_frame_counter;
}

/**
 * \param info		The stream information.
 *
 * Returns the stream type associated with \a info.
 * This information may be unknown until after the first frame was read or written!
 */
cvl_io_stream_t cvl_io_info_stream_type(cvl_io_info_t *info)
{
    cvl_assert(info != NULL);
    
    return info->_stream_type;
}

/**
 * \param output_info	The output stream information.
 * \param stream_type	The stream type.
 *
 * Sets the stream type for output streams.
 */
void cvl_io_info_set_stream_type(cvl_io_info_t *output_info, cvl_io_stream_t stream_type)
{
    cvl_assert(output_info != NULL);
    
    output_info->_stream_type = stream_type;
}

/**
 * \param info		The io stream information.
 * 
 * Returns the width associated with \a info.
 * This information may be unknown until after the first frame was read or written!
 */
int cvl_io_info_width(cvl_io_info_t *info)
{
    cvl_assert(info != NULL);
    
    return info->_width;
}

/**
 * \param output_info	The output stream information.
 * \param width		The width.
 * 
 * Sets the width for output streams.
 */
void cvl_io_info_set_width(cvl_io_info_t *output_info, int width)
{
    cvl_assert(output_info != NULL);
    
    output_info->_width = width;
}

/**
 * \param info		The io stream information.
 * 
 * Returns the height associated with \a info.
 * This information may be unknown until after the first frame was read or written!
 */
int cvl_io_info_height(cvl_io_info_t *info)
{
    cvl_assert(info != NULL);
    
    return info->_height;
}

/**
 * \param output_info	The output stream information.
 * \param height	The height.
 * 
 * Sets the height for output streams.
 */
void cvl_io_info_set_height(cvl_io_info_t *output_info, int height)
{
    cvl_assert(output_info != NULL);
    
    output_info->_height = height;
}

/**
 * \param info		The io stream information.
 * 
 * Returns the chroma subsampling type associated with \a info.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 * This information may be unknown until after the first frame was read or written!
 */
cvl_y4m_chroma_t cvl_io_info_chroma(cvl_io_info_t *info)
{
    cvl_assert(info != NULL);
    
    return info->_chroma;
}

/**
 * \param output_info	The output stream information.
 * \param chroma	The chroma subsampling type.
 * 
 * Sets the chroma subsampling type for output streams.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 */
void cvl_io_info_set_chroma(cvl_io_info_t *output_info, cvl_y4m_chroma_t chroma)
{
    cvl_assert(output_info != NULL);
    
    output_info->_chroma = chroma;
}

/**
 * \param info		The io stream information.
 * 
 * Returns the interlacing type associated with \a info.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 * This information may be unknown until after the first frame was read or written!
 */
cvl_y4m_interlacing_t cvl_io_info_interlacing(cvl_io_info_t *info)
{
    cvl_assert(info != NULL);
    
    return info->_interlacing;
}

/**
 * \param output_info	The output stream information.
 * \param interlacing	The interlacing type.
 * 
 * Sets the interlacing type for output streams.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 */
void cvl_io_info_set_interlacing(cvl_io_info_t *output_info, cvl_y4m_interlacing_t interlacing)
{
    cvl_assert(output_info != NULL);
    
    output_info->_interlacing = interlacing;
}

/**
 * \param info		The io stream information.
 * \param framerate1	The divident of the framerate.
 * \param framerate2	The divisor of the framerate.
 * 
 * Returns the framerate associated with \a info.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 * This information may be unknown until after the first frame was read or written!
 */
void cvl_io_info_framerate(cvl_io_info_t *info, int *framerate1, int *framerate2)
{
    cvl_assert(info != NULL);
    cvl_assert(framerate1 != NULL);
    cvl_assert(framerate2 != NULL);
    
    *framerate1 = info->_framerate1;
    *framerate2 = info->_framerate2;
}

/**
 * \param output_info	The output stream information.
 * \param framerate1	The divident of the framerate.
 * \param framerate2	The divisor of the framerate.
 * 
 * Sets the frame rate for output streams.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 */
void cvl_io_info_set_framerate(cvl_io_info_t *output_info, int framerate1, int framerate2)
{
    cvl_assert(output_info != NULL);
    
    output_info->_framerate1 = framerate1;
    output_info->_framerate2 = framerate2;
}

/**
 * \param info		The io stream information.
 * \param aspectratio1	The divident of the aspectratio.
 * \param aspectratio2	The divisor of the aspectratio.
 *
 * Returns the aspect ratio associated with \a info.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 * This information may be unknown until after the first frame was read or written!
 */
void cvl_io_info_aspectratio(cvl_io_info_t *info, int *aspectratio1, int *aspectratio2)
{
    cvl_assert(info != NULL);
    cvl_assert(aspectratio1 != NULL);
    cvl_assert(aspectratio2 != NULL);
    
    *aspectratio1 = info->_aspectratio1;
    *aspectratio2 = info->_aspectratio2;
}

/**
 * \param output_info	The output stream information.
 * \param aspectratio1	The divident of the aspectratio.
 * \param aspectratio2	The divisor of the aspectratio.
 *
 * Sets the aspect ratio for output streams.
 * Only has a meaning for \a CVL_IO_STREAM_Y4M streams.
 */
void cvl_io_info_set_aspectratio(cvl_io_info_t *output_info, int aspectratio1, int aspectratio2)
{
    cvl_assert(output_info != NULL);

    output_info->_aspectratio1 = aspectratio1;
    output_info->_aspectratio2 = aspectratio2;
}

/**
 * \param output_info	The output stream information.
 * \param input_info	The input stream information.
 * 
 * Automatically get output information:
 * the values in \a output_info that were not filled in will be automatically
 * taken from \a input_info when they are needed. This saves some
 * cvl_io_info_set_* calls.
 */
void cvl_io_info_link_output_to_input(cvl_io_info_t *output_info, const cvl_io_info_t *input_info)
{
    cvl_assert(output_info != NULL);
    cvl_assert(input_info != NULL);

    output_info->_input_info = input_info;
}

/**
 * \param f		The input stream.
 * \param input_info	The input information.
 * \return		The EOF status of the input stream.
 *
 * Gets EOF status of input stream.
 */
bool cvl_io_eof(FILE *f, const cvl_io_info_t *input_info UNUSED)
{
    cvl_assert(f != NULL);

    int c;

    if ((c = fgetc(f)) == EOF)
    {
	return (!ferror(f));
    }
    else
    {
	if (ungetc(c, f) == EOF)
	{
	    cvl_msg_err("%s: ungetc() failed!", __func__);
	}
	return false;
    } 
}

/**
 * \param f		The input stream.
 * \param input_info	Structure to fill in the information.
 *
 * Gets information about an input stream.
 * This function is normally called automatically by cvl_io_read().
 */
bool cvl_io_read_input_info(FILE *f, cvl_io_info_t *input_info)
{
    cvl_assert(f != NULL);
    cvl_assert(input_info != NULL);

    int c;
    
    if ((c = fgetc(f)) == EOF || ungetc(c, f) == EOF)
    {
	cvl_msg_err("unexpected EOF or input error");
	return false;
    }
    if (c == 'Y')
    {
	input_info->_stream_type = CVL_IO_STREAM_Y4M;
	return cvl_io_read_input_info_yuv(f, input_info);
    }
    else if (c == 'P')
    {
	input_info->_stream_type = CVL_IO_STREAM_PNM;
	return cvl_io_read_input_info_pnm(f, input_info);
    }
    else
    {
	cvl_msg_err("input is neither YUV4MPEG2 nor NetPBM");
	return false;
    }
}

/** 
 * \param f		The input stream.
 * \param input_info	The input information.
 * \param frame		The frame will be returned here.
 * \return		Success or error.
 * 
 * Reads a frame from an input stream.
 * \a frame will be NULL when this function fails.
 */
bool cvl_io_read(FILE *f, cvl_io_info_t *input_info, cvl_frame_t **frame)
{
    cvl_assert(f != NULL);
    cvl_assert(input_info != NULL);
    cvl_assert(frame != NULL);

    bool success;
    
    if (!input_info->_input_info_known && !cvl_io_read_input_info(f, input_info))
    {
	success = false;
    }
    else
    {
	if (input_info->_stream_type == CVL_IO_STREAM_Y4M)
	{
	    success = cvl_io_read_frame_yuv(f, input_info, frame);
	}
	else
	{
	    success = cvl_io_read_frame_pnm(f, input_info, frame);
	}
    }
    if (!success)
    {
	*frame = NULL;
    }
    return success;
}


/** 
 * \param f		The output stream.
 * \param output_info	The output information.
 * \param frame		The frame.
 * \return		Success or error.
 *
 * Writes a frame to an output stream.
 */
bool cvl_io_write(FILE *f, cvl_io_info_t *output_info, const cvl_frame_t *frame)
{
    cvl_assert(f != NULL);
    cvl_assert(output_info != NULL);
    cvl_assert(frame != NULL);

    if (!output_info->_output_info_written)
    {
	if (output_info->_input_info)
	{
	    if (output_info->_stream_type == INT_MAX)
	    {
		output_info->_stream_type = output_info->_input_info->_stream_type;
	    }
	    if (output_info->_width < 1 || output_info->_height < 1)
	    {
		output_info->_width = output_info->_input_info->_width;
		output_info->_height = output_info->_input_info->_height;
	    }
	    if (output_info->_chroma == CVL_Y4M_CHROMA_UNKNOWN)
	    {
		output_info->_chroma = output_info->_input_info->_chroma;
		if (output_info->_chroma == CVL_Y4M_CHROMA_UNKNOWN)
		{
		    /* set a default chroma subsampling */
		    output_info->_chroma = CVL_Y4M_CHROMA_420JPEG;
		}
	    }
	    if (output_info->_interlacing == CVL_Y4M_INTERLACING_UNKNOWN)
	    {
		output_info->_interlacing = output_info->_input_info->_interlacing;
	    }
	    if (output_info->_framerate1 == 0 && output_info->_framerate2 == 0)
	    {
		output_info->_framerate1 = output_info->_input_info->_framerate1;
		output_info->_framerate2 = output_info->_input_info->_framerate2;
	    }
	    if (output_info->_aspectratio1 == 0 && output_info->_aspectratio2 == 0)
	    {
		output_info->_aspectratio1 = output_info->_input_info->_aspectratio1;
		output_info->_aspectratio2 = output_info->_input_info->_aspectratio2;
	    }
	}
	if (output_info->_stream_type == CVL_IO_STREAM_Y4M)
	{
	    if (!cvl_io_write_output_info_yuv(f, output_info))
	    {
		return false;
	    }
	}
	else
	{
	    if (!cvl_io_write_output_info_pnm(f, output_info))
	    {
		return false;
	    }
	}
    }
    if (output_info->_stream_type == CVL_IO_STREAM_Y4M)
    {
	return cvl_io_write_frame_yuv(f, output_info, frame);
    }
    else
    {
	return cvl_io_write_frame_pnm(f, output_info, frame);
    }
}


/**
 * \param f		The stream to read the frame from.
 * \param frame		The frame.
 * \return		Success or error.
 *
 * Reads a frame from a stream in NetPBM format.
 */
bool cvl_io_read_pnm(FILE *f, cvl_frame_t **frame)
{
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);

    cvl_io_info_t *input_info;
    bool e;
    
    input_info = cvl_io_info_new();
    e = cvl_io_read(f, input_info, frame);
    cvl_io_info_free(input_info);
    return e;
}

/**
 * \param f		The stream to write the frame to.
 * \param frame		The frame.
 * \return		Success or error.
 *
 * Writes a frame to a stream in NetPBM format.
 */
bool cvl_io_write_pnm(FILE *f, const cvl_frame_t *frame)
{
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);

    cvl_io_info_t *output_info;
    bool e;
    
    output_info = cvl_io_info_new();
    cvl_io_info_set_stream_type(output_info, CVL_IO_STREAM_PNM);
    e = cvl_io_write(f, output_info, frame);
    cvl_io_info_free(output_info);
    return e;
}

/**
 * \param filename	The file to read the frame from.
 * \param frame		The frame.
 * \return		Success or error.
 *
 * Loads a frame from a file in NetPBM format.
 */
bool cvl_io_load_pnm(const char *filename, cvl_frame_t **frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);

    FILE *f;
    bool ret;
    
    if (!(f = fopen(filename, "r")))
    {
	cvl_msg_err("cannot open %s: %s", filename, strerror(errno));
	return false;
    }
    ret = cvl_io_read_pnm(f, frame);
    fclose(f);
    return ret;
}

/**
 * \param filename	The file to write the frame to.
 * \param frame		The frame.
 * \return		Success or error.
 *
 * Saves a frame to a file in NetPBM format.
 */
bool cvl_io_save_pnm(const char *filename, const cvl_frame_t *frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);

    FILE *f;
    bool ret;
    
    if (!(f = fopen(filename, "w")))
    {
	cvl_msg_err("cannot open %s: %s", filename, strerror(errno));
	return false;
    }
    ret = cvl_io_write_pnm(f, frame);
    fclose(f);
    return ret;
}
