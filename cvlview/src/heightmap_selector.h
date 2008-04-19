/*
 * heightmap_selector.h
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

#ifndef HEIGHTMAP_SELECTOR_H
#define HEIGHTMAP_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QSlider>

#include <cvl/cvl.h>

class HeightmapSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	QCheckBox *_enable_box;
	QRadioButton *_ch0_button;
	QRadioButton *_ch1_button;
	QRadioButton *_ch2_button;
	QRadioButton *_ch3_button;
	QRadioButton *_height_button;
	QRadioButton *_distance_button;
	QRadioButton *_quads_button;
	QRadioButton *_strip_button;
	QRadioButton *_minmax_button;
	QRadioButton *_range_button;
	QDoubleSpinBox *_height_factor_spinbox;
	QSlider *_height_factor_slider;
	QCheckBox *_showcuboid_box;
	bool _lock;

    private slots:
        void _button_clicked();
	void _set_height_factor(double f);
	void _height_factor_slider_changed(int f);

    public:
        enum { QUADS, STRIP };
        enum { HEIGHT, DISTANCE };
        enum { MINMAX, RANGE };
	HeightmapSelector(cvl_frame_t **frame, QWidget *parent = NULL);	
	~HeightmapSelector();

	bool is_enabled() const
	{
	    return _enable_box->isChecked();
	}

	int mode() const
	{
	    return _quads_button->isChecked() ? QUADS : STRIP;
	}

	int channel() const
	{
	    return _ch0_button->isChecked() ? 0 
		: _ch1_button->isChecked() ? 1
		: _ch2_button->isChecked() ? 2
		: _ch3_button->isChecked() ? 3
		: -1;
	}

	int data() const
	{
	    return _height_button->isChecked() ? HEIGHT : DISTANCE;
	}

	int range() const
	{
	    return _minmax_button->isChecked() ? MINMAX : RANGE;
	}

	float height_factor() const
	{
	    return _height_factor_spinbox->value();
	}

	bool show_cuboid() const
	{
	    return _showcuboid_box->isChecked();
	}

    public slots:
        void update();
        void reset();

    signals:
	void make_gl_context_current();
	void heightmap_changed();
};

#endif
