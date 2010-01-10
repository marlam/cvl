/*
 * pixel_info.cpp
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

#include <cstdio>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include "pixel_info.h"


PixelInfo::PixelInfo(QWidget *parent) 
	: QWidget(parent)
{
    QGridLayout *layout = new QGridLayout;

    _pos_label = new QLabel("Pos:");
    _pos_label->setTextFormat(Qt::PlainText);
    _pos_label->setTextInteractionFlags(Qt::NoTextInteraction);
    _pos_label->setEnabled(false);
    layout->addWidget(_pos_label, 0, 0);
    _ch0_label = new QLabel("Ch.0:");
    _ch0_label->setTextFormat(Qt::PlainText);
    _ch0_label->setTextInteractionFlags(Qt::NoTextInteraction);
    _ch0_label->setEnabled(false);
    layout->addWidget(_ch0_label, 0, 1);
    _ch1_label = new QLabel("Ch.1:");
    _ch1_label->setTextFormat(Qt::PlainText);
    _ch1_label->setTextInteractionFlags(Qt::NoTextInteraction);
    _ch1_label->setEnabled(false);
    layout->addWidget(_ch1_label, 0, 2);
    _ch2_label = new QLabel("Ch.2:");
    _ch2_label->setTextFormat(Qt::PlainText);
    _ch2_label->setTextInteractionFlags(Qt::NoTextInteraction);
    _ch2_label->setEnabled(false);
    layout->addWidget(_ch2_label, 0, 3);
    _ch3_label = new QLabel("Ch.3:");
    _ch3_label->setTextFormat(Qt::PlainText);
    _ch3_label->setTextInteractionFlags(Qt::NoTextInteraction);
    _ch3_label->setEnabled(false);
    layout->addWidget(_ch3_label, 0, 4);
    _lum_label = new QLabel("Lum.:");
    _lum_label->setTextFormat(Qt::PlainText);
    _lum_label->setTextInteractionFlags(Qt::NoTextInteraction);
    _lum_label->setEnabled(false);
    layout->addWidget(_lum_label, 0, 5);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

PixelInfo::~PixelInfo()
{
}

void PixelInfo::update(int x, int y, int channels, const float *val, const float *lum)
{
    if (!val)
    {
	_pos_label->setText("Pos:");
	_ch0_label->setText("Ch.0:");
	_ch1_label->setText("Ch.1:");
	_ch2_label->setText("Ch.2:");
	_ch3_label->setText("Ch.3:");
	_lum_label->setText("Lum.:");
    }
    else
    {
	char buf[64];
	snprintf(buf, 64, "Pos: %d, %d", x, y);
	_pos_label->setText(buf);
	_pos_label->setEnabled(true);
	snprintf(buf, 64, "Ch.0: %.6g", val[0]);
	_ch0_label->setText(buf);
	_ch0_label->setEnabled(true);
	if (channels > 1)
	{
	    snprintf(buf, 64, "Ch.1: %.6g", val[1]);
	    _ch1_label->setText(buf);
	    _ch1_label->setEnabled(true);
	}
	else
	{
	    _ch1_label->setText("Ch.1:");
	    _ch1_label->setEnabled(false);
	}
	if (channels > 2)
	{
	    snprintf(buf, 64, "Ch.2: %.6g", val[2]);
	    _ch2_label->setText(buf);
	    _ch2_label->setEnabled(true);
	}
	else
	{
	    _ch2_label->setText("Ch.2:");
	    _ch2_label->setEnabled(false);
	}
	if (channels > 3)
	{
	    snprintf(buf, 64, "Ch.3: %.6g", val[3]);
	    _ch3_label->setText(buf);
	    _ch3_label->setEnabled(true);
	}
	else
	{
	    _ch3_label->setText("Ch.3:");
	    _ch3_label->setEnabled(false);
	}
	if (lum)
	{
	    snprintf(buf, 64, "Lum.: %.6g", *lum);
	    _lum_label->setText(buf);
	    _lum_label->setEnabled(true);
	}
	else
	{
	    _lum_label->setText("Lum.:");
	    _lum_label->setEnabled(false);
	}
    }	
}
