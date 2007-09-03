/*
 * postproc_selector.h
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

#ifndef POSTPROC_SELECTOR_H
#define POSTPROC_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSlider>

#include "conf.h"


class PostprocSelector : public QWidget
{
    Q_OBJECT
	
    private:
	QDoubleSpinBox *_gamma_spinbox;
	QSlider *_gamma_slider;
	QDoubleSpinBox *_lightness_spinbox;
	QSlider *_lightness_slider;
	QDoubleSpinBox *_contrast_spinbox;
	QSlider *_contrast_slider;
	QDoubleSpinBox *_saturation_spinbox;
	QSlider *_saturation_slider;
	QDoubleSpinBox *_sharpness_spinbox;
	QSlider *_sharpness_slider;
	bool _lock;

    private slots:
	void set_gamma(double x);
        void gamma_slider_changed(int x);
	void set_lightness(double x);
        void lightness_slider_changed(int x);
	void set_contrast(double x);
        void contrast_slider_changed(int x);
	void set_saturation(double x);
        void saturation_slider_changed(int x);
	void set_sharpness(double x);
        void sharpness_slider_changed(int x);

    public:
	PostprocSelector(QWidget *parent = NULL);	
	~PostprocSelector();

	float get_gamma()
	{
	    return static_cast<float>(_gamma_spinbox->value());
	}

	float get_lightness()
	{
	    float x = static_cast<float>(_lightness_spinbox->value());
	    return x < 0.0f ? x * 0.75f : x * 2.0f;
	}

	float get_contrast()
	{
	    float x = static_cast<float>(_contrast_spinbox->value());
	    return x < 0.0f ? x * 0.75f : x * 2.0f;
	}

	float get_saturation()
	{
	    float x = static_cast<float>(_saturation_spinbox->value());
	    return x < 0.0f ? x * 0.75f : x * 2.0f;
	}

	float get_sharpness()
	{
	    return static_cast<float>(_sharpness_spinbox->value());
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);

    signals:
	void postproc_changed();
};

#endif
