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

#include "heightmap_selector.h"


HeightmapSelector::HeightmapSelector(cvl_frame_t **frame, QWidget *parent) : QWidget(parent)
{
    _lock = false;
    _frame = frame;
    QGridLayout *layout = new QGridLayout;

    _enable_box = new QCheckBox("Use height map");
    _enable_box->setCheckState(Qt::Unchecked);
    connect(_enable_box, SIGNAL(stateChanged(int)), this, SLOT(_set_enable(int)));
    layout->addWidget(_enable_box, 0, 0, 1, 4);

    QLabel *heightmap_label = new QLabel("Height map channel:");
    layout->addWidget(heightmap_label, 1, 0, 1, 4);
    _ch0_button = new QRadioButton("0");
    connect(_ch0_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch0_button, 2, 0, 1, 1);
    _ch1_button = new QRadioButton("1");
    connect(_ch1_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch1_button, 2, 1, 1, 1);
    _ch2_button = new QRadioButton("2");
    connect(_ch2_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch2_button, 2, 2, 1, 1);
    _ch3_button = new QRadioButton("3");
    connect(_ch3_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch3_button, 2, 3, 1, 1);
    QButtonGroup *heightmap_group = new QButtonGroup();
    heightmap_group->addButton(_ch0_button);
    heightmap_group->addButton(_ch1_button);
    heightmap_group->addButton(_ch2_button);
    heightmap_group->addButton(_ch3_button);
    _ch0_button->setChecked(true);

    QLabel *mode_label = new QLabel("View mode:");
    layout->addWidget(mode_label, 3, 0, 1, 4);
    _quads_button = new QRadioButton("Quads");
    connect(_quads_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_quads_button, 4, 0, 1, 2);
    _strip_button = new QRadioButton("Surface");
    connect(_strip_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_strip_button, 4, 2, 1, 2);
    QButtonGroup *mode_group = new QButtonGroup();
    mode_group->addButton(_quads_button);
    mode_group->addButton(_strip_button);
    _quads_button->setChecked(true);

    QLabel *height_factor_label = new QLabel("Height factor:");
    layout->addWidget(height_factor_label, 5, 0, 1, 2);
    _height_factor_spinbox = new QDoubleSpinBox();
    _height_factor_spinbox->setRange(0.0000, 99.9999);
    _height_factor_spinbox->setSingleStep(0.01);
    _height_factor_spinbox->setDecimals(4);
    _height_factor_spinbox->setValue(1.0);
    connect(_height_factor_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_height_factor(double)));
    layout->addWidget(_height_factor_spinbox, 5, 2, 1, 2);
    _height_factor_slider = new QSlider(Qt::Horizontal, this);
    _height_factor_slider->setRange(-460517, +460517);	// exp(+4.60517) = 99.9999
    _height_factor_slider->setValue(0);
    connect(_height_factor_slider, SIGNAL(valueChanged(int)), this, SLOT(_height_factor_slider_changed(int)));
    layout->addWidget(_height_factor_slider, 6, 0, 1, 4);

    layout->setRowStretch(7, 1);
    setLayout(layout);
}

HeightmapSelector::~HeightmapSelector()
{
}

void HeightmapSelector::_set_enable(int e UNUSED)
{
    emit heightmap_changed();
}

void HeightmapSelector::_button_clicked()
{
    emit heightmap_changed();
}

void HeightmapSelector::_set_height_factor(double f)
{
    _lock = true;
    _height_factor_slider->setValue(mh_iround(log(mh_max(f, 0.00001)) * 100000.0));
    _lock = false;
    emit heightmap_changed();
}

void HeightmapSelector::_height_factor_slider_changed(int f)
{
    if (!_lock)
    {
	_height_factor_spinbox->setValue(mh_clamp(exp(static_cast<double>(f) / 100000.0), 0.0000, 99.9999));
    }
}

void HeightmapSelector::update()
{
    emit make_gl_context_current();
    int active_channel = channel();
    int channels = (*_frame) ? cvl_frame_channels(*_frame) : 0;
    _ch0_button->setEnabled(channels >= 1);
    _ch1_button->setEnabled(channels >= 2);
    _ch2_button->setEnabled(channels >= 3);
    _ch3_button->setEnabled(channels >= 4);
    if (active_channel == 3 && channels >= 4)
    {
	_ch3_button->setChecked(true);
    }
    else if (active_channel == 2 && channels >= 3)
    {
	_ch2_button->setChecked(true);
    }
    else if (active_channel == 1 && channels >= 2)
    {
	_ch1_button->setChecked(true);
    }
    else
    {
	_ch0_button->setChecked(true);
    }
}

void HeightmapSelector::reset()
{
    _enable_box->setCheckState(Qt::Unchecked);
    update();
}
