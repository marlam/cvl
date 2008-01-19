/*
 * maxabslum_selector.cpp
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
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
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QGridLayout>

#include <cvl/cvl.h>

#include "mh.h"

#include "conf.h"
#include "maxabslum_selector.h"


MaxAbsLumSelector::MaxAbsLumSelector(const char *id, cvl_frame_t **frame, QWidget *parent) : QWidget(parent)
{
    _slider_param = 5000.0;
    _min = 0.01f;
    _max = 99999.99f;
    _default = 150.0f;
    _id = id;
    _frame = frame;

    QGridLayout *layout = new QGridLayout();
    layout->setMargin(0);
    
    _checkbox = new QCheckBox("Max. Luminance:");
    connect(_checkbox, SIGNAL(stateChanged(int)), this, SLOT(set_state(int)));
    layout->addWidget(_checkbox, 0, 0);

    _spinbox = new QDoubleSpinBox();
    _spinbox->setRange(_min, _max);
    _spinbox->setDecimals(2);
    _spinbox->setSingleStep(1.0);
    connect(_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set(double)));
    layout->addWidget(_spinbox, 0, 1);

    _slider = new QSlider(Qt::Horizontal, this);
    _slider->setRange(0, 9999);
    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(slider_changed(int)));
    layout->addWidget(_slider, 1, 0, 1, 2);

    layout->setRowStretch(2, 1);
    setLayout(layout);
    update();
}

MaxAbsLumSelector::~MaxAbsLumSelector()
{
}

int MaxAbsLumSelector::val_to_slider(double x)
{
    return mh_iround(10000.0 * log(1.0 + _slider_param * (x / _max)) / log(1.0 + _slider_param));
}

double MaxAbsLumSelector::slider_to_val(int x)
{
    return (exp(static_cast<double>(x) * log(1.0 + _slider_param) / 10000.0) - 1.0) / _slider_param * _max;
}

void MaxAbsLumSelector::set_state(int s UNUSED)
{
    if (_checkbox->isChecked())
    {
	_spinbox->setEnabled(true);
	_slider->setEnabled(true);
    }
    else
    {
	if (_max_abs_lum > 0.0f)
	    _spinbox->setValue(_max_abs_lum);
	_spinbox->setEnabled(false);
	_slider->setEnabled(false);
    }
    emit maxabslum_changed();
}

void MaxAbsLumSelector::set(double x)
{
    _lock = true;
    _slider->setValue(val_to_slider(x));
    _lock = false;
    emit maxabslum_changed();
}

void MaxAbsLumSelector::slider_changed(int x)
{
    if (!_lock)
	_spinbox->setValue(slider_to_val(x));
}

void MaxAbsLumSelector::update()
{
    _max_abs_lum = -1.0f;
    _lock = true;
    const char *max_luminance_tag = NULL;
    if (*_frame)
    {
	max_luminance_tag = cvl_taglist_get(cvl_frame_taglist(*_frame), "X-MAX-LUMINANCE");
    }
    if (!max_luminance_tag || strcmp(max_luminance_tag, "UNKNOWN") == 0)
    {
	_spinbox->setValue(_default);
	_slider->setValue(mh_iroundf(_default * 100.0f));
	_checkbox->setCheckState(Qt::Checked);
    }
    else
    {
	_max_abs_lum = atof(max_luminance_tag);
	_spinbox->setValue(_max_abs_lum);
	_slider->setValue(val_to_slider(_max_abs_lum));
	_checkbox->setCheckState(Qt::Unchecked);
    }
}

void MaxAbsLumSelector::get_parameters(Conf *conf) const
{
    conf->put(mh_string("%s-usemaxabslum", _id).c_str(), _checkbox->isChecked());
    conf->put(mh_string("%s-maxabslum", _id).c_str(), _spinbox->value());
}

void MaxAbsLumSelector::set_parameters(Conf *conf)
{
    _spinbox->setValue(conf->get(mh_string("%s-maxabslum", _id).c_str(), _min, _max, _default));
    _checkbox->setChecked(conf->get(mh_string("%s-usemaxabslum", _id).c_str(), true));
}
