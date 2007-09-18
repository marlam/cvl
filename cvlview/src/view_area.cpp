/*
 * view_area.cpp
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

#include "channel_selector.h"
#include "viewpoint_selector.h"
#include "interpolation_selector.h"
#include "range_selector.h"
#include "gamma_selector.h"
#include "pseudocolor_selector.h"
#include "view_area.h"


ViewArea::ViewArea(cvl_frame_t **frame, 
	int min_size,
	ChannelSelector *channel_selector,
	ViewpointSelector *viewpoint_selector,
	InterpolationSelector *interpolation_selector,
	RangeSelector *range_selector,
	GammaSelector *gamma_selector,
	PseudocolorSelector *pseudocolor_selector,
	QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    _frame = frame;
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

    _channel_selector = channel_selector;
    _viewpoint_selector = viewpoint_selector;
    _interpolation_selector = interpolation_selector;
    _range_selector = range_selector;
    _gamma_selector = gamma_selector;
    _pseudocolor_selector = pseudocolor_selector;
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
	const float epsilon = 0.0001f;
	int channel = _channel_selector->get_channel();
	float range_min = _range_selector->get_range_min(channel);
	float range_max = _range_selector->get_range_max(channel);
	float gamma = _gamma_selector->get_gamma(channel);
	bool pseudocolor = _pseudocolor_selector->is_enabled(channel);
	bool pseudocolor_cyclic = _pseudocolor_selector->is_cyclic(channel);
	float pseudocolor_startcolor = _pseudocolor_selector->get_startcolor(channel);
	float pseudocolor_lightness = _pseudocolor_selector->get_lightness(channel);
	cvl_frame_t *dst, *src;

	if (_processed_frame != *_frame)
	{
	    cvl_frame_free(_frame1);
	    cvl_frame_free(_frame2);
	    _frame1 = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame), 
		    4, CVL_UNKNOWN, CVL_FLOAT, CVL_TEXTURE);
	    _frame2 = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame), 
		    4, CVL_UNKNOWN, CVL_FLOAT, CVL_TEXTURE);
	}
	cvl_frame_free(_render_frame);

	/* Channel selector */
	if (channel == -1)
	{
	    cvl_frame_set_format(_frame1, CVL_XYZ);
	    cvl_convert_format(_frame1, *_frame);
	}
	else
	{
	    cvl_frame_set_format(_frame1, CVL_LUM);
	    cvl_channel_extract(_frame1, *_frame, channel);
	}

	/* Range selector */
	if (channel == -1)
	{
	    cvl_frame_set_format(_frame2, CVL_XYZ);
	    cvl_luminance_range(_frame2, _frame1, range_min, range_max);
	}
	else
	{
	    cvl_frame_set_format(_frame2, CVL_LUM);
	    cvl_transform_linear(_frame2, _frame1, -1, range_min, range_max);
	}

	/* Gamma correction */
	if (fabsf(gamma - 1.0) > epsilon)
	{
	    cvl_gamma_correct(_frame1, _frame2, gamma);
	    src = _frame1;
	}
	else
	{
	    src = _frame2;
	}
	dst = (src == _frame1 ? _frame2 : _frame1);

	/* Pseudo coloring */
	if (pseudocolor)
	{
	    cvl_frame_set_format(dst, CVL_HSL);
	    cvl_pseudo_color(dst, src, (channel == -1 ? 1 : 0), pseudocolor_cyclic,
	    	    pseudocolor_startcolor, pseudocolor_lightness, 0.0f, 1.0f);
	    src = dst;
	    dst = (src == _frame1 ? _frame2 : _frame1);
	}

	/* Prepare for rendering: Convert to RGB and use 8bit texture to allow
	 * interpolation even on older hardware. At this point, all values are in
	 * [0,1] anyway. */
	if (channel == -1 || pseudocolor)
	{
	    _render_frame = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
	    	    3, CVL_RGB, CVL_UINT8, CVL_TEXTURE);
	    cvl_convert_format(_render_frame, src);
	}
	else
	{
	    _render_frame = cvl_frame_new(cvl_frame_width(*_frame), cvl_frame_height(*_frame),
	    	    1, CVL_LUM, CVL_UINT8, CVL_TEXTURE);
	    glUseProgram(0);
	    cvl_transform(_render_frame, src);
	}

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
    if (_interpolation_selector->is_enabled())
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
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
    QImage frame = grabFrameBuffer();
    cvl_gl_state_restore();
    return frame.copy(_fb_x, _fb_y, _fb_w, _fb_h);
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

    pixel_info();
}

void ViewArea::wheelEvent(QWheelEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    float zoom = _viewpoint_selector->get_zoomfactor();
    int steps = event->delta() / 120;
    float diff = static_cast<float>(steps) * mh_maxf(0.01f, zoom * 0.05f);
    _viewpoint_selector->set_zoomfactor(zoom + diff);
    pixel_info();
}

