/*
 * cvl_error.c
 * 
 * This file is part of CVL, a computer vision library.
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

/** 
 * \file cvl_error.h
 * \brief Error handling. 
 *
 * CVL functions set an error status if something goes wrong. Subsequent calls
 * to CVL functions will do nothing if an error status is set. The application
 * can query the error status and act accordingly. In case of non-fatal errors,
 * the application can reset the error status.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "cvl_intern.h"
#include "cvl/cvl_error.h"


/**
 * \typedef cvl_error_t
 * The CVL error status.
 */
/** \var CVL_OK
 * No error. */
/** \var CVL_ERROR_MEM
 * Memory allocation failure. */
/** \var CVL_ERROR_GL
 * Failure of the GL library. */
/** \var CVL_ERROR_IO
 * Input/output error. */
/** \var CVL_ERROR_DATA
 * Data format error. */
/** \var CVL_ERROR_SYS
 * Operating system error. */
/** \var CVL_ERROR_ASSERT
 * An assertion failed. Probably caused by invalid arguments to a function. */


/**
 * \return	The CVL error status.
 *
 * This function returns the CVL error status.
 */
cvl_error_t cvl_error(void)
{
    return cvl_context()->error;
}


/**
 * \return	The CVL error message.
 *
 * This function returns the CVL error message.
 */
const char *cvl_error_msg(void)
{
    return cvl_context()->error_msg ? cvl_context()->error_msg : "unknown";
}


/**
 * \param e		The CVL error status.
 * \param msg		The error message.
 * \param ...		Variable argument list.
 *
 * Sets the CVL error status.
 */
void cvl_error_set(cvl_error_t e, const char *msg, ...)
{
    if (cvl_error())
	return;

    cvl_assert(e != CVL_OK);
    cvl_assert(msg != NULL);

    va_list args;
    char *str;

    cvl_context_t *ctx = cvl_context();
    ctx->error = e;
    va_start(args, msg);
    if (vasprintf(&str, msg, args) < 0)
	str = NULL;
    va_end(args);
    ctx->error_msg = str;
}


/**
 * This function resets the CVL error status.
 */
void cvl_error_reset(void)
{
    cvl_context_t *ctx = cvl_context();
    ctx->error = CVL_OK;
    free(ctx->error_msg);
    ctx->error_msg = NULL;
}
