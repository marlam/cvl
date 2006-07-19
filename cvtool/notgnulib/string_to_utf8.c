/*
 * string_to_utf8.c
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

#include <stdlib.h>
#include <locale.h>

#include "iconvme.h"
#include "localcharset.h"
#include "xalloc.h"


/* Converts a string from the character set of the users locale to UTF-8.
 * This function returns a pointer to an allocated string, or NULL if the
 * conversion fails and default_value is NULL. If the conversion fails and
 * default_value is not NULL, then the allocated string will be a duplicate of
 * the default_value string.
 * This function tries to determine the users character set regardless of the
 * current locale settings of this process, so that it works as expected for
 * programs that do not use setlocale() at all.
 */
char *string_to_utf8(const char *s, const char *default_value)
{
    char *locale_bak;
    char *result;
    
    locale_bak = setlocale(LC_CTYPE, NULL);
    if (locale_bak)
    {
	locale_bak = xstrdup(locale_bak);
	setlocale(LC_CTYPE, "");
    }
    if (!(result = iconv_string(s, locale_charset(), "UTF-8")))
    {
	if (default_value)
	{
	    result = xstrdup(default_value);
	}
    }
    if (locale_bak)
    {
	setlocale(LC_CTYPE, locale_bak);
	free(locale_bak);
    }
    return result;
}
