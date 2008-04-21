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

#include "color_selector.h"


class HeightmapSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	QCheckBox *_enable_box;
	QRadioButton *_channel_button[4];
	QRadioButton *_height_button;
	QRadioButton *_distance_button;
	QRadioButton *_quads_button;
	QRadioButton *_strip_button;
	QRadioButton *_minmax_button;
	QRadioButton *_range_button;
	QDoubleSpinBox *_height_factor_spinbox;
	QSlider *_height_factor_slider;
	QCheckBox *_showcuboid_box;
	ColorSelector *_color_selector;
	bool _lock;
	bool _reset_on_next_update;

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
	    return _channel_button[0]->isChecked() ? 0 
		: _channel_button[1]->isChecked() ? 1
		: _channel_button[2]->isChecked() ? 2
		: _channel_button[3]->isChecked() ? 3
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

	void get_cuboid_color(float *rgb)
	{
	    rgb[0] = _color_selector->get_r();
	    rgb[1] = _color_selector->get_g();
	    rgb[2] = _color_selector->get_b();
	}

    public slots:
        void update();
        void reset();

    signals:
	void make_gl_context_current();
	void heightmap_changed();
};

#endif
