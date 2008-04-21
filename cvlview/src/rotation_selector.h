/*
 * rotation_selector.h
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

#ifndef ROTATION_SELECTOR_H
#define ROTATION_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QDoubleSpinBox>
#include <QPushButton>


class RotationSelector : public QWidget
{
    Q_OBJECT
	
    private:
	QPushButton *_reset_button;
	QDoubleSpinBox *_xrot_spinbox;
	QDoubleSpinBox *_yrot_spinbox;
	bool _lock;

    private slots:
	void _reset_button_clicked();
	void _set_x_rotation(double rx);
	void _set_y_rotation(double ry);

    public:
	RotationSelector(QWidget *parent = NULL);	
	~RotationSelector();

	float get_x_rotation()
	{
	    return _xrot_spinbox->value();
	}

	float get_y_rotation()
	{
	    return _yrot_spinbox->value();
	}

	void set_x_rotation(float rx);
	void set_y_rotation(float ry);

    public slots:
        void reset();

    signals:
	void view_changed();
};

#endif
