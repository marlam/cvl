/*
 * mh_os.h
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

#ifndef MH_OS_H
#define MH_OS_H

#include <stdio.h>

#ifdef W32_NATIVE
# define MH_DIRSEP '\\'
#else
# define MH_DIRSEP '/'
#endif

#ifdef __cplusplus
extern "C" 
{
#endif

FILE *mh_tempfile(const char *base);
FILE *mh_mktempfile(const char *base, char **filename);

char *mh_get_apprcpath(const char *program_name);

#ifdef __cplusplus
}
#endif

#endif
