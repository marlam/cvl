/*
 * view_area.h
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
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

#ifndef VIEW_AREA_H
#define VIEW_AREA_H

#include "config.h"

#include <QGLWidget>
#include <QImage>
#include <QMouseEvent>
#include <QWheelEvent>

#include <cvl/cvl.h>

#include "viewpoint_selector.h"
#include "tonemap_selector.h"
#include "postproc_selector.h"


class ViewArea : public QGLWidget
{
    Q_OBJECT

    private:
	// Image
	cvl_frame_t **_frame;
	// Size of the view area
	int _width, _height;
	// Lock
	bool _lock;
	// Parameter selectors
	ViewpointSelector *_viewpoint_selector;
	TonemapSelector *_tonemap_selector;
	PostprocSelector *_postproc_selector;
	// Processed frame
	cvl_frame_t *_processed_frame;
	// Image to be rendered
	cvl_frame_t *_render_frame;
	// Recompute _render_frame?
	bool _recompute;
	// Processing buffers
	cvl_frame_t *_frame1, *_frame2;
	// Used area of the framebuffer
	int _fb_x, _fb_y, _fb_w, _fb_h;
	// Mouse position
	QPoint _mouse_pos;
	// Dragging
	bool _dragging;
	QPoint _drag_startpoint;
	// Error handling
	bool _cvl_init_failed;
	bool _rendering_fails;

	void cvl_error_msgbox();
	void pixel_info();

    public slots:
	void make_gl_context_current();
        void recompute();
        void update();

    public:
	ViewArea(cvl_frame_t **frame, 
		ViewpointSelector *viewpoint_selector,
		TonemapSelector *tonemap_selector,
		PostprocSelector *postproc_selector,
		int min_size,
		QWidget *parent = NULL);
	~ViewArea();

	void lock()
	{
	    _lock = true;
	}

	void unlock()
	{
	    _lock = false;
	}

	QImage get_image();
	QImage get_view();

	virtual void mousePressEvent(QMouseEvent *event);
    	virtual void mouseReleaseEvent(QMouseEvent *event);
   	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);

    signals:
	void update_size(int w, int h);

    protected:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
};

#endif
