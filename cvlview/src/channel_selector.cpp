/*
 * channel_selector.cpp
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

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>

#include <cvl/cvl.h>

#include "channel_selector.h"


ChannelSelector::ChannelSelector(cvl_frame_t **frame, QWidget *parent) 
	: QWidget(parent)
{
    _frame = frame;
    _channel = 0;
    _reset_on_next_update = true;

    QGridLayout *layout = new QGridLayout;
    
    _channel_button[0] = new QPushButton(tr("&0"));
    _channel_button[0]->setShortcut(tr("0"));
    layout->addWidget(_channel_button[0], 0, 0);
    _channel_button[1] = new QPushButton(tr("&1"));
    _channel_button[1]->setShortcut(tr("1"));
    layout->addWidget(_channel_button[1], 0, 1);
    _channel_button[2] = new QPushButton(tr("&2"));
    _channel_button[2]->setShortcut(tr("2"));
    layout->addWidget(_channel_button[2], 0, 2);
    _channel_button[3] = new QPushButton(tr("&3"));
    _channel_button[3]->setShortcut(tr("3"));
    layout->addWidget(_channel_button[3], 0, 3);
    _color_button = new QPushButton(tr("&Color"));
    _color_button->setShortcut(tr("C"));
    layout->addWidget(_color_button, 0, 4, 1, 2);

    for (int c = 0; c < 4; c++)
    {
	_channel_button[c]->setAutoDefault(false);
	_channel_button[c]->setCheckable(true);
    }
    _color_button->setAutoDefault(false);
    _color_button->setCheckable(true);

    connect(_channel_button[0], SIGNAL(clicked()), this, SLOT(ch0_button_clicked()));
    connect(_channel_button[1], SIGNAL(clicked()), this, SLOT(ch1_button_clicked()));
    connect(_channel_button[2], SIGNAL(clicked()), this, SLOT(ch2_button_clicked()));
    connect(_channel_button[3], SIGNAL(clicked()), this, SLOT(ch3_button_clicked()));
    connect(_color_button, SIGNAL(clicked()), this, SLOT(color_button_clicked()));

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

ChannelSelector::~ChannelSelector()
{
}

void ChannelSelector::reset()
{
    _reset_on_next_update = true;
}

void ChannelSelector::ch0_button_clicked()
{
    if (_channel_button[0]->isChecked())
    {
	_channel_button[1]->setChecked(false);
	_channel_button[2]->setChecked(false);
	_channel_button[3]->setChecked(false);
	_color_button->setChecked(false);
	_channel = 0;
	emit channel_changed();
    }
    else
    {
	_channel_button[0]->setChecked(true);
    }
}

void ChannelSelector::ch1_button_clicked()
{
    if (_channel_button[1]->isChecked())
    {
	_channel_button[0]->setChecked(false);
	_channel_button[2]->setChecked(false);
	_channel_button[3]->setChecked(false);
	_color_button->setChecked(false);
	_channel = 1;
	emit channel_changed();
    }
    else
    {
	_channel_button[1]->setChecked(true);
    }
}

void ChannelSelector::ch2_button_clicked()
{
    if (_channel_button[2]->isChecked())
    {
	_channel_button[0]->setChecked(false);
	_channel_button[1]->setChecked(false);
	_channel_button[3]->setChecked(false);
	_color_button->setChecked(false);
	_channel = 2;
	emit channel_changed();
    }
    else
    {
	_channel_button[2]->setChecked(true);
    }
}

void ChannelSelector::ch3_button_clicked()
{
    if (_channel_button[3]->isChecked())
    {
	_channel_button[0]->setChecked(false);
	_channel_button[1]->setChecked(false);
	_channel_button[2]->setChecked(false);
	_color_button->setChecked(false);
	_channel = 3;
	emit channel_changed();
    }
    else
    {
	_channel_button[3]->setChecked(true);
    }
}

void ChannelSelector::color_button_clicked()
{
    if (_color_button->isChecked())
    {
	_channel_button[0]->setChecked(false);
	_channel_button[1]->setChecked(false);
	_channel_button[2]->setChecked(false);
	_channel_button[3]->setChecked(false);
	_channel = -1;
	emit channel_changed();
    }
    else
    {
	_color_button->setChecked(true);
    }
}

void ChannelSelector::update()
{
    if (_frame && *_frame)
    {
	emit make_gl_context_current();

	for (int c = 0; c < cvl_frame_channels(*_frame); c++)
	{
	    _channel_button[c]->setEnabled(true);
	}
	for (int c = cvl_frame_channels(*_frame); c < 4; c++)
	{
	    _channel_button[c]->setEnabled(false);
	}
	_color_button->setEnabled(cvl_frame_format(*_frame) != CVL_LUM && cvl_frame_format(*_frame) != CVL_UNKNOWN);
	if (_reset_on_next_update 
		|| (_channel == -1 && !_color_button->isEnabled())
		|| (_channel >= 0 && !_channel_button[_channel]->isEnabled()))
	{
	    for (int c = 0; c < 4; c++)
	    {
		_channel_button[c]->setChecked(false);
	    }
	    _color_button->setChecked(false);
	    if (_color_button->isEnabled())
	    {
		_color_button->setChecked(true);
		color_button_clicked();
	    }
	    else
	    {
		_channel_button[0]->setChecked(true);
		ch0_button_clicked();
	    }
	}
	_reset_on_next_update = false;
    }
}
