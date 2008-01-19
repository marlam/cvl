/*
 * err.h
 *
 * This file is part of cvlview, an image viewer using the CVL library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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
 * \file err.h
 *
 * Error and exception handling.
 */

#ifndef ERR_H
#define ERR_H

#include <exception>
#include <string>
#include <fstream>
using namespace std;

#include "mh.h"


/**
 * Error and exception handling.
 */
class err : exception
{
    public:
	/** Error type. */
 	typedef enum
	{
	    /** No error. */
	    ERR_OK,
	    /** Input/output error. */
	    ERR_IO,
	    /** Data error. */
	    ERR_INVALID,
	    /** Overflow error. */
	    ERR_OVERFLOW,
	    /** OpenGL error. */
	    ERR_GLFAILED,
	    /** Library/software failure. */
	    ERR_LIBFAILED
	} error_type_t;

    private:
	error_type_t _errtype;
	string _errstr_what;
	string _errstr_why;

    public:

	/**
	 * \param errtype	Type of the error.
	 * \param what		What went wrong.
	 * \param why		Why went it wrong.
	 */
	err(error_type_t errtype = ERR_OK, string what = "", string why = "");
	~err() throw() {}
	
	/**
	 * \return	The type of the error.
	 */
	error_type_t errtype() const
	{
	    return _errtype;
	}

	/**
	 * \return	The type of the error as a string.
	 */
	string errtype_string() const;

	/**
	 * \return	What went wrong.
	 */
	string errstr_what() const
	{
	    return _errstr_what;
	}
	
	/**
	 * \return	Why it went wrong.
	 */
	string errstr_why() const
	{
	    return _errstr_why;
	}

	/**
	 * \return	A human readable error message describing this error.
	 */
	string msg() const;

    friend ostream& operator<<(ostream &os, const err &e);
};

#endif
