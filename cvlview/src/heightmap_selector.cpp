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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>

#include <cvl/cvl.h>

#include "mh.h"

#include "heightmap_selector.h"


HeightmapSelector::HeightmapSelector(cvl_frame_t **frame, QWidget *parent) : QWidget(parent)
{
    _frame = frame;
    _lock = false;
    QVBoxLayout *layout = new QVBoxLayout;

    _enable_box = new QCheckBox("Use height map");
    _enable_box->setCheckState(Qt::Unchecked);
    connect(_enable_box, SIGNAL(stateChanged(int)), this, SLOT(_set_enable(int)));
    layout->addWidget(_enable_box);

    QGroupBox *heightmap_box = new QGroupBox("Height map channel");
    _ch0_button = new QRadioButton("0", heightmap_box);
    connect(_ch0_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    _ch1_button = new QRadioButton("1", heightmap_box);
    connect(_ch1_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    _ch2_button = new QRadioButton("2", heightmap_box);
    connect(_ch2_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    _ch3_button = new QRadioButton("3", heightmap_box);
    connect(_ch3_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    _ch0_button->setChecked(true);
    QHBoxLayout *heightmap_box_layout = new QHBoxLayout;
    heightmap_box_layout->addWidget(_ch0_button);
    heightmap_box_layout->addStretch(1);
    heightmap_box_layout->addWidget(_ch1_button);
    heightmap_box_layout->addStretch(1);
    heightmap_box_layout->addWidget(_ch2_button);
    heightmap_box_layout->addStretch(1);
    heightmap_box_layout->addWidget(_ch3_button);
    heightmap_box->setLayout(heightmap_box_layout);
    layout->addWidget(heightmap_box);

    QGroupBox *mode_box = new QGroupBox("View mode");
    _quads_button = new QRadioButton("Separate quads", mode_box);
    connect(_quads_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    _strip_button = new QRadioButton("Connected surface", mode_box);
    connect(_strip_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    _quads_button->setChecked(true);
    QVBoxLayout *mode_box_layout = new QVBoxLayout;
    mode_box_layout->addWidget(_quads_button);
    mode_box_layout->addWidget(_strip_button);
    mode_box_layout->addStretch(1);
    mode_box->setLayout(mode_box_layout);
    layout->addWidget(mode_box);

    layout->addStretch(1);
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
