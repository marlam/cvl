/*
 * grid_selector.h
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

#ifndef GRID_SELECTOR_H
#define GRID_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include <QColor>


class GridSelector : public QWidget
{
    Q_OBJECT
	
    private:
	QCheckBox *_box;
	QPushButton *_button;
	QColor _color;
	float _default_color[3];

    private slots:
	void _box_clicked();
	void _button_clicked();

    public:
	GridSelector(float r = 1.0f, float g = 1.0f, float b = 1.0f, QWidget *parent = NULL);	
	~GridSelector();

	float get_r() const
	{
	    return _color.redF();
	}

	float get_g() const
	{
	    return _color.greenF();
	}

	float get_b() const
	{
	    return _color.blueF();
	}

	bool is_enabled() const
	{
	    return _box->isChecked();
	}

	void set_color(float r, float g, float b);

    public slots:
        void reset();

    signals:
	void grid_changed();
};

#endif
