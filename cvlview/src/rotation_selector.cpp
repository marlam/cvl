/*
 * rotation_selector.cpp
 *
 * This file is part of cvlview, an image viewer using the CVL library.
 *
 * Copyright (C) 2008, 2009  Martin Lambers <marlam@marlam.de>
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

    QLabel *rot_label = new QLabel("<strong>R</strong>");
    layout->addWidget(rot_label, 0, 0, 1, 1);

    _xrot_spinbox = new QDoubleSpinBox();
    _xrot_spinbox->setRange(-179.99, 179.99);
    _xrot_spinbox->setWrapping(true);
    _xrot_spinbox->setSingleStep(1.0);
    _xrot_spinbox->setValue(0.0);
    connect(_xrot_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_x_rotation(double)));
    layout->addWidget(_xrot_spinbox, 0, 1, 1, 2);

    _yrot_spinbox = new QDoubleSpinBox();
    _yrot_spinbox->setRange(-179.99, 179.99);
    _yrot_spinbox->setWrapping(true);
    _yrot_spinbox->setSingleStep(1.0);
    _yrot_spinbox->setValue(0.0);
    connect(_yrot_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_y_rotation(double)));
    layout->addWidget(_yrot_spinbox, 0, 3, 1, 2);

    _zrot_spinbox = new QDoubleSpinBox();
    _zrot_spinbox->setRange(-179.99, 179.99);
    _zrot_spinbox->setWrapping(true);
    _zrot_spinbox->setSingleStep(1.0);
    _zrot_spinbox->setValue(0.0);
    connect(_zrot_spinbox, SIGNAL(valueChanged(double)), this, SLOT(_set_z_rotation(double)));
    layout->addWidget(_zrot_spinbox, 0, 5, 1, 2);
    
    _reset_button = new QPushButton(tr("Reset"));
    connect(_reset_button, SIGNAL(clicked()), this, SLOT(_reset_button_clicked()));
    layout->addWidget(_reset_button, 0, 7, 1, 1);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

RotationSelector::~RotationSelector()
{
}

void RotationSelector::reset()
{
    _lock = true;
    _xrot_spinbox->setValue(0.0);
    _yrot_spinbox->setValue(0.0);
    _zrot_spinbox->setValue(0.0);
    _lock = false;
}

void RotationSelector::_reset_button_clicked()
{
    reset();
    emit rotation_changed();
}

void RotationSelector::_set_x_rotation(double rx UNUSED)
{
    if (!_lock)
    {
	emit rotation_changed();
    }
}

void RotationSelector::_set_y_rotation(double ry UNUSED)
{
    if (!_lock)
    {
	emit rotation_changed();
    }
}

void RotationSelector::_set_z_rotation(double rz UNUSED)
{
    if (!_lock)
    {
	emit rotation_changed();
    }
}

void RotationSelector::set_x_rotation(float rx)
{
    while (rx >= 180.0f)
	rx -= 360.0f;
    while (rx < -180.0f)
	rx += 360.0f;
    _xrot_spinbox->setValue(rx);
}

void RotationSelector::set_y_rotation(float ry)
{
    while (ry >= 180.0f)
	ry -= 360.0f;
    while (ry < -180.0f)
	ry += 360.0f;
    _yrot_spinbox->setValue(ry);
}

void RotationSelector::set_z_rotation(float rz)
{
    while (rz >= 180.0f)
	rz -= 360.0f;
    while (rz < -180.0f)
	rz += 360.0f;
    _zrot_spinbox->setValue(rz);
}

void RotationSelector::update_rotation(const quat &rotation)
{
    vec3 euler_angles_deg = degrees(toEuler(rotation));
    _lock = true;
    _xrot_spinbox->setValue(euler_angles_deg.x());
    _yrot_spinbox->setValue(euler_angles_deg.y());
    _zrot_spinbox->setValue(euler_angles_deg.z());
    _lock = false;
}
