/*
 * translation_selector.h
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

#ifndef TRANSLATION_SELECTOR_H
#define TRANSLATION_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>


class TranslationSelector : public QWidget
{
    Q_OBJECT
	
    private:
	QPushButton *_offset_reset_button;
	QSpinBox *_x_offset_spinbox;
	QSpinBox *_y_offset_spinbox;
	bool _lock;

    private slots:
	void offset_reset_button_clicked();
    	void _set_x_offset(int xo);
	void _set_y_offset(int yo);

    public:
	TranslationSelector(QWidget *parent = NULL);	
	~TranslationSelector();

	int get_x_offset()
	{
	    return _x_offset_spinbox->value();
	}

	int get_y_offset()
	{
	    return _y_offset_spinbox->value();
	}

	void set_x_offset(int xo);
	void set_y_offset(int yo);

    public slots:
        void reset();

    signals:
	void view_changed();
};

#endif
