/*
 * scale_selector.h
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

#ifndef SCALE_SELECTOR_H
#define SCALE_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QDoubleSpinBox>
#include <QPushButton>

#include <cvl/cvl.h>


class ScaleSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	QPushButton *_scale_fit_button;
	QPushButton *_scale_reset_button;
	QDoubleSpinBox *_scalefactor_spinbox;
	int _view_area_width;
	int _view_area_height;
	bool _lock;

    private slots:
	void scale_fit_button_clicked();
	void scale_reset_button_clicked();
	void _set_scalefactor(double zf);

    public:
	ScaleSelector(cvl_frame_t **frame, QWidget *parent = NULL);	
	~ScaleSelector();

	float get_scalefactor()
	{
	    return _scalefactor_spinbox->value();
	}

	void set_scalefactor(float zf);

    public slots:
	void update_view_area_size(int w, int h)
	{
	    _view_area_width = w;
	    _view_area_height = h;
	}

        void reset();

    signals:
        void make_gl_context_current();
	void view_changed();
};

#endif
