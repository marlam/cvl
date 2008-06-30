/*
 * view_area.cpp
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

#include "glvm.h"
#include "glvm-gl.h"
using namespace glvm;
#include "arcball.h"

#include "channel_info.h"
#include "channel_selector.h"
#include "scale_selector.h"
#include "translation_selector.h"
#include "rotation_selector.h"
#include "interpolation_selector.h"
#include "color_selector.h"
#include "grid_selector.h"
#include "range_selector.h"
#include "gamma_selector.h"
#include "pseudocolor_selector.h"
#include "heightmap_selector.h"
#include "view_area.h"

#include "heightmap_quads_vs.glsl.h"
#include "heightmap_strip_vs.glsl.h"
#include "heightmap_fs.glsl.h"


ViewArea::ViewArea(cvl_frame_t **frame, 
	int min_size,
	ChannelInfo *channel_info,
	ChannelSelector *channel_selector,
	ScaleSelector *scale_selector,
	TranslationSelector *translation_selector,
	RotationSelector *rotation_selector,
	InterpolationSelector *interpolation_selector,
	ColorSelector *color_selector,
	GridSelector *grid_selector,
	RangeSelector *range_selector,
	GammaSelector *gamma_selector,
	PseudocolorSelector *pseudocolor_selector,
	HeightmapSelector *heightmap_selector,
	QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    _frame = frame;
    _lock = false;
    _mouse_pos = QPoint(0, 0);
    _dragging = false;
    _arcball = new ArcBall(min_size, min_size);
    _rotation = quat(1.0f);
    _rendering_fails = false;
    _processed_frame = NULL;
    _render_frame = NULL;
    _recompute = true;
    _frame1 = NULL;
    _frame2 = NULL;
    _flat_view = true;
    _heightmap_quads_prg = 0;
    _heightmap_strip_prg = 0;
    _heightmap_texcoord0_buffer = 0;
    _heightmap_texcoord1_buffer = 0;
    _heightmap_vertex_buffer = 0;
    _heightmap_buffers_are_current = false;
    setMinimumSize(min_size, min_size);
    setMouseTracking(true);

    _channel_info = channel_info;
    _channel_selector = channel_selector;
    _scale_selector = scale_selector;
    _translation_selector = translation_selector;
    _rotation_selector = rotation_selector;
    _interpolation_selector = interpolation_selector;
    _color_selector = color_selector;
    _grid_selector = grid_selector;
    _range_selector = range_selector;
    _gamma_selector = gamma_selector;
    _pseudocolor_selector = pseudocolor_selector;
    _heightmap_selector = heightmap_selector;
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
	cvl_gl_program_free(_heightmap_quads_prg);
	cvl_gl_program_free(_heightmap_strip_prg);
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
	_heightmap_quads_prg = cvl_gl_program_new_src("heightmap-quads", 
	    	HEIGHTMAP_QUADS_VS_GLSL_STR, HEIGHTMAP_FS_GLSL_STR);
	_heightmap_strip_prg = cvl_gl_program_new_src("heightmap-strip", 
		HEIGHTMAP_STRIP_VS_GLSL_STR, HEIGHTMAP_FS_GLSL_STR);
	if (cvl_error())
	{
	    _rendering_fails = true;
	    cvl_error_msgbox();
	}
    }
}

void ViewArea::resizeGL(int width, int height)
{
    _width = width;
    _height = height;
    _arcball->resize(width, height);
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
	bool pseudocolor_inverse = _pseudocolor_selector->is_inverse(channel);
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
	    _heightmap_buffers_are_current = false;
	}
	cvl_frame_free(_render_frame);

	/* Channel selector */
	if (channel == -1)
	{
	    cvl_frame_set_format(_frame1, CVL_XYZ);
	    if (cvl_frame_format(*_frame) != CVL_UNKNOWN)
	    {
		cvl_convert_format(_frame1, *_frame);
	    }
	    else
	    {
		cvl_convert_format_forced(_frame1, *_frame, CVL_RGB);
	    }
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
	    cvl_pseudo_color(dst, src, (channel == -1 ? 1 : 0), 0.0f, 1.0f,
	    	    pseudocolor_startcolor, pseudocolor_lightness, 
		    pseudocolor_inverse, pseudocolor_cyclic);
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
    
    /* Use OpenGL for rendering */
    // Gather all data that requires CVL here, because we cannot mix GL and CVL
    // calls. This includes all calls to selector widgets.
    _flat_view = !_heightmap_selector->is_enabled();
    GLuint render_texture = cvl_frame_texture(_render_frame);
    GLuint data_texture = cvl_frame_texture(*_frame);
    float scale = _scale_selector->get_scalefactor();
    int x_offset = _translation_selector->get_x_offset();
    int y_offset = _translation_selector->get_y_offset();
    bool interpolate = _interpolation_selector->is_enabled();
    float background_r = _color_selector->get_r();
    float background_g = _color_selector->get_g();
    float background_b = _color_selector->get_b();
    bool show_grid = _grid_selector->is_enabled();
    float grid_r = _grid_selector->get_r();
    float grid_g = _grid_selector->get_g();
    float grid_b = _grid_selector->get_b();
    int height_channel = _heightmap_selector->channel();
    int height_mode = _heightmap_selector->mode();
    float height_factor = (_flat_view ? 0.0f : _heightmap_selector->height_factor());
    float channel_min = _heightmap_selector->range() == HeightmapSelector::MINMAX
	? _channel_info->get_min(height_channel) 
	: _range_selector->get_range_min(height_channel);
    float channel_max = _heightmap_selector->range() == HeightmapSelector::MINMAX
	? _channel_info->get_max(height_channel) 
	: _range_selector->get_range_max(height_channel);
    int height_invert = (_heightmap_selector->data() == HeightmapSelector::HEIGHT ? 0 : 1);
    bool height_showcuboid = _heightmap_selector->show_cuboid();
    float height_cuboid_color[3];
    _heightmap_selector->get_cuboid_color(height_cuboid_color);
    int w = cvl_frame_width(_render_frame);
    int h = cvl_frame_height(_render_frame);
    cvl_gl_state_save();	// No CVL calls allowed from now on!
    glClearColor(background_r, background_g, background_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, data_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glActiveTexture(GL_TEXTURE0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    if (interpolate)
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, _width, _height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float vpw = static_cast<float>(_width);
    float vph = static_cast<float>(_height);
    glFrustum(-vpw / 2.0f, +vpw / 2.0f, -vph / 2.0f, +vph / 2.0f, 1.0f, 1.0f + mh_maxf(vpw, vph));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(2.0f * scale * static_cast<float>(x_offset), 
	    -2.0f * scale * static_cast<float>(y_offset), -2.0f);
    glScalef(scale * 2.0f * static_cast<float>(mh_maxi(w, h)), 
	    scale * 2.0f * static_cast<float>(mh_maxi(w, h)), 1.0f);
    if (!_flat_view)
    {
	glMultMatrix(_rotation.to_matrix4());
    }
    float frame_width = static_cast<float>(w);
    float frame_height = static_cast<float>(h);
    float cuboid_left, cuboid_top, cuboid_width, cuboid_height, cuboid_right, cuboid_bottom;
    if (w >= h)
    {
	cuboid_width = 1.0f;
	cuboid_height = frame_height / frame_width;
	cuboid_left = -0.5f + 0.0f;
	cuboid_top = -0.5f + (cuboid_width - cuboid_height) / 2.0f;
    }
    else
    {
	cuboid_width = frame_width / frame_height;
	cuboid_height = 1.0f;
	cuboid_left = -0.5f + (cuboid_height - cuboid_width) / 2.0f;
	cuboid_top = -0.5f + 0.0f;
    }
    cuboid_right = cuboid_left + cuboid_width;
    cuboid_bottom = cuboid_top + cuboid_height;
    if (_flat_view)
    {
	/* Flat view */
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(cuboid_left, cuboid_top);
	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(cuboid_right, cuboid_top);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(cuboid_right, cuboid_bottom);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(cuboid_left, cuboid_bottom);
	glEnd();
	if (show_grid)
	{
	    glDisable(GL_TEXTURE_2D);
	    glDisable(GL_DEPTH_TEST);
	    glEnable(GL_LINE_SMOOTH);
	    glLineWidth(1.0f);
	    glColor3f(grid_r, grid_g, grid_b);
	    for (int y = 0; y <= h; y++)
	    {
		float yo = (static_cast<float>(y) / static_cast<float>(h)) * cuboid_height;
		glBegin(GL_LINES);
		glVertex2f(cuboid_left, cuboid_top + yo);
		glVertex2f(cuboid_right, cuboid_top + yo);
		glEnd();
	    }
	    for (int x = 0; x <= w; x++)
    	    {
		float xo = (static_cast<float>(x) / static_cast<float>(w)) * cuboid_width;
		glBegin(GL_LINES);
		glVertex2f(cuboid_left + xo, cuboid_top);
		glVertex2f(cuboid_left + xo, cuboid_bottom);
		glEnd();
	    }
	}
    }
    else if (height_mode == HeightmapSelector::QUADS)
    {
	/* Separate quads */
       	float quad_width = cuboid_width / frame_width;
	float quad_height = cuboid_height / frame_height;
	if (!_heightmap_buffers_are_current || _heightmap_buffers_mode != HeightmapSelector::QUADS)
	{
	    glDeleteBuffers(1, &_heightmap_texcoord0_buffer);
	    glDeleteBuffers(1, &_heightmap_texcoord1_buffer);
	    glDeleteBuffers(1, &_heightmap_vertex_buffer);
	    GLfloat *buf = new GLfloat[4 * 2 * w * h];
	    for (int y = 0; y < h; y++)
	    {
		float ry = static_cast<float>(y) / frame_height;
		for (int x = 0; x < w; x++)
		{
		    float rx = static_cast<float>(x) / frame_width;
		    buf[8 * (y * w + x) + 0] = rx + 0.5f / frame_width;
		    buf[8 * (y * w + x) + 1] = 1.0f - ry - 0.5 / frame_height;
		    buf[8 * (y * w + x) + 2] = buf[8 * (y * w + x) + 0];
		    buf[8 * (y * w + x) + 3] = buf[8 * (y * w + x) + 1];
		    buf[8 * (y * w + x) + 4] = buf[8 * (y * w + x) + 0];
		    buf[8 * (y * w + x) + 5] = buf[8 * (y * w + x) + 1];
		    buf[8 * (y * w + x) + 6] = buf[8 * (y * w + x) + 0];
		    buf[8 * (y * w + x) + 7] = buf[8 * (y * w + x) + 1];
		}
	    }
	    glClientActiveTexture(GL_TEXTURE1);
	    glGenBuffersARB(1, &_heightmap_texcoord1_buffer);
	    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_texcoord1_buffer);
	    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 4 * 2 * w * h * sizeof(float),
		    buf, GL_STATIC_DRAW_ARB);
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	    glTexCoordPointer(2, GL_FLOAT, 0, 0);
	    for (int y = 0; y < h; y++)
	    {
		float ry = static_cast<float>(y) / frame_height;
		for (int x = 0; x < w; x++)
		{
		    float rx = static_cast<float>(x) / frame_width;
		    buf[8 * (y * w + x) + 0] = rx;
		    buf[8 * (y * w + x) + 1] = 1.0f - ry;
		    buf[8 * (y * w + x) + 2] = rx + quad_width;
		    buf[8 * (y * w + x) + 3] = 1.0f - ry;
		    buf[8 * (y * w + x) + 4] = rx + quad_width;
		    buf[8 * (y * w + x) + 5] = 1.0f - ry - quad_height;
		    buf[8 * (y * w + x) + 6] = rx;
		    buf[8 * (y * w + x) + 7] = 1.0f - ry - quad_height;
		}
	    }
	    glClientActiveTexture(GL_TEXTURE0);
	    glGenBuffersARB(1, &_heightmap_texcoord0_buffer);
	    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_texcoord0_buffer);
	    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 4 * 2 * w * h * sizeof(float),
		    buf, GL_STATIC_DRAW_ARB);
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	    glTexCoordPointer(2, GL_FLOAT, 0, 0);
	    for (int y = 0; y < h; y++)
	    {
		float ry = static_cast<float>(y) / frame_height;
		for (int x = 0; x < w; x++)
		{
		    float rx = static_cast<float>(x) / frame_width;
		    buf[8 * (y * w + x) + 0] = cuboid_left + rx * cuboid_width;
		    buf[8 * (y * w + x) + 1] = cuboid_top + ry * cuboid_height;
		    buf[8 * (y * w + x) + 2] = cuboid_left + rx * cuboid_width + quad_width;
		    buf[8 * (y * w + x) + 3] = cuboid_top + ry * cuboid_height;
		    buf[8 * (y * w + x) + 4] = cuboid_left + rx * cuboid_width + quad_width;
		    buf[8 * (y * w + x) + 5] = cuboid_top + ry * cuboid_height + quad_height;
		    buf[8 * (y * w + x) + 6] = cuboid_left + rx * cuboid_width;
		    buf[8 * (y * w + x) + 7] = cuboid_top + ry * cuboid_height + quad_height;
		}
	    }
	    glGenBuffersARB(1, &_heightmap_vertex_buffer);
	    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_vertex_buffer);
	    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 4 * 2 * w * h * sizeof(float),
		    buf, GL_STATIC_DRAW_ARB);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glVertexPointer(2, GL_FLOAT, 0, 0);
	    delete[] buf;
	    _heightmap_buffers_mode = HeightmapSelector::QUADS;
	    _heightmap_buffers_are_current = true;
	}
	glClientActiveTexture(GL_TEXTURE1);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_texcoord1_buffer);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glClientActiveTexture(GL_TEXTURE0);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_texcoord0_buffer);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_vertex_buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glUseProgram(_heightmap_quads_prg);
	glUniform1i(glGetUniformLocation(_heightmap_quads_prg, "tex"), 0);
	glUniform1i(glGetUniformLocation(_heightmap_quads_prg, "heightmap"), 1);
	glUniform1i(glGetUniformLocation(_heightmap_quads_prg, "channel"), height_channel);
	glUniform1f(glGetUniformLocation(_heightmap_quads_prg, "factor"), height_factor);
	glUniform1f(glGetUniformLocation(_heightmap_quads_prg, "channel_min"), channel_min);
	glUniform1f(glGetUniformLocation(_heightmap_quads_prg, "channel_max"), channel_max);
	glUniform1i(glGetUniformLocation(_heightmap_quads_prg, "invert"), height_invert);
	glDrawArrays(GL_QUADS, 0, 4 * w * h);
	glUseProgram(0);
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
	/* Connected surface */
	if (!_heightmap_buffers_are_current || _heightmap_buffers_mode != HeightmapSelector::STRIP)
	{
	    glDeleteBuffers(1, &_heightmap_texcoord0_buffer);
	    glDeleteBuffers(1, &_heightmap_texcoord1_buffer);
	    glDeleteBuffers(1, &_heightmap_vertex_buffer);
	    GLfloat *buf = new GLfloat[2 * 2 * w * (h - 1)];
	    float ry_next = 0.0f + 0.5f / frame_height;
	    for (int y = 0; y < h - 1; y++)
	    {
		float ry = ry_next;
		ry_next = (static_cast<float>(y + 1) + 0.5f) / frame_height;
		for (int x = 0; x < w; x++)
		{
		    float rx = (static_cast<float>(x) + 0.5f) / frame_width;
		    buf[4 * (y * w + x) + 0] = rx;
		    buf[4 * (y * w + x) + 1] = 1.0f - ry;
		    buf[4 * (y * w + x) + 2] = rx;
		    buf[4 * (y * w + x) + 3] = 1.0f - ry_next;
		}
	    }
	    glClientActiveTexture(GL_TEXTURE0);
	    glGenBuffersARB(1, &_heightmap_texcoord0_buffer);
	    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_texcoord0_buffer);
	    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2 * 2 * w * (h - 1) * sizeof(float),
		    buf, GL_STATIC_DRAW_ARB);
	    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	    glTexCoordPointer(2, GL_FLOAT, 0, 0);
	    ry_next = 0.0f + 0.5f / frame_height;
	    for (int y = 0; y < h - 1; y++)
	    {
		float ry = ry_next;
		ry_next = (static_cast<float>(y + 1) + 0.5f) / frame_height;
		for (int x = 0; x < w; x++)
		{
		    float rx = (static_cast<float>(x) + 0.5f) / frame_width;
		    buf[4 * (y * w + x) + 0] = cuboid_left + rx * cuboid_width;
		    buf[4 * (y * w + x) + 1] = cuboid_top + ry * cuboid_height;
		    buf[4 * (y * w + x) + 2] = cuboid_left + rx * cuboid_width;
		    buf[4 * (y * w + x) + 3] = cuboid_top + ry_next * cuboid_height;
		}
	    }
	    glGenBuffersARB(1, &_heightmap_vertex_buffer);
	    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_vertex_buffer);
	    glBufferDataARB(GL_ARRAY_BUFFER_ARB, 2 * 2 * w * (h - 1) * sizeof(float),
		    buf, GL_STATIC_DRAW_ARB);
	    glEnableClientState(GL_VERTEX_ARRAY);
	    glVertexPointer(2, GL_FLOAT, 0, 0);
	    delete[] buf;
	    _heightmap_buffers_mode = HeightmapSelector::STRIP;
	    _heightmap_buffers_are_current = true;
	}
	glClientActiveTexture(GL_TEXTURE0);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_texcoord0_buffer);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, _heightmap_vertex_buffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, 0);
	glUseProgram(_heightmap_strip_prg);
	glUniform1i(glGetUniformLocation(_heightmap_strip_prg, "tex"), 0);
	glUniform1i(glGetUniformLocation(_heightmap_strip_prg, "heightmap"), 1);
	glUniform1i(glGetUniformLocation(_heightmap_strip_prg, "channel"), height_channel);
	glUniform1f(glGetUniformLocation(_heightmap_strip_prg, "factor"), height_factor);
	glUniform1f(glGetUniformLocation(_heightmap_strip_prg, "channel_min"), channel_min);
	glUniform1f(glGetUniformLocation(_heightmap_strip_prg, "channel_max"), channel_max);
	glUniform1i(glGetUniformLocation(_heightmap_strip_prg, "invert"), height_invert);
	for (int y = 0; y < h - 1; y++)
	{
	    glDrawArrays(GL_TRIANGLE_STRIP, y * 2 * w, 2 * w);
	}
	glUseProgram(0);
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
    }
    glDisable(GL_TEXTURE_2D);
    float cuboid_corners[8][3] =
    {
	{ cuboid_left, cuboid_top, -height_factor / 2.0f },
	{ cuboid_right, cuboid_top, -height_factor / 2.0f },
	{ cuboid_right, cuboid_bottom, -height_factor / 2.0f },
	{ cuboid_left, cuboid_bottom, -height_factor / 2.0f },
	{ cuboid_left, cuboid_top, +height_factor / 2.0f },
	{ cuboid_right, cuboid_top, +height_factor / 2.0f },
	{ cuboid_right, cuboid_bottom, +height_factor / 2.0f },
	{ cuboid_left, cuboid_bottom, +height_factor / 2.0f }
    };
    if (!_flat_view && height_showcuboid)
    {
	glColor3fv(height_cuboid_color);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(cuboid_corners[0]);
	glVertex3fv(cuboid_corners[1]);
	glVertex3fv(cuboid_corners[2]);
	glVertex3fv(cuboid_corners[3]);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(cuboid_corners[0]);
	glVertex3fv(cuboid_corners[1]);
	glVertex3fv(cuboid_corners[5]);
	glVertex3fv(cuboid_corners[4]);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(cuboid_corners[1]);
	glVertex3fv(cuboid_corners[2]);
	glVertex3fv(cuboid_corners[6]);
	glVertex3fv(cuboid_corners[5]);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(cuboid_corners[2]);
	glVertex3fv(cuboid_corners[3]);
	glVertex3fv(cuboid_corners[7]);
	glVertex3fv(cuboid_corners[6]);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(cuboid_corners[3]);
	glVertex3fv(cuboid_corners[0]);
	glVertex3fv(cuboid_corners[4]);
	glVertex3fv(cuboid_corners[7]);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3fv(cuboid_corners[4]);
	glVertex3fv(cuboid_corners[5]);
	glVertex3fv(cuboid_corners[6]);
	glVertex3fv(cuboid_corners[7]);
	glEnd();

    }
    /* Save area of the framebuffer that was rendered to. */
    if (_flat_view)
    {
	float x = static_cast<float>(w) / static_cast<float>(_width);
     	float y = static_cast<float>(h) / static_cast<float>(_height);
	float xo = static_cast<float>(2 * x_offset) / static_cast<float>(_width);
	float yo = static_cast<float>(2 * y_offset) / static_cast<float>(_height);
	int fbx = mh_iroundf(static_cast<float>(_width) / 2.0f + (-x + xo) * 0.5f * scale * static_cast<float>(_width));
	int fby = mh_iroundf(static_cast<float>(_height) / 2.0f - (+y - yo) * 0.5f * scale * static_cast<float>(_height));
	int fbw = mh_iroundf(x * scale * static_cast<float>(_width));
	int fbh = mh_iroundf(y * scale * static_cast<float>(_height));
	_fb_x = mh_clampi(fbx, 0, _width - 1);
    	_fb_y = mh_clampi(fby, 0, _height - 1);
    	_fb_w = mh_clampi(fbw + mh_mini(0, fbx), 1, _width - _fb_x);
   	_fb_h = mh_clampi(fbh + mh_mini(0, fby), 1, _height - _fb_y);
    }
    else
    {
	double M[16], P[16];
	int VP[4];
	glGetDoublev(GL_PROJECTION_MATRIX, P);
	glGetDoublev(GL_MODELVIEW_MATRIX, M);
	glGetIntegerv(GL_VIEWPORT, VP);
	int fbl = _width - 1;
	int fbr = 0;
	int fbt = _height - 1;
	int fbb = 0;
	for (int c = 0; c < 8; c++)
	{
	    GLdouble vpx, vpy, vpz;
	    gluProject(cuboid_corners[c][0], cuboid_corners[c][1], cuboid_corners[c][2],
		    M, P, VP, &vpx, &vpy, &vpz);
	    vpy = static_cast<double>(_height - 1) - vpy;
	    fbl = mh_maxi(0, mh_mini(fbl, mh_iround(floor(vpx))));
	    fbr = mh_mini(_width - 1, mh_maxi(fbr, mh_iround(ceil(vpx))));
	    fbt = mh_maxi(0, mh_mini(fbt, mh_iround(floor(vpy))));
	    fbb = mh_mini(_height - 1, mh_maxi(fbb, mh_iround(ceil(vpy))));
	}
	_fb_x = fbl;
	_fb_y = fbt;
	_fb_w = fbr - fbl + 1;
	_fb_h = fbb - fbt + 1;
    }
    glFlush();
    cvl_gl_state_restore();
    cvl_gl_check_errors("GL rendering");

    unlock();
}

