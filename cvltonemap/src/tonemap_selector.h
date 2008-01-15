/*
 * tonemap_selector.h
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
#include "postproc_selector.h"
#include "maxabslum_selector.h"


class TonemapParameterSelector : public QWidget
{
    Q_OBJECT

    public:
	TonemapParameterSelector() {}
	virtual ~TonemapParameterSelector() {}
	virtual void update() = 0;
	virtual const char *name() const = 0;			// User visible
	virtual const char *id() const = 0;			// Intern; used for parameter save/restore
	virtual bool is_global() const = 0;
	virtual void get_parameters(Conf *conf) const = 0;
	virtual void set_parameters(Conf *conf) = 0;
};

class TonemapSelector : public QWidget
{
    Q_OBJECT
	
    private:
	cvl_frame_t **_frame;
	int _method_count;
	int _active_method;
	TonemapParameterSelector **_parameter_selector;
	PostprocSelector **_postproc_selector;
	QComboBox *_combo_box;
	QStackedWidget *_tonemap_stack;
	QStackedWidget *_postproc_stack;

    private slots:
	void postproc_changed();
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
	static const int SCHLICK94		= 1;
	static const int TUMBLIN99		= 2;
	static const int DRAGO03		= 3;
	static const int REINHARD05		= 4;
	static const int ASHIKHMIN02		= 5;
	static const int DURAND02		= 6;
	static const int REINHARD02		= 7;

	int active_tonemap_method() const 
	{ 
	    return _active_method; 
	}
	
	TonemapParameterSelector *parameter_selector() const
	{
	    return _parameter_selector[_active_method];
	}

	PostprocSelector *postproc_selector() const
	{
	    return _postproc_selector[_active_method];
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);

    friend class TonemapRangeSelectionParameterSelector;
    friend class TonemapSchlick94ParameterSelector;
    friend class TonemapTumblin99ParameterSelector;
    friend class TonemapDrago03ParameterSelector;
    friend class TonemapReinhard05ParameterSelector;
    friend class TonemapAshikhmin02ParameterSelector;
    friend class TonemapDurand02ParameterSelector;
    friend class TonemapReinhard02ParameterSelector;
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
	
	const char *id() const
	{
	    return "manualrangeselection";
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

/* Schlick94 */

class TonemapSchlick94ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	QDoubleSpinBox *_p_spinbox;
	QSlider *_p_slider;
	bool _lock;

    private slots:
	void set_p(double x);
        void p_slider_changed(int x);

    public:
	TonemapSchlick94ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapSchlick94ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Schlick 94";
	}
	
	const char *id() const
	{
	    return "schlick94";
	}

	bool is_global() const
	{
	    return true;
	}

	float get_p() const
	{
	    return _p_spinbox->value();
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);
};

/* Tumblin99 */

class TonemapTumblin99ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	MaxAbsLumSelector *_max_abs_lum_selector;
	QDoubleSpinBox *_disp_adapt_level_spinbox;
	QSlider *_disp_adapt_level_slider;
	QDoubleSpinBox *_max_contrast_spinbox;
	QSlider *_max_contrast_slider;
	bool _lock;

    private slots:
	void max_abs_lum_changed();
	void set_disp_adapt_level(double x);
        void disp_adapt_level_slider_changed(int x);
	void set_max_contrast(double x);
        void max_contrast_slider_changed(int x);

    public:
	TonemapTumblin99ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapTumblin99ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Tumblin 99";
	}
	
	const char *id() const
	{
	    return "tumblin99";
	}

	bool is_global() const
	{
	    return true;
	}

	float get_max_abs_lum() const
	{
	    return _max_abs_lum_selector->value();
	}

	float get_disp_adapt_level() const
	{
	    return static_cast<float>(_disp_adapt_level_spinbox->value());
	}

	float get_max_contrast() const
	{
	    return static_cast<float>(_max_contrast_spinbox->value());
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);
};

/* Drago03 */

