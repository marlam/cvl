/*
 * tonemap_selector.cpp
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

#include "config.h"

#include <cstring>
#include <cstdlib>

#include <QWidget>
#include <QGridLayout>
#include <QComboBox>
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>

#include <cvl/cvl.h>

#include "mh.h"

#include "conf.h"
#include "tonemap_selector.h"

/* Common values for the common "Maximum absolute luminance" setting */
#define MAXABSLUM_MIN 0.01
#define MAXABSLUM_MAX 99999.99
#define MAXABSLUM_DEFAULT 150.0


TonemapSelector::TonemapSelector(cvl_frame_t **frame, QWidget *parent)
    : QWidget(parent)
{
    _frame = frame;

    /* When adding a new method, adjust only this block: ---> */
    _tonemap_method_count = 3;

    _tonemap_parameter_selector = new TonemapParameterSelector *[_tonemap_method_count];

    _tonemap_parameter_selector[0] = new TonemapRangeSelectionParameterSelector(this, _frame);
    _tonemap_parameter_selector[1] = new TonemapDrago03ParameterSelector(this, _frame);
    _tonemap_parameter_selector[2] = new TonemapDurand02ParameterSelector(this, _frame);
    /* <--- End. */

    QGridLayout *layout = new QGridLayout();
    _combo_box = new QComboBox();
    _combo_box->setEditable(false);
    for (int i = 0; i < _tonemap_method_count; i++)
    {
	_combo_box->addItem(mh_string("%c %s", 
		    _tonemap_parameter_selector[i]->is_global() ? 'G' : 'L',
		    _tonemap_parameter_selector[i]->name()).c_str());
    }
    connect(_combo_box, SIGNAL(activated(int)), this, SLOT(tonemap_activator(int)));
    layout->addWidget(_combo_box, 0, 0);
    _stacked_widget = new QStackedWidget();
    for (int i = 0; i < _tonemap_method_count; i++)
    {
	_stacked_widget->addWidget(_tonemap_parameter_selector[i]);
    }
    layout->addWidget(_stacked_widget, 1, 0);
    layout->setRowStretch(2, 1);
    setLayout(layout);

    tonemap_activator(0);
}

TonemapSelector::~TonemapSelector()
{
    delete[] _tonemap_parameter_selector;
}

void TonemapSelector::tonemap_activator(int index)
{
    _combo_box->setCurrentIndex(index);
    _stacked_widget->setCurrentIndex(index);
    _active_tonemap_method = index;
    emit tonemap_changed();
}

void TonemapSelector::update()
{
    for (int i = 0; i < _tonemap_method_count; i++)
    {
	_tonemap_parameter_selector[i]->update();
    }
}

void TonemapSelector::get_parameters(Conf *conf) const
{
    for (int i = 0; i < _tonemap_method_count; i++)
    {
	_tonemap_parameter_selector[i]->get_parameters(conf);
    }
    conf->put("method", _tonemap_parameter_selector[_active_tonemap_method]->name());
}

void TonemapSelector::set_parameters(Conf *conf)
{
    for (int i = 0; i < _tonemap_method_count; i++)
    {
	_tonemap_parameter_selector[i]->set_parameters(conf);
    }
    int method_index = 0;
    const char *method = conf->get("method");
    for (int i = 0; i < _tonemap_method_count; i++)
    {
	if (strcmp(method, _tonemap_parameter_selector[i]->name()) == 0)
	{
	    method_index = i;
	    break;
	}
    }
    tonemap_activator(method_index);
}


/* Range Selection */

RangeSelector::RangeSelector(QWidget *parent) : QLabel(parent)
{
    setMouseTracking(true);
    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sp.setHeightForWidth(false);
    setSizePolicy(sp);

    _tolerance = 2;
    _tolerance_normalized = static_cast<float>(_tolerance) / static_cast<float>(width() - 1);
    _allow_change_left = false;
    _allow_change_right = false;
    _allow_drag = false;
    _change_left = false;
    _change_right = false;
    _drag = false;
    _range_left_normalized = -1.0f;
    _range_right_normalized = +2.0f;
}

RangeSelector::~RangeSelector()
{
}

QSize RangeSelector::sizeHint() const
{
    return QSize(1, 50);
}

float RangeSelector::logtransf(float x)
{
    const float base = 50.0f;
    return mh_clampf(logf(1.0f + x * (base - 1.0f)) / logf(base), 0.0f, 1.0f);
}

