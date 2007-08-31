/*
 * gamma_selector.cpp
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
#include "gamma_selector.h"


GammaSelector::GammaSelector(ChannelSelector *channel_selector, QWidget *parent) 
	: QWidget(parent)
{
    _channel_selector = channel_selector;
    _lock = false;
    for (int c = -1; c <= 3; c++)
    {
	_enabled[c + 1] = true;
	_gamma[c + 1] = 1.0f;
    }

    QGridLayout *layout = new QGridLayout;

    _enable_box = new QCheckBox("Enable gamma correction");
    _enable_box->setCheckState(Qt::Checked);
    connect(_enable_box, SIGNAL(stateChanged(int)), this, SLOT(_set_enable(int)));
    layout->addWidget(_enable_box, 0, 0, 1, 2);

    QLabel *gamma_label = new QLabel("Gamma:");
    layout->addWidget(gamma_label, 1, 0);
    _gamma_spinbox = new QDoubleSpinBox();
    _gamma_spinbox->setRange(0.01, 9.99);
    _gamma_spinbox->setSingleStep(0.01);
    _gamma_spinbox->setValue(1.0);
    connect(_gamma_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_gamma(double)));
    layout->addWidget(_gamma_spinbox, 1, 1);
    _gamma_slider = new QSlider(Qt::Horizontal, this);
    _gamma_slider->setRange(0, 1799);
    _gamma_slider->setValue(900);
    connect(_gamma_slider, SIGNAL(valueChanged(int)), this, SLOT(_gamma_slider_changed(int)));
    layout->addWidget(_gamma_slider, 2, 0, 1, 2);

    layout->setRowStretch(3, 1);
    setLayout(layout);
}

GammaSelector::~GammaSelector()
{
}

void GammaSelector::_set_enable(int e UNUSED)
{
    int c = _channel_selector->get_channel() + 1;
    _enabled[c] = (_enable_box->checkState() == Qt::Checked);
    emit gamma_changed();
}

void GammaSelector::_set_gamma(double g)
{
    _lock = true;
    int c = _channel_selector->get_channel() + 1;
    _gamma[c] = static_cast<float>(g);
    int v;
    if (g < 1.0)
    {
	v = mh_iround(static_cast<double>(g) * 899.0);
    }
    else
    {
	v = 800 + mh_iround(g * 100.0);
    }
    _gamma_slider->setValue(v);
    _lock = false;
    emit gamma_changed();
}

void GammaSelector::_gamma_slider_changed(int g)
{
    if (!_lock)
    {
	if (g >= 900)
	{
	    _gamma_spinbox->setValue(static_cast<double>(g - 800) / 100.0);
	}
	else
	{
	    _gamma_spinbox->setValue(static_cast<double>(g) / 899.0);
	}
    }
}

void GammaSelector::update_channel()
{
    int c = _channel_selector->get_channel() + 1;
    _enable_box->setCheckState(_enabled[c] ? Qt::Checked : Qt::Unchecked);
    _gamma_spinbox->setValue(_gamma[c]);
}
