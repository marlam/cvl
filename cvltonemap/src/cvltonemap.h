/*
 * cvltonemap.h
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
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

#ifndef CVLTONEMAP_H
#define CVLTONEMAP_H

#include "config.h"

#include <GL/gl.h>

#include <QtCore>
#include <QMainWindow>
#include <QWidget>
#include <QToolBar>
#include <QDir>

#include <cvl/cvl.h>

#include "err.h"
#include "conf.h"

#include "tonemap_selector.h"
#include "viewpoint_selector.h"
#include "precision_selector.h"
#include "view_area.h"

class CVLTonemap : public QMainWindow
{
    Q_OBJECT

    private:
	const char *_conf_file_name;
	Conf *_conf;
	QDir _last_open_dir;
	QDir _last_save_dir;
	Conf *_parameters;
	string *_parameters_file_name;

	cvl_frame_t *_frame;
	string *_frame_filename;

	QWidget *_widget;
	QToolBar *_toolbar;
	TonemapSelector *_tonemap_selector;
	ViewpointSelector *_viewpoint_selector;
	PrecisionSelector *_precision_selector;
	ViewArea *_view_area;

	void save(bool whole_image);
	void copy(bool whole_image);
	void save_parameters(const char *file_name);
	void load_parameters(const char *file_name);

    protected:
   	void closeEvent(QCloseEvent *event);	
	
    public:
	CVLTonemap();
	~CVLTonemap();
	void load_image(const char *filename);

    signals:
	void make_gl_context_current();
        void new_image();

    private slots:
	void open_image();
        void save_image();
        void save_view();
	void load_parameters();
	void save_parameters();
	void save_parameters_as();
	void copy_image();
	void copy_view();
	void change_precision();
	void show_aboutbox();
};

#endif
