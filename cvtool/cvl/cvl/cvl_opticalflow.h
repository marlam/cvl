/*
 * cvl_opticalflow.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006  Martin Lambers <marlam@marlam.de>
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
 *   along with this program; if not, write to the Free Software Foundation,
 *   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CVL_OPTICALFLOW_H
#define CVL_OPTICALFLOW_H

#include "cvl_frame.h"

cvl_field_t *cvl_opticalflow_hs(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	double lambda, int iterations);
cvl_field_t *cvl_opticalflow_lk(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	int k);
cvl_field_t *cvl_opticalflow_clg(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	double lambda, double omega, int iterations);

cvl_field_t *cvl_opticalflow_bm_sad(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	int k, int maxdist, double distweight, double lumweight);
cvl_field_t *cvl_opticalflow_bm_asw(const cvl_frame_t *f1, const cvl_frame_t *f2, 
	int k, int maxdist, double gamma_c, double gamma_p);

cvl_field_t *cvl_opticalflow_cc(const cvl_field_t *fw, const cvl_field_t *bw, 
	int threshold, double *fraction);

#endif
