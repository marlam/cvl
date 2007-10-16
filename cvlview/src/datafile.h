/*
 * datafile.h
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

/**
 * \file datafile.h
 * \brief Data file handling.
 *
 * Data file handling.
 */

#include <cstdio>

#include <cvl/cvl.h>

#include "err.h"


#ifndef DATAFILE_H
#define DATAFILE_H

class DataFile
{
    private:
	char *_filename;
	FILE *_f;
	bool _eof_seen;
	int _known_datasets;
	int _offsets_buflen;
	off_t *_offsets;
	int _index;

    public:
	DataFile(const char *filename) throw (err);
	~DataFile();

	void set_index(int i) throw (err);
	void prev() throw (err);
	void next() throw (err);

	cvl_frame_t *read() throw (err);

	int index();
	int total();
};

#endif
