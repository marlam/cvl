/*
 * cvlview.h
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

#ifndef CVLVIEW_H
#define CVLVIEW_H

#include "config.h"

#define GLEW_STATIC 1
#include <GL/glew.h>

#include <QtCore>
#include <QMainWindow>
#include <QWidget>
#include <QToolBar>
#include <QToolBox>
#include <QDir>
#include <QLabel>

#include <cvl/cvl.h>

#include "err.h"
#include "conf.h"
#include "datafile.h"

#include "dataset_selector.h"
#include "channel_selector.h"
#include "viewpoint_selector.h"
#include "interpolation_selector.h"
#include "frame_info.h"
#include "channel_info.h"
#include "range_selector.h"
#include "gamma_selector.h"
#include "pseudocolor_selector.h"
#include "heightmap_selector.h"
#include "view_area.h"
#include "pixel_info.h"

class CVLView : public QMainWindow
{
    Q_OBJECT

    private:
	const char *_conf_file_name;
	Conf *_conf;
	QDir _last_open_dir;
	QDir _last_save_dir;

	DataFile *_datafile;
	cvl_frame_t *_frame;

	QWidget *_widget;
	QToolBar *_toolbar;
	QToolBox *_toolbox;
	DatasetSelector *_dataset_selector;
	ChannelSelector *_channel_selector;
	ViewpointSelector *_viewpoint_selector;
	InterpolationSelector *_interpolation_selector;
	FrameInfo *_frame_info;
	ChannelInfo *_channel_info;
	RangeSelector *_range_selector;
	GammaSelector *_gamma_selector;
	PseudocolorSelector *_pseudocolor_selector;
	HeightmapSelector *_heightmap_selector;
	ViewArea *_view_area;
	PixelInfo *_pixel_info;

	void save(bool whole_image);
	void copy(bool whole_image);

    protected:
   	void closeEvent(QCloseEvent *event);	
	
    public:
	CVLView();
	~CVLView();
	void activate_frame(cvl_frame_t *frame);
	void load_datafile(const char *filename);

    signals:
	void make_gl_context_current();
        void new_datafile();
        void new_frame();

    private slots:
	void open_datafile();
	void open_frame();
        void save_image();
        void save_view();
	void copy_image();
	void copy_view();
	void show_aboutbox();
};

#endif