class TonemapDrago03ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	MaxAbsLumSelector *_max_abs_lum_selector;
	QDoubleSpinBox *_max_disp_lum_spinbox;
	QSlider *_max_disp_lum_slider;
	QDoubleSpinBox *_bias_spinbox;
	QSlider *_bias_slider;
	bool _lock;

    private slots:
	void max_abs_lum_changed();
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
	
	const char *id() const
	{
	    return "drago03";
	}

	bool is_global() const
	{
	    return true;
	}

	float get_max_abs_lum() const
	{
	    return _max_abs_lum_selector->value();
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

/* Reinhard05 */

class TonemapReinhard05ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	QDoubleSpinBox *_f_spinbox;
	QSlider *_f_slider;
	QDoubleSpinBox *_c_spinbox;
	QSlider *_c_slider;
	QDoubleSpinBox *_l_spinbox;
	QSlider *_l_slider;
	bool _lock;

    private slots:
	void set_f(double x);
        void f_slider_changed(int x);
	void set_c(double x);
        void c_slider_changed(int x);
	void set_l(double x);
        void l_slider_changed(int x);

    public:
	TonemapReinhard05ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapReinhard05ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Reinhard 05";
	}
	
	const char *id() const
	{
	    return "reinhard05";
	}

	bool is_global() const
	{
	    return true;
	}

	float get_f() const
	{
	    return static_cast<float>(_f_spinbox->value());
	}

	float get_c() const
	{
	    return static_cast<float>(_c_spinbox->value());
	}

	float get_l() const
	{
	    return static_cast<float>(_l_spinbox->value());
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);
};

/* Ashikhmin02 */

class TonemapAshikhmin02ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	MaxAbsLumSelector *_max_abs_lum_selector;
	QDoubleSpinBox *_threshold_spinbox;
	QSlider *_threshold_slider;
	bool _lock;

    private slots:
	void max_abs_lum_changed();
	void set_threshold(double x);
        void threshold_slider_changed(int x);

    public:
	TonemapAshikhmin02ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapAshikhmin02ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Ashikhmin 02";
	}
	
	const char *id() const
	{
	    return "ashikhmin02";
	}

	bool is_global() const
	{
	    return false;
	}

	float get_max_abs_lum() const
	{
	    return _max_abs_lum_selector->value();
	}

	float get_threshold() const
	{
	    return static_cast<float>(_threshold_spinbox->value() / 100.0f);
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
	MaxAbsLumSelector *_max_abs_lum_selector;
	QDoubleSpinBox *_sigma_spatial_spinbox;
	QSlider *_sigma_spatial_slider;
	QDoubleSpinBox *_sigma_luminance_spinbox;
	QSlider *_sigma_luminance_slider;
	QDoubleSpinBox *_base_contrast_spinbox;
	QSlider *_base_contrast_slider;
	bool _lock;

    private slots:
	void max_abs_lum_changed();
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
	
	const char *id() const
	{
	    return "durand02";
	}

	bool is_global() const
	{
	    return false;
	}

	float get_max_abs_lum() const
	{
	    return _max_abs_lum_selector->value();
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

/* Reinhard02 */

class TonemapReinhard02ParameterSelector : public TonemapParameterSelector
{
    Q_OBJECT

    private:
	TonemapSelector *_tonemap_selector;
	cvl_frame_t **_frame;
	QDoubleSpinBox *_brightness_spinbox;
	QSlider *_brightness_slider;
	QDoubleSpinBox *_white_spinbox;
	QSlider *_white_slider;
	QDoubleSpinBox *_sharpness_spinbox;
	QSlider *_sharpness_slider;
	QDoubleSpinBox *_threshold_spinbox;
	QSlider *_threshold_slider;
	bool _lock;

    private slots:
	void set_brightness(double x);
        void brightness_slider_changed(int x);
	void set_white(double x);
        void white_slider_changed(int x);
	void set_sharpness(double x);
        void sharpness_slider_changed(int x);
	void set_threshold(double x);
        void threshold_slider_changed(int x);

    public:
	TonemapReinhard02ParameterSelector(
		TonemapSelector *tonemap_selector, cvl_frame_t **frame);
	~TonemapReinhard02ParameterSelector();
	void update();
	
	const char *name() const
	{
	    return "Reinhard 02";
	}
	
	const char *id() const
	{
	    return "reinhard02";
	}

	bool is_global() const
	{
	    return false;
	}

	float get_brightness() const
	{
	    return static_cast<float>(_brightness_spinbox->value());
	}

	float get_white() const
	{
	    return static_cast<float>(_white_spinbox->value());
	}

	float get_sharpness() const
	{
	    return static_cast<float>(_sharpness_spinbox->value());
	}

	float get_threshold() const
	{
	    return static_cast<float>(_threshold_spinbox->value() / 100.0f);
	}

	void get_parameters(Conf *conf) const;
	void set_parameters(Conf *conf);
};

#endif
