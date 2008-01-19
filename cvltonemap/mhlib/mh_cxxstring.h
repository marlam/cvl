/*
 * mh_cxxstring.h
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

#ifndef MH_CXXSTRING_H
#define MH_CXXSTRING_H

#ifdef __cplusplus

#include <cstdarg>
#include <string>

#ifdef __GNUC__
std::string mh_string(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#else
std::string mh_string(const char *format, ...);
#endif

std::string &mh_string_replace(std::string &str, const std::string &s, const std::string &r);

#endif

#endif
