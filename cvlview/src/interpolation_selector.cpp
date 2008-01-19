/*
 * interpolation_selector.cpp
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
#include <QPushButton>

#include "interpolation_selector.h"


InterpolationSelector::InterpolationSelector(QWidget *parent) 
	: QWidget(parent)
{
    QGridLayout *layout = new QGridLayout;
    _interpolation_button = new QPushButton(tr("&I"));
    _interpolation_button->setShortcut(tr("i"));
    layout->addWidget(_interpolation_button, 0, 0);
    _interpolation_button->setAutoDefault(false);
    _interpolation_button->setCheckable(true);
    _interpolation_button->setChecked(false);
    connect(_interpolation_button, SIGNAL(clicked()), this, SLOT(interpolation_button_clicked()));
    layout->setRowStretch(1, 1);
    setLayout(layout);
}

InterpolationSelector::~InterpolationSelector()
{
}

void InterpolationSelector::reset()
{
    _interpolation_button->setChecked(false);
    emit interpolation_changed();
}

void InterpolationSelector::interpolation_button_clicked()
{
    emit interpolation_changed();
}
