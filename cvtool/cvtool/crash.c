/*
 * crash.c
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

#if HAVE_SIGACTION

# include <signal.h>
# include <stdlib.h>

# include <cvl/cvl.h>

# include "crash.h"

void crashhandler(int signum)
{
    cvl_msg_err("Caught signal %s (%d). Aborting.", 
	    (signum == SIGILL ? "SIGILL" : (signum == SIGFPE ? "SIGFPE" : "SIGSEGV")),
	    signum);
    cvl_print_backtrace();
    abort();
}

void init_crashhandler(void)
{
    struct sigaction signal_handler;

    signal_handler.sa_handler = crashhandler;
    sigemptyset(&signal_handler.sa_mask);
    signal_handler.sa_flags = 0;
    (void)sigaction(SIGILL, &signal_handler, NULL);
    (void)sigaction(SIGFPE, &signal_handler, NULL);
    (void)sigaction(SIGSEGV, &signal_handler, NULL);
}

#else

# include "crash.h"

void init_crashhandler(void)
{
}

#endif