float RangeSelector::invlogtransf(float y)
{
    const float base = 50.0f;
    return mh_clampf((expf(y * logf(base)) - 1.0f) / (base - 1.0f), 0.0f, 1.0f);
}

float RangeSelector::normalized_x_to_rangeval(float x)
{
    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    bool log_x = parameter_selector->_log_x_box->isChecked();
    return (1.0f - (log_x ? logtransf(1.0f - x) : 1.0f - x));
}

float RangeSelector::rangeval_to_normalized_x(float x)
{
    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    bool log_x = parameter_selector->_log_x_box->isChecked();
    return log_x ? (1.0f - invlogtransf(1.0f - x)) : x;
}

void RangeSelector::paintEvent(QPaintEvent *e UNUSED)
{
    QPainter p(this);
    p.fillRect(1, 1, width() - 2, height() - 2, QBrush(QColor(255, 255, 255)));
    p.setPen(QColor(0, 0, 0));
    p.drawRect(0, 0, width() - 1, height() - 1);

    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    if (!(*(parameter_selector->_frame)))
	return;

    /* Get the data */
    int histogram_size = parameter_selector->_histogram_size;
    int *histogram = parameter_selector->_histogram;
    int histogram_max = parameter_selector->_histogram_max;
    float range_min = parameter_selector->_range_min;
    float range_max = parameter_selector->_range_max;
    bool log_x = parameter_selector->_log_x_box->isChecked();
    bool log_y = parameter_selector->_log_y_box->isChecked();

    /* Draw selected range */
    _range_left_normalized = rangeval_to_normalized_x(range_min);
    _range_right_normalized = rangeval_to_normalized_x(range_max);
    int range_left = 1 + static_cast<int>(static_cast<float>(width() - 3) * _range_left_normalized);
    int range_right = 1 + static_cast<int>(static_cast<float>(width() - 3) * _range_right_normalized);
    p.fillRect(range_left, 1, range_right - range_left + 1, height() - 2, QBrush(QColor(0, 255, 0)));

    /* Draw histogram */
    p.setPen(QColor(0, 0, 255));
    int last_x = 0;
    for (int bin = 0; bin < histogram_size; bin++)
    {
	float lengthf = static_cast<float>(histogram[bin]) / static_cast<float>(histogram_max);
	int length = static_cast<int>((log_y ? logtransf(lengthf) : lengthf) * static_cast<float>(height() - 2));
	float binf = static_cast<float>(bin) / static_cast<float>(histogram_size - 1);
	if (log_x)
	    binf = logtransf(binf);
	int x = 1 + mh_iroundf(binf * static_cast<float>(width() - 3));
	if (length > 0)
	{
	    if (x == last_x)
	    {
		p.drawLine(x, height() - 2 - (length - 1), x, height() - 2);
	    }
	    else
	    {
		for (int xx = last_x + 1; xx <= x; xx++)
		{
		    p.drawLine(xx, height() - 2 - (length - 1), xx, height() - 2);
		}
	    }
	}
	last_x = x;
    }
}

