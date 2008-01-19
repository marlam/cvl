/*
 * mh_timer.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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
 * \file mh_timer.h
 * \brief Measure CPU and real time.
 *
 * Functions to measure the amount of CPU and real time that a process consumes.
 */

#include "config.h"

#ifdef W32_NATIVE
# include <time.h>
# include <sys/timeb.h>
#else
# include <unistd.h>
# include <sys/times.h>
# include <time.h>
#endif

#include "mh_timer.h"

/**
 * \param type		The timer type.
 * \param timer		The timer.
 *
 * Sets \a timer to the of the given timer type.
 */
void mh_timer_set(mh_timer_type_t type, mh_timer_t *timer)
{
#ifdef W32_NATIVE
    if (type == MH_TIMER_CPU)
    {
	clock_t c = clock();
	timer->time = c / CLOCKS_PER_SEC;
	timer->millitm = ((c - timer->time * CLOCKS_PER_SEC) * 1000) / CLOCKS_PER_SEC;
    }
    else
    {
	_ftime(timer);
    }
#else
    if (type == MH_TIMER_CPU)
    {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, timer);
    }
    else
    {
	clock_gettime(CLOCK_MONOTONIC, timer);
    }
#endif
}

double mh_timer_get(const mh_timer_t *start, const mh_timer_t *stop)
{
#ifdef W32_NATIVE
    return (double)(stop->time - start->time) 
	+ (double)(stop->millitm - start->millitm) / 1000.0;
#else
    return (double)(stop->tv_sec - start->tv_sec) 
	+ (double)(stop->tv_nsec - start->tv_nsec) / 1000000000.0;
#endif
}
