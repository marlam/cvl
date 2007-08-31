/*
 * conf.cpp
 *
 * This file is part of cvlview, an image viewer using the CVL library.
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

#include "config.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <string>
#include <limits>
#include <cfloat>

#include "mh.h"

#include "err.h"
#include "conf.h"


void Conf::dump() const
{
    mh_msg_DBG("Conf dump: size = %d, length = %d", _size, _length);
    for (int i = 0; i < _length; i++)
	mh_msg_DBG("Conf dump: item %2d (cruft = %d): %s=%s", 
		i + 1, _cruft[i] ? 1 : 0, _name[i], _value[i]);
}

void Conf::erase()
{
    for (int i = 0; i < _length; i++)
    {
	free(_name[i]);
	free(_value[i]);
    }
    _length = 0;
    _size = 0;
    free(_name);
    _name = NULL;
    free(_value);
    _value = NULL;
    free(_cruft);
    _cruft = NULL;
}

Conf::Conf()
{
    _length = 0;
    _size = 0;
    _name = NULL;
    _value = NULL;
    _cruft = NULL;
}

Conf::~Conf()
{
    erase();
}

void Conf::load(const char *filename) throw (err)
{
    erase();

    FILE *f;
    if (!(f = fopen(filename, "r")))
	throw err(err::ERR_IO, "Opening " + string(filename) + " failed", strerror(errno));

    bool error = false;
    char linebuf[4096];
    while (fgets(linebuf, 4096, f))
    {
	int linelen = strlen(linebuf);
	if (linelen == 0 || linebuf[linelen - 1] != '\n')
	{
	    error = true;
	    break;
	}
	linebuf[--linelen] = '\0';
	char *first_space = strchr(linebuf, ' ');
	if (!first_space || first_space == linebuf || linelen - (first_space - linebuf + 1) == 0)
	{
	    error = true;
	    break;
	}
	*first_space = '\0';
	put(linebuf, first_space + 1);
    }
    if (error)
    {
	erase();
	throw err(err::ERR_IO, "The contents of " + string(filename) + " are invalid", strerror(errno));
    }
    if (ferror(f))
    {
	erase();
	throw err(err::ERR_IO, "Reading from " + string(filename) + " failed", strerror(errno));
    }
    (void)fclose(f);

    for (int i = 0; i < _length; i++)
	_cruft[i] = true;

    mh_msg_DBG("Conf load from %s", filename);
    dump();
}

void Conf::save(const char *filename) const throw (err)
{
    mh_msg_DBG("Conf save to %s", filename);
    dump();
    
    FILE *f;
    if (!(f = fopen(filename, "w")))
	throw err(err::ERR_IO, "Opening " + string(filename) + " failed", strerror(errno));

    for (int i = 0; i < _length; i++)
    {
	fputs(_name[i], f);
	fputc(' ', f);
	fputs(_value[i], f);
	fputc('\n', f);
    }
    if (ferror(f))
	throw err(err::ERR_IO, "Writing to " + string(filename) + " failed", strerror(errno));
    (void)fclose(f);
}

const char *Conf::get(const char *name)
{
    int a = 0;
    int b = _length - 1;

    while (b >= a)
    {
	int c = (a + b) / 2;
	int cmp = strcmp(_name[c], name);
	if (cmp < 0)
	{
	    a = c + 1;
	}
	else if (cmp > 0)
	{
	    b = c - 1;
	}
	else
	{
	    _cruft[c] = false;
	    return _value[c];
	}
    }
    return NULL;    
}
	
void Conf::put(const char *name, const char *value)
{
    int a = 0;
    int b = _length - 1;

    while (b >= a)
    {
	int c = (a + b) / 2;
	int cmp = strcmp(_name[c], name);
	if (cmp < 0)
	{
	    a = c + 1;
	}
	else if (cmp > 0)
	{
	    b = c - 1;
	}
	else
	{
	    _cruft[c] = false;
	    free(_value[c]);
	    _value[c] = mh_strdup(value);
	    return;
	}
    }
    if (_length == _size)
    {
	_size += 50;
	_name = static_cast<char **>(mh_realloc(_name, _size * sizeof(char *)));
	_value = static_cast<char **>(mh_realloc(_value, _size * sizeof(char *)));
	_cruft = static_cast<bool *>(mh_realloc(_cruft, _size * sizeof(bool)));
    }
    for (int i = _length; i > a; i--)
    {
	_name[i] = _name[i - 1];
	_value[i] = _value[i - 1];
	_cruft[i] = _cruft[i - 1];
    }
    _name[a] = mh_strdup(name);
    _value[a] = mh_strdup(value);
    _cruft[a] = false;
    _length++;
}

void Conf::remove_cruft()
{
    for (int i = _length - 1; i >= 0; i--)
    {
	if (_cruft[i])
	{
	    free(_name[i]);
    	    free(_value[i]);
	    for (int j = i; j < _length - 1; j++)
	    {
		_name[j] = _name[j + 1];
		_value[j] = _value[j + 1];
		_cruft[j] = _cruft[j + 1];
	    }
	    _length--;
	}
    }
}

/* Convenience functions. */

const char *Conf::get(const char *name, const char *safe_default)
{
    const char *val = get(name);
    return val ? val : safe_default;
}

int Conf::get(const char *name, int min, int max, int safe_default)
{
    const char *str = get(name);
    if (!str)
	return safe_default;
    errno = 0;
    char *p;
    long value = strtol(str, &p, 0);
    if (p == str || *p != '\0' || errno == ERANGE || value < min || value > max)
	return safe_default;
    return value;
}

int Conf::get(const char *name, int safe_default)
{
    return get(name, INT_MIN, INT_MAX, safe_default);
}

double Conf::get(const char *name, double min, double max, double safe_default)
{
    const char *str = get(name);
    if (!str)
	return safe_default;
    errno = 0;
    char *p;
    double value = strtod(str, &p);
    if (p == str || *p != '\0' || errno == ERANGE || value < min || value > max)
	return safe_default;
    return value;
}

double Conf::get(const char *name, double safe_default)
{
    return get(name, -DBL_MAX, DBL_MAX, safe_default);
}

bool Conf::get(const char *name, bool safe_default)
{
    const char *str = get(name);
    if (!str)
	return safe_default;
    if (strcmp(str, "true") == 0 || strcmp(str, "on") == 0 || strcmp(str, "1") == 0)
	return true;
    else if (strcmp(str, "false") == 0 || strcmp(str, "off") == 0 || strcmp(str, "0") == 0)
	return false;
    else
	return safe_default;
}

void Conf::put(const char *name, int value)
{
    char *str = mh_asprintf("%d", value);
    put(name, str);
    free(str);
}

void Conf::put(const char *name, double value)
{
    char *str = mh_asprintf("%.15f", value);
    put(name, str);
    free(str);
}

void Conf::put(const char *name, bool value)
{
    put(name, value ? "true" : "false");
}