void RangeSelector::mouseMoveEvent(QMouseEvent *e)
{
    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    if (!(*(parameter_selector->_frame)))
	return;

    if (_change_left)
    {
	int endpoint = e->x();
	int offset = endpoint - _startpoint;
	float offset_normalized = static_cast<float>(offset) / static_cast<float>(width() - 3);
	if (_range_left_normalized + offset_normalized >= _range_right_normalized - _tolerance_normalized)
	    offset_normalized = _range_right_normalized - _tolerance_normalized - _range_left_normalized;
	else if (_range_left_normalized + offset_normalized < 0.0f)
	    offset_normalized = - _range_left_normalized;
	float new_range_min = normalized_x_to_rangeval(_range_left_normalized + offset_normalized);
	parameter_selector->_range_min = new_range_min;
	_startpoint = endpoint;
    }
    else if (_change_right)
    {
	int endpoint = e->x();
	int offset = endpoint - _startpoint;
	float offset_normalized = static_cast<float>(offset) / static_cast<float>(width() - 3);
	if (_range_right_normalized + offset_normalized <= _range_left_normalized + _tolerance_normalized)
	    offset_normalized = _range_left_normalized + _tolerance_normalized - _range_right_normalized;
	else if (_range_right_normalized + offset_normalized > 1.0f)
	    offset_normalized = 1.0f - _range_right_normalized;
	float new_range_max = normalized_x_to_rangeval(_range_right_normalized + offset_normalized);
	parameter_selector->_range_max = new_range_max;
	_startpoint = endpoint;
    }
    else if (_drag)
    {
	int endpoint = e->x();
	int offset = endpoint - _startpoint;
	float offset_normalized = static_cast<float>(offset) / static_cast<float>(width() - 3);
	if (_range_left_normalized + offset_normalized < 0.0f)
	    offset_normalized = - _range_left_normalized;
	else if (_range_right_normalized + offset_normalized > 1.0f)
	    offset_normalized = 1.0f - _range_right_normalized;
	float new_range_min = normalized_x_to_rangeval(_range_left_normalized + offset_normalized);
	float new_range_max = normalized_x_to_rangeval(_range_right_normalized + offset_normalized);
	parameter_selector->_range_min = new_range_min;
	parameter_selector->_range_max = new_range_max;
	_startpoint = endpoint;
    }
    else
    {
	float x_normalized = static_cast<float>(e->x()) / static_cast<float>(width() - 3);
	if (fabsf(x_normalized - _range_left_normalized) <= _tolerance_normalized)
	{
	    _allow_change_left = true;
	    _allow_change_right = false;
	    _allow_drag = false;
	    setCursor(QCursor(Qt::SplitHCursor));
	}
	else if (fabsf(x_normalized - _range_right_normalized) <= _tolerance_normalized)
	{
	    _allow_change_left = false;
	    _allow_change_right = true;
	    _allow_drag = false;
	    setCursor(QCursor(Qt::SplitHCursor));
	}
	else if (x_normalized >= _range_left_normalized && x_normalized <= _range_right_normalized)
	{
	    _allow_change_left = false;
	    _allow_change_right = false;
	    _allow_drag = true;
	    setCursor(QCursor(Qt::SizeHorCursor));
	}
	else
	{
	    _allow_change_left = false;
	    _allow_change_right = false;
	    _allow_drag = false;
	    unsetCursor();
	}
    }
    update();
}

void RangeSelector::mousePressEvent(QMouseEvent *e)
{
    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    if (!(*(parameter_selector->_frame)))
	return;

    if (e->button() == Qt::LeftButton)
    {
	_startpoint = e->x();
	if (_allow_change_left)
	{
	    _change_left = true;
	}
	else if (_allow_change_right)
	{
	    _change_right = true;
	}
	else if (_allow_drag)
	{
	    _drag = true;
	}
    }
}

void RangeSelector::mouseReleaseEvent(QMouseEvent *e)
{
    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    if (!(*(parameter_selector->_frame)))
	return;

    if (e->button() == Qt::LeftButton)
    {
	_change_left = false;
    	_change_right = false;
	_drag = false;
	parameter_selector->emit_tonemap_changed();
    }
}

void RangeSelector::wheelEvent(QWheelEvent *e)
{
    TonemapRangeSelectionParameterSelector *parameter_selector 
	= reinterpret_cast<TonemapRangeSelectionParameterSelector *>(parentWidget());
    if (!(*(parameter_selector->_frame)))
	return;

    float steps = static_cast<float>(e->delta() / 120);
    float new_range_left_normalized, new_range_right_normalized;
    if (steps < 0.0f)
    {
	new_range_left_normalized = mh_maxf(0.0f, _range_left_normalized + steps * 0.05);
	new_range_right_normalized = mh_min(1.0f, _range_right_normalized - steps * 0.05);
    }
    else
    {
	new_range_left_normalized = _range_left_normalized + steps * 0.05;
	new_range_right_normalized = _range_right_normalized - steps * 0.05;
	if (new_range_left_normalized + _tolerance_normalized > new_range_right_normalized - _tolerance_normalized)
	{
	    float center = (_range_right_normalized - _range_left_normalized) / 2.0 + _range_left_normalized;
	    new_range_left_normalized = center - _tolerance_normalized;
	    new_range_right_normalized = center + _tolerance_normalized;
	}
    }
    float new_range_min = normalized_x_to_rangeval(new_range_left_normalized);
    float new_range_max = normalized_x_to_rangeval(new_range_right_normalized);
    parameter_selector->_range_min = new_range_min;
    parameter_selector->_range_max = new_range_max;
    update();
    parameter_selector->emit_tonemap_changed();
}

