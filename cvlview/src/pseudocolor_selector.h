/*
 * pseudocolor_selector.h
 *
 * This file is part of cvlview, an image viewer using the CVL library.
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

#ifndef PSEUDOCOLOR_SELECTOR_H
#define PSEUDOCOLOR_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSlider>

#include "channel_selector.h"

class PseudocolorSelector : public QWidget
{
    Q_OBJECT
	
    private:
	ChannelSelector *_channel_selector;
	QCheckBox *_enable_box;
	QCheckBox *_cyclic_box;
	QDoubleSpinBox *_startcolor_spinbox;
	QSlider *_startcolor_slider;
	QDoubleSpinBox *_lightness_spinbox;
	QSlider *_lightness_slider;
	bool _lock;
	bool _enabled[5];
	bool _cyclic[5];
	float _startcolor[5];
	float _lightness[5];

    private slots:
	void _set_enable(int e UNUSED);
	void _set_cyclic(int x UNUSED);
	void _set_startcolor(double s);
        void _startcolor_slider_changed(int s);
	void _set_lightness(double l);
        void _lightness_slider_changed(int l);

    public:
	PseudocolorSelector(ChannelSelector *channel_selector, QWidget *parent = NULL);	
	~PseudocolorSelector();

	bool is_enabled(int c)
	{
	    return _enabled[c + 1];
	}

	bool is_cyclic(int c)
	{
	    return _cyclic[c + 1];
	}

	float get_startcolor(int c)
	{
	    return _startcolor[c + 1];
	}

	float get_lightness(int c)
	{
	    return _lightness[c + 1];
	}

    public slots:
	void update_channel();

    signals:
	void pseudocolor_changed();
};

#endif
