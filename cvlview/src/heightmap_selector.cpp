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
    _lock = false;
    _frame = frame;
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
    _ch0_button = new QRadioButton("0");
    connect(_ch0_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch0_button, 2, 2, 1, 1);
    _ch1_button = new QRadioButton("1");
    connect(_ch1_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch1_button, 2, 3, 1, 1);
    _ch2_button = new QRadioButton("2");
    connect(_ch2_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch2_button, 2, 4, 1, 1);
    _ch3_button = new QRadioButton("3");
    connect(_ch3_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_ch3_button, 2, 5, 1, 1);
    QButtonGroup *heightmap_group = new QButtonGroup();
    heightmap_group->addButton(_ch0_button);
    heightmap_group->addButton(_ch1_button);
    heightmap_group->addButton(_ch2_button);
    heightmap_group->addButton(_ch3_button);
    _ch0_button->setChecked(true);

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

    QLabel *range_label = new QLabel("Range:");
    layout->addWidget(range_label, 4, 0, 1, 2);
    _minmax_button = new QRadioButton("Full");
    connect(_minmax_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_minmax_button, 4, 2, 1, 2);
    _range_button = new QRadioButton("Selected");
    connect(_range_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_range_button, 4, 4, 1, 2);
    QButtonGroup *range_group = new QButtonGroup();
    range_group->addButton(_minmax_button);
    range_group->addButton(_range_button);
    _minmax_button->setChecked(true);

    QLabel *height_factor_label = new QLabel("Height factor:");
    layout->addWidget(height_factor_label, 5, 0, 1, 2);
    _height_factor_spinbox = new QDoubleSpinBox();
    _height_factor_spinbox->setRange(0.00, 4.00);
    _height_factor_spinbox->setSingleStep(0.01);
    _height_factor_spinbox->setDecimals(2);
    _height_factor_spinbox->setValue(1.0);
    connect(_height_factor_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_height_factor(double)));
    layout->addWidget(_height_factor_spinbox, 5, 2, 1, 4);
    _height_factor_slider = new QSlider(Qt::Horizontal, this);
    _height_factor_slider->setRange(0, 2000);
    _height_factor_slider->setValue(1000);
    connect(_height_factor_slider, SIGNAL(valueChanged(int)), this, SLOT(_height_factor_slider_changed(int)));
    layout->addWidget(_height_factor_slider, 6, 0, 1, 6);

    _showcuboid_box = new QCheckBox("Show Cuboid");
    _showcuboid_box->setCheckState(Qt::Checked);
    connect(_showcuboid_box, SIGNAL(stateChanged(int)), this, SLOT(_button_clicked()));
    layout->addWidget(_showcuboid_box, 7, 0, 1, 4);
    _color_selector = new ColorSelector(1.0f, 1.0f, 1.0f, this);
    layout->addWidget(_color_selector, 7, 4, 1, 2);

    layout->setRowStretch(8, 1);
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
    _height_factor_slider->setValue(mh_iround(sqrt(f) * 1000.0));
    _lock = false;
    emit heightmap_changed();
}

void HeightmapSelector::_height_factor_slider_changed(int f)
{
    if (!_lock)
    {
	_height_factor_spinbox->setValue(pow(static_cast<double>(f) / 1000.0, 2.0));
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
