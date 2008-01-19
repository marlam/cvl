/*
 * mh_msg.h
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007, 2008  Martin Lambers <marlam@marlam.de>
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

#ifndef MH_MSG_H
#define MH_MSG_H

#include "config.h"	// for UNUSED

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" 
{
#endif

typedef enum
{
    MH_MSG_DBG = 0,
    MH_MSG_INF = 1,
    MH_MSG_WRN = 2,
    MH_MSG_ERR = 3,
    MH_MSG_REQ = 4
} mh_msg_level_t;

void mh_msg_set_default_stream(FILE *f);
void mh_msg_set_output_level(mh_msg_level_t level);
void mh_msg_fmt_set_columns(int columns);
void mh_msg_fmt_set_columns_from_env(void);

#ifdef __GNUC__
void mh_msg_set_program_name(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_set_command_name(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg(FILE *f, mh_msg_level_t level, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void mh_msg_dbg(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_inf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_wrn(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_err(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_req(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_fmt(FILE *f, mh_msg_level_t level, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
void mh_msg_fmt_dbg(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_fmt_inf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_fmt_wrn(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_fmt_err(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void mh_msg_fmt_req(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
#else
void mh_msg_set_program_name(const char *format, ...);
void mh_msg_set_command_name(const char *format, ...);
void mh_msg(FILE *f, mh_msg_level_t level, const char *format, ...);
void mh_msg_dbg(const char *format, ...);
void mh_msg_inf(const char *format, ...);
void mh_msg_wrn(const char *format, ...);
void mh_msg_err(const char *format, ...);
void mh_msg_req(const char *format, ...);
void mh_msg_fmt(FILE *f, mh_msg_level_t level, const char *format, ...);
void mh_msg_fmt_dbg(const char *format, ...);
void mh_msg_fmt_inf(const char *format, ...);
void mh_msg_fmt_wrn(const char *format, ...);
void mh_msg_fmt_err(const char *format, ...);
void mh_msg_fmt_req(const char *format, ...);
#endif

/**
 * Like mh_msg_dbg(), but does nothing if DEBUG is 0.
 */
#if DEBUG
#define mh_msg_DBG(...) mh_msg_dbg(__VA_ARGS__)
#else
#define mh_msg_DBG(...) /* empty */
#endif

/**
 * Like mh_msg_fmt_dbg(), but does nothing if DEBUG is 0.
 */
#if DEBUG
#define mh_msg_fmt_DBG(...) mh_msg_fmt_dbg(__VA_ARGS__)
#else
#define mh_msg_fmt_DBG(...) /* empty */
#endif

#ifdef __cplusplus
}
#endif

#endif
