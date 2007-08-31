/*
 * mh_timer.h
 * 
 * This file is part of cvtool, a computer vision tool.
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

#ifndef MH_TIMER_H
#define MH_TIMER_H

#ifdef W32_NATIVE
# include <time.h>
# include <sys/timeb.h>
#else
# include <unistd.h>
# include <sys/times.h>
# include <time.h>
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * \typedef mh_timer_type_t
 * Type of the timer.
 */
/** \var MH_TIMER_CPU
 * CPU time. */
/** \var MH_TIMER_REAL
 * Real time. */

typedef enum { MH_TIMER_CPU, MH_TIMER_REAL } mh_timer_type_t;

#ifdef W32_NATIVE
typedef struct _timeb mh_timer_t;
#else
typedef struct timespec mh_timer_t;
#endif

/**
 * \param type		Timing type.
 * \param timer		Pointer to a timer.
 *
 * Sets the given \a timer of the given \a type.
 */
void mh_timer_set(mh_timer_type_t type, mh_timer_t *timer);

/**
 * \param start		The start timer.
 * \param stop		The stop timer.
 *
 * Returns the time in seconds that has passed between the setting of \a start
 * and \a stop.
 */
double mh_timer_get(const mh_timer_t *start, const mh_timer_t *stop);

#ifdef __cplusplus
}
#endif

#endif
