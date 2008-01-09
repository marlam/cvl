/*
 * view_area.cpp
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#include <cstring>
#include <cmath>

#define GLEW_STATIC 1
#include <GL/glew.h>

#include <QGLWidget>
#include <QImage>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWheelEvent>

#include <cvl/cvl.h>

#include "mh.h"

#include "viewpoint_selector.h"
#include "tonemap_selector.h"
#include "postproc_selector.h"
#include "view_area.h"


ViewArea::ViewArea(cvl_frame_t **frame, 
	ViewpointSelector *viewpoint_selector,
	TonemapSelector *tonemap_selector,
	int min_size,
	QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    _frame = frame;
    _viewpoint_selector = viewpoint_selector;
    _tonemap_selector = tonemap_selector;
    _lock = false;
    _mouse_pos = QPoint(0, 0);
    _dragging = false;
    _rendering_fails = false;
    _processed_frame = NULL;
    _render_frame = NULL;
    _recompute = true;
    _frame1 = NULL;
    _frame2 = NULL;
    setMinimumSize(min_size, min_size);
    setMouseTracking(true);
    _fb_x = -1;
    // The following is necessary to force a call to initializeGL(), so that the
    // application can be sure that both OpenGL and CVL are initialized when
    // this Constructor finished.
    update();
}

ViewArea::~ViewArea()
{
    if (!_cvl_init_failed)
    {
	makeCurrent();
	cvl_frame_free(_frame1);
	cvl_frame_free(_frame2);
	cvl_frame_free(_render_frame);
	cvl_deinit();
    }
}

void ViewArea::cvl_error_msgbox()
{
    makeCurrent();
    QMessageBox::critical(this, tr("Error"), 
	    tr("<p>Rendering failed. No further rendering will be done.</p>"
		"<p>Please report this bug to <a href=\"mailto:%1\">%1</a><br>"
		"and include the following error message:</p>"
		"<pre>%2</pre>").arg(PACKAGE_BUGREPORT).arg(cvl_error_msg()));
}

void ViewArea::initializeGL()
{
    cvl_init();

    if (cvl_error())
    {
	_cvl_init_failed = true;
	_rendering_fails = true;
	QMessageBox::critical(this, tr("Error"), 
	    	tr("<p>Cannot initialize the CVL library:"
	    	    "<pre>%1</pre>"
		    "Please make sure that you use the latest drivers "
		    "for your graphics card.</p>").arg(cvl_error_msg()));
    }
    else
    {
	_cvl_init_failed = false;
    }
}

void ViewArea::resizeGL(int width, int height)
{
    _width = width;
    _height = height;
    emit update_size(width, height);
}

void ViewArea::paintGL()
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    lock();

    if (_recompute)
    {
	if (_processed_frame != *_frame)
	{
	    cvl_frame_free(_frame1);
	    cvl_frame_free(_frame2);
	    cvl_frame_free(_render_frame);
	    _frame1 = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
    		    3, CVL_XYZ, CVL_FLOAT, CVL_TEXTURE);
   	    _frame2 = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
  		    3, CVL_XYZ, CVL_FLOAT, CVL_TEXTURE);
	    _render_frame = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
		    3, CVL_RGB, CVL_UINT8, CVL_TEXTURE);
	}

	/* Tone Mapping */
	if (_tonemap_selector->active_tonemap_method() == TonemapSelector::RANGE_SELECTION)
	{
	    TonemapRangeSelectionParameterSelector *parameter_selector
		= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(
			_tonemap_selector->parameter_selector());
	    float lum_min = parameter_selector->get_min_luminance();
	    float lum_max = parameter_selector->get_max_luminance();
	    cvl_luminance_range(_frame1, *_frame, lum_min, lum_max);
	}
	else if (_tonemap_selector->active_tonemap_method() == TonemapSelector::SCHLICK94)
	{
	    TonemapSchlick94ParameterSelector *parameter_selector
		= reinterpret_cast<TonemapSchlick94ParameterSelector *>(
			_tonemap_selector->parameter_selector());
	    float p = parameter_selector->get_p();
	    cvl_tonemap_schlick94(_frame1, *_frame, p);
	}
	else if (_tonemap_selector->active_tonemap_method() == TonemapSelector::TUMBLINRUSHMEIER99)
	{
	    TonemapTumblinRushmeier99ParameterSelector *parameter_selector
		= reinterpret_cast<TonemapTumblinRushmeier99ParameterSelector *>(
			_tonemap_selector->parameter_selector());
	    float max_abs_lum = parameter_selector->get_max_abs_lum();
	    float disp_adapt_level = parameter_selector->get_disp_adapt_level();
	    float max_contrast = parameter_selector->get_max_contrast();
	    cvl_tonemap_tumblinrushmeier99(_frame1, *_frame, max_abs_lum, disp_adapt_level, max_contrast);
	}
	else if (_tonemap_selector->active_tonemap_method() == TonemapSelector::DRAGO03)
	{
	    TonemapDrago03ParameterSelector *parameter_selector
		= reinterpret_cast<TonemapDrago03ParameterSelector *>(
			_tonemap_selector->parameter_selector());
	    float max_abs_lum = parameter_selector->get_max_abs_lum();
	    float bias = parameter_selector->get_bias();
	    float max_disp_lum = parameter_selector->get_max_disp_lum();
	    cvl_tonemap_drago03(_frame1, *_frame, max_abs_lum, bias, max_disp_lum);
	}
	else if (_tonemap_selector->active_tonemap_method() == TonemapSelector::DURAND02)
	{
	    TonemapDurand02ParameterSelector *parameter_selector
		= reinterpret_cast<TonemapDurand02ParameterSelector *>(
			_tonemap_selector->parameter_selector());
	    float max_abs_lum = parameter_selector->get_max_abs_lum();
	    float sigma_spatial = parameter_selector->get_sigma_spatial();
	    float sigma_luminance = parameter_selector->get_sigma_luminance();
	    float base_contrast = parameter_selector->get_base_contrast();
	    cvl_tonemap_durand02(_frame1, *_frame, max_abs_lum,
		    mh_mini(4, cvl_gauss_sigma_to_k(sigma_spatial)),
		    sigma_spatial, sigma_luminance, base_contrast);
	}

	/* Postprocessing */
	
	PostprocSelector *postproc_selector = _tonemap_selector->postproc_selector();
	float gamma = postproc_selector->get_gamma();
	float lightness = postproc_selector->get_lightness();
	float contrast = postproc_selector->get_contrast();
	float saturation = postproc_selector->get_saturation();
	float sharpness = postproc_selector->get_sharpness();
	cvl_frame_t *src = _frame1;
	cvl_frame_t *dst = _frame2;
	const float epsilon = 0.0001f;

	if (fabsf(gamma - 1.0f) > epsilon)
	{
	    cvl_gamma_correct(dst, src, gamma);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	}
	if (fabsf(saturation) > epsilon || fabsf(lightness) > epsilon || fabsf(contrast) > epsilon)
	{
	    cvl_frame_set_format(dst, CVL_HSL);
	    cvl_convert_format(dst, src);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	    cvl_frame_set_format(dst, CVL_HSL);
	    cvl_color_adjust(dst, src, 0.0f, saturation, lightness, contrast);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	    cvl_frame_set_format(dst, CVL_XYZ);
	    cvl_convert_format(dst, src);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	    cvl_frame_set_format(dst, CVL_XYZ);
	}
	if (sharpness < - epsilon)
	{
	    float sigma = - sharpness * 2.0f;
	    int k = cvl_gauss_sigma_to_k(sigma);
	    cvl_gauss(dst, src, k, k, sigma, sigma);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	}
	else if (sharpness > epsilon)
	{
	    float c = sharpness * 0.75f;
	    cvl_laplace(dst, src, c);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	}

	/* Prepare for rendering: Convert to RGB and use 8bit texture to allow
	 * interpolation even on older hardware. At this point, all values are in
	 * [0,1] anyway. */
	cvl_convert_format(_render_frame, src);

	_processed_frame = *_frame;
    	_recompute = false;
    }

    if (cvl_error())
    {
	_rendering_fails = true;
	cvl_error_msgbox();
	return;
    }
    
    /* Viewpoint selector */
    float zoom = _viewpoint_selector->get_zoomfactor();
    int x_offset = _viewpoint_selector->get_x_offset();
    int y_offset = _viewpoint_selector->get_y_offset();

    int w = cvl_frame_width(_render_frame);
    int h = cvl_frame_height(_render_frame);
    float x = static_cast<float>(w) / static_cast<float>(_width);
    float y = static_cast<float>(h) / static_cast<float>(_height);
    float xo = static_cast<float>(2 * x_offset) / static_cast<float>(_width);
    float yo = static_cast<float>(2 * y_offset) / static_cast<float>(_height);

    GLuint render_texture = cvl_frame_texture(_render_frame);

    /* Use OpenGL for rendering */
    cvl_gl_state_save();
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, _width, _height);
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glScalef(zoom, zoom, 1.0f);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-x + xo, +y - yo, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(+x + xo, +y - yo, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(+x + xo, -y - yo, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-x + xo, -y - yo, 0.0f);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    cvl_gl_state_restore();

    /* Save area of the framebuffer that was rendered to */
    _fb_x = mh_clampi(mh_iroundf(_width / 2.0f + (-x + xo) * 0.5f * zoom * static_cast<float>(_width)), 0, _width - 1);
    _fb_y = mh_clampi(mh_iroundf(_height / 2.0f - (+y - yo) * 0.5f * zoom * static_cast<float>(_height)), 0, _height - 1);
    _fb_w = mh_clampi(mh_iroundf(x * zoom * static_cast<float>(_width)), 1, _width - _fb_x);
    _fb_h = mh_clampi(mh_iroundf(y * zoom * static_cast<float>(_height)), 1, _height - _fb_y);

    unlock();
}

