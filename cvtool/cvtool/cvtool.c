/*
 * cvtool.c
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
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#ifdef W32_NATIVE
#include <fcntl.h>
#include <io.h>
#endif

#include <cvl/cvl.h>

#include "mh.h"


char *program_name;

/*
 * The command functions. All live in their own .c file (except for the trivial
 * help and version commands).
 * Add the aproppriate lines for each command in this section.
 * The commands must appear in ascending order according to strcmp(), because we
 * do a binary search on the command name.
 */

typedef struct
{
    const char *name;
    int (*cmd)(int argc, char *argv[]);
    void (*cmd_print_help)(void);
} cvtool_command_t;

#define COMMAND_DECL(NAME)	\
    int cmd_ ## NAME (int argc, char *argv[]); \
    void cmd_ ## NAME ## _print_help (void);

#define COMMAND(NAME)  { #NAME, cmd_ ## NAME, cmd_ ## NAME ## _print_help }

COMMAND_DECL(affine)
COMMAND_DECL(blend)
COMMAND_DECL(channelcombine)
COMMAND_DECL(channelextract)
COMMAND_DECL(color)
COMMAND_DECL(combine)
COMMAND_DECL(convert)
COMMAND_DECL(convolve)
COMMAND_DECL(create)
COMMAND_DECL(cut)
COMMAND_DECL(diff)
COMMAND_DECL(draw)
COMMAND_DECL(edge)
COMMAND_DECL(flip)
COMMAND_DECL(flop)
COMMAND_DECL(foreach)
COMMAND_DECL(gamma)
COMMAND_DECL(gauss)
COMMAND_DECL(help)
COMMAND_DECL(info)
COMMAND_DECL(invert)
COMMAND_DECL(laplace)
COMMAND_DECL(layer)
COMMAND_DECL(max)
COMMAND_DECL(mean)
COMMAND_DECL(median)
COMMAND_DECL(merge)
COMMAND_DECL(min)
COMMAND_DECL(mix)
COMMAND_DECL(resize)
COMMAND_DECL(reverse)
COMMAND_DECL(rotate)
COMMAND_DECL(scale)
COMMAND_DECL(select)
COMMAND_DECL(shear)
COMMAND_DECL(split)
COMMAND_DECL(tonemap)
COMMAND_DECL(unsharpmask)
COMMAND_DECL(version)
COMMAND_DECL(visualize)
COMMAND_DECL(wavelets)

cvtool_command_t commands[] = 
{
    COMMAND(affine),
    COMMAND(blend),
    COMMAND(channelcombine),
    COMMAND(channelextract),
    COMMAND(color),
    COMMAND(combine),
    COMMAND(convert),
    COMMAND(convolve),
    COMMAND(create),
    COMMAND(cut),
    COMMAND(diff),
    COMMAND(draw),
    COMMAND(edge),
    COMMAND(flip),
    COMMAND(flop),
    COMMAND(foreach),
    COMMAND(gamma),
    COMMAND(gauss),
    COMMAND(help),
    COMMAND(info),
    COMMAND(invert),
    COMMAND(laplace),
    COMMAND(layer),
    COMMAND(max),
    COMMAND(mean),
    COMMAND(median),
    COMMAND(merge),
    COMMAND(min),
    COMMAND(mix),
    COMMAND(resize),
    COMMAND(reverse),
    COMMAND(rotate),
    COMMAND(scale),
    COMMAND(select),
    COMMAND(shear),
    COMMAND(split),
    COMMAND(tonemap),
    COMMAND(unsharpmask),
    COMMAND(version),
    COMMAND(visualize),
    COMMAND(wavelets),
    { NULL, NULL, NULL }
};
    

/*
 * If you just want to add a command, there's no need to change anything below
 * this line.
 */

int cmd_strcmp(const cvtool_command_t *c1, const cvtool_command_t *c2)
{
    return strcmp(c1->name, c2->name);
}

int cmd_find(const char *cmd)
{
    cvtool_command_t *p;
    cvtool_command_t key = { cmd, NULL, NULL };
    
    p = bsearch(&key, commands, 
	    sizeof(commands) / sizeof(commands[0]) - 1, 
	    sizeof(cvtool_command_t), 
	    (int (*)(const void *, const void *))cmd_strcmp);
    return p ? (p - commands) : -1;
}

void cmd_version_print_help(void)
{
    mh_msg_fmt_req(
	    "version\n"
	    "\n"
	    "Print version information.");
}

int cmd_version(int argc, char *argv[])
{
    mh_msg_set_command_name("%s", argv[0]);
    if (argc == 1)
    {
	mh_msg_fmt_req("%s version %s\n"
		"Copyright (C) 2008  Martin Lambers and others.\n"
		"This is free software. You may redistribute copies of it under the terms of "
		"the GNU General Public License.\n"
		"There is NO WARRANTY, to the extent permitted by law.",
		PACKAGE_NAME, VERSION);
	return 0;
    }
    else
    {
	mh_msg_err("too many arguments");
	return 1;
    }
}

