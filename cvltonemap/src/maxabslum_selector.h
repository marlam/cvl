/*
 * maxabslum_selector.h
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
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

#ifndef MAXABSLUM_H
#define MAXABSLUM_H

#include "config.h"

#include <QWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSlider>

#include <cvl/cvl.h>

#include "conf.h"


class MaxAbsLumSelector : public QWidget
{
    Q_OBJECT

    private:
	double _slider_param;
	float _min;
	float _max;
	float _default;
	const char *_id;
	cvl_frame_t **_frame;
	float _max_abs_lum;
	bool _lock;
	QCheckBox *_checkbox;
	QDoubleSpinBox *_spinbox;
	QSlider *_slider;

    private:
	int val_to_slider(double x);
	double slider_to_val(int x);

    private slots:
	void set_state(int s UNUSED);
	void set(double x);
	void slider_changed(int x);

    public:
	MaxAbsLumSelector(const char *id, cvl_frame_t **frame, QWidget *parent = NULL);
	~MaxAbsLumSelector();

	void update();

	float value() const
	{
	    return _spinbox->value();
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);

    signals:
	void maxabslum_changed();
};

#endif
