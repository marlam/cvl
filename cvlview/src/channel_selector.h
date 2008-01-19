/*
 * channel_selector.h
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

#ifndef CHANNEL_SELECTOR_H
#define CHANNEL_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QPushButton>

#include <cvl/cvl.h>

class ChannelSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	QPushButton *_channel_button[4];
	QPushButton *_color_button;
	int _channel;
	bool _reset_on_next_update;

    private slots:
	void ch0_button_clicked();
    	void ch1_button_clicked();
	void ch2_button_clicked();
	void ch3_button_clicked();
	void color_button_clicked();

    public:
	ChannelSelector(cvl_frame_t **frame, QWidget *parent = NULL);	
	~ChannelSelector();

	int get_channel(void)
	{
	    return _channel;
	}

    signals:
	void make_gl_context_current();
	void channel_changed();

    public slots:
	void update();
        void reset();
};

#endif
