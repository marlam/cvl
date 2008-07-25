/*
 * precision_selector.h
 *
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

#ifndef PRECISION_SELECTOR_H
#define PRECISION_SELECTOR_H

#include "config.h"

#include <QComboBox>


class PrecisionSelector : public QWidget
{
    Q_OBJECT

    private:
	QComboBox *_combo_box;
	bool _lock;

    private slots:
	void _selection(int);

    public:
        static const int FLOAT16 = 0;
	static const int FLOAT32 = 1;
	PrecisionSelector(QWidget *parent = NULL);	
	~PrecisionSelector();
	int get_precision();
	void set_precision(int);

    signals:
	void precision_changed();
};

#endif
