/*
 * err.cpp
 *
 * This file is part of cvlview, an image viewer using the CVL library.
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

#include "config.h"

#include <string>
#include <exception>
#include <fstream>

#include "mh.h"

#include "err.h"


err::err(error_type_t errtype, string what, string why)
    : _errtype(errtype), _errstr_what(what), _errstr_why(why)
{
    mh_msg_fmt_wrn("Exception %s: %s", this->errtype_string().c_str(), this->msg().c_str());
}

string err::errtype_string() const
{
    switch(_errtype)
    {
	case ERR_OK:
	    return "ERR_OK";
	case ERR_IO:
	    return "ERR_IO";
	case ERR_INVALID:
	    return "ERR_INVALID";
	case ERR_OVERFLOW:
	    return "ERR_OVERFLOW";
	case ERR_GLFAILED:
	    return "ERR_GLFAILED";
	case ERR_LIBFAILED:
	    return "ERR_LIBFAILED";
	default:
	    mh_msg_wrn("%s: unknown errtype", __func__);
	    return "?";
    }
}

string err::msg() const
{
    string s = "";

    if (_errstr_what != "" && _errstr_why != "")
    {
	s += _errstr_what + ": " + _errstr_why;
    }
    else if (_errstr_what != "" && _errstr_why == "")
    {
	s += _errstr_what;
    }
    else if (_errstr_what == "" && _errstr_why != "")
    {
	s += _errstr_why;
    }
    return s;
}

ostream &operator<<(ostream &os, const err &e)
{
    os << e.msg();
    return os;
}
