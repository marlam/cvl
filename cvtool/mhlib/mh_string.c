/*
 * mh_string.c
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

/** 
 * \file mh_string.h
 * \brief Working with C strings.
 *
 * Working with C strings.
 */

#include <string.h>
#include <stdlib.h>

#include "mh_alloc.h"
#include "mh_string.h"


/**
 * \param s	The string in which the replacements will be made.
 * \param a	The string that will be replaced.
 * \param b	The replacement for the string a.
 * \return	The resulting string.
 *
 * Replaces all occurences of \a a in \a s with \a b.
 * \a s must be an allocated string. Its new value is returned.
 */
char *mh_str_replace(char *s, const char *a, const char *b)
{
    char *p;
    size_t next_pos = 0;
    size_t slen = strlen(s);
    size_t alen = strlen(a);
    size_t blen = strlen(b);

    if (slen == 0 || alen == 0)
	return s;
    while ((p = strstr(s + next_pos, a)))
    {
	size_t i = p - s;
	slen = slen - alen + blen;
	if (blen < alen)
	{
	    for (size_t j = i + blen; j < slen + 1; j++)
		s[j] = s[j + (alen - blen)];
	    s = mh_realloc(s, (slen + 1) * sizeof(char));
	}
	else if (blen > alen)
	{
	    s = mh_realloc(s, (slen + 1) * sizeof(char));
	    for (size_t j = slen; j >= i + blen - alen; j--)
		s[j] = s[j - (blen - alen)];
	}
	strncpy(s + i, b, blen);
	next_pos = i + blen;
    }
    return s;
}

/**
 * \param s		The string from which a part is copied.
 * \param i		The start index of the part.
 * \param l		The length of the part.
 *
 * Copies the the part from \a s that starts at index \a i and has length \a l
 * and returns it as an allocated string.
 */
char *mh_str_part(const char *s, size_t i, size_t l)
{
    char *p = mh_alloc((l + 1) * sizeof(char));
    strncpy(p, s + i, l);
    p[l] = '\0';
    return p;
}

/**
 * \param a		A string.
 * \param b		A string.
 * \return		The concatenation of a and b.
 *
 * Concatenates the string \a b to the string \a a and returns the
 * new values of \a a in an allocated string.\n
 * \a a must be an allocated string, or NULL.
 */
char *mh_str_cat(char *a, const char *b)
{
    size_t alen = a ? strlen(a) : 0;
    size_t blen = strlen(b);
    a = mh_realloc(a, (alen + blen + 1) * sizeof(char));
    strcpy(a + alen, b);
    return a;
}
