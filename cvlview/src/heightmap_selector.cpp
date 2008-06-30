/*
 * heightmap_selector.cpp
 *
 * This file is part of cvlview, an image viewer using the CVL library.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QSlider>

#include <cvl/cvl.h>

#include "mh.h"

#include "color_selector.h"
#include "heightmap_selector.h"


HeightmapSelector::HeightmapSelector(cvl_frame_t **frame, QWidget *parent) : QWidget(parent)
{
    _frame = frame;
    _reset_on_next_update = true;
    _lock = false;
    QGridLayout *layout = new QGridLayout;

    _enable_box = new QCheckBox("Enable 3D View");
    _enable_box->setCheckState(Qt::Unchecked);
    connect(_enable_box, SIGNAL(stateChanged(int)), this, SLOT(_button_clicked()));
    layout->addWidget(_enable_box, 0, 0, 1, 6);

    QLabel *mode_label = new QLabel("Mode:");
    layout->addWidget(mode_label, 1, 0, 1, 2);
    _quads_button = new QRadioButton("Quads");
    connect(_quads_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_quads_button, 1, 2, 1, 2);
    _strip_button = new QRadioButton("Surface");
    connect(_strip_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_strip_button, 1, 4, 1, 2);
    QButtonGroup *mode_group = new QButtonGroup();
    mode_group->addButton(_quads_button);
    mode_group->addButton(_strip_button);
    _quads_button->setChecked(true);

    QLabel *channel_label = new QLabel("Z channel:");
    layout->addWidget(channel_label, 2, 0, 1, 2);
    _channel_button[0] = new QRadioButton("0");
    connect(_channel_button[0], SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_channel_button[0], 2, 2, 1, 1);
    _channel_button[1] = new QRadioButton("1");
    connect(_channel_button[1], SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_channel_button[1], 2, 3, 1, 1);
    _channel_button[2] = new QRadioButton("2");
    connect(_channel_button[2], SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_channel_button[2], 2, 4, 1, 1);
    _channel_button[3] = new QRadioButton("3");
    connect(_channel_button[3], SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_channel_button[3], 2, 5, 1, 1);
    QButtonGroup *heightmap_group = new QButtonGroup();
    heightmap_group->addButton(_channel_button[0]);
    heightmap_group->addButton(_channel_button[1]);
    heightmap_group->addButton(_channel_button[2]);
    heightmap_group->addButton(_channel_button[3]);
    _channel_button[0]->setChecked(true);

    QLabel *data_label = new QLabel("Z data type:");
    layout->addWidget(data_label, 3, 0, 1, 2);
    _height_button = new QRadioButton("Height");
    connect(_height_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_height_button, 3, 2, 1, 2);
    _distance_button = new QRadioButton("Distance");
    connect(_distance_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_distance_button, 3, 4, 1, 2);
    QButtonGroup *data_group = new QButtonGroup();
    data_group->addButton(_height_button);
    data_group->addButton(_distance_button);
    _height_button->setChecked(true);

    QLabel *height_factor_label = new QLabel("Height factor:");
    layout->addWidget(height_factor_label, 4, 0, 1, 2);
    _height_factor_spinbox = new QDoubleSpinBox();
    _height_factor_spinbox->setSingleStep(0.001);
    _height_factor_spinbox->setDecimals(3);
    _height_factor_spinbox->setRange(0.000, 9.999);
    _height_factor_spinbox->setValue(1.0);
    connect(_height_factor_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_height_factor(double)));
    layout->addWidget(_height_factor_spinbox, 4, 2, 1, 4);
    _height_factor_slider = new QSlider(Qt::Horizontal, this);
    _height_factor_slider->setRange(0, 1042);
    _height_factor_slider->setValue(301);
    connect(_height_factor_slider, SIGNAL(valueChanged(int)), this, SLOT(_height_factor_slider_changed(int)));
    layout->addWidget(_height_factor_slider, 5, 0, 1, 6);

    _showcuboid_box = new QCheckBox("Show Cuboid");
    _showcuboid_box->setCheckState(Qt::Checked);
    connect(_showcuboid_box, SIGNAL(stateChanged(int)), this, SLOT(_button_clicked()));
    layout->addWidget(_showcuboid_box, 6, 0, 1, 4);
    _color_selector = new ColorSelector(1.0f, 1.0f, 1.0f, this);
    layout->addWidget(_color_selector, 6, 4, 1, 2);

    layout->setRowStretch(7, 1);
    setLayout(layout);
}

HeightmapSelector::~HeightmapSelector()
{
}

void HeightmapSelector::_button_clicked()
{
    emit heightmap_changed();
}

void HeightmapSelector::_set_height_factor(double f)
{
    _lock = true;
    _height_factor_slider->setValue(mh_iround(log10(f + 1.0) * 1000.0));
   _lock = false;
    emit heightmap_changed();
}

void HeightmapSelector::_height_factor_slider_changed(int f)
{
    if (!_lock)
    {
	_height_factor_spinbox->setValue(pow(10.0, static_cast<double>(f / 1000.0)) - 1.0);
    }
}

void HeightmapSelector::update()
{
    emit make_gl_context_current();
    if (_reset_on_next_update)
    {
	_enable_box->setCheckState(Qt::Unchecked);
	_quads_button->setChecked(true);
	_height_factor_spinbox->setValue(1.0);
	_showcuboid_box->setChecked(true);
	_color_selector->set_color(1.0f, 1.0f, 1.0f);

	if (!*_frame)
	{
	    _channel_button[0]->setEnabled(false);
	    _channel_button[1]->setEnabled(false);
	    _channel_button[2]->setEnabled(false);
	    _channel_button[3]->setEnabled(false);
	    _channel_button[0]->setChecked(true);
	    _height_button->setChecked(true);
	}
	else
	{
	    int channels = cvl_frame_channels(*_frame);
	    _channel_button[0]->setEnabled(channels >= 1);
	    _channel_button[1]->setEnabled(channels >= 2);
	    _channel_button[2]->setEnabled(channels >= 3);
	    _channel_button[3]->setEnabled(channels >= 4);
	    bool found_channel = false;
	    for (int c = channels - 1; c >= 0 ; c--)
	    {
		if (cvl_frame_channel_name(*_frame, c))
		{
		    if (strstr(cvl_frame_channel_name(*_frame, c), "HEIGHT"))
		    {
			_channel_button[c]->setChecked(true);
			_height_button->setChecked(true);
			found_channel = true;
			break;
		    }
		    else if (strstr(cvl_frame_channel_name(*_frame, c), "DEPTH"))
		    {
			_channel_button[c]->setChecked(true);
			_distance_button->setChecked(true);
			found_channel = true;
			break;
		    }
		}
	    }
	    if (!found_channel && cvl_frame_format(*_frame) != CVL_XYZ)
	    {
		for (int c = channels - 1; c >= 0 ; c--)
		{
		    if (cvl_frame_channel_name(*_frame, c))
		    {
			if (strcmp(cvl_frame_channel_name(*_frame, c), "Z") == 0)
			{
			    _channel_button[c]->setChecked(true);
			    _distance_button->setChecked(true);
			    found_channel = true;
			    break;
			}
			else if (strcmp(cvl_frame_channel_name(*_frame, c), "A") == 0)
			{
			    _channel_button[c]->setChecked(true);
			    _height_button->setChecked(true);
			    found_channel = true;
			    break;
			}
		    }
		}
	    }
	    if (!found_channel)
	    {
    		_channel_button[0]->setChecked(true);
    		_height_button->setChecked(true);
    	    }
	}
	_reset_on_next_update = false;
    }
    else
    {
	int active_channel = channel();
	int channels = (*_frame) ? cvl_frame_channels(*_frame) : 0;
	_channel_button[0]->setEnabled(channels >= 1);
	_channel_button[1]->setEnabled(channels >= 2);
	_channel_button[2]->setEnabled(channels >= 3);
	_channel_button[3]->setEnabled(channels >= 4);
	if (active_channel == 3 && channels >= 4)
	{
	    _channel_button[3]->setChecked(true);
	}
	else if (active_channel == 2 && channels >= 3)
	{
	    _channel_button[2]->setChecked(true);
	}
	else if (active_channel == 1 && channels >= 2)
	{
	    _channel_button[1]->setChecked(true);
	}
	else
	{
	    _channel_button[0]->setChecked(true);
	}
    }
}

void HeightmapSelector::reset()
{
    _reset_on_next_update = true;
}
