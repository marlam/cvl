/*
 * dataset_selector.cpp
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

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

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
    _total_label = new QLabel("/ 0000");
    layout->addWidget(_total_label, 0, 2);
    _total_label->setFixedWidth(_total_label->sizeHint().width());
    _total_label->setText("");

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
}

void DatasetSelector::set_nr(int nr)
{
    if (!*_datafile)
	return;

    if (!_lock)
    {
	_lock = true;
	
	nr--;
	if ((*_datafile)->total() == -1)
	{
	    if (nr < (*_datafile)->index())
	    {
		(*_datafile)->set_index(nr);
	    }
	    else
	    {
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
	    }
	}
	else
	{
	    (*_datafile)->set_index(nr);
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

	emit dataset_changed();
	_nr_spinbox->setValue((*_datafile)->index());
	if ((*_datafile)->total() != -1)
	{
	    _total_label->setText(mh_string("/ %4d", (*_datafile)->total() - 1).c_str());
	}
	_lock = false;
    }
}
