/*
 * channel_info.h
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

#ifndef CHANNEL_INFO_H
#define CHANNEL_INFO_H

#include "config.h"

#include <QWidget>
#include <QLabel>
#include <QCheckBox>

#include <cvl/cvl.h>

#include "channel_selector.h"


class ChannelInfo : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	ChannelSelector *_channel_selector;
	QLabel *_min_label;
	QLabel *_max_label;
	QCheckBox *_extended_box;
	QLabel *_meanstr_label;
	QLabel *_mean_label;
	QLabel *_medianstr_label;
	QLabel *_median_label;
	QLabel *_stddevstr_label;
	QLabel *_stddev_label;
	QLabel *_dynrangestr_label;
	QLabel *_dynrange_label;
	float _min[5], _max[5];
	float _mean[5], _median[5], _stddev[5], _dynrange[5];
	cvl_frame_t *_processed_frame;
	cvl_frame_t *_processed_frame_extended;
	float to_decibel(float dr);

    private slots:
	void set_extended(int e UNUSED);

    public slots:
	void update();

    public:
	ChannelInfo(cvl_frame_t **frame, 
		ChannelSelector *channel_selector,
		QWidget *parent = NULL);	
	~ChannelInfo();

	float get_min(int channel)
	{
	    return _min[channel + 1];
	}

	float get_max(int channel)
	{
	    return _max[channel + 1];
	}

    signals:
	void make_gl_context_current();
};

#endif
