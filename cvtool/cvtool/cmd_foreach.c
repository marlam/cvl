/*
 * cmd_foreach.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
extern int errno;

#include "inttostr.h"
#include "pipe.h"
#include "wait-process.h"
#include "xalloc.h"

#include "tempfile.h"

#include <cvl/cvl.h>


void cmd_foreach_print_help(void)
{
    cvl_msg_fmt_req(
	    "foreach [-s|--shell=<shell>] [-n|--n=<n>] cmd\n"
	    "\n"
	    "Execute the given command for every frame. The command is expected to read "
	    "n frames from standard input (default is n=1), and write an arbitrary "
	    "number (including zero) of frames to standard output. "
	    "The original frame(s) that were given to the command are replaced by the "
	    "output of the command. "
	    "The frames that the command produces are converted "
	    "to the format of the original frames. "
	    "The foreach command replaces the following special strings in the command cmd "
	    "before executing the command: %%N (replaced with frame number), "
	    "%%W (replaced with frame width), and %%H (replaced with frame height). If n is "
	    "greater than 1, these values refer to the first frame that is piped to the "
	    "command.\n"
	    "The command cmd is executed by passing it to the system shell. The default is "
	    "\"/bin/sh -c\" on most systems. This can be overridden with the --shell option. "
	    "It expects a string with zero or one spaces: The first part of the "
	    "string is the shell, the second part (if any) is the first option to the shell. "
	    "The next option will then be the command to execute.");
}


/*
 * string_replace()
 *
 * Replace all occurences of 's' in the string 'str' with 'r'.
 * 'str' must be allocated; it will be realloc'ed when necessary.
 */

char *string_replace(char *str, const char *s, const char *r)
{
    char *p, *new_str;
    size_t next_pos = 0;
    size_t rlen = strlen(r);

    while ((p = strstr(str + next_pos, s)))
    {
	new_str = xmalloc((strlen(str) + rlen - 1) * sizeof(char));
	strncpy(new_str, str, (size_t)(p - str));
	strcpy(new_str + (size_t)(p - str), r);
	strcpy(new_str + (size_t)(p - str) + rlen, 
		str + (size_t)(p - str) + 2);
	next_pos = (size_t)(p - str) + rlen;
	free(str);
	str = new_str;
    }
    return str;
}


/*
 * Now the real command
 */

