/*
 * tonemap_selector.cpp
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
#include "postproc_selector.h"
#include "maxabslum_selector.h"
#include "tonemap_selector.h"


TonemapSelector::TonemapSelector(cvl_frame_t **frame, QWidget *parent)
    : QWidget(parent)
{
    _frame = frame;

    /* When adding a new method, adjust only this block: ---> */
    _method_count = 8;
    _parameter_selector = new TonemapParameterSelector *[_method_count];
    _parameter_selector[0] = new TonemapRangeSelectionParameterSelector(this, _frame);
    _parameter_selector[1] = new TonemapSchlick94ParameterSelector(this, _frame);
    _parameter_selector[2] = new TonemapTumblin99ParameterSelector(this, _frame);
    _parameter_selector[3] = new TonemapDrago03ParameterSelector(this, _frame);
    _parameter_selector[4] = new TonemapReinhard05ParameterSelector(this, _frame);
    _parameter_selector[5] = new TonemapAshikhmin02ParameterSelector(this, _frame);
    _parameter_selector[6] = new TonemapDurand02ParameterSelector(this, _frame);
    _parameter_selector[7] = new TonemapReinhard02ParameterSelector(this, _frame);
    /* <--- End. */

    _postproc_selector = new PostprocSelector *[_method_count];
    for (int i = 0; i < _method_count; i++)
    {
	_postproc_selector[i] = new PostprocSelector(_parameter_selector[i]->id());
	connect(_postproc_selector[i], SIGNAL(postproc_changed()), this, SLOT(postproc_changed()));
    }

    _combo_box = new QComboBox();
    _combo_box->setEditable(false);
    for (int i = 0; i < _method_count; i++)
    {
	_combo_box->addItem(mh_string("%c %s", 
		    _parameter_selector[i]->is_global() ? 'G' : 'L',
		    _parameter_selector[i]->name()).c_str());
    }
    connect(_combo_box, SIGNAL(activated(int)), this, SLOT(tonemap_activator(int)));

    _tonemap_stack = new QStackedWidget();
    for (int i = 0; i < _method_count; i++)
    {
	_tonemap_stack->addWidget(_parameter_selector[i]);
    }
    _postproc_stack = new QStackedWidget();
    for (int i = 0; i < _method_count; i++)
    {
	_postproc_stack->addWidget(_postproc_selector[i]);
    }

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(_combo_box, 0, 0);
    layout->addWidget(_tonemap_stack, 1, 0);
    layout->addWidget(_postproc_stack, 2, 0);
    layout->setRowStretch(3, 1);
    setLayout(layout);

    tonemap_activator(1);
}

TonemapSelector::~TonemapSelector()
{
    delete[] _parameter_selector;
    delete[] _postproc_selector;
}

void TonemapSelector::postproc_changed()
{
    emit tonemap_changed();
}

void TonemapSelector::tonemap_activator(int index)
{
    _combo_box->setCurrentIndex(index);
    _tonemap_stack->setCurrentIndex(index);
    _postproc_stack->setCurrentIndex(index);
    _active_method = index;
    emit tonemap_changed();
}

void TonemapSelector::update()
{
    for (int i = 0; i < _method_count; i++)
    {
	_parameter_selector[i]->update();
    }
}

void TonemapSelector::get_parameters(Conf *conf) const
{
    for (int i = 0; i < _method_count; i++)
    {
	_parameter_selector[i]->get_parameters(conf);
	_postproc_selector[i]->get_parameters(conf);
    }
    conf->put("method", _parameter_selector[_active_method]->id());
}

