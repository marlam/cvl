/*
 * channel_info.cpp
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

#include <cstdlib>
#include <cmath>

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>

#include <cvl/cvl.h>

#include "mh.h"

#include "channel_selector.h"
#include "channel_info.h"


ChannelInfo::ChannelInfo(cvl_frame_t **frame,
	ChannelSelector *channel_selector, 
	QWidget *parent) 
	: QWidget(parent)
{
    _frame = frame;
    _processed_frame = NULL;
    _channel_selector = channel_selector;

    QGridLayout *layout = new QGridLayout;
    
    QLabel *minstr_label = new QLabel("Min:");
    layout->addWidget(minstr_label, 0, 0);
    _min_label = new QLabel("");
    layout->addWidget(_min_label, 0, 1);

    QLabel *maxstr_label = new QLabel("Max:");
    layout->addWidget(maxstr_label, 1, 0);
    _max_label = new QLabel("");
    layout->addWidget(_max_label, 1, 1);

    _extended_box = new QCheckBox("Extended statistics (slow)");
    _extended_box->setCheckState(Qt::Unchecked);
    connect(_extended_box, SIGNAL(stateChanged(int)), this, SLOT(set_extended(int)));
    layout->addWidget(_extended_box, 2, 0, 1, 2);
    
    _meanstr_label = new QLabel("Mean:");
    _meanstr_label->setEnabled(false);
    layout->addWidget(_meanstr_label, 3, 0);
    _mean_label = new QLabel("");
    layout->addWidget(_mean_label, 3, 1);

    _medianstr_label = new QLabel("Median:");
    _medianstr_label->setEnabled(false);
    layout->addWidget(_medianstr_label, 4, 0);
    _median_label = new QLabel("");
    layout->addWidget(_median_label, 4, 1);

    _stddevstr_label = new QLabel("Std. Deviation:");
    _stddevstr_label->setEnabled(false);
    layout->addWidget(_stddevstr_label, 5, 0);
    _stddev_label = new QLabel("");
    layout->addWidget(_stddev_label, 5, 1);

    _dynrangestr_label = new QLabel("Dynamic range:");
    _dynrangestr_label->setEnabled(false);
    layout->addWidget(_dynrangestr_label, 6, 0);
    _dynrange_label = new QLabel("");
    layout->addWidget(_dynrange_label, 6, 1);

    layout->setRowStretch(7, 1);
    setLayout(layout);
}

ChannelInfo::~ChannelInfo()
{
}

void ChannelInfo::reset()
{
    _extended_box->setCheckState(Qt::Unchecked);
}

float ChannelInfo::to_decibel(float dr)
{
    return 10.0f * logf(dr) / logf(10.0f);
}

void ChannelInfo::set_extended(int e UNUSED)
{
    update();
}

void ChannelInfo::update()
{
    if (!*_frame)
	return;

    emit make_gl_context_current();

    if (cvl_error())
	return;

    if (_extended_box->checkState() == Qt::Checked)
    {
    	if (_processed_frame != *_frame || _processed_frame_extended != *_frame)
	{
	    cvl_statistics(*_frame, _min + 1, _max + 1, _median + 1, _mean + 1, _stddev + 1, _dynrange + 1);
	    if (cvl_frame_format(*_frame) != CVL_LUM && cvl_frame_format(*_frame) != CVL_UNKNOWN)
	    {
		float cmin[4], cmax[4], cmedian[4], cmean[4], cstddev[4], cdynrange[4];
		cvl_frame_t *ctmp = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
			1, CVL_LUM, CVL_FLOAT, CVL_TEXTURE);
		cvl_convert_format(ctmp, *_frame);
		cvl_statistics(ctmp, cmin, cmax, cmedian, cmean, cstddev, cdynrange);
		cvl_frame_free(ctmp);
		_min[0] = cmin[0];
		_max[0] = cmax[0];
		_median[0] = cmedian[0];
		_mean[0] = cmean[0];
		_stddev[0] = cstddev[0];
		_dynrange[0] = cdynrange[0];
	    }
	    _processed_frame = *_frame;
	    _processed_frame_extended = *_frame;
	}
	int c = _channel_selector->get_channel();
	_min_label->setText(mh_string("%.6g", _min[c + 1]).c_str());
	_max_label->setText(mh_string("%.6g", _max[c + 1]).c_str());
	_meanstr_label->setEnabled(true);
	_mean_label->setText(mh_string("%.6g", _mean[c + 1]).c_str());
	_medianstr_label->setEnabled(true);
	_median_label->setText(mh_string("%.6g", _median[c + 1]).c_str());
	_stddevstr_label->setEnabled(true);
	_stddev_label->setText(mh_string("%.6g", _stddev[c + 1]).c_str());
	_dynrangestr_label->setEnabled(true);
	_dynrange_label->setText(mh_string("%.6g dB", to_decibel(_dynrange[c + 1])).c_str());
    }
    else
    {
    	if (_processed_frame != *_frame)
	{
	    cvl_statistics(*_frame, _min + 1, _max + 1, NULL, NULL, NULL, NULL);
	    if (cvl_frame_format(*_frame) != CVL_LUM && cvl_frame_format(*_frame) != CVL_UNKNOWN)
	    {
		float cmin[4], cmax[4];
		cvl_frame_t *ctmp = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
			1, CVL_LUM, CVL_FLOAT, CVL_TEXTURE);
		cvl_convert_format(ctmp, *_frame);
		cvl_statistics(ctmp, cmin, cmax, NULL, NULL, NULL, NULL);
		cvl_frame_free(ctmp);
		_min[0] = cmin[0];
		_max[0] = cmax[0];
	    }
	    _processed_frame = *_frame;
	}
	int c = _channel_selector->get_channel();
	_min_label->setText(mh_string("%.6g", _min[c + 1]).c_str());
	_max_label->setText(mh_string("%.6g", _max[c + 1]).c_str());
	_meanstr_label->setEnabled(false);
	_mean_label->setText("");
	_medianstr_label->setEnabled(false);
	_median_label->setText("");
	_stddevstr_label->setEnabled(false);
	_stddev_label->setText("");
	_dynrangestr_label->setEnabled(false);
	_dynrange_label->setText("");
    }
}
