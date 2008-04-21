/*
 * color_selector.h
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

#ifndef COLOR_SELECTOR_H
#define COLOR_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QPushButton>
#include <QColor>


class ColorSelector : public QWidget
{
    Q_OBJECT
	
    private:
	QPushButton *_button;
	QColor _color;
	float _default_color[3];

    private slots:
	void _button_clicked();

    public:
	ColorSelector(float r = 0.3f, float g = 0.3f, float b = 0.3f, QWidget *parent = NULL);	
	~ColorSelector();

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

	void set_color(float r, float g, float b);

    public slots:
        void reset();

    signals:
	void color_changed();
};

#endif
