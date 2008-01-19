/*
 * viewpoint_selector.h
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

#ifndef VIEWPOINT_SELECTOR_H
#define VIEWPOINT_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>

#include <cvl/cvl.h>


class ViewpointSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	QPushButton *_zoom_fit_button;
	QPushButton *_zoom_reset_button;
	QPushButton *_offset_reset_button;
	QDoubleSpinBox *_zoomfactor_spinbox;
	QSpinBox *_x_offset_spinbox;
	QSpinBox *_y_offset_spinbox;
	int _view_area_width;
	int _view_area_height;
	bool _lock;

    private slots:
	void zoom_fit_button_clicked();
	void zoom_reset_button_clicked();
	void offset_reset_button_clicked();
	void _set_zoomfactor(double zf);
    	void _set_x_offset(int xo);
	void _set_y_offset(int yo);

    public:
	ViewpointSelector(cvl_frame_t **frame, QWidget *parent = NULL);	
	~ViewpointSelector();

	float get_zoomfactor()
	{
	    return _zoomfactor_spinbox->value();
	}

	int get_x_offset()
	{
	    return _x_offset_spinbox->value();
	}

	int get_y_offset()
	{
	    return _y_offset_spinbox->value();
	}

	void set_zoomfactor(float zf);
	void set_x_offset(int xo);
	void set_y_offset(int yo);

    public slots:
	void update_view_area_size(int w, int h)
	{
	    _view_area_width = w;
	    _view_area_height = h;
	}

    signals:
        void make_gl_context_current();
	void viewpoint_changed();
};

#endif
