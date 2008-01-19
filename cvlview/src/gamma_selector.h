/*
 * gamma_selector.h
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

#ifndef GAMMA_SELECTOR_H
#define GAMMA_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSlider>

#include "channel_selector.h"


class GammaSelector : public QWidget
{
    Q_OBJECT
	
    private:
	ChannelSelector *_channel_selector;
	QCheckBox *_enable_box;
	QDoubleSpinBox *_gamma_spinbox;
	QSlider *_gamma_slider;
	bool _enabled[5];
	float _gamma[5];
	bool _lock;

    private slots:
	void _set_enable(int e UNUSED);
	void _set_gamma(double g);
	void _gamma_slider_changed(int g);

    public:
	GammaSelector(ChannelSelector *channel_selector, QWidget *parent = NULL);	
	~GammaSelector();

	float get_gamma(int channel)
	{
	    return _enabled[channel + 1] ? _gamma[channel + 1] : 1.0f;
	}

    public slots:
        void update_channel();
        void reset();

    signals:
	void gamma_changed();
};

#endif
