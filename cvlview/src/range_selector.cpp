/*
 * range_selector.cpp
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

#include "config.h"

#include <cmath>

#include <GL/gl.h>

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QFrame>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

#include <cvl/cvl.h>

#include "mh.h"

#include "channel_selector.h"
#include "channel_info.h"
#include "range_selector.h"


Selector::Selector(QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking(true);
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sp.setHeightForWidth(false);
    setSizePolicy(sp);

    _tolerance = 2;
    _tolerance_normalized = static_cast<float>(_tolerance) / static_cast<float>(width() - 1);
    _allow_change_left = false;
    _allow_change_right = false;
    _allow_drag = false;
    _change_left = false;
    _change_right = false;
    _drag = false;
    _range_left_normalized = -1.0f;
    _range_right_normalized = +2.0f;
}

Selector::~Selector()
{
}

QSize Selector::sizeHint() const
{
    return QSize(1, 50);
}

float Selector::logtransf(float x)
{
    const float base = 50.0f;
    return mh_clampf(logf(1.0f + x * (base - 1.0f)) / logf(base), 0.0f, 1.0f);
}

float Selector::invlogtransf(float y)
{
    const float base = 50.0f;
    return mh_clampf((expf(y * logf(base)) - 1.0f) / (base - 1.0f), 0.0f, 1.0f);
}

float Selector::normalized_x_to_rangeval(float x)
{
    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    int channel = range_selector->_channel + 1;
    float channel_min = range_selector->_channel_min[channel];
    float channel_max = range_selector->_channel_max[channel];
    bool log_x = range_selector->_log_x[channel];

    return (1.0f - (log_x ? logtransf(1.0f - x) : 1.0f - x))
	* (channel_max - channel_min) + channel_min;
}

float Selector::rangeval_to_normalized_x(float x)
{
    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    int channel = range_selector->_channel + 1;
    float channel_min = range_selector->_channel_min[channel];
    float channel_max = range_selector->_channel_max[channel];
    bool log_x = range_selector->_log_x[channel];

    float r = (x - channel_min) / (channel_max - channel_min);
    return log_x ? (1.0f - invlogtransf(1.0f - r)) : r;
}

void Selector::paintEvent(QPaintEvent *e UNUSED)
{
    QPainter p(this);
    p.fillRect(1, 1, width() - 2, height() - 2, QBrush(QColor(255, 255, 255)));
    p.setPen(QColor(0, 0, 0));
    p.drawRect(0, 0, width() - 1, height() - 1);

    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    if (!(*(range_selector->_frame)))
	return;

    /* Get the data */
    int channel = range_selector->_channel + 1;
    int histogram_size = range_selector->_histogram_size;
    int *histogram = range_selector->_histogram + channel * histogram_size;
    int histmax = range_selector->_histmax[channel];
    float range_min = range_selector->_range_min[channel];
    float range_max = range_selector->_range_max[channel];
    bool log_x = range_selector->_log_x[channel];
    bool log_y = range_selector->_log_y[channel];

    /* Draw selected range */
    _range_left_normalized = rangeval_to_normalized_x(range_min);
    _range_right_normalized = rangeval_to_normalized_x(range_max);
    int range_left = 1 + static_cast<int>(static_cast<float>(width() - 3) * _range_left_normalized);
    int range_right = 1 + static_cast<int>(static_cast<float>(width() - 3) * _range_right_normalized);
    p.fillRect(range_left, 1, range_right - range_left + 1, height() - 2, QBrush(QColor(0, 255, 0)));

    /* Draw histogram */
    p.setPen(QColor(0, 0, 255));
    int last_x = 0;
    for (int bin = 0; bin < histogram_size; bin++)
    {
	float lengthf = static_cast<float>(histogram[bin]) / static_cast<float>(histmax);
	int length = static_cast<int>((log_y ? logtransf(lengthf) : lengthf) * static_cast<float>(height() - 2));
	float binf = static_cast<float>(bin) / static_cast<float>(histogram_size - 1);
	if (log_x)
	{
	    binf = logtransf(binf);
	}
	int x = 1 + mh_iroundf(binf * static_cast<float>(width() - 3));
	if (length > 0)
	{
	    if (x == last_x)
	    {
		p.drawLine(x, height() - 2 - (length - 1), x, height() - 2);
	    }
	    else
	    {
		for (int xx = last_x + 1; xx <= x; xx++)
		{
		    p.drawLine(xx, height() - 2 - (length - 1), xx, height() - 2);
		}
	    }
	}
	last_x = x;
    }
}