void ViewArea::recompute()
{
    _recompute = true;
    updateGL();
}

void ViewArea::update()
{
    updateGL();
}

void ViewArea::make_gl_context_current()
{
    makeCurrent();
}

QImage ViewArea::get_image()
{
    if (_rendering_fails || !*_frame || _lock || _fb_x < 0)
	return QImage();

    makeCurrent();
    int w = cvl_frame_width(_render_frame);
    int h = cvl_frame_height(_render_frame);
    uint8_t *p = static_cast<uint8_t *>(cvl_frame_pointer(_render_frame));
    QImage img(w, h, QImage::Format_RGB32); 
    for (int y = 0; y < h; y++)
    {
	for (int x = 0; x < w; x++)
	{
	    uint8_t *pixel = p + 3 * (y * w + x);
	    img.setPixel(x, y, qRgb(pixel[0], pixel[1], pixel[2]));
	}
    }
    return img;
}

QImage ViewArea::get_view()
{
    if (_rendering_fails || !*_frame || _lock || _fb_x < 0)
	return QImage();

    makeCurrent();
    cvl_gl_state_save();
    QImage img = grabFrameBuffer();
    cvl_gl_state_restore();
    return img.copy(_fb_x, _fb_y, _fb_w, _fb_h);
}

void ViewArea::mousePressEvent(QMouseEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    if (event->button() == Qt::LeftButton)
    {
	_drag_startpoint = event->pos();
	_dragging = true;
    }
}

void ViewArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    if (event->button() == Qt::LeftButton)
    {
	_dragging = false;
    }
}

void ViewArea::mouseMoveEvent(QMouseEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    _mouse_pos = event->pos();

    if (_dragging)
    {
	// Drag
       	QPoint drag_endpoint = event->pos();
	int drag_offset_x = mh_iroundf(static_cast<float>(drag_endpoint.x() - _drag_startpoint.x())
		/ _viewpoint_selector->get_zoomfactor());
	int drag_offset_y = mh_iroundf(static_cast<float>(drag_endpoint.y() - _drag_startpoint.y())
		/ _viewpoint_selector->get_zoomfactor());
	_drag_startpoint = drag_endpoint;
	lock();
	_viewpoint_selector->set_x_offset(_viewpoint_selector->get_x_offset() + drag_offset_x);
	_viewpoint_selector->set_y_offset(_viewpoint_selector->get_y_offset() + drag_offset_y);
	unlock();
	update();
    }
}

void ViewArea::wheelEvent(QWheelEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    float zoom = _viewpoint_selector->get_zoomfactor();
    int steps = event->delta() / 120;
    float diff = static_cast<float>(steps) * mh_maxf(0.01f, zoom * 0.05f);
    _viewpoint_selector->set_zoomfactor(zoom + diff);
}
