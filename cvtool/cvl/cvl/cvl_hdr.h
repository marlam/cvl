/*
 * cvl_hdr.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008, 2009, 2010  Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_HDR_H
#define CVL_HDR_H

extern CVL_EXPORT float cvl_log_avg_lum(cvl_frame_t *frame, cvl_frame_t *tmp, float max_abs_lum);

extern CVL_EXPORT void cvl_tonemap_schlick94(cvl_frame_t *dst, cvl_frame_t *src, float p);

extern CVL_EXPORT void cvl_tonemap_tumblin99(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum,
	float log_avg_lum, float display_adaptation_level, float max_displayable_contrast);

extern CVL_EXPORT void cvl_tonemap_drago03(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, float bias, float max_disp_lum);

extern CVL_EXPORT void cvl_tonemap_reinhard05(cvl_frame_t *dst, cvl_frame_t *src, 
	float min_lum, float avg_lum, float log_avg_lum,
	cvl_frame_t *rgb, const float channel_avg[3],
	float f, float c, float l);

extern CVL_EXPORT void cvl_tonemap_ashikhmin02(cvl_frame_t *dst, cvl_frame_t *src, 
	float min_abs_lum, float max_abs_lum,
	cvl_frame_t *tmp, float threshold);

extern CVL_EXPORT void cvl_tonemap_durand02(cvl_frame_t *dst, cvl_frame_t *src, float max_abs_lum, 
	cvl_frame_t *tmp, int k, float sigma_spatial, float sigma_luminance, float base_contrast);

extern CVL_EXPORT void cvl_tonemap_reinhard02(cvl_frame_t *dst, cvl_frame_t *src, 
	cvl_frame_t *tmp, float log_avg_lum,
	float brightness, float white, float sharpness, float threshold);

#endif
