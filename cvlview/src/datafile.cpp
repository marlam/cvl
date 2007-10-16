/*
 * datafile.cpp
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

#include <cvl/cvl.h>

#include "mh.h"

#include "err.h"
#include "datafile.h"


DataFile::DataFile(const char *filename) throw (err)
{
    int c;

    if (!(_f = fopen(filename, "r")))
    {
	throw err(err::ERR_IO, "Opening " + string(filename) + " failed", strerror(errno));
    }
    if ((c = fgetc(_f)) == EOF || ungetc(c, _f) == EOF)
    {
	fclose(_f);
	throw err(err::ERR_IO, "Reading from " + string(filename) + " failed", 
		ferror(_f) ? "input error" : "file is empty");
    }
    _eof_seen = false;
    _filename = mh_strdup(filename);
    _known_datasets = 1;
    _offsets_buflen = 3;
    _offsets = static_cast<off_t *>(mh_alloc(_offsets_buflen * sizeof(off_t)));
    _offsets[0] = 0;
    _index = 0;
}

DataFile::~DataFile()
{
    free(_filename);
    fclose(_f);
    free(_offsets);
}

void DataFile::set_index(int i) throw (err)
{
    _index = mh_clampi(i, 0, _known_datasets - 1);
    if (fseeko(_f, _offsets[_index], SEEK_SET) != 0)
    {
	throw err(err::ERR_IO, "Seeking in " + string(_filename) + " failed", strerror(errno));
    }
}

void DataFile::prev() throw (err)
{
    set_index(_index - 1);
}

void DataFile::next() throw (err)
{
    set_index(_index  + 1);
}

cvl_frame_t *DataFile::read() throw (err)
{
    cvl_frame_t *frame;
    cvl_read(_f, NULL, &frame);
    if (cvl_error())
    {
	throw err(err::ERR_IO, "Reading from " + string(_filename) + " failed", cvl_error_msg());
    }
    else if (!frame)
    {
	_eof_seen = true;
	return NULL;
    }
    else
    {
	_index++;
	if (_index >= _known_datasets)
	{
	    _known_datasets++;
	}
	if (_offsets_buflen < _known_datasets)
	{
	    _offsets_buflen += 3;
	    _offsets = static_cast<off_t *>(mh_realloc(_offsets, _offsets_buflen * sizeof(off_t)));
	}
	if ((_offsets[_index] = ftello(_f)) < 0)
	{
	    throw err(err::ERR_IO, "Reading from " + string(_filename) + " failed", strerror(errno));
	}		
	return frame;
    }
}

int DataFile::index()
{
    return _index;
}

int DataFile::total()
{
    return (_eof_seen ? _known_datasets : -1);
}
