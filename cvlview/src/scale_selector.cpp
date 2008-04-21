/*
 * scale_selector.cpp
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
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

#include <cvl/cvl.h>

#include "mh.h"

#include "scale_selector.h"


ScaleSelector::ScaleSelector(cvl_frame_t **frame, QWidget *parent) 
	: QWidget(parent)
{
    _frame = frame;
    _view_area_width = 1;
    _view_area_height = 1;
    _lock = false;

    QGridLayout *layout = new QGridLayout;

    QLabel *zf_label = new QLabel("Scale:");
    layout->addWidget(zf_label, 0, 0, 1, 2);
    _scalefactor_spinbox = new QDoubleSpinBox();
    _scalefactor_spinbox->setRange(0.01, 999.99);
    _scalefactor_spinbox->setSingleStep(0.01);
    _scalefactor_spinbox->setValue(1.0);
    connect(_scalefactor_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_scalefactor(double)));
    layout->addWidget(_scalefactor_spinbox, 0, 2, 1, 2);
    
    _scale_fit_button = new QPushButton(tr("&Fit"));
    _scale_fit_button->setShortcut(tr("f"));
    _scale_fit_button->setFixedSize(QSize(_scale_fit_button->sizeHint().width() / 2, _scale_fit_button->sizeHint().height()));
    connect(_scale_fit_button, SIGNAL(clicked()), this, SLOT(scale_fit_button_clicked()));
    layout->addWidget(_scale_fit_button, 0, 4, 1, 1);
    
    _scale_reset_button = new QPushButton(tr("1&:1"));
    _scale_reset_button->setShortcut(tr(":"));
    _scale_reset_button->setFixedSize(QSize(_scale_reset_button->sizeHint().width() / 2, _scale_reset_button->sizeHint().height()));
    connect(_scale_reset_button, SIGNAL(clicked()), this, SLOT(scale_reset_button_clicked()));
    layout->addWidget(_scale_reset_button, 0, 5, 1, 1);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

ScaleSelector::~ScaleSelector()
{
}

void ScaleSelector::reset()
{
    _scalefactor_spinbox->setValue(1.0);
}

void ScaleSelector::scale_fit_button_clicked()
{
    if (!_frame || !*_frame)
	return;

    emit make_gl_context_current();

    float iw = static_cast<float>(cvl_frame_width(*_frame));
    float ih = static_cast<float>(cvl_frame_height(*_frame));
    float vw = static_cast<float>(_view_area_width);
    float vh = static_cast<float>(_view_area_height);

    float wf = vw / iw;
    float wh = vh / ih;

    _scalefactor_spinbox->setValue(mh_minf(wf, wh));
    emit view_changed();
}

void ScaleSelector::scale_reset_button_clicked()
{
    _scalefactor_spinbox->setValue(1.0f);
    emit view_changed();
}

void ScaleSelector::_set_scalefactor(double zf UNUSED)
{
    if (!_lock)
    {
	emit view_changed();
    }
}

void ScaleSelector::set_scalefactor(float zf)
{
    _scalefactor_spinbox->setValue(zf);
    emit view_changed();
}
