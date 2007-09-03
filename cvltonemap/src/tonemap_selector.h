/*
 * tonemap_selector.h
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

#ifndef TONEMAP_SELECTOR_H
#define TONEMAP_SELECTOR_H

#include "config.h"

#include <QWidget>
#include <QComboBox>
#include <QStackedWidget>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QMouseEvent>
#include <QWheelEvent>

#include <cvl/cvl.h>

#include "conf.h"


class TonemapParameterSelector : public QWidget
{
    Q_OBJECT

    public:
	TonemapParameterSelector() {}
	virtual ~TonemapParameterSelector() {}
	virtual void update() = 0;
	virtual const char *name() const = 0;
	virtual bool is_global() const = 0;
	virtual void get_parameters(Conf *conf) const = 0;
	virtual void set_parameters(Conf *conf) = 0;
};

class TonemapSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	int _tonemap_method_count;
	TonemapParameterSelector **_tonemap_parameter_selector;
	int _active_tonemap_method;
	QComboBox *_combo_box;
	QStackedWidget *_stacked_widget;

    private slots:
	void tonemap_activator(int index);

    public slots:
	void update();

    signals:
        void tonemap_changed();
	void update_selectors();
	void make_gl_context_current();

    public:
	TonemapSelector(cvl_frame_t **frame, QWidget *parent = NULL);
	~TonemapSelector();
	
	static const int RANGE_SELECTION	= 0;
	static const int DRAGO03		= 1;
	static const int DURAND02		= 2;

	int active_tonemap_method() const 
	{ 
	    return _active_tonemap_method; 
	}
	
	TonemapParameterSelector *tonemap_parameter_selector() const
	{
	    return _tonemap_parameter_selector[_active_tonemap_method];
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);

    friend class TonemapRangeSelectionParameterSelector;
    friend class TonemapDrago03ParameterSelector;
    friend class TonemapDurand02ParameterSelector;
};

/* Range Selection */

class RangeSelector : public QLabel
{
    Q_OBJECT

    private:
	int _tolerance;
	float _tolerance_normalized;
	bool _allow_change_left;
	bool _allow_change_right;
	bool _allow_drag;
	bool _change_left;
	bool _change_right;
	bool _drag;
	int _startpoint;
	float _range_left_normalized;
	float _range_right_normalized;
	float logtransf(float x);
	float invlogtransf(float x);
	float normalized_x_to_rangeval(float x);
	float rangeval_to_normalized_x(float y);
    public:
	RangeSelector(QWidget *parent = NULL);
	~RangeSelector();
	QSize sizeHint() const;
    protected:
	void paintEvent(QPaintEvent *e UNUSED);
	void mouseMoveEvent(QMouseEvent *e);
     	void mousePressEvent(QMouseEvent *e);
     	void mouseReleaseEvent(QMouseEvent *e);
   	void wheelEvent(QWheelEvent *e);
};

class TonemapRangeSelectionParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	RangeSelector *_range_selector;
	QCheckBox *_log_x_box;
	QCheckBox *_log_y_box;
	int _histogram_size;
	int *_histogram;
	int _histogram_max;
	float _range_min;
	float _range_max;
	void emit_tonemap_changed();

    public:
	TonemapRangeSelectionParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapRangeSelectionParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Manual Range Selection";
	}

	bool is_global() const
	{
	    return true;
	}

	float get_min_luminance() const
	{
	    return _range_min;
	}
	
	float get_max_luminance() const
	{
	    return _range_max;
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);

    friend class RangeSelector;
};

/* Drago03 */

class TonemapDrago03ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	float _max_abs_lum;
	QCheckBox *_max_abs_lum_checkbox;
	QDoubleSpinBox *_max_abs_lum_spinbox;
	QSlider *_max_abs_lum_slider;
	QDoubleSpinBox *_max_disp_lum_spinbox;
	QSlider *_max_disp_lum_slider;
	QDoubleSpinBox *_bias_spinbox;
	QSlider *_bias_slider;
	bool _lock;

    private slots:
	void set_max_abs_lum_state(int x UNUSED);
	void set_max_abs_lum(double x);
        void max_abs_lum_slider_changed(int x);
	void set_max_disp_lum(double x);
        void max_disp_lum_slider_changed(int x);
	void set_bias(double x);
        void bias_slider_changed(int x);

    public:
	TonemapDrago03ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapDrago03ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Drago 03";
	}

	bool is_global() const
	{
	    return true;
	}

	float get_max_abs_lum() const
	{
	    return static_cast<float>(_max_abs_lum_spinbox->value());
	}

	float get_max_disp_lum() const
	{
	    return static_cast<float>(_max_disp_lum_spinbox->value());
	}

	float get_bias() const
	{
	    return static_cast<float>(_bias_spinbox->value());
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);
};

/* Durand02 */

class TonemapDurand02ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	float _max_abs_lum;
	QCheckBox *_max_abs_lum_checkbox;
	QDoubleSpinBox *_max_abs_lum_spinbox;
	QSlider *_max_abs_lum_slider;
	QDoubleSpinBox *_sigma_spatial_spinbox;
	QSlider *_sigma_spatial_slider;
	QDoubleSpinBox *_sigma_luminance_spinbox;
	QSlider *_sigma_luminance_slider;
	QDoubleSpinBox *_base_contrast_spinbox;
	QSlider *_base_contrast_slider;
	bool _lock;

    private slots:
	void set_max_abs_lum_state(int x);
	void set_max_abs_lum(double x);
        void max_abs_lum_slider_changed(int x);
	void set_sigma_spatial(double x);
        void sigma_spatial_slider_changed(int x);
	void set_sigma_luminance(double x);
        void sigma_luminance_slider_changed(int x);
	void set_base_contrast(double x);
        void base_contrast_slider_changed(int x);

    public:
	TonemapDurand02ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapDurand02ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Durand 02";
	}

	bool is_global() const
	{
	    return false;
	}

	float get_max_abs_lum() const
	{
	    return static_cast<float>(_max_abs_lum_spinbox->value());
	}

	float get_sigma_spatial() const
	{
	    return static_cast<float>(_sigma_spatial_spinbox->value());
	}

	float get_sigma_luminance() const
	{
	    return static_cast<float>(_sigma_luminance_spinbox->value());
	}

	float get_base_contrast() const
	{
	    return static_cast<float>(_base_contrast_spinbox->value());
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);
};

#endif
