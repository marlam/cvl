/*
 * interpolation_selector.h
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

#ifndef INTERPOLATION_SELECTOR_H
#define INTERPOLATION_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QPushButton>


class InterpolationSelector : public QWidget
{
    Q_OBJECT
	
    private:
	QPushButton *_interpolation_button;

    private slots:
	void interpolation_button_clicked();

    public:
	InterpolationSelector(QWidget *parent = NULL);	
	~InterpolationSelector();

	bool is_enabled()
	{
	    return _interpolation_button->isChecked();
	}

    public slots:
	void reset();

    signals:
	void interpolation_changed();
};

#endif