void cmd_help_print_help(void)
{
    mh_msg_fmt_req(
	    "help [<command>]\n"
	    "\n"
	    "Print general or command specific help.");
}

int cmd_help(int argc, char *argv[])
{
    mh_msg_set_command_name("%s", argv[0]);
    if (argc == 1)
    {
	mh_msg_fmt_req(
		"Usage: %s [-q|--quiet] [-v|--verbose] <command> [argument...]\n"
		"\n"
		"Available commands:\n",
		program_name);
	for (int i = 0; commands[i].name; i++)
	{
	    mh_msg_req("%s", commands[i].name);
	}
	mh_msg_fmt_req(
		"\n"
		"Use \"%s help <command>\" for command specific help.\n"
		"Report bugs to <%s>.", program_name, PACKAGE_BUGREPORT);
	return 0;
    }
    else if (argc == 2)
    {
	int cmd_index = cmd_find(argv[1]);
    	if (cmd_index < 0)
	{
	    mh_msg_err("command unknown: %s", argv[1]);
	    return 1;
	}
	else
	{
	    commands[cmd_index].cmd_print_help();
	    return 0;
	}
    }
    else
    {
	mh_msg_err("too many arguments");
	return 1;
    }
}

int main(int argc, char *argv[])
{
    int exitcode = 0;
    
#ifdef W32_NATIVE
    _setmode(_fileno(stdin), _O_BINARY);	    
    _setmode(_fileno(stdout), _O_BINARY);	    
    _fmode = _O_BINARY;
    setbuf(stderr, NULL);
#endif

    program_name = strrchr(argv[0], MH_DIRSEP);
    program_name = program_name ? program_name + 1 : argv[0];
    mh_msg_set_program_name("%s", program_name);
#if DEBUG
    mh_msg_set_output_level(MH_MSG_DBG);
#else
    mh_msg_set_output_level(MH_MSG_INF);
#endif
    mh_msg_fmt_set_columns_from_env();
    mh_crashhandler_init();

    if (argc < 2)
    {
	char help[] = "help";
	char *my_argv[] = { help, NULL };
	cmd_help(1, my_argv);
	exitcode = 1;
    }
    else if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
	strcpy(argv[1], "help");
	exitcode = cmd_help(argc - 1, &(argv[1]));
    }
    else if (argc == 2 && strcmp(argv[1], "--version") == 0)
    {
	strcpy(argv[1], "version");
	exitcode = cmd_version(argc - 1, &(argv[1]));
    }
    else
    {
	int argv_cmd_index = 1;
	if (argc > argv_cmd_index + 1 && (strcmp(argv[argv_cmd_index], "-q") == 0 
		    || strcmp(argv[argv_cmd_index], "--quiet") == 0))
	{
	    argv_cmd_index++;
	    mh_msg_set_output_level(MH_MSG_WRN);
	}
	if (argc > argv_cmd_index + 1 && (strcmp(argv[argv_cmd_index], "-v") == 0 
		    || strcmp(argv[argv_cmd_index], "--verbose") == 0))
	{
    	    argv_cmd_index++;
	    mh_msg_set_output_level(MH_MSG_DBG);
	}
	int cmd_index = cmd_find(argv[argv_cmd_index]);
	if (cmd_index < 0)
	{
	    mh_msg_err("command unknown: %s", argv[argv_cmd_index]);
	    exitcode = 1;
	}
    	else if (commands[cmd_index].cmd == cmd_help || commands[cmd_index].cmd == cmd_version)
	{
	    /* Do not cretae a GL context for these simple commands */
	    exitcode = commands[cmd_index].cmd(argc - 1, &(argv[1]));
	}
	else
	{
	    const char *display_name;
#ifdef W32_NATIVE
	    display_name = NULL;
#else
	    if (!(display_name = getenv("DISPLAY")))
	    {
		mh_msg_err("Cannot create OpenGL context: No environment variable DISPLAY.");
		exitcode = 1;
	    }
	    else
#endif
	    {
		cvl_gl_context_t *ctx = cvl_gl_context_new(display_name);
		if (!ctx)
		{
#ifdef W32_NATIVE
		    mh_msg_err("Cannot create OpenGL context");
#else
		    mh_msg_err("Cannot create OpenGL context on display %s", display_name);
#endif
		    exitcode = 1;
		}
		else
		{
		    bool cvl_initialized = false;
		    cvl_init();
		    if (!cvl_error())
		    {
			cvl_initialized = true;
			exitcode = commands[cmd_index].cmd(argc - argv_cmd_index, &(argv[argv_cmd_index]));
		    }
		    if (cvl_error())
		    {
			mh_msg_err("%s", cvl_error_msg());
			exitcode = 1;
		    }
		    if (cvl_initialized)
		    {
			cvl_deinit();
		    }
		    cvl_gl_context_free(ctx);
		}
	    }
	}
    }
    return exitcode;
}
