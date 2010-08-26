/*
 * dataset_selector.cpp
 *
 * This file is part of cvlview, an image viewer using the CVL library.
 *
 * Copyright (C) 2007, 2008, 2009, 2010
 * Martin Lambers <marlam@marlam.de>
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

#include <climits>

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QApplication>

#include "mh.h"

#include "datafile.h"

#include "dataset_selector.h"


DatasetSelector::DatasetSelector(DataFile **datafile, QWidget *parent) 
	: QWidget(parent)
{
    _datafile = datafile;
    _lock = false;

    QGridLayout *layout = new QGridLayout;
    
    QLabel *label = new QLabel("Data set:");
    layout->addWidget(label, 0, 0);
    _nr_spinbox = new QSpinBox();
    _nr_spinbox->setRange(1, 9999);
    _nr_spinbox->setSingleStep(1);
    _nr_spinbox->setValue(1);
#if QT_VERSION >= 0x040300
    _nr_spinbox->setKeyboardTracking(false);
#endif
    connect(_nr_spinbox, SIGNAL(valueChanged(int)), this, SLOT(set_nr(int)));
    layout->addWidget(_nr_spinbox, 0, 1);

    _scan_button = new QPushButton("Scan");
    connect(_scan_button, SIGNAL(clicked()), this, SLOT(scan_button_clicked()));
    int scan_button_width = _scan_button->sizeHint().width() / 2;
    _total_label = new QLabel("/ 0000");
    int total_label_width = _total_label->sizeHint().width();
    int maxwidth = mh_maxi(scan_button_width, total_label_width);
    _scan_button->setFixedWidth(maxwidth);
    _total_label->setFixedWidth(maxwidth);
    layout->addWidget(_scan_button, 0, 2);
    layout->addWidget(_total_label, 0, 2);
    _total_label->setVisible(false);
    _total_label_was_set = false;

    _reload_button = new QPushButton("Reload");
    connect(_reload_button, SIGNAL(clicked()), this, SLOT(reload_button_clicked()));
    _reload_button->setFixedWidth(_reload_button->sizeHint().width() / 3 * 2);
    layout->addWidget(_reload_button, 0, 3);

    layout->setRowStretch(1, 1);
    setLayout(layout);
}

DatasetSelector::~DatasetSelector()
{
}

void DatasetSelector::reset()
{
    _total_label->setText("");
    _nr_spinbox->setValue(1);
    _scan_button->setVisible(true);
    _total_label->setVisible(false);
    _total_label_was_set = false;
}

void DatasetSelector::set_nr(int nr)
{
    if (!*_datafile)
	return;
    if (_lock)
        return;

    _lock = true;

    nr--;
    (*_datafile)->set_index(nr);
    emit make_gl_context_current();
    while ((*_datafile)->index() < nr)
    {
        cvl_frame_t *dummy = NULL;
        try
        {
            dummy = (*_datafile)->read();
        }
        catch (err e)
        {
            // Ignore the error here. The application will get it when
            // trying to read the frame.
        }
        if (!dummy)
        {
            break;
        }
        cvl_frame_free(dummy);
    }
    // Test for corner case: we are at EOF but don't know it yet.
    if ((*_datafile)->total() == -1)
    {
        try
        {
            cvl_frame_free((*_datafile)->read());
            (*_datafile)->prev();
        }
        catch (err e)
        {
            // Ignore the error here. The application will get it when
            // trying to read the frame.
        }
    }

    if ((*_datafile)->index() == (*_datafile)->total() - 1)
    {
        try
        {
            (*_datafile)->prev();
        }
        catch (err e)
        {
            // Ignore the error here. The application will get it when
            // trying to read the frame.
        }
    }

    if ((*_datafile)->total() != -1)
    {
        if (!_total_label_was_set)
        {
            _total_label->setText(mh_string("/ %4d", (*_datafile)->total() - 1).c_str());
            _scan_button->setVisible(false);
            _total_label->setVisible(true);
            _total_label_was_set = true;
        }
    }
    else
    {
        if (_total_label_was_set)
        {
            reset();
        }
    }

    emit dataset_changed();
    _nr_spinbox->setValue((*_datafile)->index());
    _lock = false;
}

void DatasetSelector::scan_button_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int nr = _nr_spinbox->value();
    set_nr(INT_MAX);
    set_nr(nr);
    QApplication::restoreOverrideCursor();
}

void DatasetSelector::reload_button_clicked()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    set_nr(_nr_spinbox->value());
    QApplication::restoreOverrideCursor();
}
