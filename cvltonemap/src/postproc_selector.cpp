/*
 * postproc_selector.cpp
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
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
#include <QDoubleSpinBox>
#include <QSlider>

#include "mh.h"

#include "postproc_selector.h"


PostprocSelector::PostprocSelector(QWidget *parent) : QWidget(parent)
{
    _lock = false;
    QGridLayout *layout = new QGridLayout;

    QLabel *gamma_label = new QLabel("Gamma:");
    layout->addWidget(gamma_label, 0, 0);
    _gamma_spinbox = new QDoubleSpinBox();
    _gamma_spinbox->setRange(0.25, 4.00);
    _gamma_spinbox->setSingleStep(0.01);
    _gamma_spinbox->setValue(1.0);
    connect(_gamma_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_gamma(double)));
    layout->addWidget(_gamma_spinbox, 0, 1);
    _gamma_slider = new QSlider(Qt::Horizontal, this);
    _gamma_slider->setRange(-300, 300);
    _gamma_slider->setValue(0);
    connect(_gamma_slider, SIGNAL(valueChanged(int)), this, SLOT(gamma_slider_changed(int)));
    layout->addWidget(_gamma_slider, 1, 0, 1, 2);

    QLabel *lightness_label = new QLabel("Lightness:");
    layout->addWidget(lightness_label, 2, 0);
    _lightness_spinbox = new QDoubleSpinBox();
    _lightness_spinbox->setRange(-1.00, +1.00);
    _lightness_spinbox->setSingleStep(0.01);
    _lightness_spinbox->setValue(0.0);
    connect(_lightness_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_lightness(double)));
    layout->addWidget(_lightness_spinbox, 2, 1);
    _lightness_slider = new QSlider(Qt::Horizontal, this);
    _lightness_slider->setRange(-100, +100);
    _lightness_slider->setValue(0);
    connect(_lightness_slider, SIGNAL(valueChanged(int)), this, SLOT(lightness_slider_changed(int)));
    layout->addWidget(_lightness_slider, 3, 0, 1, 2);

    QLabel *contrast_label = new QLabel("Contrast:");
    layout->addWidget(contrast_label, 4, 0);
    _contrast_spinbox = new QDoubleSpinBox();
    _contrast_spinbox->setRange(-1.00, +1.00);
    _contrast_spinbox->setSingleStep(0.01);
    _contrast_spinbox->setValue(0.0);
    connect(_contrast_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_contrast(double)));
    layout->addWidget(_contrast_spinbox, 4, 1);
    _contrast_slider = new QSlider(Qt::Horizontal, this);
    _contrast_slider->setRange(-100, +100);
    _contrast_slider->setValue(0);
    connect(_contrast_slider, SIGNAL(valueChanged(int)), this, SLOT(contrast_slider_changed(int)));
    layout->addWidget(_contrast_slider, 5, 0, 1, 2);

    QLabel *saturation_label = new QLabel("Saturation:");
    layout->addWidget(saturation_label, 6, 0);
    _saturation_spinbox = new QDoubleSpinBox();
    _saturation_spinbox->setRange(-1.00, +1.00);
    _saturation_spinbox->setSingleStep(0.01);
    _saturation_spinbox->setValue(0.0);
    connect(_saturation_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_saturation(double)));
    layout->addWidget(_saturation_spinbox, 6, 1);
    _saturation_slider = new QSlider(Qt::Horizontal, this);
    _saturation_slider->setRange(-100, +100);
    _saturation_slider->setValue(0);
    connect(_saturation_slider, SIGNAL(valueChanged(int)), this, SLOT(saturation_slider_changed(int)));
    layout->addWidget(_saturation_slider, 7, 0, 1, 2);

    QLabel *sharpness_label = new QLabel("Sharpness:");
    layout->addWidget(sharpness_label, 8, 0);
    _sharpness_spinbox = new QDoubleSpinBox();
    _sharpness_spinbox->setRange(-1.00, +1.00);
    _sharpness_spinbox->setSingleStep(0.01);
    _sharpness_spinbox->setValue(0.0);
    connect(_sharpness_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_sharpness(double)));
    layout->addWidget(_sharpness_spinbox, 8, 1);
    _sharpness_slider = new QSlider(Qt::Horizontal, this);
    _sharpness_slider->setRange(-100, +100);
    _sharpness_slider->setValue(0);
    connect(_sharpness_slider, SIGNAL(valueChanged(int)), this, SLOT(sharpness_slider_changed(int)));
    layout->addWidget(_sharpness_slider, 9, 0, 1, 2);

    layout->setRowStretch(10, 1);
    setLayout(layout);
}

PostprocSelector::~PostprocSelector()
{
}

void PostprocSelector::set_gamma(double x)
{
    _lock = true;
    if (x < 1.0)
	_gamma_slider->setValue(mh_iround(-300.0 * (1.0 - (x - 0.25) / 0.75)));
    else
	_gamma_slider->setValue(mh_iround(300.0 * ((x - 1.00) / 4.00)));
    _lock = false;
    emit postproc_changed();
}

void PostprocSelector::gamma_slider_changed(int x)
{
    if (!_lock)
    {
	if (x < 0)
	    _gamma_spinbox->setValue((static_cast<double>(x) / 300.0) * 0.75 + 1.0);
	else
	    _gamma_spinbox->setValue((static_cast<double>(x) / 300.0) * 3.0 + 1.0);
    }
}

void PostprocSelector::set_lightness(double x)
{
    _lock = true;
    _lightness_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit postproc_changed();
}

void PostprocSelector::lightness_slider_changed(int x)
{
    if (!_lock)
	_lightness_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void PostprocSelector::set_contrast(double x)
{
    _lock = true;
    _contrast_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit postproc_changed();
}

void PostprocSelector::contrast_slider_changed(int x)
{
    if (!_lock)
	_contrast_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void PostprocSelector::set_saturation(double x)
{
    _lock = true;
    _saturation_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit postproc_changed();
}

void PostprocSelector::saturation_slider_changed(int x)
{
    if (!_lock)
	_saturation_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void PostprocSelector::set_sharpness(double x)
{
    _lock = true;
    _sharpness_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit postproc_changed();
}

void PostprocSelector::sharpness_slider_changed(int x)
{
    if (!_lock)
	_sharpness_spinbox->setValue(static_cast<double>(x) / 100.0);
}
