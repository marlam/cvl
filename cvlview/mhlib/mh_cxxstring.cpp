/*
 * mh_cxxstring.cpp
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
 * \file mh_cxxstring.h
 * \brief Working with C++ strings.
 * 
 * Working with C++ strings.
 */

#ifdef __cplusplus

#include "config.h"

#include <cstdarg>
#include <cstdlib>
#include <string>

#include "mh_alloc.h"
#include "mh_cxxstring.h"


/**
 * \param format	The printf() style format argument.
 * \param ...		Variable argument list.
 * \return		A C++ string object.
 */
std::string mh_string(const char *format, ...)
{
    char *c_str;
    va_list args;
    va_start(args, format);
    c_str = mh_vasprintf(format, args);
    va_end(args);
    std::string s(c_str);
    free(c_str);
    return s;
}

/**
 * \param str	The string in which the replacements will be made.
 * \param s	The string that will be replaced.
 * \param r	The replacement for the string s.
 * \return	The resulting string.
 *
 * Replaces all occurences of \a s in \a str with \a r.
 * Returns \a str.
 */
std::string &mh_string_replace(std::string &str, const std::string &s, const std::string &r)
{
    size_t s_len = s.length();
    size_t r_len = r.length();
    size_t p = 0;
    
    while ((p = str.find(s, p)) != std::string::npos)
    {
	str.replace(p, s_len, r);
	p += r_len;
    }
    return str;
}

#endif