int cmd_foreach(int argc, char *argv[])
{
    cvl_option_string_t shell = { NULL, NULL };
    cvl_option_int_t n = { 1, 1, INT_MAX };
    cvl_option_t options[] =
    {
	{ "shell",   's', CVL_OPTION_STRING, &shell, false },
	{ "n",       'n', CVL_OPTION_INT,    &n,     false },
	cvl_option_null
    };
    int argument_index;
    char *cmd;
    cvl_io_info_t *input_info;
    cvl_io_info_t *output_info;
    char Nbuf[INT_BUFSIZE_BOUND(intmax_t)];
    char Wbuf[INT_BUFSIZE_BOUND(intmax_t)];
    char Hbuf[INT_BUFSIZE_BOUND(intmax_t)];
    char *N = NULL, *W = NULL, *H = NULL;
    char *command = NULL;
    char *pipe_argv[4];
    int pipe_argv_cmd_index;
    pid_t pipe_pid = -1;
    char *pipe_input_name = NULL;
    FILE *pipe_input = NULL;
    int pipe_output_fd = -1;
    FILE *pipe_output = NULL;
    int pipe_status;
    cvl_io_info_t *pipe_input_info = NULL;
    cvl_io_info_t *pipe_output_info = NULL;
    cvl_frame_t *frame;
    int i;
    bool error;

    
    cvl_msg_set_command_name("%s", argv[0]);
    if (!cvl_getopt(argc, argv, options, 1, 1, &argument_index))
    {
	return 1;
    }
    cmd = argv[argument_index];

    /* How to start the shell */
    if (shell.value)
    {
	char *space = strchr(shell.value, ' ');
	if (space)
	{
	    *space = '\0';
	    pipe_argv[0] = shell.value;
	    pipe_argv[1] = space + 1;
	    pipe_argv_cmd_index = 2;
	}
	else
	{
	    pipe_argv[0] = shell.value;
	    pipe_argv_cmd_index = 1;
	}
    }
    else
    {
#ifdef W32_NATIVE
	char *tmp = getenv("COMSPEC");
	if (!tmp)
	{
	    tmp = (char *)"C:\\WINDOWS\\CMD.EXE";
	}
	pipe_argv[0] = tmp;
	pipe_argv[1] = (char *)"/C";
	pipe_argv_cmd_index = 2;
#else
	pipe_argv[0] = (char *)"/bin/sh";
	pipe_argv[1] = (char *)"-c";
	pipe_argv_cmd_index = 2;
#endif
    }
    pipe_argv[2] = NULL;
    pipe_argv[3] = NULL;    
    
    input_info = cvl_io_info_new();
    output_info = cvl_io_info_new();
    cvl_io_info_link_output_to_input(output_info, input_info);

    error = false;
    while (!cvl_io_eof(stdin))
    {
	/* Create temporary file which will serve as stdin for the command */
	if (!(pipe_input = tempfile(PROGRAM_NAME "-foreach-", &pipe_input_name)))
	{
	    cvl_msg_err("cannot create temporary file: %s", strerror(errno));
	    error = true;
	    break;
	}	
	
	/* Copy n frames into temporary file */
	pipe_input_info = cvl_io_info_new();
	cvl_io_info_link_output_to_input(pipe_input_info, input_info);
	i = 0;
	while (!cvl_io_eof(stdin) && i < n.value)
	{
	    if (i == 0)
	    {
		N = imaxtostr(cvl_io_info_frame_counter(input_info), Nbuf);
	    }
	    if (!cvl_io_read(stdin, input_info, &frame))
	    {
		error = true;
		break;
	    }
	    if (i == 0)
	    {
		W = imaxtostr(cvl_frame_width(frame), Wbuf);
		H = imaxtostr(cvl_frame_height(frame), Hbuf);
	    }
	    if (!cvl_io_write(pipe_input, pipe_input_info, frame))
	    {
		cvl_frame_free(frame);
		error = true;
		break;
	    }
	    cvl_frame_free(frame);
	    i++;
	}
	cvl_io_info_free(pipe_input_info);
	pipe_input_info = NULL;
	if (error)
	{
	    break;
	}
	if (fclose(pipe_input) != 0)
	{
	    pipe_input = NULL;
	    cvl_msg_err("cannot close temporary file: %s", strerror(errno));
	    error = true;
	    break;
	}
	pipe_input = NULL;

	/* Open pipe to read from command. The command's stdin will be the
	 * temporary file. */
	command = xstrdup(cmd);
	command = string_replace(command, "%N", N);
	command = string_replace(command, "%W", W);
	command = string_replace(command, "%H", H);
	pipe_argv[pipe_argv_cmd_index] = command;
	if ((pipe_pid = create_pipe_in(pipe_argv[0], pipe_argv[0], pipe_argv, 
			pipe_input_name, false, true, false, &pipe_output_fd)) < 0)
	{
	    cvl_msg_err("cannot execute command '%s'", command);
	    error = true;
	    break;
	}
	if (!(pipe_output = fdopen(pipe_output_fd, "r")))
	{
	    cvl_msg_err("cannot read from command '%s': %s", command, strerror(errno));
	    error = true;
	    break;
	}

	/* Copy the output of the command to our stdout */	
	pipe_output_info = cvl_io_info_new();
	cvl_io_info_link_output_to_input(output_info, input_info);
    	while (!cvl_io_eof(pipe_output))
	{
	    if (!cvl_io_read(pipe_output, pipe_output_info, &frame))
	    {
		error = true;
		break;
	    }
	    cvl_io_info_set_width(output_info, cvl_frame_width(frame));
	    cvl_io_info_set_height(output_info, cvl_frame_height(frame));
	    if (!cvl_io_write(stdout, output_info, frame))
	    {
		cvl_frame_free(frame);
		error = true;
		break;
	    }
	    cvl_frame_free(frame);
	}
	if (error)
	{
	    break;
	}
	cvl_io_info_free(pipe_output_info);
	pipe_output_info = NULL;
	(void)remove(pipe_input_name);
	free(pipe_input_name);
	pipe_input_name = NULL;

	/* Close the pipe */
	pipe_status = wait_subprocess(pipe_pid, command, false, true, true, false);
	if (pipe_status == 127)
	{
	    cvl_msg_err("command '%s' failed to execute", command);
	    error = true;
	    break;
	}
	if (pipe_status != 0)
	{
	    cvl_msg_err("'%s' failed with exit status %d", command, pipe_status);
	    error = true;
	    break;
	}
	free(command);
	command = NULL;
    }
    
    if (command)
    {
	free(command);
    }
    if (pipe_input_name)
    {
	(void)remove(pipe_input_name);
	free(pipe_input_name);
    }
    if (pipe_input)
    {
	fclose(pipe_input);
    }
    if (pipe_output)
    {
	fclose(pipe_output);
    }
    if (pipe_input_info)
    {
	cvl_io_info_free(pipe_input_info);
    }
    if (pipe_output_info)
    {
	cvl_io_info_free(pipe_output_info);
    }
    cvl_io_info_free(input_info);
    cvl_io_info_free(output_info);
    return error ? 1 : 0;
}