void TonemapSelector::set_parameters(Conf *conf)
{
    for (int i = 0; i < _method_count; i++)
    {
	_parameter_selector[i]->set_parameters(conf);
	_postproc_selector[i]->set_parameters(conf);
    }
    int method_index = 0;
    const char *method = conf->get("method");
    for (int i = 0; i < _method_count; i++)
    {
	if (strcmp(method, _parameter_selector[i]->id()) == 0)
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
    conf->put(mh_string("%s-min", id()).c_str(), _range_min);
    conf->put(mh_string("%s-max", id()).c_str(), _range_max);
    conf->put(mh_string("%s-loghorz", id()).c_str(), _log_x_box->isChecked());
    conf->put(mh_string("%s-logvert", id()).c_str(), _log_y_box->isChecked());
}

void TonemapRangeSelectionParameterSelector::set_parameters(Conf *conf)
{
    _range_min = conf->get(mh_string("%s-min", id()).c_str(), 0.0f, 1.0f, 0.0f);
    _range_max = conf->get(mh_string("%s-max", id()).c_str(), 0.0f, 1.0f, 1.0f);
    _log_x_box->setChecked(conf->get(mh_string("%s-loghorz", id()).c_str(), true));
    _log_y_box->setChecked(conf->get(mh_string("%s-logvert", id()).c_str(), true));
}


/* Schlick94 */

TonemapSchlick94ParameterSelector::TonemapSchlick94ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    QLabel *lp = new QLabel("Brightness: ");
    layout->addWidget(lp, 0, 0);
    _p_spinbox = new QDoubleSpinBox();
    _p_spinbox->setRange(1.0, 999.99);
    _p_spinbox->setDecimals(2);
    _p_spinbox->setSingleStep(0.1);
    connect(_p_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_p(double)));
    layout->addWidget(_p_spinbox, 0, 1);
    _p_slider = new QSlider(Qt::Horizontal, this);
    _p_slider->setRange(1, 99999);
    connect(_p_slider, SIGNAL(valueChanged(int)), this, SLOT(p_slider_changed(int)));
    layout->addWidget(_p_slider, 1, 0, 1, 2);

    layout->setRowStretch(2, 1);
    setLayout(layout);

    update();
}

TonemapSchlick94ParameterSelector::~TonemapSchlick94ParameterSelector()
{
}

void TonemapSchlick94ParameterSelector::update()
{
    _lock = true;
    _p_spinbox->setValue(100.0);
    _p_slider->setValue(10000);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapSchlick94ParameterSelector::set_p(double x)
{
    _lock = true;
    _p_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapSchlick94ParameterSelector::p_slider_changed(int x)
{
    if (!_lock)
	_p_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapSchlick94ParameterSelector::get_parameters(Conf *conf) const
{
    conf->put(mh_string("%s-p", id()).c_str(), _p_spinbox->value());
}

void TonemapSchlick94ParameterSelector::set_parameters(Conf *conf)
{
    _p_spinbox->setValue(conf->get(mh_string("%s-p", id()).c_str(), 1.00, 999.99, 200.0));
}


/* Tumblin99 */

TonemapTumblin99ParameterSelector::TonemapTumblin99ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    _max_abs_lum_selector = new MaxAbsLumSelector(id(), _frame, this);
    layout->addWidget(_max_abs_lum_selector, 0, 0, 1, 2);
    connect(_max_abs_lum_selector, SIGNAL(maxabslum_changed()), this, SLOT(max_abs_lum_changed()));

    QLabel *ld = new QLabel("Display Adaptation Level: ");
    layout->addWidget(ld, 2, 0);
    _disp_adapt_level_spinbox = new QDoubleSpinBox();
    _disp_adapt_level_spinbox->setRange(0.01, 999.99);
    _disp_adapt_level_spinbox->setDecimals(2);
    _disp_adapt_level_spinbox->setSingleStep(0.1);
    connect(_disp_adapt_level_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_disp_adapt_level(double)));
    layout->addWidget(_disp_adapt_level_spinbox, 2, 1);
    _disp_adapt_level_slider = new QSlider(Qt::Horizontal, this);
    _disp_adapt_level_slider->setRange(1, 99999);
    connect(_disp_adapt_level_slider, SIGNAL(valueChanged(int)), this, SLOT(disp_adapt_level_slider_changed(int)));
    layout->addWidget(_disp_adapt_level_slider, 3, 0, 1, 2);

    QLabel *lb = new QLabel("Max. displayable contrast:");
    layout->addWidget(lb, 4, 0);
    _max_contrast_spinbox = new QDoubleSpinBox();
    _max_contrast_spinbox->setRange(0.01, 99.99);
    _max_contrast_spinbox->setDecimals(2);
    _max_contrast_spinbox->setSingleStep(0.1);
    connect(_max_contrast_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_max_contrast(double)));
    layout->addWidget(_max_contrast_spinbox, 4, 1);
    _max_contrast_slider = new QSlider(Qt::Horizontal, this);
    _max_contrast_slider->setRange(1, 9999);
    connect(_max_contrast_slider, SIGNAL(valueChanged(int)), this, SLOT(max_contrast_slider_changed(int)));
    layout->addWidget(_max_contrast_slider, 5, 0, 1, 2);

    layout->setRowStretch(6, 1);
    setLayout(layout);

    update();
}

