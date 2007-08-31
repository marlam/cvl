/*
 * main.cpp
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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

#include <exception>
#include <cstdio>
#include <cstdlib>

#ifdef W32_NATIVE
#include <ios>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#include <QApplication>

#include "mh.h"

#include "cvltonemap.h"


char *program_name;

static void cvltonemap_print_version(void)
{
    mh_msg_fmt_req("%s version %s\n"
	    "Copyright (C) 2007  Martin Lambers and others.\n"
    	    "This is free software. You may redistribute copies of it under the terms of "
	    "the GNU General Public License.\n"
	    "There is NO WARRANTY, to the extent permitted by law.",
	    PACKAGE_NAME, VERSION);
}

static void cvltonemap_print_help(void)
{
    mh_msg_fmt_req("Usage: %s [file]\n"
	    "Report bugs to <%s>.", 
	    program_name, PACKAGE_BUGREPORT);
}

static void cvltonemap_exceptional_exception(void)
{
    mh_msg_err("Uncaught exception.");
    mh_crash();
}

int main(int argc, char *argv[])
{
#ifdef W32_NATIVE
    _setmode(_fileno(stdin), _O_BINARY);	    
    _setmode(_fileno(stdout), _O_BINARY);	    
    _fmode = _O_BINARY;
    setbuf(stderr, NULL);
#endif

    program_name = strrchr(argv[0], DIRSEP);
    program_name = program_name ? program_name + 1 : argv[0];
    mh_msg_set_program_name("%s", program_name);
#if DEBUG
    mh_msg_set_output_level(MH_MSG_DBG);
#else
    mh_msg_set_output_level(MH_MSG_INF);
#endif
    mh_msg_fmt_set_columns_from_env();
    mh_crashhandler_init();
    std::set_unexpected(cvltonemap_exceptional_exception);
    std::set_terminate(cvltonemap_exceptional_exception);

    QApplication *app = new QApplication(argc, argv);

    mh_option_info_t version = { false, cvltonemap_print_version };
    mh_option_info_t help = { false, cvltonemap_print_help };
    mh_option_t options[] = 
    { 
	{ "version", '\0', MH_OPTION_INFO, &version, false },
	{ "help",    '\0', MH_OPTION_INFO, &help,    false },
	mh_option_null 
    };
    int arg_count, arg_index;
    if (!mh_getopt(argc, argv, options, 0, 1, &arg_index))
    {
	return 1;
    }
    if (help.value || version.value)
    {
	return 0;
    }
    arg_count = argc - arg_index;

    CVLToneMap *cvltonemap = new CVLToneMap();
    cvltonemap->show();
    if (arg_count > 0)
    {
	cvltonemap->load_image(argv[arg_index]);
    }
    int retval = app->exec();
    delete cvltonemap;
    delete app;
    return retval;
}
