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

#include <cvl/cvl.h>

class HeightmapSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	QCheckBox *_enable_box;
	QRadioButton *_ch0_button;
	QRadioButton *_ch1_button;
	QRadioButton *_ch2_button;
	QRadioButton *_ch3_button;
	QRadioButton *_quads_button;
	QRadioButton *_strip_button;

    private slots:
	void _set_enable(int e);
        void _button_clicked();

    public:
        enum { QUADS, STRIP };
	HeightmapSelector(cvl_frame_t **frame, QWidget *parent = NULL);	
	~HeightmapSelector();

	bool is_enabled()
	{
	    return _enable_box->isChecked();
	}

	int mode()
	{
	    return _quads_button->isChecked() ? QUADS : STRIP;
	}

	int channel()
	{
	    return _ch0_button->isChecked() ? 0 
		: _ch1_button->isChecked() ? 1
		: _ch2_button->isChecked() ? 2
		: _ch3_button->isChecked() ? 3
		: -1;
	}

    public slots:
        void update();
        void reset();

    signals:
	void make_gl_context_current();
	void heightmap_changed();
};

#endif