float ViewArea::rgb_to_lum(float r, float g, float b)
{
    const float m1 = 0.212656f;
    const float m2 = 0.715158;
    const float m3 = 0.072186;
    if (r <= 0.04045f && g <= 0.04045f && b <= 0.04045f)
    {
	r /= 12.92f;
	g /= 12.92f;
	b /= 12.92f;
    }
    else
    {
	r = powf((r + 0.055f) / 1.055f, 2.4f);
	g = powf((g + 0.055f) / 1.055f, 2.4f);
	b = powf((b + 0.055f) / 1.055f, 2.4f);
    }
    return m1 * r + m2 * g + m3 * b;
}

float ViewArea::hsl_to_lum_helper(float tmp2, float tmp1, float H)
{
    float ret;

    if (H < 0.0f)
	H += 1.0f;
    else if (H > 1.0f)
	H -= 1.0f;

    if (H < 1.0f / 6.0f)
	ret = (tmp2 + (tmp1 - tmp2) * (360.0f / 60.0f) * H);
    else if (H < 1.0f / 2.0f)
	ret = tmp1;
    else if (H < 2.0f / 3.0f)
	ret = (tmp2 + (tmp1 - tmp2) * ((2.0f / 3.0f) - H) * (360.0f / 60.0f));
    else
	ret = tmp2;
    return ret;
}

float ViewArea::hsl_to_lum(float h, float s, float l)
{
    float r, g, b;
    float tmp1, tmp2;

    if (l < 0.5f)
	tmp1 = l * (1.0f + s);
    else
	tmp1 = (l + s) - (l * s);
    tmp2 = 2.0f * l - tmp1;
    r = hsl_to_lum_helper(tmp2, tmp1, h + (1.0f / 3.0f));
    g = hsl_to_lum_helper(tmp2, tmp1, h);
    b = hsl_to_lum_helper(tmp2, tmp1, h - (1.0f / 3.0f));

    return rgb_to_lum(r, g, b);
}

void ViewArea::pixel_info()
{
    makeCurrent();

    // Show pixel info
    float mouse_x = static_cast<float>(_mouse_pos.x());
    float mouse_y = static_cast<float>(_mouse_pos.y());
    float mouse_x_centered = mouse_x - static_cast<float>(_width) / 2.0f;
    float mouse_y_centered = mouse_y - static_cast<float>(_height) / 2.0f;
    float frame_x_centered = mouse_x_centered / _viewpoint_selector->get_zoomfactor() - static_cast<float>(_viewpoint_selector->get_x_offset());
    float frame_y_centered = mouse_y_centered / _viewpoint_selector->get_zoomfactor() - static_cast<float>(_viewpoint_selector->get_y_offset());
    int frame_x = mh_iroundf(floorf(frame_x_centered + static_cast<float>(cvl_frame_width(*_frame)) / 2.0f));
    int frame_y = mh_iroundf(floorf(frame_y_centered + static_cast<float>(cvl_frame_height(*_frame)) / 2.0f));

    if (frame_x >= 0 && frame_y >= 0 && frame_x < cvl_frame_width(*_frame) && frame_y < cvl_frame_height(*_frame))
    {
	cvl_get(*_frame, -1, frame_x, frame_y, _pixel_val);
	//mh_msg_inf("(%d,%d): %f %f %f %f", frame_x, frame_y, val[0], val[1], val[2], val[3]);
	if (cvl_frame_format(*_frame) == CVL_LUM)
	{
	    _pixel_lum = _pixel_val[0];
	    emit update_pixel_info(frame_x, frame_y, cvl_frame_channels(*_frame), _pixel_val, &_pixel_lum);
	}
	else if (cvl_frame_format(*_frame) == CVL_XYZ)
	{
	    _pixel_lum = _pixel_val[1];
	    emit update_pixel_info(frame_x, frame_y, cvl_frame_channels(*_frame), _pixel_val, &_pixel_lum);
	}
	else if (cvl_frame_format(*_frame) == CVL_RGB)
	{
	    _pixel_lum = rgb_to_lum(_pixel_val[0], _pixel_val[1], _pixel_val[2]);
	    emit update_pixel_info(frame_x, frame_y, cvl_frame_channels(*_frame), _pixel_val, &_pixel_lum);
	}
	else if (cvl_frame_format(*_frame) == CVL_HSL)
	{
	    _pixel_lum = hsl_to_lum(_pixel_val[0], _pixel_val[1], _pixel_val[2]);
	    emit update_pixel_info(frame_x, frame_y, cvl_frame_channels(*_frame), _pixel_val, &_pixel_lum);
	}
	else // UNKNOWN
	{
	    emit update_pixel_info(frame_x, frame_y, cvl_frame_channels(*_frame), _pixel_val, NULL);
	}
    }
    else
    {
	//mh_msg_inf("(%d,%d)", frame_x, frame_y);
	emit update_pixel_info(-1, -1, 0, NULL, NULL);
    }
}