void Selector::mouseMoveEvent(QMouseEvent *e)
{
    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    if (!(*(range_selector->_frame)))
	return;

    if (_change_left)
    {
	int endpoint = e->x();
	int offset = endpoint - _startpoint;
	float offset_normalized = static_cast<float>(offset) / static_cast<float>(width() - 3);
	if (_range_left_normalized + offset_normalized >= _range_right_normalized - _tolerance_normalized)
	    offset_normalized = _range_right_normalized - _tolerance_normalized - _range_left_normalized;
	else if (_range_left_normalized + offset_normalized < 0.0f)
	    offset_normalized = - _range_left_normalized;
	float new_range_min = normalized_x_to_rangeval(_range_left_normalized + offset_normalized);
	range_selector->set_range_min(new_range_min);
	_startpoint = endpoint;
    }
    else if (_change_right)
    {
	int endpoint = e->x();
	int offset = endpoint - _startpoint;
	float offset_normalized = static_cast<float>(offset) / static_cast<float>(width() - 3);
	if (_range_right_normalized + offset_normalized <= _range_left_normalized + _tolerance_normalized)
	    offset_normalized = _range_left_normalized + _tolerance_normalized - _range_right_normalized;
	else if (_range_right_normalized + offset_normalized > 1.0f)
	    offset_normalized = 1.0f - _range_right_normalized;
	float new_range_max = normalized_x_to_rangeval(_range_right_normalized + offset_normalized);
	range_selector->set_range_max(new_range_max);
	_startpoint = endpoint;
    }
    else if (_drag)
    {
	int endpoint = e->x();
	int offset = endpoint - _startpoint;
	float offset_normalized = static_cast<float>(offset) / static_cast<float>(width() - 3);
	if (_range_left_normalized + offset_normalized < 0.0f)
	    offset_normalized = - _range_left_normalized;
	else if (_range_right_normalized + offset_normalized > 1.0f)
	    offset_normalized = 1.0f - _range_right_normalized;
	float new_range_min = normalized_x_to_rangeval(_range_left_normalized + offset_normalized);
	float new_range_max = normalized_x_to_rangeval(_range_right_normalized + offset_normalized);
	range_selector->set_range(new_range_min, new_range_max);
	_startpoint = endpoint;
    }
    else
    {
	float x_normalized = static_cast<float>(e->x()) / static_cast<float>(width() - 3);
	if (fabsf(x_normalized - _range_left_normalized) <= _tolerance_normalized)
	{
	    _allow_change_left = true;
	    _allow_change_right = false;
	    _allow_drag = false;
	    setCursor(QCursor(Qt::SplitHCursor));
	}
	else if (fabsf(x_normalized - _range_right_normalized) <= _tolerance_normalized)
	{
	    _allow_change_left = false;
	    _allow_change_right = true;
	    _allow_drag = false;
	    setCursor(QCursor(Qt::SplitHCursor));
	}
	else if (x_normalized >= _range_left_normalized && x_normalized <= _range_right_normalized)
	{
	    _allow_change_left = false;
	    _allow_change_right = false;
	    _allow_drag = true;
	    setCursor(QCursor(Qt::SizeHorCursor));
	}
	else
	{
	    _allow_change_left = false;
	    _allow_change_right = false;
	    _allow_drag = false;
	    unsetCursor();
	}
    }
    update();
}

void Selector::mousePressEvent(QMouseEvent *e)
{
    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    if (!(*(range_selector->_frame)))
	return;

    if (e->button() == Qt::LeftButton)
    {
	_startpoint = e->x();
	if (_allow_change_left)
	{
	    _change_left = true;
	}
	else if (_allow_change_right)
	{
	    _change_right = true;
	}
	else if (_allow_drag)
	{
	    _drag = true;
	}
    }
}

void Selector::mouseReleaseEvent(QMouseEvent *e)
{
    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    if (!(*(range_selector->_frame)))
	return;

    if (e->button() == Qt::LeftButton)
    {
	_change_left = false;
    	_change_right = false;
	_drag = false;
	emit range_selector->range_changed();
    }
}

