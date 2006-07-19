/*
 * pgmtovol.c
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef W32_NATIVE
#include <fcntl.h>
#include <io.h>
#endif

#include <cvl/cvl.h>

char *program_name;

void print_help(void)
{
    cvl_msg_fmt_req("Usage: %s\n"
	    "Reads a series of PGM images from standard input, converts them to volume data "
	    "in the .vol format, and writes this data to standard output.\n"
	    "Report bugs to <%s>.",
	    program_name, PACKAGE_BUGREPORT);
}

void print_version(void)
{
    cvl_msg_fmt_req("pgmtovol (%s) %s\n"
	    "Copyright (C) 2006  Martin Lambers.\n"
	    "This is free software. You may redistribute copies of it under the terms of "
	    "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
	    "There is NO WARRANTY, to the extent permitted by law.",
	    PACKAGE_NAME, PACKAGE_VERSION);
}

void *xnrealloc(void *ptr, size_t nmemb, size_t size)
{
    /* This was borrowed from the gnulib xalloc module */
    if (((size_t)(sizeof(ptrdiff_t) <= sizeof(size_t) ? -1 : -2) / size < nmemb)
	    || (!(ptr = realloc(ptr, nmemb * size)) && nmemb != 0))
    {
	cvl_msg_err("cannot allocate memory");
	abort();
    }
    return ptr;
}

void *xmalloc(size_t size)
{
    void *ptr;
    
    if (!(ptr = malloc(size)))
    {
	cvl_msg_err("cannot allocate memory");
	abort();
    }
    return ptr;
}

int main(int argc, char *argv[])
{
    cvl_option_info_t option_help = { false, print_help };
    cvl_option_info_t option_version = { false, print_version };
    cvl_option_t options[] = 
    { 
	{ "help",    '\0', CVL_OPTION_INFO, &option_help,    false },
	{ "version", '\0', CVL_OPTION_INFO, &option_version, false },
	cvl_option_null 
    };
    cvl_io_info_t *input_info;
    cvl_frame_t **frames = NULL;
    int depth = 0;
    bool error = false;

#ifdef W32_NATIVE
    _setmode(_fileno(stdin), _O_BINARY);	    
    _setmode(_fileno(stdout), _O_BINARY);	    
    program_name = strrchr(argv[0], '\\');
#else
    program_name = strrchr(argv[0], '/');
#endif
    program_name = program_name ? program_name + 1 : argv[0];
    cvl_msg_set_program_name("%s", program_name);
    cvl_msg_set_output_level(CVL_MSG_INF);
    if (!cvl_getopt(argc, argv, options, 0, 0, NULL))
    {
	return 1;
    }
    if (option_help.value || option_version.value)
    {
	return 0;
    }
    
    input_info = cvl_io_info_new();
    while (!cvl_io_eof(stdin))
    {
	frames = xnrealloc(frames, ++depth, sizeof(cvl_frame_t *));
	if (!cvl_io_read(stdin, input_info, &frames[depth - 1]))
    	{
	    error = true;
	    break;
	}
	if (cvl_frame_pixel_type(frames[depth - 1]) != CVL_PIXEL_GRAY)
	{
	    cvl_msg_err("cannot handle color PNM images");
	    error = true;
	    break;
	}
	if (depth > 1)
    	{
	    if (cvl_frame_width(frames[depth - 1]) != cvl_frame_width(frames[0])
		    || cvl_frame_height(frames[depth - 1]) != cvl_frame_height(frames[0]))
	    {
		cvl_msg_err("all frames must have the same dimensions");
		error = true;
		break;
	    }
	}
    }
    cvl_io_info_free(input_info);
    if (!error && depth > 0)
    {
	int width = cvl_frame_width(frames[0]);
	int height = cvl_frame_height(frames[0]);
	int idummy[3] = { 0, 0, 0 };
	char cdummy = '\n';
	
	if (fprintf(stdout, "X: %d\nY: %d\nZ: %d\nVoxel-Size: 1\n.\n", width, height, depth) < 0
		|| fwrite(idummy, sizeof(int), 3, stdout) != 3
		|| fwrite(&cdummy, sizeof(char), 1, stdout) != 1)
	{
	    cvl_msg_err("output error");
	    error = true;
	}
	if (!error)
	{
	    uint8_t *raw = xmalloc(width * height * sizeof(uint8_t));
	    for (int z = 0; z < depth; z++)
	    {
		for (int y = 0; y < cvl_frame_height(frames[0]); y++)
		{
		    for (int x = 0; x < cvl_frame_width(frames[0]); x++)
		    {
			raw[y * width + x] = (uint8_t)cvl_frame_get(frames[z], x, y);
		    }
		}
		if (fwrite(raw, width * height * sizeof(uint8_t), 1, stdout) != 1)
		{
		    cvl_msg_err("output error");
		    error = true;
		    break;
		}
	    }
	    free(raw);
	}
	for (int z = 0; z < depth; z++)
	{
	    cvl_frame_free(frames[z]);
	}
	free(frames);
    }

    return error ? 1 : 0;
}
