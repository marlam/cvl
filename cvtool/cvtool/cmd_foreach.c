/*
 * cmd_foreach.c
 * 
 * This file is part of cvtool, a computer vision tool.
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include <cvl/cvl.h>

#include "inttostr.h"
#include "pipe.h"
#include "wait-process.h"

#include "mh.h"


void cmd_foreach_print_help(void)
{
    mh_msg_fmt_req(
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


int cmd_foreach(int argc, char *argv[])
{
    mh_option_string_t shell = { NULL, NULL };
    mh_option_int_t n = { 1, 1, INT_MAX };
    mh_option_t options[] =
    {
	{ "shell",   's', MH_OPTION_STRING, &shell, false },
	{ "n",       'n', MH_OPTION_INT,    &n,     false },
	mh_option_null
    };
    int argument_index;
    char *cmd;
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
    cvl_stream_type_t stream_type;
    cvl_frame_t *frame;
    int frame_counter;
    int i;
    bool error;
    bool eof;

    
    mh_msg_set_command_name("%s", argv[0]);
    if (!mh_getopt(argc, argv, options, 1, 1, &argument_index))
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
    
    error = false;
    eof = false;
    frame_counter = 0;
    for (;;)
    {
	/* Create temporary file which will serve as stdin for the command */
	if (!(pipe_input = mh_mktempfile(PACKAGE_NAME "-foreach-", &pipe_input_name)))
	{
	    mh_msg_err("Cannot create temporary file: %s", strerror(errno));
	    error = true;
	    break;
	}	
	
	/* Copy n frames into temporary file */
	i = 0;
	while (i < n.value)
	{
	    if (i == 0)
	    {
		N = imaxtostr(frame_counter, Nbuf);
	    }
	    cvl_read(stdin, &stream_type, &frame);
	    error = cvl_error();
	    eof = !error && !frame;
	    if (!frame)
		break;
	    frame_counter++;
	    if (i == 0)
	    {
		W = imaxtostr(cvl_frame_width(frame), Wbuf);
		H = imaxtostr(cvl_frame_height(frame), Hbuf);
	    }
	    cvl_write(pipe_input, stream_type, frame);
    	    cvl_frame_free(frame);
	    i++;
	    error = error || cvl_error();
	    if (error)
		break;
	}
	if (error || (eof && i == 0))
	{
	    break;
	}
	if (fclose(pipe_input) != 0)
	{
	    pipe_input = NULL;
	    mh_msg_err("Cannot close temporary file: %s", strerror(errno));
	    error = true;
	    break;
	}
	pipe_input = NULL;

	/* Open pipe to read from command. The command's stdin will be the
	 * temporary file. */
	command = mh_strdup(cmd);
	command = mh_str_replace(command, "%N", N);
	command = mh_str_replace(command, "%W", W);
	command = mh_str_replace(command, "%H", H);
	pipe_argv[pipe_argv_cmd_index] = command;
	if ((pipe_pid = create_pipe_in(pipe_argv[0], pipe_argv[0], pipe_argv, 
			pipe_input_name, false, true, false, &pipe_output_fd)) < 0)
	{
	    mh_msg_err("Cannot execute command '%s'", command);
	    error = true;
	    break;
	}
	if (!(pipe_output = fdopen(pipe_output_fd, "r")))
	{
	    mh_msg_err("Cannot read from command '%s': %s", command, strerror(errno));
	    error = true;
	    break;
	}

	/* Copy the output of the command to our stdout */	
	for (;;)
	{
	    cvl_read(pipe_output, &stream_type, &frame);
	    error = error || cvl_error();
	    if (!frame)
		break;
	    cvl_write(stdout, stream_type, frame);
	    cvl_frame_free(frame);
	    error = error || cvl_error();
	}
	if (error)
	{
	    break;
	}
	(void)remove(pipe_input_name);
	free(pipe_input_name);
	pipe_input_name = NULL;

	/* Close the pipe */
	pipe_status = wait_subprocess(pipe_pid, command, false, true, true, false);
	if (pipe_status == 127)
	{
	    mh_msg_err("Command '%s' failed to execute", command);
	    error = true;
	    break;
	}
	if (pipe_status != 0)
	{
	    mh_msg_err("Command '%s' failed with exit status %d", command, pipe_status);
	    error = true;
	    break;
	}
	free(command);
	command = NULL;
	if (eof)
	    break;
    }
    
    if (command)
	free(command);
    if (pipe_input_name)
    {
	(void)remove(pipe_input_name);
	free(pipe_input_name);
    }
    if (pipe_input)
	fclose(pipe_input);
    if (pipe_output)
	fclose(pipe_output);
    return error ? 1 : 0;
}
