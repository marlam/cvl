/*
 * translation_selector.cpp
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

#include "mh.h"

#include "translation_selector.h"


TranslationSelector::TranslationSelector(QWidget *parent) 
	: QWidget(parent)
{
    _lock = false;

    QGridLayout *layout = new QGridLayout;

    QLabel *xo_label = new QLabel("X:");
    layout->addWidget(xo_label, 0, 0, 1, 1);
    _x_offset_spinbox = new QSpinBox();
    _x_offset_spinbox->setRange(-9999, +9999);
    _x_offset_spinbox->setSingleStep(1);
    _x_offset_spinbox->setValue(0);
    connect(_x_offset_spinbox, SIGNAL(valueChanged(int)), this, SLOT(_set_x_offset(int)));
    layout->addWidget(_x_offset_spinbox, 0, 1, 1, 2);
    
    QLabel *yo_label = new QLabel("Y:");
    layout->addWidget(yo_label, 0, 3, 1, 1);
    _y_offset_spinbox = new QSpinBox();
    _y_offset_spinbox->setRange(-9999, +9999);
    _y_offset_spinbox->setSingleStep(1);
    _y_offset_spinbox->setValue(0);
    connect(_y_offset_spinbox, SIGNAL(valueChanged(int)), this, SLOT(_set_y_offset(int)));
    layout->addWidget(_y_offset_spinbox, 0, 4, 1, 2);
    
    _offset_reset_button = new QPushButton(tr("Ce&nter"));
    _offset_reset_button->setShortcut(tr("n"));
    connect(_offset_reset_button, SIGNAL(clicked()), this, SLOT(offset_reset_button_clicked()));
    layout->addWidget(_offset_reset_button, 0, 6, 1, 2);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

TranslationSelector::~TranslationSelector()
{
}

void TranslationSelector::reset()
{
    _x_offset_spinbox->setValue(0);
    _y_offset_spinbox->setValue(0);
}

void TranslationSelector::offset_reset_button_clicked()
{
    _lock = true;
    _x_offset_spinbox->setValue(0);
    _y_offset_spinbox->setValue(0);
    _lock = false;
    emit view_changed();
}

void TranslationSelector::_set_x_offset(int xo UNUSED)
{
    if (!_lock)
    {
	emit view_changed();
    }
}

void TranslationSelector::_set_y_offset(int yo UNUSED)
{
    if (!_lock)
    {
	emit view_changed();
    }
}

void TranslationSelector::set_x_offset(int xo)
{
    _x_offset_spinbox->setValue(xo);
    emit view_changed();
}

void TranslationSelector::set_y_offset(int yo)
{
    _y_offset_spinbox->setValue(yo);
    emit view_changed();
}