TonemapRangeSelectionParameterSelector::TonemapRangeSelectionParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;

    _histogram_size = 1024;
    _histogram = new int[_histogram_size];
    _range_min = 0.0f;
    _range_max = 1.0f;
    _range_selector = new RangeSelector(this);
    _log_x_box = new QCheckBox("Logarithmic horizontal scale");
    _log_x_box->setCheckState(Qt::Checked);
    connect(_log_x_box, SIGNAL(stateChanged(int)), _range_selector, SLOT(update()));
    _log_y_box = new QCheckBox("Logarithmic vertical scale");
    _log_y_box->setCheckState(Qt::Checked);
    connect(_log_y_box, SIGNAL(stateChanged(int)), _range_selector, SLOT(update()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_range_selector, 0, 0);
    layout->addWidget(_log_x_box, 1, 0);
    layout->addWidget(_log_y_box, 2, 0);
    layout->setRowStretch(3, 1);
    setLayout(layout);
}

TonemapRangeSelectionParameterSelector::~TonemapRangeSelectionParameterSelector()
{
    delete[] _histogram;
}

void TonemapRangeSelectionParameterSelector::update()
{
    emit _tonemap_selector->make_gl_context_current();

    _range_min = 0.0f;
    _range_max = 1.0f;
    cvl_histogram(*_frame, 1, _histogram_size, &_range_min, &_range_max, _histogram);
    _histogram_max = _histogram[0];
    for (int i = 1; i < _histogram_size; i++)
    {
	if (_histogram[i] > _histogram_max)
	    _histogram_max = _histogram[i];
    }
    _range_selector->update();
}

void TonemapRangeSelectionParameterSelector::emit_tonemap_changed()
{
    emit _tonemap_selector->tonemap_changed();
}

void TonemapRangeSelectionParameterSelector::get_parameters(Conf *conf) const
{
    conf->put("rangeselection-min", _range_min);
    conf->put("rangeselection-max", _range_max);
    conf->put("rangeselection-loghorz", _log_x_box->isChecked());
    conf->put("rangeselection-logvert", _log_y_box->isChecked());
}

void TonemapRangeSelectionParameterSelector::set_parameters(Conf *conf)
{
    _range_min = conf->get("rangeselection-min", 0.0f, 1.0f, 0.0f);
    _range_max = conf->get("rangeselection-max", 0.0f, 1.0f, 1.0f);
    _log_x_box->setChecked(conf->get("rangeselection-loghorz", true));
    _log_y_box->setChecked(conf->get("rangeselection-logvert", true));
}


/* Drago03 */

TonemapDrago03ParameterSelector::TonemapDrago03ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    _max_abs_lum_checkbox = new QCheckBox("Max. Luminance:");
    connect(_max_abs_lum_checkbox, SIGNAL(stateChanged(int)), this, SLOT(set_max_abs_lum_state(int)));
    layout->addWidget(_max_abs_lum_checkbox, 0, 0);
    _max_abs_lum_spinbox = new QDoubleSpinBox();
    _max_abs_lum_spinbox->setRange(MAXABSLUM_MIN, MAXABSLUM_MAX);
    _max_abs_lum_spinbox->setDecimals(2);
    _max_abs_lum_spinbox->setSingleStep(1.0);
    connect(_max_abs_lum_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_max_abs_lum(double)));
    layout->addWidget(_max_abs_lum_spinbox, 0, 1);
    _max_abs_lum_slider = new QSlider(Qt::Horizontal, this);
    _max_abs_lum_slider->setRange(mh_iround(MAXABSLUM_MIN * 100.0), mh_iround(MAXABSLUM_MAX * 100.0));
    connect(_max_abs_lum_slider, SIGNAL(valueChanged(int)), this, SLOT(max_abs_lum_slider_changed(int)));
    layout->addWidget(_max_abs_lum_slider, 1, 0, 1, 2);

    QLabel *ld = new QLabel("Max. Display Lum.: ");
    layout->addWidget(ld, 2, 0);
    _max_disp_lum_spinbox = new QDoubleSpinBox();
    _max_disp_lum_spinbox->setRange(0.01, 999.99);
    _max_disp_lum_spinbox->setDecimals(2);
    _max_disp_lum_spinbox->setSingleStep(0.1);
    connect(_max_disp_lum_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_max_disp_lum(double)));
    layout->addWidget(_max_disp_lum_spinbox, 2, 1);
    _max_disp_lum_slider = new QSlider(Qt::Horizontal, this);
    _max_disp_lum_slider->setRange(1, 99999);
    connect(_max_disp_lum_slider, SIGNAL(valueChanged(int)), this, SLOT(max_disp_lum_slider_changed(int)));
    layout->addWidget(_max_disp_lum_slider, 3, 0, 1, 2);

    QLabel *lb = new QLabel("Bias:");
    layout->addWidget(lb, 4, 0);
    _bias_spinbox = new QDoubleSpinBox();
    _bias_spinbox->setRange(0.01, 1.00);
    _bias_spinbox->setDecimals(2);
    _bias_spinbox->setSingleStep(0.01);
    connect(_bias_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_bias(double)));
    layout->addWidget(_bias_spinbox, 4, 1);
    _bias_slider = new QSlider(Qt::Horizontal, this);
    _bias_slider->setRange(1, 100);
    connect(_bias_slider, SIGNAL(valueChanged(int)), this, SLOT(bias_slider_changed(int)));
    layout->addWidget(_bias_slider, 5, 0, 1, 2);

    layout->setRowStretch(6, 1);
    setLayout(layout);

    update();
}

