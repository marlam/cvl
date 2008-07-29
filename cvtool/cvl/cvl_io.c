/*
 * cvl_io.c
 * 
 * This file is part of CVL, a computer vision library.
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

/** 
 * \file cvl_io.h
 * \brief Read and write frames.
 *
 * Read and write frames.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <GL/glew.h>

#include "cvl_intern.h"
#include "cvl/cvl_taglist.h"
#include "cvl/cvl_frame.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_io.h"


/**
 * \typedef cvl_stream_type_t
 * Type of a stream or file.
 */
/** \var CVL_PNM
 * A NetPBM PNM file or stream (*.pbm, *.pgm, *.ppm, *.pam). */
/** \var CVL_PFS
 * A PFS file or stream (*.pfs) */

/*
 * PNM input/output
 */

// Skip whitespace and optional comments.
static bool cvl_pnm_skip(FILE *f)
{
    int c;
    bool in_comment = false;

    for (;;)
    {
     	c = fgetc(f);
	if (c == EOF)
	{
	    return !ferror(f);
	}
	else
	{
	    if (in_comment)
	    {
		if (c == '\n')
		    in_comment = false;
	    }
	    else
	    {
		if (c == '#')
		    in_comment = true;
		else if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n'))
		    return !(ungetc(c, f) == EOF);
	    }
	}
    }
}

/**
 * \param f		The stream.
 * \param frame		Storage space for the frame.
 *
 * Reads a PNM frame (*.pbm, *.pgm, *.ppm, *.pam) from the stream \a f.
 */