void ViewArea::make_gl_context_current()
{
    makeCurrent();
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

void ViewArea::rotation_changed()
{
    _rotation.from_euler_angles(
	    mh_deg_to_rad(_rotation_selector->get_x_rotation()),
	    mh_deg_to_rad(_rotation_selector->get_y_rotation()),
	    mh_deg_to_rad(_rotation_selector->get_z_rotation()));
    update();
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
    if (cvl_frame_format(_render_frame) == CVL_LUM)
    {
	for (int y = 0; y < h; y++)
	{
	    for (int x = 0; x < w; x++)
	    {
		uint8_t pixel = p[y * w + x];
		img.setPixel(x, y, qRgb(pixel, pixel, pixel));
	    }
	}
    }
    else
    {
	for (int y = 0; y < h; y++)
	{
	    for (int x = 0; x < w; x++)
	    {
		uint8_t *pixel = p + 3 * (y * w + x);
		img.setPixel(x, y, qRgb(pixel[0], pixel[1], pixel[2]));
	    }
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
    if (event->button() == Qt::RightButton)
    {
	_arcball->start(event->pos().x(), event->pos().y());
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
    if (event->button() == Qt::RightButton)
    {
	_arcball->stop();
    }
}

void ViewArea::mouseMoveEvent(QMouseEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    _mouse_pos = event->pos();

    if (_dragging)
    {
       	QPoint drag_endpoint = event->pos();
	int drag_offset_x = mh_iroundf(static_cast<float>(drag_endpoint.x() - _drag_startpoint.x())
		/ _scale_selector->get_scalefactor());
	int drag_offset_y = mh_iroundf(static_cast<float>(drag_endpoint.y() - _drag_startpoint.y())
		/ _scale_selector->get_scalefactor());
	_drag_startpoint = drag_endpoint;
	lock();
	_translation_selector->set_x_offset(_translation_selector->get_x_offset() + drag_offset_x);
	_translation_selector->set_y_offset(_translation_selector->get_y_offset() + drag_offset_y);
	unlock();
	update();
    }
    quat new_rotation = _arcball->rotation(event->pos().x(), event->pos().y(), _rotation);
    if ((new_rotation - _rotation).magnitude() >= 0.00001f)
    {
	_rotation = normalize(new_rotation);
	emit update_rotation(_rotation);
	update();
    }

    pixel_info();
}

void ViewArea::wheelEvent(QWheelEvent *event)
{
    if (_rendering_fails || !*_frame || _lock)
	return;

    float scale = _scale_selector->get_scalefactor();
    int steps = event->delta() / 120;
    float diff = static_cast<float>(steps) * mh_maxf(0.01f, scale * 0.05f);
    _scale_selector->set_scalefactor(scale + diff);
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
    if (!_flat_view)
    {
	emit update_pixel_info(-1, -1, 0, NULL, NULL);
	return;
    }

    makeCurrent();

    // Show pixel info
    float mouse_x = static_cast<float>(_mouse_pos.x());
    float mouse_y = static_cast<float>(_mouse_pos.y());
    float mouse_x_centered = mouse_x - static_cast<float>(_width) / 2.0f;
    float mouse_y_centered = mouse_y - static_cast<float>(_height) / 2.0f;
    float frame_x_centered = mouse_x_centered / _scale_selector->get_scalefactor() - static_cast<float>(_translation_selector->get_x_offset());
    float frame_y_centered = mouse_y_centered / _scale_selector->get_scalefactor() - static_cast<float>(_translation_selector->get_y_offset());
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
