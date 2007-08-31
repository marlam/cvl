/*
 * mh_string.h
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

#ifndef MH_STRING_H
#define MH_STRING_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" 
{
#endif

char *mh_str_replace(char *str, const char *s, const char *r);
char *mh_str_part(const char *s, size_t i, size_t l);
char *mh_str_cat(char *a, const char *b);

#ifdef __cplusplus
}
#endif

#endif
