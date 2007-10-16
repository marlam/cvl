/*
 * frame_info.cpp
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

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include <cvl/cvl.h>

#include "mh.h"

#include "datafile.h"
#include "frame_info.h"


FrameInfo::FrameInfo(DataFile **datafile, cvl_frame_t **frame, QWidget *parent) 
	: QWidget(parent)
{
    _datafile = datafile;
    _frame = frame;

    QGridLayout *layout = new QGridLayout;
    
    _line0 = new QLabel("Data set: ");
    layout->addWidget(_line0, 0, 0);
    _line1 = new QLabel("Size: ");
    layout->addWidget(_line1, 1, 0);
    _line2 = new QLabel("Data: ");
    layout->addWidget(_line2, 2, 0);
    for (int c = 0; c < 4; c++)
    {
	_ch_line[c] = new QLabel(mh_string("Channel %d:", c).c_str());
	layout->addWidget(_ch_line[c], 3 + c, 0);
    }

    layout->setRowStretch(7, 1);
    setLayout(layout);
}

FrameInfo::~FrameInfo()
{
}

void FrameInfo::update()
{
    if (_frame && *_frame)
    {
	emit make_gl_context_current();

	if ((*_datafile)->total() > 0)
	{
	    _line0->setText(mh_string("Data set: %d / %d", (*_datafile)->index(), (*_datafile)->total() - 1).c_str());
	}
	else
	{
	    _line0->setText(mh_string("Data set: %d", (*_datafile)->index()).c_str());
	}
	_line1->setText(mh_string("Size: %d x %d", cvl_frame_width(*_frame), cvl_frame_height(*_frame)).c_str());
	_line2->setText(mh_string("Data: %s, %s", 
		cvl_frame_format(*_frame) == CVL_LUM ? "Luminance" : 
		cvl_frame_format(*_frame) == CVL_UNKNOWN ? "Unknown" : "Color",
		cvl_frame_type(*_frame) == CVL_FLOAT ? "floating point": "integer").c_str());

	for (int c = 0; c < cvl_frame_channels(*_frame); c++)
	{
	    _ch_line[c]->setText(mh_string("Channel %d: %s", c, cvl_frame_channel_name(*_frame, c)).c_str());
	    _ch_line[c]->setEnabled(true);
	}
	for (int c = cvl_frame_channels(*_frame); c < 4; c++)
	{
	    _ch_line[c]->setText(mh_string("Channel %d:", c).c_str());
	    _ch_line[c]->setEnabled(false);
	}
    }
}