TonemapTumblin99ParameterSelector::~TonemapTumblin99ParameterSelector()
{
}

void TonemapTumblin99ParameterSelector::max_abs_lum_changed()
{
    emit _tonemap_selector->tonemap_changed();
}

void TonemapTumblin99ParameterSelector::update()
{
    _lock = true;
    _max_abs_lum_selector->update();
    _disp_adapt_level_spinbox->setValue(100.0);
    _disp_adapt_level_slider->setValue(10000);
    _max_contrast_spinbox->setValue(70.0);
    _max_contrast_slider->setValue(7000);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapTumblin99ParameterSelector::set_disp_adapt_level(double x)
{
    _lock = true;
    _disp_adapt_level_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapTumblin99ParameterSelector::disp_adapt_level_slider_changed(int x)
{
    if (!_lock)
	_disp_adapt_level_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapTumblin99ParameterSelector::set_max_contrast(double x)
{
    _lock = true;
    _max_contrast_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapTumblin99ParameterSelector::max_contrast_slider_changed(int x)
{
    if (!_lock)
	_max_contrast_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapTumblin99ParameterSelector::get_parameters(Conf *conf) const
{
    _max_abs_lum_selector->get_parameters(conf);
    conf->put(mh_string("%s-disp_adapt_level", id()).c_str(), _disp_adapt_level_spinbox->value());
    conf->put(mh_string("%s-max_contrast", id()).c_str(), _max_contrast_spinbox->value());
}

void TonemapTumblin99ParameterSelector::set_parameters(Conf *conf)
{
    _max_abs_lum_selector->set_parameters(conf);
    _disp_adapt_level_spinbox->setValue(conf->get(mh_string("%s-disp_adapt_level", id()).c_str(), 0.01, 999.99, 200.0));
    _max_contrast_spinbox->setValue(conf->get(mh_string("%s-max_contrast", id()).c_str(), 0.01, 999.99, 70.0));
}


/* Drago03 */

TonemapDrago03ParameterSelector::TonemapDrago03ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    _max_abs_lum_selector = new MaxAbsLumSelector(id(), _frame, this);
    layout->addWidget(_max_abs_lum_selector, 0, 0, 1, 2);
    connect(_max_abs_lum_selector, SIGNAL(maxabslum_changed()), this, SLOT(max_abs_lum_changed()));

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

void TonemapDrago03ParameterSelector::max_abs_lum_changed()
{
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDrago03ParameterSelector::update()
{
    _lock = true;
    _max_abs_lum_selector->update();
    _max_disp_lum_spinbox->setValue(200.0);
    _max_disp_lum_slider->setValue(20000);
    _bias_spinbox->setValue(0.85);
    _bias_slider->setValue(85);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
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
    _max_abs_lum_selector->get_parameters(conf);
    conf->put(mh_string("%s-maxdisplum", id()).c_str(), _max_disp_lum_spinbox->value());
    conf->put(mh_string("%s-bias", id()).c_str(), _bias_spinbox->value());
}

void TonemapDrago03ParameterSelector::set_parameters(Conf *conf)
{
    _max_abs_lum_selector->set_parameters(conf);
    _max_disp_lum_spinbox->setValue(conf->get(mh_string("%s-maxdisplum", id()).c_str(), 0.01, 999.99, 200.0));
    _bias_spinbox->setValue(conf->get(mh_string("%s-bias", id()).c_str(), 0.01, 1.00, 0.85));
}


/* Reinhard05 */

TonemapReinhard05ParameterSelector::TonemapReinhard05ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    QLabel *lf = new QLabel("Brightness: ");
    layout->addWidget(lf, 0, 0);
    _f_spinbox = new QDoubleSpinBox();
    _f_spinbox->setRange(-8.0, +8.0);
    _f_spinbox->setDecimals(1);
    _f_spinbox->setSingleStep(0.1);
    connect(_f_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_f(double)));
    layout->addWidget(_f_spinbox, 0, 1);
    _f_slider = new QSlider(Qt::Horizontal, this);
    _f_slider->setRange(-80, +80);
    connect(_f_slider, SIGNAL(valueChanged(int)), this, SLOT(f_slider_changed(int)));
    layout->addWidget(_f_slider, 1, 0, 1, 2);

    QLabel *lc = new QLabel("Chromatic Adaptation:");
    layout->addWidget(lc, 2, 0);
    _c_spinbox = new QDoubleSpinBox();
    _c_spinbox->setRange(0.00, 1.00);
    _c_spinbox->setDecimals(2);
    _c_spinbox->setSingleStep(0.01);
    connect(_c_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_c(double)));
    layout->addWidget(_c_spinbox, 2, 1);
    _c_slider = new QSlider(Qt::Horizontal, this);
    _c_slider->setRange(0, 100);
    connect(_c_slider, SIGNAL(valueChanged(int)), this, SLOT(c_slider_changed(int)));
    layout->addWidget(_c_slider, 3, 0, 1, 2);

    QLabel *ll = new QLabel("Light Adaptation:");
    layout->addWidget(ll, 4, 0);
    _l_spinbox = new QDoubleSpinBox();
    _l_spinbox->setRange(0.00, 1.00);
    _l_spinbox->setDecimals(2);
    _l_spinbox->setSingleStep(0.01);
    connect(_l_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_l(double)));
    layout->addWidget(_l_spinbox, 4, 1);
    _l_slider = new QSlider(Qt::Horizontal, this);
    _l_slider->setRange(0, 100);
    connect(_l_slider, SIGNAL(valueChanged(int)), this, SLOT(l_slider_changed(int)));
    layout->addWidget(_l_slider, 5, 0, 1, 2);

    layout->setRowStretch(6, 1);
    setLayout(layout);

    update();
}

TonemapReinhard05ParameterSelector::~TonemapReinhard05ParameterSelector()
{
}

void TonemapReinhard05ParameterSelector::update()
{
    _lock = true;
    _f_spinbox->setValue(0.0);
    _f_slider->setValue(0);
    _c_spinbox->setValue(0.5);
    _c_slider->setValue(50);
    _l_spinbox->setValue(0.5);
    _l_slider->setValue(50);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard05ParameterSelector::set_f(double x)
{
    _lock = true;
    _f_slider->setValue(mh_iround(10.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard05ParameterSelector::f_slider_changed(int x)
{
    if (!_lock)
	_f_spinbox->setValue(static_cast<double>(x) / 10.0);
}

void TonemapReinhard05ParameterSelector::set_c(double x)
{
    _lock = true;
    _c_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard05ParameterSelector::c_slider_changed(int x)
{
    if (!_lock)
	_c_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapReinhard05ParameterSelector::set_l(double x)
{
    _lock = true;
    _l_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard05ParameterSelector::l_slider_changed(int x)
{
    if (!_lock)
	_l_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapReinhard05ParameterSelector::get_parameters(Conf *conf) const
{
    conf->put(mh_string("%s-f", id()).c_str(), _f_spinbox->value());
    conf->put(mh_string("%s-c", id()).c_str(), _c_spinbox->value());
    conf->put(mh_string("%s-l", id()).c_str(), _l_spinbox->value());
}

void TonemapReinhard05ParameterSelector::set_parameters(Conf *conf)
{
    _f_spinbox->setValue(conf->get(mh_string("%s-f", id()).c_str(), -8.0, 8.0, 0.0));
    _c_spinbox->setValue(conf->get(mh_string("%s-c", id()).c_str(), 0.0, 1.0, 0.5));
    _l_spinbox->setValue(conf->get(mh_string("%s-l", id()).c_str(), 0.0, 1.0, 0.5));
}


/* Ashikhmin02 */

TonemapAshikhmin02ParameterSelector::TonemapAshikhmin02ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    _max_abs_lum_selector = new MaxAbsLumSelector(id(), _frame, this);
    layout->addWidget(_max_abs_lum_selector, 0, 0, 1, 2);
    connect(_max_abs_lum_selector, SIGNAL(maxabslum_changed()), this, SLOT(max_abs_lum_changed()));

    QLabel *l3 = new QLabel("Threshold:");
    layout->addWidget(l3, 2, 0);
    _threshold_spinbox = new QDoubleSpinBox();
    _threshold_spinbox->setRange(0.001, 10.000);
    _threshold_spinbox->setDecimals(4);
    _threshold_spinbox->setSingleStep(0.1);
    connect(_threshold_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_threshold(double)));
    layout->addWidget(_threshold_spinbox, 2, 1);
    _threshold_slider = new QSlider(Qt::Horizontal, this);
    _threshold_slider->setRange(1, 10000);
    connect(_threshold_slider, SIGNAL(valueChanged(int)), this, SLOT(threshold_slider_changed(int)));
    layout->addWidget(_threshold_slider, 3, 0, 1, 2);

    layout->setRowStretch(4, 1);
    setLayout(layout);

    update();
}

TonemapAshikhmin02ParameterSelector::~TonemapAshikhmin02ParameterSelector()
{
}

void TonemapAshikhmin02ParameterSelector::update()
{
    _lock = true;
    _max_abs_lum_selector->update();
    _threshold_spinbox->setValue(0.5);
    _threshold_slider->setValue(500);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapAshikhmin02ParameterSelector::max_abs_lum_changed()
{
    emit _tonemap_selector->tonemap_changed();
}

void TonemapAshikhmin02ParameterSelector::set_threshold(double x)
{
    _lock = true;
    _threshold_slider->setValue(mh_iround(1000.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapAshikhmin02ParameterSelector::threshold_slider_changed(int x)
{
    if (!_lock)
	_threshold_spinbox->setValue(static_cast<double>(x) / 1000.0);
}

void TonemapAshikhmin02ParameterSelector::get_parameters(Conf *conf) const
{
    _max_abs_lum_selector->get_parameters(conf);
    conf->put(mh_string("%s-threshold", id()).c_str(), _threshold_spinbox->value());
}

void TonemapAshikhmin02ParameterSelector::set_parameters(Conf *conf)
{
    _max_abs_lum_selector->set_parameters(conf);
    _threshold_spinbox->setValue(conf->get(mh_string("%s-threshold", id()).c_str(), 0.001, 10.0, 1.0));
}


/* Durand02 */

TonemapDurand02ParameterSelector::TonemapDurand02ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    _max_abs_lum_selector = new MaxAbsLumSelector(id(), _frame, this);
    layout->addWidget(_max_abs_lum_selector, 0, 0, 1, 2);
    connect(_max_abs_lum_selector, SIGNAL(maxabslum_changed()), this, SLOT(max_abs_lum_changed()));

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

void TonemapDurand02ParameterSelector::max_abs_lum_changed()
{
    emit _tonemap_selector->tonemap_changed();
}

void TonemapDurand02ParameterSelector::update()
{
    _lock = true;
    _max_abs_lum_selector->update();
    _sigma_spatial_spinbox->setValue(0.4);
    _sigma_spatial_slider->setValue(40);
    _sigma_luminance_spinbox->setValue(1.0);
    _sigma_luminance_slider->setValue(100);
    _base_contrast_spinbox->setValue(5.0);
    _base_contrast_slider->setValue(500);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
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
    _max_abs_lum_selector->get_parameters(conf);
    conf->put(mh_string("%s-sigmaspatial", id()).c_str(), _sigma_spatial_spinbox->value());
    conf->put(mh_string("%s-sigmaluminance", id()).c_str(), _sigma_luminance_spinbox->value());
    conf->put(mh_string("%s-basecontrast", id()).c_str(), _base_contrast_spinbox->value());
}

void TonemapDurand02ParameterSelector::set_parameters(Conf *conf)
{
    _max_abs_lum_selector->set_parameters(conf);
    _sigma_spatial_spinbox->setValue(conf->get(mh_string("%s-sigmaspatial", id()).c_str(), 0.01, 9.99, 0.4));
    _sigma_luminance_spinbox->setValue(conf->get(mh_string("%s-sigmaluminance", id()).c_str(), 0.01, 9.99, 1.0));
    _base_contrast_spinbox->setValue(conf->get(mh_string("%s-basecontrast", id()).c_str(), 1.01, 9.99, 5.0));
}


/* Reinhard02 */

TonemapReinhard02ParameterSelector::TonemapReinhard02ParameterSelector(
	TonemapSelector *tonemap_selector, cvl_frame_t **frame)
{
    _tonemap_selector = tonemap_selector;
    _frame = frame;
    _lock = false;

    QGridLayout *layout = new QGridLayout();

    QLabel *l0 = new QLabel("Brightness:");
    layout->addWidget(l0, 0, 0);
    _brightness_spinbox = new QDoubleSpinBox();
    _brightness_spinbox->setRange(0.001, 1.000);
    _brightness_spinbox->setDecimals(3);
    _brightness_spinbox->setSingleStep(0.01);
    connect(_brightness_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_brightness(double)));
    layout->addWidget(_brightness_spinbox, 0, 1);
    _brightness_slider = new QSlider(Qt::Horizontal, this);
    _brightness_slider->setRange(1, 1000);
    connect(_brightness_slider, SIGNAL(valueChanged(int)), this, SLOT(brightness_slider_changed(int)));
    layout->addWidget(_brightness_slider, 1, 0, 1, 2);

    QLabel *l1 = new QLabel("White Value:");
    layout->addWidget(l1, 2, 0);
    _white_spinbox = new QDoubleSpinBox();
    _white_spinbox->setRange(0.1, 99.9);
    _white_spinbox->setDecimals(2);
    _white_spinbox->setSingleStep(0.1);
    connect(_white_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_white(double)));
    layout->addWidget(_white_spinbox, 2, 1);
    _white_slider = new QSlider(Qt::Horizontal, this);
    _white_slider->setRange(1, 999);
    connect(_white_slider, SIGNAL(valueChanged(int)), this, SLOT(white_slider_changed(int)));
    layout->addWidget(_white_slider, 3, 0, 1, 2);

    QLabel *l2 = new QLabel("Sharpness:");
    layout->addWidget(l2, 4, 0);
    _sharpness_spinbox = new QDoubleSpinBox();
    _sharpness_spinbox->setRange(0.01, 99.99);
    _sharpness_spinbox->setDecimals(2);
    _sharpness_spinbox->setSingleStep(0.1);
    connect(_sharpness_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_sharpness(double)));
    layout->addWidget(_sharpness_spinbox, 4, 1);
    _sharpness_slider = new QSlider(Qt::Horizontal, this);
    _sharpness_slider->setRange(1, 9999);
    connect(_sharpness_slider, SIGNAL(valueChanged(int)), this, SLOT(sharpness_slider_changed(int)));
    layout->addWidget(_sharpness_slider, 5, 0, 1, 2);

    QLabel *l3 = new QLabel("Threshold:");
    layout->addWidget(l3, 6, 0);
    _threshold_spinbox = new QDoubleSpinBox();
    _threshold_spinbox->setRange(0.001, 1.000);
    _threshold_spinbox->setDecimals(4);
    _threshold_spinbox->setSingleStep(0.1);
    connect(_threshold_spinbox, SIGNAL(valueChanged(double)), this, SLOT(set_threshold(double)));
    layout->addWidget(_threshold_spinbox, 6, 1);
    _threshold_slider = new QSlider(Qt::Horizontal, this);
    _threshold_slider->setRange(1, 1000);
    connect(_threshold_slider, SIGNAL(valueChanged(int)), this, SLOT(threshold_slider_changed(int)));
    layout->addWidget(_threshold_slider, 7, 0, 1, 2);

    layout->setRowStretch(8, 1);
    setLayout(layout);

    update();
}

TonemapReinhard02ParameterSelector::~TonemapReinhard02ParameterSelector()
{
}

void TonemapReinhard02ParameterSelector::update()
{
    _lock = true;
    _brightness_spinbox->setValue(1.0);
    _brightness_slider->setValue(100);
    _white_spinbox->setValue(10.0);
    _white_slider->setValue(100);
    _sharpness_spinbox->setValue(10.0);
    _sharpness_slider->setValue(1000);
    _threshold_spinbox->setValue(0.005);
    _threshold_slider->setValue(500);
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard02ParameterSelector::set_brightness(double x)
{
    _lock = true;
    _brightness_slider->setValue(mh_iround(1000.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard02ParameterSelector::brightness_slider_changed(int x)
{
    if (!_lock)
	_brightness_spinbox->setValue(static_cast<double>(x) / 1000.0);
}

void TonemapReinhard02ParameterSelector::set_white(double x)
{
    _lock = true;
    _white_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard02ParameterSelector::white_slider_changed(int x)
{
    if (!_lock)
	_white_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapReinhard02ParameterSelector::set_sharpness(double x)
{
    _lock = true;
    _sharpness_slider->setValue(mh_iround(100.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard02ParameterSelector::sharpness_slider_changed(int x)
{
    if (!_lock)
	_sharpness_spinbox->setValue(static_cast<double>(x) / 100.0);
}

void TonemapReinhard02ParameterSelector::set_threshold(double x)
{
    _lock = true;
    _threshold_slider->setValue(mh_iround(1000.0 * x));
    _lock = false;
    emit _tonemap_selector->tonemap_changed();
}

void TonemapReinhard02ParameterSelector::threshold_slider_changed(int x)
{
    if (!_lock)
	_threshold_spinbox->setValue(static_cast<double>(x) / 1000.0);
}

void TonemapReinhard02ParameterSelector::get_parameters(Conf *conf) const
{
    conf->put(mh_string("%s-brightness", id()).c_str(), _brightness_spinbox->value());
    conf->put(mh_string("%s-white", id()).c_str(), _white_spinbox->value());
    conf->put(mh_string("%s-sharpness", id()).c_str(), _sharpness_spinbox->value());
    conf->put(mh_string("%s-threshold", id()).c_str(), _threshold_spinbox->value());
}

void TonemapReinhard02ParameterSelector::set_parameters(Conf *conf)
{
    _brightness_spinbox->setValue(conf->get(mh_string("%s-brightness", id()).c_str(), 0.01, 1.0, 1.0));
    _white_spinbox->setValue(conf->get(mh_string("%s-white", id()).c_str(), 0.01, 99.9, 10.0));
    _sharpness_spinbox->setValue(conf->get(mh_string("%s-sharpness", id()).c_str(), 0.01, 99.99, 10.0));
    _threshold_spinbox->setValue(conf->get(mh_string("%s-threshold", id()).c_str(), 0.001, 1.0, 0.005));
}
