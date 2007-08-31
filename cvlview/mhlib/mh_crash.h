/*
 * mh_crash.h
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

#ifndef MH_CRASH_H
#define MH_CRASH_H

#ifdef __cplusplus
extern "C" 
{
#endif

extern void (*mh_failure_handler)(void);
void mh_set_failurehandler(void (*failure_handler)(void));
void mh_print_backtrace(void);
void mh_crash(void);
void mh_crashhandler(int signum);
void mh_crashhandler_init(void);

#ifdef __cplusplus
}
#endif

#endif
