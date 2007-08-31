/*
 * pseudocolor_selector.cpp
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

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSlider>

#include "mh.h"

#include "channel_selector.h"
#include "pseudocolor_selector.h"


PseudocolorSelector::PseudocolorSelector(ChannelSelector *channel_selector, QWidget *parent) 
	: QWidget(parent)
{
    _channel_selector = channel_selector;
    _lock = false;
    for (int c = -1; c <= 3; c++)
    {
	_enabled[c + 1] = false;
	_cyclic[c + 1] = false;
	_startcolor[c + 1] = 0.0f;
	_lightness[c + 1] = 0.0f;
    }
    QGridLayout *layout = new QGridLayout;

    _enable_box = new QCheckBox("Use pseudo colors");
    _enable_box->setCheckState(Qt::Unchecked);
    connect(_enable_box, SIGNAL(stateChanged(int)), this, SLOT(_set_enable(int)));
    layout->addWidget(_enable_box, 0, 0, 1, 2);

    _cyclic_box = new QCheckBox("Cyclic colors");
    _cyclic_box->setCheckState(Qt::Unchecked);
    connect(_cyclic_box, SIGNAL(stateChanged(int)), this, SLOT(_set_cyclic(int)));
    layout->addWidget(_cyclic_box, 1, 0, 1, 2);

    QLabel *startcolor_label = new QLabel("Start color:");
    layout->addWidget(startcolor_label, 2, 0);
    _startcolor_spinbox = new QDoubleSpinBox();
    _startcolor_spinbox->setRange(0.00, 1.00);
    _startcolor_spinbox->setSingleStep(0.01);
    _startcolor_spinbox->setValue(0.0);
    connect(_startcolor_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_startcolor(double)));
    layout->addWidget(_startcolor_spinbox, 2, 1);
    _startcolor_slider = new QSlider(Qt::Horizontal, this);
    _startcolor_slider->setRange(0, 101);
    _startcolor_slider->setValue(0);
    connect(_startcolor_slider, SIGNAL(valueChanged(int)), this, SLOT(_startcolor_slider_changed(int)));
    layout->addWidget(_startcolor_slider, 3, 0, 1, 2);

    QLabel *lightness_label = new QLabel("Lightness variability:");
    layout->addWidget(lightness_label, 4, 0);
    _lightness_spinbox = new QDoubleSpinBox();
    _lightness_spinbox->setRange(0.00, 1.00);
    _lightness_spinbox->setSingleStep(0.01);
    _lightness_spinbox->setValue(0.0);
    connect(_lightness_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_lightness(double)));
    layout->addWidget(_lightness_spinbox, 4, 1);
    _lightness_slider = new QSlider(Qt::Horizontal, this);
    _lightness_slider->setRange(0, 101);
    _lightness_slider->setValue(0);
    connect(_lightness_slider, SIGNAL(valueChanged(int)), this, SLOT(_lightness_slider_changed(int)));
    layout->addWidget(_lightness_slider, 5, 0, 1, 2);

    layout->setRowStretch(6, 1);
    setLayout(layout);
}

PseudocolorSelector::~PseudocolorSelector()
{
}

void PseudocolorSelector::_set_enable(int e UNUSED)
{
    int c = _channel_selector->get_channel() + 1;
    _enabled[c] = (_enable_box->checkState() == Qt::Checked);
    emit pseudocolor_changed();
}

void PseudocolorSelector::_set_cyclic(int x UNUSED)
{
    int c = _channel_selector->get_channel() + 1;
    _cyclic[c] = (_cyclic_box->checkState() == Qt::Checked);
    emit pseudocolor_changed();
}

void PseudocolorSelector::_set_startcolor(double s)
{
    _lock = true;
    int c = _channel_selector->get_channel() + 1;
    _startcolor[c] = static_cast<float>(s);
    _startcolor_slider->setValue(mh_iround(static_cast<double>(s) * 101.0));
    _lock = false;
    emit pseudocolor_changed();
}

void PseudocolorSelector::_startcolor_slider_changed(int s)
{
    if (!_lock)
	_startcolor_spinbox->setValue(static_cast<double>(s) / 101.0);
}

void PseudocolorSelector::_set_lightness(double l)
{
    _lock = true;
    int c = _channel_selector->get_channel() + 1;
    _lightness[c] = static_cast<float>(l);
    _lightness_slider->setValue(mh_iround(static_cast<double>(l) * 101.0));
    _lock = false;
    emit pseudocolor_changed();
}

void PseudocolorSelector::_lightness_slider_changed(int s)
{
    if (!_lock)
	_lightness_spinbox->setValue(static_cast<double>(s) / 101.0);
}

void PseudocolorSelector::update_channel()
{
    int c = _channel_selector->get_channel() + 1;
    _enable_box->setCheckState(_enabled[c] ? Qt::Checked : Qt::Unchecked);
    _cyclic_box->setCheckState(_cyclic[c] ? Qt::Checked : Qt::Unchecked);
    _startcolor_spinbox->setValue(_startcolor[c]);
    _lightness_spinbox->setValue(_lightness[c]);
}
