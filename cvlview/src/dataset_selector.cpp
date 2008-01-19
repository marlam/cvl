/*
 * dataset_selector.cpp
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

#include "datafile.h"

#include "dataset_selector.h"


DatasetSelector::DatasetSelector(DataFile **datafile, QWidget *parent) 
	: QWidget(parent)
{
    _datafile = datafile;

    QGridLayout *layout = new QGridLayout;
    
    _p_button = new QPushButton(QIcon(":icons/prev.png"), "");
    _p_button->setShortcut(tr("Left"));
    _p_button->setAutoDefault(false);
    connect(_p_button, SIGNAL(clicked()), this, SLOT(p_button_clicked()));
    layout->addWidget(_p_button, 0, 0);
    _n_button = new QPushButton(QIcon(":icons/next.png"), "");
    _n_button->setShortcut(tr("Right"));
    _n_button->setAutoDefault(false);
    connect(_n_button, SIGNAL(clicked()), this, SLOT(n_button_clicked()));
    layout->addWidget(_n_button, 0, 1);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

DatasetSelector::~DatasetSelector()
{
}

void DatasetSelector::p_button_clicked()
{
    if (!*_datafile)
	return;

    if ((*_datafile)->index() > 1)
    {
	(*_datafile)->prev();	// move file pointer to current data set
	(*_datafile)->prev();	// move file pointer to previous data set
	emit dataset_changed();
    }
}

void DatasetSelector::n_button_clicked()
{
    if (!*_datafile)
	return;

    if ((*_datafile)->index() != (*_datafile)->total())
    {
	// filepointer already points to next data set
	emit dataset_changed();
    }
}
