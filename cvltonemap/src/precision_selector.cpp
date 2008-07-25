/*
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
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

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

#include "precision_selector.h"


PrecisionSelector::PrecisionSelector(QWidget *parent) 
	: QWidget(parent)
{
    _lock = false;

    QGridLayout *layout = new QGridLayout;

    QLabel *label = new QLabel("Precision:");
    layout->addWidget(label, 0, 0);

    _combo_box = new QComboBox();
    _combo_box->setEditable(false);
    _combo_box->addItem("16 Bit");
    _combo_box->addItem("32 Bit");
    _combo_box->setCurrentIndex(0);
    connect(_combo_box, SIGNAL(activated(int)), this, SLOT(_selection(int)));

    layout->addWidget(_combo_box, 0, 1);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

PrecisionSelector::~PrecisionSelector()
{
}

void PrecisionSelector::_selection(int)
{
    if (!_lock)
    {
	emit precision_changed();
    }
}

int PrecisionSelector::get_precision()
{
    return _combo_box->currentIndex();
}

void PrecisionSelector::set_precision(int p)
{
    _lock = true;
    _combo_box->setCurrentIndex(p);
    _lock = false;
}
