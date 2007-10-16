/*
 * range_selector.h
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

#ifndef RANGE_SELECTOR_H
#define RANGE_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QMouseEvent>
#include <QWheelEvent>

#include <cvl/cvl.h>

#include "channel_selector.h"
#include "channel_info.h"


class Selector : public QLabel
{
    Q_OBJECT

    private:
	int _tolerance;
	float _tolerance_normalized;
	bool _allow_change_left;
	bool _allow_change_right;
	bool _allow_drag;
	bool _change_left;
	bool _change_right;
	bool _drag;
	int _startpoint;
	float _range_left_normalized;
	float _range_right_normalized;
	float logtransf(float x);
	float invlogtransf(float x);
	float normalized_x_to_rangeval(float x);
	float rangeval_to_normalized_x(float y);
    public:
	Selector(QWidget *parent = NULL);
	~Selector();
	QSize sizeHint() const;
    protected:
	void paintEvent(QPaintEvent *e UNUSED);
	void mouseMoveEvent(QMouseEvent *e);
     	void mousePressEvent(QMouseEvent *e);
     	void mouseReleaseEvent(QMouseEvent *e);
   	void wheelEvent(QWheelEvent *e);
};

class RangeSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	ChannelSelector *_channel_selector;
	ChannelInfo *_channel_info;
	// Range Selector contents:
	QLabel *_range_label;
	Selector *_selector;
	QCheckBox *_log_x_box;
	QCheckBox *_log_y_box;
	// Histogram information
	int _histogram_size;
	int *_histogram;
	int _histmax[5];
	// Value range
	float _channel_min[5];
	float _channel_max[5];
	// Selected range information
	float _range_min[5];
	float _range_max[5];
	// Display properties
	bool _log_x[5];
	bool _log_y[5];
	// Current channel
	int _channel;
	// Reset?
	bool _reset_on_next_update;

    private slots:
	void set_range_min(float range_min);
	void set_range_max(float range_max);
	void set_range(float range_min, float range_max);
	void set_log_x();
	void set_log_y();

    public:
	RangeSelector(cvl_frame_t **frame,
		ChannelSelector *channel_selector,
		ChannelInfo *channel_info,
		QWidget *parent = NULL);	
	~RangeSelector();

	float get_range_min(int channel)
	{
	    return _range_min[channel + 1];
	}

	float get_range_max(int channel)
	{
	    return _range_max[channel + 1];
	}

    signals:
	void make_gl_context_current();
	void range_changed();

    public slots:
        void update_channel();
	void update();
	void reset();

    friend class Selector;
};

#endif
