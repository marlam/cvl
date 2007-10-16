/*
 * frame_info.h
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

#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include "config.h"

#include <QWidget>
#include <QLabel>

#include <cvl/cvl.h>

#include "datafile.h"


class FrameInfo : public QWidget
{
    Q_OBJECT
	
    private:
	DataFile **_datafile;
	cvl_frame_t **_frame;
	QLabel *_line0;
	QLabel *_line1;
	QLabel *_line2;
	QLabel *_ch_line[4];

    public slots:
	void update();

    public:
	FrameInfo(DataFile **datafile, cvl_frame_t **frame, QWidget *parent = NULL);	
	~FrameInfo();

    signals:
	void make_gl_context_current();
};

#endif