TonemapDrago03ParameterSelector::~TonemapDrago03ParameterSelector()
{
}

void TonemapDrago03ParameterSelector::update()
{
    _max_abs_lum = -1.0f;
    _lock = true;
    const char *max_luminance_tag = NULL;
    if (*_frame)
    {
	max_luminance_tag = cvl_taglist_get(cvl_frame_taglist(*_frame), "X-MAX-LUMINANCE");
    }
    if (!max_luminance_tag || strcmp(max_luminance_tag, "UNKNOWN") == 0)
    {
	_max_abs_lum_spinbox->setValue(MAXABSLUM_DEFAULT);
	_max_abs_lum_slider->setValue(mh_iround(MAXABSLUM_DEFAULT * 100));
	_max_abs_lum_checkbox->setCheckState(Qt::Checked);
    }
    else
    {
	_max_abs_lum = atof(max_luminance_tag);
	_max_abs_lum_spinbox->setValue(_max_abs_lum);
	_max_abs_lum_slider->setValue(mh_iround(_max_abs_lum * 100.0));
	_max_abs_lum_checkbox->setCheckState(Qt::Unchecked);
    }
    _max_disp_lum_spinbox->setValue(200.0);
    _max_disp_lum_slider->setValue(20000);
    _bias_spinbox->setValue(0.85);
    _bias_slider->setValue(85);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDrago03ParameterSelector::set_max_abs_lum_state(int x UNUSED)
{
    if (_max_abs_lum_checkbox->isChecked())
    {
	_max_abs_lum_spinbox->setEnabled(true);
	_max_abs_lum_slider->setEnabled(true);
    }
    else
    {
	if (_max_abs_lum > 0.0f)
	    _max_abs_lum_spinbox->setValue(_max_abs_lum);
	_max_abs_lum_spinbox->setEnabled(false);
	_max_abs_lum_slider->setEnabled(false);
    }
}

void TonemapDrago03ParameterSelector::set_max_abs_lum(double x)
{
    _lock = true;
    _max_abs_lum_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDrago03ParameterSelector::max_abs_lum_slider_changed(int x)
{
    if (!_lock)
	_max_abs_lum_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDrago03ParameterSelector::set_max_disp_lum(double x)
{
    _lock = true;
    _max_disp_lum_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDrago03ParameterSelector::max_disp_lum_slider_changed(int x)
{
    if (!_lock)
	_max_disp_lum_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDrago03ParameterSelector::set_bias(double x)
{
    _lock = true;
    _bias_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDrago03ParameterSelector::bias_slider_changed(int x)
{
    if (!_lock)
	_bias_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDrago03ParameterSelector::get_parameters(Conf *conf) const
{
    conf->put("drago03-usemaxabslum", _max_abs_lum_checkbox->isChecked());
    conf->put("drago03-maxabslum", _max_abs_lum_spinbox->value());
    conf->put("drago03-maxdisplum", _max_disp_lum_spinbox->value());
    conf->put("drago03-bias", _bias_spinbox->value());
}

void TonemapDrago03ParameterSelector::set_parameters(Conf *conf)
{
    _max_abs_lum_spinbox->setValue(conf->get("drago03-maxabslum", 
		MAXABSLUM_MIN, MAXABSLUM_MAX, MAXABSLUM_DEFAULT));
    _max_abs_lum_checkbox->setChecked(conf->get("drago03-usemaxabslum", true));
    _max_disp_lum_spinbox->setValue(conf->get("drago03-maxdisplum", 0.01, 999.99, 200.0));
    _bias_spinbox->setValue(conf->get("drago03-bias", 0.01, 1.00, 0.85));
}


/* Durand02 */

TonemapDurand02ParameterSelector::TonemapDurand02ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    _max_abs_lum_checkbox = new QCheckBox("Max. Luminance:");
    connect(_max_abs_lum_checkbox, SIGNAL(stateChanged(int)), this, SLOT(set_max_abs_lum_state(int)));
    layout->addWidget(_max_abs_lum_checkbox, 0, 0);
    _max_abs_lum_spinbox = new QDoubleSpinBox();
    _max_abs_lum_spinbox->setRange(MAXABSLUM_MIN, MAXABSLUM_MAX);
    _max_abs_lum_spinbox->setDecimals(2);
    _max_abs_lum_spinbox->setSingleStep(1.0);
    connect(_max_abs_lum_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_max_abs_lum(double)));
    layout->addWidget(_max_abs_lum_spinbox, 0, 1);
    _max_abs_lum_slider = new QSlider(Qt::Horizontal, this);
    _max_abs_lum_slider->setRange(mh_iround(MAXABSLUM_MIN * 100.0), mh_iround(MAXABSLUM_MAX * 100.0));
    connect(_max_abs_lum_slider, SIGNAL(valueChanged(int)), this, SLOT(max_abs_lum_slider_changed(int)));
    layout->addWidget(_max_abs_lum_slider, 1, 0, 1, 2);

    QLabel *lss = new QLabel("Sigma spatial:");
    layout->addWidget(lss, 2, 0);
    _sigma_spatial_spinbox = new QDoubleSpinBox();
    _sigma_spatial_spinbox->setRange(0.01, 9.99);
    _sigma_spatial_spinbox->setDecimals(2);
    _sigma_spatial_spinbox->setSingleStep(0.1);
    connect(_sigma_spatial_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_sigma_spatial(double)));
    layout->addWidget(_sigma_spatial_spinbox, 2, 1);
    _sigma_spatial_slider = new QSlider(Qt::Horizontal, this);
    _sigma_spatial_slider->setRange(1, 999);
    connect(_sigma_spatial_slider, SIGNAL(valueChanged(int)), this, SLOT(sigma_spatial_slider_changed(int)));
    layout->addWidget(_sigma_spatial_slider, 3, 0, 1, 2);

    QLabel *lsl = new QLabel("Sigma luminance:");
    layout->addWidget(lsl, 4, 0);
    _sigma_luminance_spinbox = new QDoubleSpinBox();
    _sigma_luminance_spinbox->setRange(0.01, 9.99);
    _sigma_luminance_spinbox->setDecimals(2);
    _sigma_luminance_spinbox->setSingleStep(0.1);
    connect(_sigma_luminance_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_sigma_luminance(double)));
    layout->addWidget(_sigma_luminance_spinbox, 4, 1);
    _sigma_luminance_slider = new QSlider(Qt::Horizontal, this);
    _sigma_luminance_slider->setRange(1, 999);
    connect(_sigma_luminance_slider, SIGNAL(valueChanged(int)), this, SLOT(sigma_luminance_slider_changed(int)));
    layout->addWidget(_sigma_luminance_slider, 5, 0, 1, 2);

    QLabel *lbc = new QLabel("Base contrast:");
    layout->addWidget(lbc, 6, 0);
    _base_contrast_spinbox = new QDoubleSpinBox();
    _base_contrast_spinbox->setRange(1.01, 9.99);
    _base_contrast_spinbox->setDecimals(2);
    _base_contrast_spinbox->setSingleStep(0.1);
    connect(_base_contrast_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_base_contrast(double)));
    layout->addWidget(_base_contrast_spinbox, 6, 1);
    _base_contrast_slider = new QSlider(Qt::Horizontal, this);
    _base_contrast_slider->setRange(101, 999);
    connect(_base_contrast_slider, SIGNAL(valueChanged(int)), this, SLOT(base_contrast_slider_changed(int)));
    layout->addWidget(_base_contrast_slider, 7, 0, 1, 2);

    layout->setRowStretch(8, 1);
    setLayout(layout);

    update();
}

TonemapDurand02ParameterSelector::~TonemapDurand02ParameterSelector()
{
}

void TonemapDurand02ParameterSelector::update()
{
    _max_abs_lum = -1.0f;
    _lock = true;
    const char *max_luminance_tag = NULL;
    if (*_frame)
    {
	max_luminance_tag = cvl_taglist_get(cvl_frame_taglist(*_frame), "X-MAX-LUMINANCE");
    }
    if (!max_luminance_tag || strcmp(max_luminance_tag, "UNKNOWN") == 0)
    {
	_max_abs_lum_spinbox->setValue(MAXABSLUM_DEFAULT);
	_max_abs_lum_slider->setValue(mh_iround(MAXABSLUM_DEFAULT * 100.0));
	_max_abs_lum_checkbox->setCheckState(Qt::Checked);
    }
    else
    {
	_max_abs_lum = atof(max_luminance_tag);
	_max_abs_lum_spinbox->setValue(_max_abs_lum);
	_max_abs_lum_slider->setValue(mh_iround(_max_abs_lum * 100.0));
	_max_abs_lum_checkbox->setCheckState(Qt::Unchecked);
    }
    _sigma_spatial_spinbox->setValue(0.4);
    _sigma_spatial_slider->setValue(40);
    _sigma_luminance_spinbox->setValue(1.0);
    _sigma_luminance_slider->setValue(100);
    _base_contrast_spinbox->setValue(5.0);
    _base_contrast_slider->setValue(500);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDurand02ParameterSelector::set_max_abs_lum_state(int x UNUSED)
{
    if (_max_abs_lum_checkbox->isChecked())
    {
	_max_abs_lum_spinbox->setEnabled(true);
	_max_abs_lum_slider->setEnabled(true);
    }
    else
    {
	if (_max_abs_lum > 0.0f)
	    _max_abs_lum_spinbox->setValue(_max_abs_lum);
	_max_abs_lum_spinbox->setEnabled(false);
	_max_abs_lum_slider->setEnabled(false);
    }
}

void TonemapDurand02ParameterSelector::set_max_abs_lum(double x)
{
    _lock = true;
    _max_abs_lum_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDurand02ParameterSelector::max_abs_lum_slider_changed(int x)
{
    if (!_lock)
	_max_abs_lum_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDurand02ParameterSelector::set_sigma_spatial(double x)
{
    _lock = true;
    _sigma_spatial_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDurand02ParameterSelector::sigma_spatial_slider_changed(int x)
{
    if (!_lock)
	_sigma_spatial_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDurand02ParameterSelector::set_sigma_luminance(double x)
{
    _lock = true;
    _sigma_luminance_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDurand02ParameterSelector::sigma_luminance_slider_changed(int x)
{
    if (!_lock)
	_sigma_luminance_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDurand02ParameterSelector::set_base_contrast(double x)
{
    _lock = true;
    _base_contrast_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDurand02ParameterSelector::base_contrast_slider_changed(int x)
{
    if (!_lock)
	_base_contrast_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapDurand02ParameterSelector::get_parameters(Conf *conf) const
{
    conf->put("durand02-usemaxabslum", _max_abs_lum_checkbox->isChecked());
    conf->put("durand02-maxabslum", _max_abs_lum_spinbox->value());
    conf->put("durand02-sigmaspatial", _sigma_spatial_spinbox->value());
    conf->put("durand02-sigmaluminance", _sigma_luminance_spinbox->value());
    conf->put("durand02-basecontrast", _base_contrast_spinbox->value());
}

void TonemapDurand02ParameterSelector::set_parameters(Conf *conf)
{
    _max_abs_lum_spinbox->setValue(conf->get("durand02-maxabslum", 
		MAXABSLUM_MIN, MAXABSLUM_MAX, MAXABSLUM_DEFAULT));
    _max_abs_lum_checkbox->setChecked(conf->get("durand02-usemaxabslum", true));
    _sigma_spatial_spinbox->setValue(conf->get("durand02-sigmaspatial", 0.01, 9.99, 0.4));
    _sigma_luminance_spinbox->setValue(conf->get("durand02-sigmaluminance", 0.01, 9.99, 1.0));
    _base_contrast_spinbox->setValue(conf->get("durand02-basecontrast", 1.01, 9.99, 5.0));
}
