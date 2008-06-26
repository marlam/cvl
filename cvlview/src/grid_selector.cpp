/*
 * grid_selector.cpp
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
#include <QCheckBox>
#include <QPushButton>
#include <QColorDialog>
#include <QPixmap>
#include <QIcon>

#include "mh.h"

#include "grid_selector.h"


GridSelector::GridSelector(float r, float g, float b, QWidget *parent) 
	: QWidget(parent)
{
    _default_color[0] = r;
    _default_color[1] = g;
    _default_color[2] = b;

    QGridLayout *layout = new QGridLayout;

    _box = new QCheckBox("Show grid");
    _box->setChecked(false);
    _box->setFixedSize(QSize(_box->sizeHint().width(), _box->sizeHint().height()));
    connect(_box, SIGNAL(stateChanged(int)), this, SLOT(_box_clicked()));
    layout->addWidget(_box, 0, 0);
    _button = new QPushButton();
    _button->setFixedSize(QSize(_button->sizeHint().width() / 2, _button->sizeHint().height()));
    connect(_button, SIGNAL(clicked()), this, SLOT(_button_clicked()));
    layout->addWidget(_button, 0, 1);

    set_color(r, g, b);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

GridSelector::~GridSelector()
{
}

void GridSelector::reset()
{
    set_color(_default_color[0], _default_color[1], _default_color[2]);
}

void GridSelector::_box_clicked()
{
    emit grid_changed();
}

void GridSelector::_button_clicked()
{
    QColor color = QColorDialog::getColor(_color, this);
    if (color.isValid()) 
    {
	set_color(color.redF(), color.greenF(), color.blueF());
    }
}

void GridSelector::set_color(float r, float g, float b)
{
    _color = QColor(mh_iroundf(r * 255.0f), mh_iroundf(g * 255.0f), mh_iroundf(b * 255.0f));
    QPixmap pm(64, 64);
    pm.fill(_color);
    _button->setIcon(QIcon(pm));
    emit grid_changed();
}
