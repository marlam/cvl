/*
 * color_selector.cpp
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
#include <QPushButton>
#include <QColorDialog>
#include <QPixmap>
#include <QIcon>

#include "mh.h"

#include "color_selector.h"


ColorSelector::ColorSelector(float r, float g, float b, QWidget *parent) 
	: QWidget(parent)
{
    QGridLayout *layout = new QGridLayout;

    _color = QColor(mh_iround(r * 255.0f), mh_iround(g * 255.0f), mh_iround(b * 255.0f));
    _button = new QPushButton();
    _button->setFixedSize(QSize(_button->sizeHint().width() / 2, _button->sizeHint().height()));
    QPixmap pm(64, 64);
    pm.fill(_color);
    _button->setIcon(QIcon(pm));
    connect(_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_button, 0, 0);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

ColorSelector::~ColorSelector()
{
}

void ColorSelector::reset()
{
    _color = QColor(77, 77, 77);
}

void ColorSelector::_button_clicked()
{
    QColor new_color = QColorDialog::getColor(_color, this);
    if (new_color.isValid()) 
    {
	_color = new_color;
	QPixmap pm(64, 64);
	pm.fill(_color);
	_button->setIcon(QIcon(pm));
    }
    emit color_changed();
}