void Selector::wheelEvent(QWheelEvent *e)
{
    RangeSelector *range_selector = reinterpret_cast<RangeSelector *>(parentWidget());
    if (!(*(range_selector->_frame)))
	return;

    float steps = static_cast<float>(e->delta() / 120);
    float new_range_left_normalized, new_range_right_normalized;
    if (steps < 0.0f)
    {
	new_range_left_normalized = mh_maxf(0.0f, _range_left_normalized + steps * 0.05);
	new_range_right_normalized = mh_min(1.0f, _range_right_normalized - steps * 0.05);
    }
    else
    {
	new_range_left_normalized = _range_left_normalized + steps * 0.05;
	new_range_right_normalized = _range_right_normalized - steps * 0.05;
	if (new_range_left_normalized + _tolerance_normalized > new_range_right_normalized - _tolerance_normalized)
	{
	    float center = (_range_right_normalized - _range_left_normalized) / 2.0 + _range_left_normalized;
	    new_range_left_normalized = center - _tolerance_normalized;
	    new_range_right_normalized = center + _tolerance_normalized;
	}
    }
    float new_range_min = normalized_x_to_rangeval(new_range_left_normalized);
    float new_range_max = normalized_x_to_rangeval(new_range_right_normalized);
    range_selector->set_range(new_range_min, new_range_max);
    update();
    emit range_selector->range_changed();
}

