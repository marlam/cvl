/*
 * mh_crash.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007  Martin Lambers <marlam@marlam.de>
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

/** 
 * \file mh_crash.h
 * \brief A simple crash handler.
 *
 * A simple crash handler that prints debugging information if available.
 */

#include "config.h"

#include <stdlib.h>
#if HAVE_BACKTRACE
# include <stdlib.h>
# include <execinfo.h>
#endif
#if HAVE_SIGACTION
# include <signal.h>
#endif

#include "mh_msg.h"
#include "mh_assert.h"
#include "mh_crash.h"


void (*mh_failure_handler)(void) = abort;

void mh_set_failure_handler(void (*failure_handler)(void))
{
    mh_failure_handler = failure_handler;
}

#if HAVE_BACKTRACE

/**
 * Obtains a backtrace and prints it. Useful for debugging purposes.
 * This function currently only works on GNU/Linux. On other systems, it does
 * nothing.
 */
void mh_print_backtrace(void)
{
    /* Adapted from the example in the glibc manual. */
    void *array[64];
    int size;
    char **strings;
    
    size = backtrace(array, 64);

    if (size == 0)
    {
	mh_msg_err("No backtrace available.");
    }
    else
    {
	mh_msg_err("Backtrace:");
	strings = backtrace_symbols(array, size);
	for (int i = 0; i < size; i++)
	{
	    mh_msg_err("    %s", strings[i]);
	}
	free(strings);
    }
}

#else

void mh_print_backtrace(void)
{
}

#endif


/**
 * Prints a backtrace (if available) and an error message, and calls abort().
 */
void mh_crash(void)
{
    mh_print_backtrace();
    mh_msg_err("Please report this bug to <%s>.", PACKAGE_BUGREPORT);
    mh_failure_handler();
}

#if HAVE_SIGACTION

/**
 * \param signum	Signal number.
 *
 * Prints an error message about the signal \a signum, and then calls
 * mh_crash().
 */
void mh_crashhandler(int signum)
{
    mh_msg_err("Caught signal %d (%s). Aborting.", signum,
	    (signum == SIGILL ? "SIGILL" : (signum == SIGFPE ? "SIGFPE" : "SIGSEGV")));
    mh_crash();
}

/**
 * Initializes a simple crash handler that is called on SIGILL, SIGFPE, and
 * SIGSEGV. It will print an error message and debugging information if
 * available.
 */
void mh_crashhandler_init(void)
{
    struct sigaction signal_handler;

    signal_handler.sa_handler = mh_crashhandler;
    sigemptyset(&signal_handler.sa_mask);
    signal_handler.sa_flags = 0;
    (void)sigaction(SIGILL, &signal_handler, NULL);
    (void)sigaction(SIGFPE, &signal_handler, NULL);
    (void)sigaction(SIGSEGV, &signal_handler, NULL);
}

#else

void mh_crashhandler_init(void)
{
}

#endif
