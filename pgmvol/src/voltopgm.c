/*
 * voltopgm.c
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
#include <string.h>
#include <stdlib.h>
#ifdef W32_NATIVE
#include <fcntl.h>
#include <io.h>
#endif

#include <cvl/cvl.h>

char *program_name;

void print_help(void)
{
    cvl_msg_fmt_req("Usage: %s\n"
	    "Reads volume data in the .vol format from standard input, converts it to a series "
	    "of PGM images, and writes these images to standard output.\n"
	    "Report bugs to <%s>.",
	    program_name, PACKAGE_BUGREPORT);
}

void print_version(void)
{
    cvl_msg_fmt_req("voltopgm (%s) %s\n"
	    "Copyright (C) 2006  Martin Lambers.\n"
	    "This is free software. You may redistribute copies of it under the terms of "
	    "the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
	    "There is NO WARRANTY, to the extent permitted by law.",
	    PACKAGE_NAME, PACKAGE_VERSION);
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
    bool error = false;
    const int linebufsize = 256;
    char linebuf[linebufsize];
    int width, height, depth, voxel_size;
    
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

    /* Read header */
    width = -1;
    height = -1;
    depth = -1;
    voxel_size = -1;
    while (!error && fgets(linebuf, linebufsize, stdin))
    {
	if (!strchr(linebuf, '\n'))
	{
	    cvl_msg_err("volume header line too long");
	    error = true;
	    break;
	}
	if (strncmp(linebuf, "X: ", 3) == 0)
	{
	    width = atoi(linebuf + 3);
	}
	else if (strncmp(linebuf, "Y: ", 3) == 0)
	{
	    height = atoi(linebuf + 3);
	}
	else if (strncmp(linebuf, "Z: ", 3) == 0)
	{
	    depth = atoi(linebuf + 3);
	}
	else if (strncmp(linebuf, "Voxel-Size: ", 12) == 0)
	{
	    voxel_size = atoi(linebuf + 12);
	}
	else if (strcmp(linebuf, ".\n") == 0)
	{
	    break;
	}
    }
    if (!error && (width < 1 || height < 1 || depth < 1 || voxel_size < 1))
    {
	cvl_msg_err("invalid volume header");
	error = true;
    }
    if (!error && voxel_size != 1)
    {
	cvl_msg_err("cannot handle voxel size != 1 yet");
	error = true;
    }
    if (error)
    {
	return 1;
    }
 
    /* Read the volume data and write the images */
    // read three dummy value + \n
    int idummy[3];
    char cdummy;
    if (fread(idummy, sizeof(int), 3, stdin) != 3 || fread(&cdummy, sizeof(char), 1, stdin) != 1)
    {
	cvl_msg_err("input error");
	error = true;
    }
    if (!error)
    {
	cvl_io_info_t *output_info = cvl_io_info_new();
	cvl_io_info_set_stream_type(output_info, CVL_IO_STREAM_PNM);
	cvl_io_info_set_width(output_info, width);
	cvl_io_info_set_height(output_info, height);
	for (int z = 0; !error && z < depth; z++)
	{
	    cvl_frame_t *frame = cvl_frame_new(CVL_PIXEL_GRAY, width, height);
	    for (int y = 0; !error && y < height; y++)
	    {
		for (int x = 0; x < width; x++)
		{
		    uint8_t v;
		    if (fread(&v, sizeof(uint8_t), 1, stdin) != 1)
		    {
			cvl_msg_err("input error");
			error = true;
			break;
		    }
		    cvl_frame_set(frame, x, y, v);
		}
	    }
	    if (!error && !cvl_io_write(stdout, output_info, frame))
	    {
		cvl_frame_free(frame);
		error = true;
		break;
	    }
	    cvl_frame_free(frame);
	}
	cvl_io_info_free(output_info);
    }
	
    return error ? 1 : 0;
}
