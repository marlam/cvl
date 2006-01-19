/*
 * cvl_msg.h
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

#ifndef CVL_MSG_H
#define CVL_MSG_H

#include <stdio.h>

typedef enum
{
    CVL_MSG_DBG = 0,
    CVL_MSG_INF = 1,
    CVL_MSG_WRN = 2,
    CVL_MSG_ERR = 3,
    CVL_MSG_REQ = 4
} cvl_msg_level_t;

void cvl_msg_set_output_level(cvl_msg_level_t level);
void cvl_msg_fmt_set_columns(int columns);

#ifdef __GNUC__
void cvl_msg_set_program_name(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_set_command_name(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg(FILE *f, cvl_msg_level_t level, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void cvl_msg_dbg(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_inf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_wrn(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_err(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_req(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_fmt(FILE *f, cvl_msg_level_t level, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void cvl_msg_fmt_dbg(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_fmt_inf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_fmt_wrn(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_fmt_err(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void cvl_msg_fmt_req(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#else
void cvl_msg_set_program_name(const char *format, ...);
void cvl_msg_set_command_name(const char *format, ...);
void cvl_msg(FILE *f, cvl_msg_level_t level, const char *format, ...);
void cvl_msg_dbg(const char *format, ...);
void cvl_msg_inf(const char *format, ...);
void cvl_msg_wrn(const char *format, ...);
void cvl_msg_err(const char *format, ...);
void cvl_msg_req(const char *format, ...);
void cvl_msg_fmt(FILE *f, cvl_msg_level_t level, const char *format, ...);
void cvl_msg_fmt_dbg(const char *format, ...);
void cvl_msg_fmt_inf(const char *format, ...);
void cvl_msg_fmt_wrn(const char *format, ...);
void cvl_msg_fmt_err(const char *format, ...);
void cvl_msg_fmt_req(const char *format, ...);
#endif

#endif
