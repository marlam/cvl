/*
 * cvtool.c
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
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
extern int errno;
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <cvl/cvl.h>

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
COMMAND_DECL(binarize)
COMMAND_DECL(blend)
COMMAND_DECL(channel)
COMMAND_DECL(color)
COMMAND_DECL(combine)
COMMAND_DECL(convert)
COMMAND_DECL(convolve)
COMMAND_DECL(create)
COMMAND_DECL(cut)
COMMAND_DECL(dibr)
COMMAND_DECL(diff)
COMMAND_DECL(edge)
COMMAND_DECL(equalize)
COMMAND_DECL(flip)
COMMAND_DECL(flop)
COMMAND_DECL(foreach)
COMMAND_DECL(grid)
COMMAND_DECL(help)
COMMAND_DECL(info)
COMMAND_DECL(invert)
COMMAND_DECL(merge)
COMMAND_DECL(opticalflow)
COMMAND_DECL(resize)
COMMAND_DECL(reverse)
COMMAND_DECL(rotate)
COMMAND_DECL(scale)
COMMAND_DECL(select)
COMMAND_DECL(shear)
COMMAND_DECL(smooth)
COMMAND_DECL(split)
COMMAND_DECL(stereoview)
COMMAND_DECL(trackdepth)
COMMAND_DECL(vectors)
COMMAND_DECL(version)

cvtool_command_t commands[] = 
{
    COMMAND(affine),
    COMMAND(binarize),
    COMMAND(blend),
    COMMAND(channel),
    COMMAND(color),
    COMMAND(combine),
    COMMAND(convert),
    COMMAND(convolve),
    COMMAND(create),
    COMMAND(cut),
    COMMAND(dibr),
    COMMAND(diff),
    COMMAND(edge),
    COMMAND(equalize),
    COMMAND(flip),
    COMMAND(flop),
    COMMAND(foreach),
    COMMAND(grid),
    COMMAND(help),
    COMMAND(info),
    COMMAND(invert),
    COMMAND(merge),
    COMMAND(opticalflow),
    COMMAND(resize),
    COMMAND(reverse),
    COMMAND(rotate),
    COMMAND(scale),
    COMMAND(select),
    COMMAND(shear),
    COMMAND(smooth),
    COMMAND(split),
    COMMAND(stereoview),
    COMMAND(trackdepth),
    COMMAND(vectors),
    COMMAND(version),
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
    cvl_msg_fmt_req(
	    "version\n"
	    "\n"
	    "Print version information.");
}

int cmd_version(int argc, char *argv[])
{
    cvl_msg_set_command_name("%s", argv[0]);
    if (argc == 1)
    {
	cvl_msg_fmt_req("%s version %s\n"
		"Copyright (C) 2005, 2006  Martin Lambers <%s>.\n"
		"This is free software; see the source for copying conditions. There is NO "
		"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.",
		PROGRAM_NAME, VERSION, PACKAGE_BUGREPORT);
	return 0;
    }
    else
    {
	cvl_msg_err("too many arguments");
	return 1;
    }
}

void cmd_help_print_help(void)
{
    cvl_msg_fmt_req(
	    "help [<command>]\n"
	    "\n"
	    "Print general or command specific help.");
}

int cmd_help(int argc, char *argv[])
{
    cvl_msg_set_command_name("%s", argv[0]);
    if (argc == 1)
    {
	cvl_msg_fmt_req(
		"Usage: %s [-q|--quiet] [-v|--verbose] <command> [argument...]\n"
		"\n"
		"Available commands:\n",
		program_name);
	for (int i = 0; commands[i].name; i++)
	{
	    cvl_msg_req("%s", commands[i].name);
	}
	cvl_msg_fmt_req(
		"\n"
		"Use \"%s help <command>\" for command specific help.", 
		program_name);
	return 0;
    }
    else if (argc == 2)
    {
	int cmd_index = cmd_find(argv[1]);
    	if (cmd_index < 0)
	{
	    cvl_msg_err("command unknown: %s", argv[1]);
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
	cvl_msg_err("too many arguments");
	return 1;
    }
}

int main(int argc, char *argv[])
{
    char *column_string;
    int exitcode;
    
#ifdef _WIN32
    /* This must be done before anything is read or written */
    _setmode(_fileno(stdin), _O_BINARY);	    
    _setmode(_fileno(stdout), _O_BINARY);	    
    /* Open all files in binary mode by default */
    _fmode = _O_BINARY;
#endif

#ifdef _WIN32
    program_name = strrchr(argv[0], '\\');
#else
    program_name = strrchr(argv[0], '/');
#endif
    program_name = program_name ? program_name + 1 : argv[0];
    
    cvl_msg_set_program_name("%s", program_name);
    cvl_msg_set_output_level(CVL_MSG_INF);
    if ((column_string = getenv("COLUMNS")))
    {
	char *p;
	long columns;
	
	errno = 0;
	columns = strtol(column_string, &p, 0);
	if (p != column_string && *p == '\0' && errno != ERANGE && columns > 0 && columns <= INT_MAX)
	{
	    cvl_msg_fmt_set_columns(columns);
	}
    }

    if (argc < 2)
    {
	char help[] = "help";
	char *my_argv[] = { help, NULL };
	cmd_help(1, my_argv);
	return 1;
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
	    cvl_msg_set_output_level(CVL_MSG_WRN);
	}
	if (argc > argv_cmd_index + 1 && (strcmp(argv[argv_cmd_index], "-v") == 0 
		    || strcmp(argv[argv_cmd_index], "--verbose") == 0))
	{
    	    argv_cmd_index++;
	    cvl_msg_set_output_level(CVL_MSG_DBG);
	}
	int cmd_index = cmd_find(argv[argv_cmd_index]);
	if (cmd_index < 0)
	{
	    cvl_msg_err("command unknown: %s", argv[argv_cmd_index]);
	    exitcode = 1;
	}
    	else
	{
	    exitcode = commands[cmd_index].cmd(argc - argv_cmd_index, &(argv[argv_cmd_index]));
	}
    }
    return exitcode;
}

void xalloc_die(void)
{
    cvl_msg_err("%s", strerror(ENOMEM));
    exit(1);
}
