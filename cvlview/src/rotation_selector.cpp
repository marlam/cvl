/*
 * rotation_selector.cpp
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
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

#include "mh.h"

#include "rotation_selector.h"


RotationSelector::RotationSelector(QWidget *parent) 
	: QWidget(parent)
{
    _lock = false;

    QGridLayout *layout = new QGridLayout;

    QLabel *rot_label = new QLabel("Rotation:");
    layout->addWidget(rot_label, 0, 0, 1, 2);

    QLabel *xrot_label = new QLabel("X:");
    layout->addWidget(xrot_label, 0, 2, 1, 1);
    _xrot_spinbox = new QDoubleSpinBox();
    _xrot_spinbox->setRange(0.0, 359.9);
    _xrot_spinbox->setWrapping(true);
    _xrot_spinbox->setSingleStep(1.0);
    _xrot_spinbox->setValue(0.0);
    connect(_xrot_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_x_rotation(double)));
    layout->addWidget(_xrot_spinbox, 0, 3, 1, 2);

    QLabel *yrot_label = new QLabel("Y:");
    layout->addWidget(yrot_label, 0, 5, 1, 1);
    _yrot_spinbox = new QDoubleSpinBox();
    _yrot_spinbox->setRange(0.0, 359.9);
    _yrot_spinbox->setWrapping(true);
    _yrot_spinbox->setSingleStep(1.0);
    _yrot_spinbox->setValue(0.0);
    connect(_yrot_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_y_rotation(double)));
    layout->addWidget(_yrot_spinbox, 0, 6, 1, 2);
    
    _reset_button = new QPushButton(tr("Reset"));
    connect(_reset_button, SIGNAL(clicked()), this, SLOT(_reset_button_clicked()));
    layout->addWidget(_reset_button, 0, 8, 1, 1);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

RotationSelector::~RotationSelector()
{
}

void RotationSelector::reset()
{
    _xrot_spinbox->setValue(0.0);
    _yrot_spinbox->setValue(0.0);
}

void RotationSelector::_reset_button_clicked()
{
    reset();
    emit view_changed();
}

void RotationSelector::_set_x_rotation(double rx UNUSED)
{
    if (!_lock)
    {
	emit view_changed();
    }
}

void RotationSelector::_set_y_rotation(double ry UNUSED)
{
    if (!_lock)
    {
	emit view_changed();
    }
}

void RotationSelector::set_x_rotation(float rx)
{
    while (rx >= 360.0f)
	rx -= 360.0f;
    while (rx < 0.0f)
	rx += 360.0f;
    _xrot_spinbox->setValue(rx);
    emit view_changed();
}

void RotationSelector::set_y_rotation(float ry)
{
    while (ry >= 360.0f)
	ry -= 360.0f;
    while (ry < 0.0f)
	ry += 360.0f;
    _yrot_spinbox->setValue(ry);
    emit view_changed();
}
