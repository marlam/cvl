/*
 * cvl_io.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010
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

#ifndef CVL_IO_H
#define CVL_IO_H

#include <stdio.h>
#include <stdbool.h>

typedef enum
{
    CVL_PNM = 0,
    CVL_PFS = 1
} cvl_stream_type_t;

extern CVL_EXPORT void cvl_read(FILE *f, cvl_stream_type_t *type, cvl_frame_t **frame);
extern CVL_EXPORT void cvl_write(FILE *f, cvl_stream_type_t type, cvl_frame_t *frame);

extern CVL_EXPORT void cvl_read_pnm(FILE *f, cvl_frame_t **frame);
extern CVL_EXPORT void cvl_write_pnm(FILE *f, cvl_frame_t *frame);
extern CVL_EXPORT void cvl_read_pfs(FILE *f, cvl_frame_t **frame);
extern CVL_EXPORT void cvl_write_pfs(FILE *f, cvl_frame_t *frame);

extern CVL_EXPORT void cvl_load(const char *filename, cvl_stream_type_t *type, cvl_frame_t **frame);
extern CVL_EXPORT void cvl_save(const char *filename, cvl_stream_type_t type, cvl_frame_t *frame);

extern CVL_EXPORT void cvl_load_pnm(const char *filename, cvl_frame_t **frame);
extern CVL_EXPORT void cvl_save_pnm(const char *filename, cvl_frame_t *frame);
extern CVL_EXPORT void cvl_load_pfs(const char *filename, cvl_frame_t **frame);
extern CVL_EXPORT void cvl_save_pfs(const char *filename, cvl_frame_t *frame);

#endif