void cvl_read_pnm(FILE *f, cvl_frame_t **frame)
{
    if (frame)
	*frame = NULL;
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    const char errmsg[] = "Cannot read PNM frame";
    typedef enum { PBM, PGM, RG, PPM, RGBA } subformat_t;
    subformat_t subformat;
    int width, height, size, maxval;
    int c;
    
    if ((c = fgetc(f)) != 'P')
    {
	if (c != EOF)
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "not a PNM file");
	}
	else if (ferror(f))
	{
	    cvl_error_set(CVL_ERROR_IO, "%s: %s", errmsg, "input error");
	}
	// else EOF
	return;
    }
    c = fgetc(f);
    if (c == '4')
    {
	subformat = PBM;
	maxval = 1;
	if (!cvl_pnm_skip(f)
		|| fscanf(f, "%d", &width) != 1 || width < 1 || !cvl_pnm_skip(f)
		|| fscanf(f, "%d", &height) != 1 || height < 1 || fgetc(f) == EOF)
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "PBM header invalid");
	    return;
	}
    }
    else if (c == '5')
    {
	subformat = PGM;
	if (!cvl_pnm_skip(f)
		|| fscanf(f, "%d", &width) != 1 || width < 1 || !cvl_pnm_skip(f)
		|| fscanf(f, "%d", &height) != 1 || height < 1 || !cvl_pnm_skip(f)
		|| fscanf(f, "%d", &maxval) != 1 || maxval < 1 || maxval > 65535 || fgetc(f) == EOF)
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "PGM header invalid");
	    return;
	}
    }
    else if (c == '6')
    {
	subformat = PPM;
	if (!cvl_pnm_skip(f)
		|| fscanf(f, "%d", &width) != 1 || width < 1 || !cvl_pnm_skip(f)
		|| fscanf(f, "%d", &height) != 1 || height < 1 || !cvl_pnm_skip(f)
		|| fscanf(f, "%d", &maxval) != 1 || maxval < 1 || maxval > 65535 || fgetc(f) == EOF)
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "PPM header invalid");
	    return;
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
	    if (!cvl_pnm_skip(f) || (c = fgetc(f)) == EOF || ungetc(c, f) == EOF)
	    {
		cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "invalid PAM header tag");
		return;
	    }
	    switch (c)
	    {
		case '#':
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		    ok = cvl_pnm_skip(f);
		    break;
		    
		case 'W':
		    ok = (fscanf(f, "WIDTH %d", &width) == 1 && width > 0 && cvl_pnm_skip(f));
		    break;
		    
		case 'H':
		    ok = (fscanf(f, "HEIGHT %d", &height) == 1 && height > 0 && cvl_pnm_skip(f));
		    break;
		    
		case 'D':
		    ok = (fscanf(f, "DEPTH %d", &depth) == 1 && depth > 0 && cvl_pnm_skip(f));
		    break;
		    
		case 'M':
		    ok = (fscanf(f, "MAXVAL %d", &maxval) == 1 
			    && maxval > 0 && maxval < 65536 && cvl_pnm_skip(f));
		    break;
		    
		case 'T':
		    ok = (fscanf(f, "TUPLTYPE %13s", tupletype) == 1 && cvl_pnm_skip(f));
		    break;
		    
		case 'E':
		    ok = (fgetc(f) == 'E' && fgetc(f) == 'N' && fgetc(f) == 'D' 
			    && fgetc(f) == 'H' && fgetc(f) == 'D' && fgetc(f) == 'R' && fgetc(f) == '\n');
		    header_end = true;
		    break;
		    
		default:
		    ok = false;
		    break;
	    }
	}
	if (!header_end || !ok)
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "PAM header invalid");
	    return;
	}
	if (strcmp(tupletype, "BLACKANDWHITE") == 0)
	{
	    subformat = PBM;
	}
	else if (depth == 1)
	{
	    subformat = PGM;
	}
	else if (depth == 2)
	{
	    subformat = RG;
	}
	else if (depth == 3)
	{
	    subformat = PPM;
	}
	else if (depth == 4)
	{
	    subformat = RGBA;
	}
	else
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "unknown tuple type in PAM header");
	    return;
	}	    
    }
    else
    {
	cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, ferror(f) ? "input error" : "unknown PNM subtype");
	return;
    }
    
    cvl_context_t *ctx = cvl_context();
    if (width > ctx->cvl_gl_max_tex_size || height > ctx->cvl_gl_max_tex_size)
    {
	cvl_error_set(CVL_ERROR_GL, "%s: size %dx%d is too large: OpenGL implementation limit is %dx%d", 
		errmsg, width, height, ctx->cvl_gl_max_tex_size, ctx->cvl_gl_max_tex_size);
	return;
    }

    *frame = cvl_frame_new(width, height, 
	    subformat == PBM ? 1 : subformat == PGM ? 1: subformat == RG ? 2 : subformat == PPM ? 3 : 4,
	    subformat == PBM ? CVL_LUM : subformat == PGM ? CVL_LUM : subformat == PPM ? CVL_RGB : CVL_UNKNOWN,
	    maxval < 256 ? CVL_UINT8 : CVL_FLOAT, CVL_MEM);
    size = width * height;
    if (subformat == PBM)
    {
	size_t rawsize = height * ((width - 1) / 8 + 1);
	size_t rawindex = 0;
	uint8_t *pbmdata;
	uint8_t *ptr = cvl_frame_pointer(*frame);

	if (!(pbmdata = malloc(rawsize * sizeof(uint8_t))))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    cvl_frame_free(*frame);
	    *frame = NULL;
	    return;
	}
	if (fread(pbmdata, sizeof(uint8_t), rawsize, f) != rawsize)
	{
	    cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "EOF or input error in PBM data");
	    cvl_frame_free(*frame);
	    *frame = NULL;
	    free(pbmdata);
	    return;
	}
	for (int y = 0; y < height; y++)
	{
	    for (int x = 0; x < width; x += 8)
	    {
		uint8_t d = pbmdata[rawindex++];
		ptr[y * width + x] = (d & 0x80 ? 0 : 255);
		if (x + 1 < width)
		    ptr[y * width + x + 1] = (d & 0x40 ? 0 : 255);
		if (x + 2 < width)
		    ptr[y * width + x + 2] = (d & 0x20 ? 0 : 255);
		if (x + 3 < width)
		    ptr[y * width + x + 3] = (d & 0x10 ? 0 : 255);
		if (x + 4 < width)
		    ptr[y * width + x + 4] = (d & 0x08 ? 0 : 255);
		if (x + 5 < width)
		    ptr[y * width + x + 5] = (d & 0x04 ? 0 : 255);
		if (x + 6 < width)
		    ptr[y * width + x + 6] = (d & 0x02 ? 0 : 255);
		if (x + 7 < width)
		    ptr[y * width + x + 7] = (d & 0x01 ? 0 : 255);
	    }
	}
	free(pbmdata);
    }
    else if (subformat == PGM)
    {
	size_t rawsize = size * (maxval < 256 ? 1 : 2) * sizeof(uint8_t);
	if (maxval < 256)
	{
	    if (fread(cvl_frame_pointer(*frame), rawsize, 1, f) != 1)
	    {
		cvl_frame_free(*frame);
		*frame = NULL;
		cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "EOF or input error in PGM data");
		return;
	    }
	}
	else
	{
	    uint8_t *pgmdata;
	    float *ptr = cvl_frame_pointer(*frame);
	    if (!(pgmdata = malloc(rawsize)))
	    {
		cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
		cvl_frame_free(*frame);
		*frame = NULL;
		return;
	    }
	    if (fread(pgmdata, rawsize, 1, f) != 1)
	    {
		cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "EOF or input error in PGM data");
		cvl_frame_free(*frame);
		*frame = NULL;
		free(pgmdata);
		return;
	    }
	    for (size_t i = 0; i < (size_t)size; i++)
	    {
		ptr[i] = (float)(((int)pgmdata[i * 2] << 8) | (int)pgmdata[i * 2 + 1]) / 65535.0f;
	    }
	    free(pgmdata);
	}
    }
    else if (subformat == RG)
    {
	size_t rawsize = size * (maxval < 256 ? 1 : 2) * 2 * sizeof(uint8_t);
	uint8_t *rgdata;
    	if (!(rgdata = malloc(rawsize)))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    cvl_frame_free(*frame);
	    *frame = NULL;
	    return;
	}
	if (fread(rgdata, rawsize, 1, f) != 1)
	{
	    cvl_frame_free(*frame);
	    *frame = NULL;
	    cvl_error_set(CVL_ERROR_DATA, "%s: EOF or input error in RG data", errmsg);
	    free(rgdata);
	    return;
	}
	if (maxval < 256)
	{
	    uint8_t *ptr = cvl_frame_pointer(*frame);
	    for (size_t i = 0; i < (size_t)size; i++)
	    {
		ptr[4 * i + 0] = rgdata[2 * i + 0];
		ptr[4 * i + 1] = rgdata[2 * i + 1];
		ptr[4 * i + 2] = 0;
		ptr[4 * i + 3] = 0;
	    }
	}
	else
	{
	    float *ptr = cvl_frame_pointer(*frame);
	    for (size_t i = 0; i < (size_t)size; i++)
	    {
		ptr[4 * i + 0] = (float)(((int)rgdata[(2 * i + 0) * 2 + 0] << 8) | (int)rgdata[(2 * i + 0) * 2 + 1]) / 65535.0f;
		ptr[4 * i + 1] = (float)(((int)rgdata[(2 * i + 1) * 2 + 0] << 8) | (int)rgdata[(2 * i + 1) * 2 + 1]) / 65535.0f;
		ptr[4 * i + 2] = 0.0f;
		ptr[4 * i + 3] = 0.0f;
	    }
	}
	free(rgdata);
	cvl_frame_set_channel_name(*frame, 0, "R");
	cvl_frame_set_channel_name(*frame, 1, "G");
    }
    else if (subformat == PPM)
    {
	size_t rawsize = size * (maxval < 256 ? 1 : 2) * 3 * sizeof(uint8_t);
	if (maxval < 256)
	{
	    if (fread(cvl_frame_pointer(*frame), rawsize, 1, f) != 1)
	    {
		cvl_frame_free(*frame);
		*frame = NULL;
		cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "EOF or input error in PPM data");
		return;
	    }
	}
	else
	{
	    uint8_t *ppmdata;
	    float *ptr = cvl_frame_pointer(*frame);
	    if (!(ppmdata = malloc(rawsize)))
	    {
		cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
		cvl_frame_free(*frame);
		*frame = NULL;
		return;
	    }
	    if (fread(ppmdata, rawsize, 1, f) != 1)
	    {
		cvl_frame_free(*frame);
		*frame = NULL;
		cvl_error_set(CVL_ERROR_DATA, "%s: EOF or input error in PPM data", errmsg);
		free(ppmdata);
		return;
	    }
	    for (size_t i = 0; i < (size_t)size; i++)
	    {
		ptr[3 * i + 0] = (float)(((int)ppmdata[(3 * i + 0) * 2 + 0] << 8) | (int)ppmdata[(3 * i + 0) * 2 + 1]) / 65535.0f;
		ptr[3 * i + 1] = (float)(((int)ppmdata[(3 * i + 1) * 2 + 0] << 8) | (int)ppmdata[(3 * i + 1) * 2 + 1]) / 65535.0f;
		ptr[3 * i + 2] = (float)(((int)ppmdata[(3 * i + 2) * 2 + 0] << 8) | (int)ppmdata[(3 * i + 2) * 2 + 1]) / 65535.0f;
	    }
	    free(ppmdata);
	}
    }
    else if (subformat == RGBA)
    {
	size_t rawsize = size * (maxval < 256 ? 1 : 2) * 4 * sizeof(uint8_t);
	if (maxval < 256)
	{
	    if (fread(cvl_frame_pointer(*frame), rawsize, 1, f) != 1)
	    {
		cvl_frame_free(*frame);
		*frame = NULL;
		cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "EOF or input error in RGBA data");
		return;
	    }
	}
	else
	{
	    uint8_t *rgbadata;
	    float *ptr = cvl_frame_pointer(*frame);
	    if (!(rgbadata = malloc(rawsize)))
	    {
		cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
		cvl_frame_free(*frame);
		*frame = NULL;
		return;
	    }
	    if (fread(rgbadata, rawsize, 1, f) != 1)
	    {
		cvl_frame_free(*frame);
		*frame = NULL;
		cvl_error_set(CVL_ERROR_DATA, "%s: EOF or input error in RGBA data", errmsg);
		free(rgbadata);
		return;
	    }
	    for (size_t i = 0; i < (size_t)size; i++)
	    {
		ptr[4 * i + 0] = (float)(((int)rgbadata[(4 * i + 0) * 2 + 0] << 8) | (int)rgbadata[(4 * i + 0) * 2 + 1]) / 65535.0f;
		ptr[4 * i + 1] = (float)(((int)rgbadata[(4 * i + 1) * 2 + 0] << 8) | (int)rgbadata[(4 * i + 1) * 2 + 1]) / 65535.0f;
		ptr[4 * i + 2] = (float)(((int)rgbadata[(4 * i + 2) * 2 + 0] << 8) | (int)rgbadata[(4 * i + 2) * 2 + 1]) / 65535.0f;
		ptr[4 * i + 3] = (float)(((int)rgbadata[(4 * i + 3) * 2 + 0] << 8) | (int)rgbadata[(4 * i + 3) * 2 + 1]) / 65535.0f;
	    }
	    free(rgbadata);
	}
	cvl_frame_set_channel_name(*frame, 0, "R");
	cvl_frame_set_channel_name(*frame, 1, "G");
	cvl_frame_set_channel_name(*frame, 2, "B");
	cvl_frame_set_channel_name(*frame, 3, "A");
    }
}

