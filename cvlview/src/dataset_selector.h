/*
 * dataset_selector.h
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

#ifndef DATASET_SELECTOR_H
#define DATASET_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QPushButton>

#include "datafile.h"


class DatasetSelector : public QWidget
{
    Q_OBJECT
	
    private:
	DataFile **_datafile;
	QPushButton *_p_button;
	QPushButton *_n_button;

    private slots:
	void p_button_clicked();
    	void n_button_clicked();

    public:
	DatasetSelector(DataFile **datafile, QWidget *parent = NULL);	
	~DatasetSelector();

    signals:
	void dataset_changed();
};

#endif
