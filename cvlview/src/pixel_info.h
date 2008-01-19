/*
 * pixel_info.h
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

#ifndef PIXEL_INFO_H
#define PIXEL_INFO_H

#include "config.h"

#include <QWidget>
#include <QLabel>


class PixelInfo : public QWidget
{
    Q_OBJECT
	
    private:
	QLabel *_pos_label;
	QLabel *_ch0_label;
	QLabel *_ch1_label;
	QLabel *_ch2_label;
	QLabel *_ch3_label;
	QLabel *_lum_label;

    public:
	PixelInfo(QWidget *parent = NULL);	
	~PixelInfo();

    public slots:
	void update(int x, int y, int channels, const float *val, const float *lum);
};

#endif