/**
 * \param f		The stream.
 * \param frame		The frame.
 *
 * Writes the frame \a frame to the stream \a f in PNM format. The PNM subformat
 * is PGM for frames with a single channel, PPM for frames with two channels,
 * and PAM for all other frames.
 * Information loss will occur when saving #CVL_FLOAT or #CVL_FLOAT16 frames.
 */
void cvl_write_pnm(FILE *f, cvl_frame_t *frame)
{
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    bool error;
    size_t size = cvl_frame_width(frame) * cvl_frame_height(frame);

    cvl_frame_t *out;
    if (cvl_frame_format(frame) == CVL_LUM 
	    || cvl_frame_format(frame) == CVL_RGB
	    || cvl_frame_format(frame) == CVL_UNKNOWN)
    {
	out = frame;
    }
    else
    {
	out = cvl_frame_new_tpl(frame);
	cvl_frame_set_format(out, CVL_RGB);
	cvl_convert_format(out, frame);
    }
    
    if (cvl_frame_type(out) == CVL_UINT8)
    {
	uint8_t *p = cvl_frame_pointer(out);
	if (cvl_frame_format(out) == CVL_UNKNOWN)
	{
	    if (cvl_frame_channels(out) == 1)
	    {
		uint8_t *np;
		if (!(np = malloc(1 * size * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
	    	    np[i] = p[4 * i];
		}
		error = (fprintf(f, "P5\n%d %d\n255\n", cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 1 * size * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	    else if (cvl_frame_channels(out) == 2)
	    {
		uint8_t *np;
		if (!(np = malloc(2 * size * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
	    	    np[2 * i + 0] = p[4 * i + 0];
    		    np[2 * i + 1] = p[4 * i + 1];
		}
		error = (fprintf(f, "P7\nWIDTH %d\nHEIGHT %d\nDEPTH 2\nMAXVAL 255\nTUPLTYPE RG\nENDHDR\n", 
			    cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 2 * size * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	    else if (cvl_frame_channels(out) == 3)
	    {
		uint8_t *np;
		if (!(np = malloc(3 * size * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
	    	    np[3 * i + 0] = p[4 * i + 0];
    		    np[3 * i + 1] = p[4 * i + 1];
		    np[3 * i + 2] = p[4 * i + 2];
		}
		error = (fprintf(f, "P6\n%d %d\n255\n", cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 3 * size * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	    else
	    {
		error = (fprintf(f, "P7\nWIDTH %d\nHEIGHT %d\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGBA\nENDHDR\n", 
			    cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(p, 4 * size * sizeof(uint8_t), 1, f) != 1);
	    }
	}
	else
	{
	    error = (fprintf(f, "P%d\n%d %d\n255\n", 
	    		cvl_frame_format(out) == CVL_LUM ? 5 : 6,
	    		cvl_frame_width(out), cvl_frame_height(out)) < 0
	    	    || fwrite(p, (cvl_frame_format(frame) == CVL_LUM ? 1 : 3) * size * sizeof(uint8_t), 1, f) != 1);
	}
    }
    else
    {
	float *fp = cvl_frame_pointer(out);
	if (cvl_frame_format(out) == CVL_UNKNOWN)
	{
	    if (cvl_frame_channels(out) == 1)
	    {
		uint8_t *np;
		if (!(np = malloc(1 * size * 2 * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
		    unsigned int v = fp[4 * i];
		    np[2 * i + 0] = (v >> 8);
		    np[2 * i + 1] = (v & 0xff);
		}
		error = (fprintf(f, "P5\n%d %d\n65535\n", cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 1 * size * 2 * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	    else if (cvl_frame_channels(out) == 2)
	    {
		uint8_t *np;
		if (!(np = malloc(2 * size * 2 * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
		    unsigned int v;
		    v = fp[4 * i + 0];
		    np[4 * i + 0] = (v >> 8);
		    np[4 * i + 1] = (v & 0xff);
		    v = fp[4 * i + 1];
		    np[4 * i + 2] = (v >> 8);
		    np[4 * i + 3] = (v & 0xff);
		}
		error = (fprintf(f, "P7\nWIDTH %d\nHEIGHT %d\nDEPTH 2\nMAXVAL 65535\nTUPLTYPE RG\nENDHDR\n", 
			    cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 2 * size * 2 * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	    else if (cvl_frame_channels(out) <= 3)
	    {
		uint8_t *np;
		if (!(np = malloc(3 * size * 2 * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
		    unsigned int v;
		    v = fp[4 * i + 0];
		    np[6 * i + 0] = (v >> 8);
		    np[6 * i + 1] = (v & 0xff);
		    v = fp[4 * i + 1];
		    np[6 * i + 2] = (v >> 8);
		    np[6 * i + 3] = (v & 0xff);
		    v = fp[4 * i + 2];
	    	    np[6 * i + 4] = (v >> 8);
    		    np[6 * i + 5] = (v & 0xff);
		}
		error = (fprintf(f, "P6\n%d %d\n65535\n", cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 3 * size * 2 * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	    else
	    {
		uint8_t *np;
		if (!(np = malloc(4 * size * 2 * sizeof(uint8_t))))
		{
		    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		    if (out != frame)
		    {
			cvl_frame_free(out);
		    }
		    return;
		}
		for (size_t i = 0; i < size; i++)
		{
		    unsigned int v;
		    v = fp[4 * i + 0];
		    np[8 * i + 0] = (v >> 8);
		    np[8 * i + 1] = (v & 0xff);
		    v = fp[4 * i + 1];
		    np[8 * i + 2] = (v >> 8);
		    np[8 * i + 3] = (v & 0xff);
		    v = fp[4 * i + 2];
		    np[8 * i + 4] = (v >> 8);
		    np[8 * i + 5] = (v & 0xff);
		    v = fp[4 * i + 3];
		    np[8 * i + 6] = (v >> 8);
		    np[8 * i + 7] = (v & 0xff);
		}
		error = (fprintf(f, "P7\nWIDTH %d\nHEIGHT %d\nDEPTH 4\nMAXVAL 65535\nTUPLTYPE RGBA\nENDHDR\n", 
			    cvl_frame_width(out), cvl_frame_height(out)) < 0
			|| fwrite(np, 4 * size * 2 * sizeof(uint8_t), 1, f) != 1);
		free(np);
	    }
	}
	else
	{
	    int components = (cvl_frame_format(frame) == CVL_LUM ? 1 : 3);
	    uint8_t *np;
	    if (!(np = malloc(size * components * 2 * sizeof(uint8_t))))
	    {
		cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
		if (out != frame)
	    	{
    		    cvl_frame_free(out);
		}
		return;
	    }
	    for (size_t i = 0; i < components * size; i++)
	    {
	    	unsigned int v = fp[i] * 65535.0f;
		np[2 * i + 0] = (v >> 8);
		np[2 * i + 1] = (v & 0xff);
	    }
	    error = (fprintf(f, "P%d\n%d %d\n65535\n", 
			cvl_frame_format(out) == CVL_LUM ? 5 : 6,
			cvl_frame_width(out), cvl_frame_height(out)) < 0
		    || fwrite(np, components * size * 2 * sizeof(uint8_t), 1, f) != 1);
	    free(np);
	}
    }

    if (out != frame)
    {
	cvl_frame_free(out);
    }

    if (error)
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot write PNM frame: output error");
    }
}

/**
 * \param filename	The file name.
 * \param frame		Storage space for the frame.
 *
 * Reads a frame from the file \a filename in PNM format.
 */
void cvl_load_pnm(const char *filename, cvl_frame_t **frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    FILE *f;
    if (!(f = fopen(filename, "r")))
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot open %s: %s", filename, strerror(errno));
	return;
    }
    cvl_read_pnm(f, frame);
    fclose(f);
}

/**
 * \param filename	The file name.
 * \param frame		The frame.
 *
 * Writes the frame \a frame to the file \a filename as a PNM frame.
 */
void cvl_save_pnm(const char *filename, cvl_frame_t *frame)
{
    if (cvl_error())
	return;

    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);

    cvl_save(filename, CVL_PNM, frame);
}


/*
 * PFS
 */

typedef enum 
{ 
    CVL_PFS_SIZE,
    CVL_PFS_CHANNELS,
    CVL_PFS_INPUT_ERROR, 
    CVL_PFS_INVALID_DATA_ERROR, 
    CVL_PFS_UNSUPPORTED_FEATURE_ERROR,
    CVL_PFS_EOF_IN_DATA,
    CVL_PFS_ENOMEM
} cvl_read_pfs_errtype_t;

// Helper to read a PFS tag line
static bool cvl_read_pfs_tagline(FILE *f, char *name, char *value, int maxlen, cvl_read_pfs_errtype_t *errtype)
{
    int c;
    int namelen = 0;
    int valuelen = 0;
    bool in_value = false;

    for (;;)
    {
	*errtype = CVL_PFS_INVALID_DATA_ERROR;
	c = fgetc(f);
	if (c == EOF && ferror(f))
	{
	    *errtype = CVL_PFS_INPUT_ERROR;
	    return false;
	}
	else if (c == EOF && !ferror(f))
	{
	    return false;
	}
	else if (c == '\n')
	{
	    if (!in_value)
		return false;
	    value[valuelen] = '\0';
	    return true;
	}
	else if (c == '=')
	{
	    if (!in_value)
	    {
		name[namelen] = '\0';
		in_value = true;
	    }
	}
	else if (!iscntrl(c) && (in_value == true || c != ':'))
	{
	    if (in_value)
	    {
		if (valuelen == maxlen)
		    return false;
		value[valuelen++] = c;
	    }
	    else
	    {
		if (namelen == maxlen)
		    return false;
		name[namelen++] = c;
	    }
	}
	else
	{
	    return false;
	}
    }
}

/**
 * \param f		The stream.
 * \param frame		Storage space for the frame.
 *
 * Reads a PFS frame (*.pfs) from the stream \a f.
 */
void cvl_read_pfs(FILE *f, cvl_frame_t **frame)
{
    if (frame)
	*frame = NULL;
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    const char errmsg[] = "Cannot read PFS frame";
    cvl_read_pfs_errtype_t errtype;
    int width, height, channel_count, frame_tag_count, channel_tag_count;
    size_t size;
    const int max_tag_len = 1023;
    char channel_name[4][max_tag_len + 1];
    char name[max_tag_len + 1];
    char value[max_tag_len + 1];
    cvl_taglist_t *taglist = NULL;
    char endh_str[4];
    float *channel[4] = { NULL, NULL, NULL, NULL };
    cvl_context_t *ctx = cvl_context();
    int c;
    size_t l;

    if ((c = fgetc(f)) != 'P')
    {
	if (c != EOF)
	{
	    errtype = CVL_PFS_INVALID_DATA_ERROR;
	    goto error_exit;
	}
	else if (ferror(f))
	{
	    errtype = CVL_PFS_INPUT_ERROR;
	    goto error_exit;
	}
	else // EOF
	{
	    return;
	}
    }
    if (fscanf(f, "FS1\n%d %d\n%d\n%d\n", &width, &height, &channel_count, &frame_tag_count) != 4
	    || width < 1 || height < 1 || channel_count < 1 || frame_tag_count < 0)
    {
	errtype = (ferror(f) ? CVL_PFS_INPUT_ERROR : CVL_PFS_INVALID_DATA_ERROR);
	goto error_exit;
    }
    if (width > ctx->cvl_gl_max_tex_size || height > ctx->cvl_gl_max_tex_size)
    {
	errtype = CVL_PFS_SIZE;
	goto error_exit;
    }
    if (channel_count > 4)
    {
	errtype = CVL_PFS_CHANNELS;
	goto error_exit;
    }
    size = width * height;
    taglist = cvl_taglist_new();
    for (int i = 0; i < frame_tag_count; i++)
    {
	if (!cvl_read_pfs_tagline(f, name, value, max_tag_len, &errtype))
	    goto error_exit;
	cvl_taglist_put(taglist, name, value);
    }
    for (int i = 0; i < channel_count; i++)
    {
	if (!(fgets(channel_name[i], max_tag_len + 1, f))
		|| (l = strlen(channel_name[i])) < 1
		|| channel_name[i][l - 1] != '\n')
	{
	    errtype = (ferror(f) ? CVL_PFS_INPUT_ERROR : CVL_PFS_INVALID_DATA_ERROR);
	    goto error_exit;
	}
	channel_name[i][l - 1] = '\0';
	if (fscanf(f, "%d\n", &channel_tag_count) != 1 || channel_tag_count < 0)
	{
	    errtype = (ferror(f) ? CVL_PFS_INPUT_ERROR : CVL_PFS_INVALID_DATA_ERROR);
	    goto error_exit;
	}
	// Ignore channel tags for now
	for (int j = 0; j < channel_tag_count; j++)
	    if (!cvl_read_pfs_tagline(f, name, value, max_tag_len, &errtype))
		goto error_exit;
    }
    if (fread(endh_str, sizeof(char), 4, f) != 4 || strncmp(endh_str, "ENDH", 4) != 0)
    {
	errtype = (ferror(f) ? CVL_PFS_INPUT_ERROR : CVL_PFS_INVALID_DATA_ERROR);
	goto error_exit;
    }
    if (channel_count == 1)
    {
	*frame = cvl_frame_new(width, height, 1, CVL_LUM, CVL_FLOAT, CVL_MEM);
	float *p = cvl_frame_pointer(*frame);
	if (!p)
    	{
	    errtype = CVL_PFS_ENOMEM;
	    goto error_exit;
	}
	if (fread(p, sizeof(float), size, f) != size)
     	{
	    errtype = (ferror(f) ? CVL_PFS_INPUT_ERROR : CVL_PFS_EOF_IN_DATA);
	    goto error_exit;
	}
	if (strcmp(channel_name[0], "Y") != 0)
	{
	    cvl_frame_set_format(*frame, CVL_UNKNOWN);
	}
    }
    else
    {
	for (int i = 0; i < channel_count; i++)
	{
	    if (!(channel[i] = malloc(size * sizeof(float))))
	    {
		errtype = CVL_PFS_ENOMEM;
		goto error_exit;
	    }
	    if (fread(channel[i], sizeof(float), size, f) != size)
	    {
		errtype = (ferror(f) ? CVL_PFS_INPUT_ERROR : CVL_PFS_EOF_IN_DATA);
		goto error_exit;
	    }
	}
	if (channel_count == 2)
	{
	    *frame = cvl_frame_new(width, height, 2, CVL_UNKNOWN, CVL_FLOAT, CVL_MEM);
	    float *p = cvl_frame_pointer(*frame);
	    if (!p)
	    {
		errtype = CVL_PFS_ENOMEM;
		goto error_exit;
	    }
	    for (size_t i = 0; i < size; i++)
	    {
		p[4 * i + 0] = channel[0][i];
		p[4 * i + 1] = channel[1][i];
		p[4 * i + 2] = 0.0f;
		p[4 * i + 3] = 0.0f;
	    }
	}
	else if (channel_count == 3)
	{
	    float *X, *Y, *Z;
	    X = (strcmp(channel_name[0], "X") == 0 ? channel[0] :
		    strcmp(channel_name[1], "X") == 0 ? channel[1] :
		    strcmp(channel_name[2], "X") == 0 ? channel[2] : NULL);
	    Y = (strcmp(channel_name[0], "Y") == 0 ? channel[0] :
		    strcmp(channel_name[1], "Y") == 0 ? channel[1] :
		    strcmp(channel_name[2], "Y") == 0 ? channel[2] : NULL);
	    Z = (strcmp(channel_name[0], "Z") == 0 ? channel[0] :
		    strcmp(channel_name[1], "Z") == 0 ? channel[1] :
		    strcmp(channel_name[2], "Z") == 0 ? channel[2] : NULL);
	    float *R, *G, *B;
	    R = (strcmp(channel_name[0], "R") == 0 ? channel[0] :
		    strcmp(channel_name[1], "R") == 0 ? channel[1] :
		    strcmp(channel_name[2], "R") == 0 ? channel[2] : NULL);
	    G = (strcmp(channel_name[0], "G") == 0 ? channel[0] :
		    strcmp(channel_name[1], "G") == 0 ? channel[1] :
		    strcmp(channel_name[2], "G") == 0 ? channel[2] : NULL);
	    B = (strcmp(channel_name[0], "B") == 0 ? channel[0] :
		    strcmp(channel_name[1], "B") == 0 ? channel[1] :
		    strcmp(channel_name[2], "B") == 0 ? channel[2] : NULL);
	    if (X && Y && Z)
	    {
		*frame = cvl_frame_new(width, height, 3, CVL_XYZ, CVL_FLOAT, CVL_MEM);
		float *p = cvl_frame_pointer(*frame);
		if (!p)
		{
		    errtype = CVL_PFS_ENOMEM;
		    goto error_exit;
		}
		for (size_t i = 0; i < size; i++)
		{
		    p[3 * i + 0] = X[i];
		    p[3 * i + 1] = Y[i];
		    p[3 * i + 2] = Z[i];
		}
	    }
	    else if (R && G && B)
	    {
		*frame = cvl_frame_new(width, height, 3, CVL_RGB, CVL_FLOAT, CVL_MEM);
		float *p = cvl_frame_pointer(*frame);
		if (!p)
		{
		    errtype = CVL_PFS_ENOMEM;
		    goto error_exit;
		}
		for (size_t i = 0; i < size; i++)
		{
		    p[3 * i + 0] = R[i];
		    p[3 * i + 1] = G[i];
		    p[3 * i + 2] = B[i];
		}
	    }
	    else
	    {
		*frame = cvl_frame_new(width, height, 3, CVL_UNKNOWN, CVL_FLOAT, CVL_MEM);
		float *p = cvl_frame_pointer(*frame);
		if (!p)
		{
		    errtype = CVL_PFS_ENOMEM;
		    goto error_exit;
		}
		for (size_t i = 0; i < size; i++)
		{
		    p[4 * i + 0] = channel[0][i];
		    p[4 * i + 1] = channel[1][i];
		    p[4 * i + 2] = channel[2][i];
		    p[4 * i + 3] = 0.0f;
		}
	    }
	}
	else
	{
	    *frame = cvl_frame_new(width, height, 4, CVL_UNKNOWN, CVL_FLOAT, CVL_MEM);
    	    float *p = cvl_frame_pointer(*frame);
	    if (!p)
	    {
		errtype = CVL_PFS_ENOMEM;
		goto error_exit;
	    }
	    for (size_t i = 0; i < size; i++)
	    {
		p[4 * i + 0] = channel[0][i];
		p[4 * i + 1] = channel[1][i];
		p[4 * i + 2] = channel[2][i];
		p[4 * i + 3] = channel[3][i];
	    }
	}
    }
    if (cvl_frame_format(*frame) == CVL_UNKNOWN)
    {
	for (int i = 0; i < cvl_frame_channels(*frame); i++)
	    cvl_frame_set_channel_name(*frame, i, channel_name[i]);
    }
    cvl_frame_set_taglist(*frame, taglist);
    free(channel[0]);
    free(channel[1]);
    free(channel[2]);
    free(channel[3]);

    return;

error_exit:
    if (errtype == CVL_PFS_SIZE)
	cvl_error_set(CVL_ERROR_GL, "%s: size %dx%d is too large: OpenGL implementation limit is %dx%d", 
		errmsg, width, height, ctx->cvl_gl_max_tex_size, ctx->cvl_gl_max_tex_size);
    else if (errtype == CVL_PFS_CHANNELS)
	cvl_error_set(CVL_ERROR_IO, "%s: %d channels are too much: OpenGL can only handle 4", errmsg, channel_count);
    else if (errtype == CVL_PFS_INPUT_ERROR)
	cvl_error_set(CVL_ERROR_IO, "%s: %s", errmsg, strerror(errno));
    else if (errtype == CVL_PFS_INVALID_DATA_ERROR)
	cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "invalid header");
    else if (errtype == CVL_PFS_EOF_IN_DATA)
	cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "incomplete data");
    else if (errtype == CVL_PFS_ENOMEM)
	cvl_error_set(CVL_ERROR_MEM, "%s: %s", errmsg, strerror(ENOMEM));
    else
	cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "unsupported data type");
    free(channel[0]);
    free(channel[1]);
    free(channel[2]);
    free(channel[3]);
    if (!*frame)
	cvl_taglist_free(taglist);
    cvl_frame_free(*frame);
    *frame = NULL;
    return;
}

/**
 * \param f		The stream.
 * \param frame		The frame.
 *
 * Writes the frame \a frame to the stream \a f in PFS format. 
 * For frames with format #CVL_LUM, only the Y channel is written.
 */
void cvl_write_pfs(FILE *f, cvl_frame_t *frame)
{
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    bool error;
    const char *name, *value;
    
    size_t size = cvl_frame_size(frame);
    int frame_tag_count = cvl_taglist_length(cvl_frame_taglist(frame));

    error = (fprintf(f, "PFS1\n%d %d\n%d\n%d\n", 
		cvl_frame_width(frame), cvl_frame_height(frame),
		cvl_frame_channels(frame), frame_tag_count) < 0);
    for (int i = 0; !error && i < frame_tag_count; i++)
    {
	cvl_taglist_get_i(cvl_frame_taglist(frame), i, &name, &value);
	error = (fprintf(f, "%s=%s\n", name, value) < 0);
    }
    if (cvl_frame_format(frame) == CVL_LUM)
    {
	error = (fprintf(f, "Y\n0\nENDH") < 0
		|| fwrite(cvl_frame_pointer(frame), size * sizeof(float), 1, f) != 1);
    }
    else
    {
	cvl_frame_t *outframe;
	float *p;

	if (cvl_frame_format(frame) == CVL_XYZ || cvl_frame_format(frame) == CVL_UNKNOWN)
	{
	    outframe = frame;
	}
	else
	{
	    outframe = cvl_frame_new_tpl(frame);
	    cvl_frame_set_format(outframe, CVL_XYZ);
	    cvl_convert_format(outframe, frame);
	}

	if (cvl_frame_format(outframe) == CVL_XYZ)
	{
	    error = (fprintf(f, "X\n0\nY\n0\nZ\n0\nENDH") < 0);
	}
	else
	{
	    error = false;
	    error = error || (fprintf(f, "%s\n0\n",
			cvl_frame_channel_name(outframe, 0) ? cvl_frame_channel_name(outframe, 0) : "CHANNEL0") < 0);
	    if (cvl_frame_channels(outframe) > 1)
		error = error || (fprintf(f, "%s\n0\n", 
			    cvl_frame_channel_name(outframe, 1) ? cvl_frame_channel_name(outframe, 1) : "CHANNEL1") < 0);
	    if (cvl_frame_channels(outframe) > 2)
		error = error || (fprintf(f, "%s\n0\n", 
			    cvl_frame_channel_name(outframe, 2) ? cvl_frame_channel_name(outframe, 2) : "CHANNEL2") < 0);
	    if (cvl_frame_channels(outframe) > 3)
		error = error || (fprintf(f, "%s\n0\n", 
			    cvl_frame_channel_name(outframe, 3) ? cvl_frame_channel_name(outframe, 3) : "CHANNEL3") < 0);
	    error = error || (fprintf(f, "ENDH") < 0);
	}
     	glBindTexture(GL_TEXTURE_2D, cvl_frame_texture(frame));
	if (!(p = malloc(size * sizeof(float))))
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(errno));
	    return;
	}
    	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, p);
	error = error || (fwrite(p, size * sizeof(float), 1, f) != 1);
	if (cvl_frame_channels(outframe) > 1)
	{
	    glGetTexImage(GL_TEXTURE_2D, 0, GL_GREEN, GL_FLOAT, p);
	    error = error || (fwrite(p, size * sizeof(float), 1, f) != 1);
	}
	if (cvl_frame_channels(outframe) > 2)
	{
	    glGetTexImage(GL_TEXTURE_2D, 0, GL_BLUE, GL_FLOAT, p);
	    error = error || (fwrite(p, size * sizeof(float), 1, f) != 1);
	}
	if (cvl_frame_channels(outframe) > 3)
	{
	    glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_FLOAT, p);
	    error = error || (fwrite(p, size * sizeof(float), 1, f) != 1);
	}
	free (p);
	
	if (outframe != frame)
	    cvl_frame_free(outframe);
    }
    if (error)
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot write PFS frame: output error");
    }
}

/**
 * \param filename	The file name.
 * \param frame		Storage space for the frame.
 *
 * Reads a frame from the file \a filename in PFS format.
 */
void cvl_load_pfs(const char *filename, cvl_frame_t **frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    FILE *f;
    if (!(f = fopen(filename, "r")))
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot open %s: %s", filename, strerror(errno));
	return;
    }
    cvl_read_pfs(f, frame);
    fclose(f);
}

/**
 * \param filename	The file name.
 * \param frame		The frame.
 *
 * Writes the frame \a frame to the file \a filename as a PFS frame.
 */
void cvl_save_pfs(const char *filename, cvl_frame_t *frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    cvl_save(filename, CVL_PFS, frame);
}


/*
 * Stream type independent interface
 */

/**
 * \param f		The stream.
 * \param type		Storage space for the stream type, or NULL.
 * \param frame		Storage space for the frame.
 *
 * Reads a frame from the stream \a f in PNM or PFS format. If \a type is not
 * NULL, the detected stream type will be stored in it.
 */
void cvl_read(FILE *f, cvl_stream_type_t *type, cvl_frame_t **frame)
{
    *frame = NULL;
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    const char *errmsg = "Cannot read frame";
    int c;

    // Detect the stream type before calling cvl_read_pnm() or cvl_read_pfs().
    // Since both PNM and PFS files start with 'P', we need two fgetc() and 
    // ungetc() calls. Only one ungetc() is guaranteed to work in C99. This may
    // cause trouble on some platforms.
    c = fgetc(f);
    if (c == EOF)
    {
	if (ferror(f))
	    cvl_error_set(CVL_ERROR_IO, "%s: %s", errmsg, "input error");
	return;
    }
    if (c != 'P')
    {
	cvl_error_set(CVL_ERROR_DATA, "%s: %s", errmsg, "input is neither PNM nor PFS");
	return;
    }
    c = fgetc(f);
    if (c == EOF)
    {
	cvl_error_set(CVL_ERROR_IO, "%s: %s", errmsg, ferror(f) ? "input error" : "EOF");
	return;
    }
    if (ungetc(c, f) == EOF || ungetc('P', f) == EOF)
    {
	cvl_error_set(CVL_ERROR_IO, "%s: %s", errmsg, "input error");
	return;
    }
    if (c == 'F')
    {
	if (type)
	    *type = CVL_PFS;
	cvl_read_pfs(f, frame);
    }
    else
    {
	if (type)
	    *type = CVL_PNM;
	cvl_read_pnm(f, frame);
    }
}

/**
 * \param f		The stream.
 * \param type		The stream type.
 * \param frame		The frame.
 *
 * Writes the frame \a frame to the stream \a f. The \a type parameter determines
 * if a PNM or PFS frame is written.
 */
void cvl_write(FILE *f, cvl_stream_type_t type, cvl_frame_t *frame)
{
    cvl_assert(f != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    if (type == CVL_PNM)
	cvl_write_pnm(f, frame);
    else
	cvl_write_pfs(f, frame);
}

/**
 * \param filename	The file name.
 * \param type		Storage space for the stream type, or NULL.
 * \param frame		Storage space for the frame.
 *
 * Reads a frame from the file \a filename in PNM or PFS format. If \a type is
 * not NULL, the detected stream type will be stored in it.
 */
void cvl_load(const char *filename, cvl_stream_type_t *type, cvl_frame_t **frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    FILE *f;
    if (!(f = fopen(filename, "r")))
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot open %s: %s", filename, strerror(errno));
	return;
    }
    cvl_read(f, type, frame);
    fclose(f);
}

/**
 * \param filename	The file name.
 * \param type		The stream type.
 * \param frame		The frame.
 *
 * Writes the frame \a frame to the file \a filename. The \a type parameter 
 * determines if a PNM or PFS frame is written.
 */
void cvl_save(const char *filename, cvl_stream_type_t type, cvl_frame_t *frame)
{
    cvl_assert(filename != NULL);
    cvl_assert(frame != NULL);
    if (cvl_error())
	return;

    FILE *f;
    if (!(f = fopen(filename, "w")))
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot open %s: %s", filename, strerror(errno));
	return;
    }
    cvl_write(f, type, frame);
    fclose(f);
}