RangeSelector::RangeSelector(cvl_frame_t **frame,
	ChannelSelector *channel_selector,
	ChannelInfo *channel_info,
	QWidget *parent) 
	: QWidget(parent)
{
    _frame = frame;
    _channel_selector = channel_selector;
    _channel_info = channel_info;
    _reset_on_next_update = true;

    _histogram_size = 1024;
    _histogram = new int[5 * _histogram_size];
    for (int c = 0; c < 5; c++)
    {
	_range_min[c] = +1.0f;
	_range_max[c] = -1.0f;
	_log_x[c] = false;
	_log_y[c] = false;
    }
    _channel = _channel_selector->get_channel();
    _range_label = new QLabel("Range:");
    _selector = new Selector(this);
    _log_x_box = new QCheckBox("Logarithmic horizontal scale");
    _log_x_box->setCheckState(Qt::Unchecked);
    connect(_log_x_box, SIGNAL(stateChanged(int)), this, SLOT(set_log_x()));
    _log_y_box = new QCheckBox("Logarithmic vertical scale");
    _log_y_box->setCheckState(Qt::Unchecked);
    connect(_log_y_box, SIGNAL(stateChanged(int)), this, SLOT(set_log_y()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_range_label, 0, 0);
    layout->addWidget(_selector, 1, 0);
    layout->addWidget(_log_x_box, 2, 0);
    layout->addWidget(_log_y_box, 3, 0);
    layout->setRowStretch(4, 1);
    setLayout(layout);
}

RangeSelector::~RangeSelector()
{
    delete[] _histogram;
}

void RangeSelector::reset()
{
    _reset_on_next_update = true;
}

void RangeSelector::set_range_min(float range_min)
{
    _range_min[_channel + 1] = range_min;
}

void RangeSelector::set_range_max(float range_max)
{
    _range_max[_channel + 1] = range_max;
}

void RangeSelector::set_range(float range_min, float range_max)
{
    _range_min[_channel + 1] = range_min;
    _range_max[_channel + 1] = range_max;
}

void RangeSelector::set_log_x()
{
    _log_x[_channel + 1] = _log_x_box->isChecked();
    _selector->update();
}

void RangeSelector::set_log_y()
{
    _log_y[_channel + 1] = _log_y_box->isChecked();
    _selector->update();
}

void RangeSelector::update_channel()
{
    _channel = _channel_selector->get_channel();
    _range_label->setText(mh_string("Range: [%.4g, %.4g]", _channel_min[_channel + 1], _channel_max[_channel + 1]).c_str());
    _log_x_box->setCheckState(_log_x[_channel + 1] ? Qt::Checked : Qt::Unchecked);
    _log_y_box->setCheckState(_log_y[_channel + 1] ? Qt::Checked : Qt::Unchecked);
    _selector->update();
}

void RangeSelector::update()
{
    emit make_gl_context_current();

    /* Initialize information */

    // Set minmax range of histogram
    if (cvl_frame_format(*_frame) == CVL_UNKNOWN 
	    && cvl_frame_channel_name(*_frame, 0) 
	    && strcmp(cvl_frame_channel_name(*_frame, 0), "X-SAR-A") == 0
	    && (cvl_frame_channels(*_frame) == 1 
		|| (cvl_frame_channels(*_frame) == 2 
		    && cvl_frame_channel_name(*_frame, 1)
		    && strcmp(cvl_frame_channel_name(*_frame, 1), "X-SAR-P") == 0)))
    {
	// SAR Data
	_channel_min[1] = 0.0f;
    	_channel_max[1] = 1.0f;
	_log_x[1] = true;
	_log_y[1] = true;
	_channel_min[2] = - M_PI;
    	_channel_max[2] = + M_PI;
	_log_x[2] = false;
	_log_y[2] = false;
    }
    else if (cvl_frame_format(*_frame) == CVL_UNKNOWN)
    {
	// Unknown data.
    	for (int c = 0; c < 4; c++)
	{
	    _channel_min[c + 1] = _channel_info->get_min(c);
	    _channel_max[c + 1] = _channel_info->get_max(c);
	    _log_x[c + 1] = false;
	    _log_y[c + 1] = false;
	}
    }
    else if (cvl_frame_format(*_frame) == CVL_XYZ
	    && _channel_info->get_max(1) > 1.0001)
    {
	// Guess: HDR frame data, calibrated to SI units.
    	for (int c = 0; c < 4; c++)
	{
	    _channel_min[c + 1] = 0.0f;
	    _channel_max[c + 1] = _channel_info->get_max(c); 
	    _log_x[c + 1] = false;
	    _log_y[c + 1] = false;
	}
	_channel_min[0] = 0.0f;
	_channel_max[0] = _channel_info->get_max(1);
	_log_x[-1 + 1] = true;	// color channel
	_log_x[1 + 1] = true;	// Y channel
    }
    else
    {
	// Guess: LDR frame data. 
	// Could also be HDR frame data normalized to [0,1].
    	for (int c = 0; c < 4; c++)
	{
	    _channel_min[c + 1] = 0.0f;
	    _channel_max[c + 1] = 1.0f;
	    _log_x[c + 1] = false;
	    _log_y[c + 1] = false;
	}
	_channel_min[0] = 0.0f;
	_channel_max[0] = 1.0f;
	if (_reset_on_next_update)
	{
	    _log_x[0] = false;
	    _log_y[0] = false;
	}
    }

    // Build histogram
    cvl_histogram(*_frame, -1, _histogram_size, _channel_min + 1, _channel_max + 1, _histogram + _histogram_size);
    for (int c = 0; c < 4; c++)
    {
	_histmax[c + 1] = _histogram[(c + 1) * _histogram_size + 0];
	for (int i = 1; i < _histogram_size; i++)
	{
	    if (_histogram[(c + 1) * _histogram_size + i] > _histmax[c + 1])
		_histmax[c + 1] = _histogram[(c + 1) * _histogram_size + i];
	}
    }
    for (int c = 0; c < 4; c++)
    {
	if (_reset_on_next_update || _range_min[c + 1] > _range_max[c + 1] 
		|| _range_min[c + 1] < _channel_min[c + 1] || _range_max[c + 1] > _channel_max[c + 1])
	{
	    _range_min[c + 1] = _channel_min[c + 1];
	    _range_max[c + 1] = _channel_max[c + 1];
	}
    }
    if (cvl_frame_format(*_frame) != CVL_LUM && cvl_frame_format(*_frame) != CVL_UNKNOWN)
    {
	cvl_frame_t *ctmp = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
		1, CVL_LUM, CVL_FLOAT, CVL_TEXTURE);
	cvl_convert_format(ctmp, *_frame);
	cvl_histogram(ctmp, 0, _histogram_size, _channel_min, _channel_max, _histogram);
	cvl_frame_free(ctmp);
	_histmax[0] = _histogram[0];
	for (int i = 1; i < _histogram_size; i++)
	{
    	    if (_histogram[i] > _histmax[0])
		_histmax[0] = _histogram[i];
	}
	if (_reset_on_next_update || _range_min[0] > _range_max[0]
		|| _range_min[0] < _channel_min[0] || _range_max[0] > _channel_max[0]);
	{
	    _range_min[0] = _channel_min[0];
	    _range_max[0] = _channel_max[0];
	}
    }
    
    _reset_on_next_update = false;
    update_channel();	// this will update everything
}
