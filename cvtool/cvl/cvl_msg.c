/*
 * cvl_msg.c
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

/** 
 * \file cvl_msg.h
 * \brief Printing messages.
 *
 * Printing messages with different importance levels.
 */

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "xvasprintf.h"

#include "isblank.h"

#include "cvl/cvl_msg.h"


/**
 * \typedef cvl_msg_level_t
 * Importance level of a message. 
 */
/** \var CVL_MSG_DBG
 * Debugging message. */
/** \var CVL_MSG_INF
 * Informational message. */
/** \var CVL_MSG_WRN
 * Non-critical warning. */
/** \var CVL_MSG_ERR
 * Error message. */
/** \var CVL_MSG_REQ
 * Requested information. */


#define CVL_MSG_NAMESIZE	128
static char cvl_msg_program_name[CVL_MSG_NAMESIZE] = "";
static char cvl_msg_command_name[CVL_MSG_NAMESIZE] = "";
static cvl_msg_level_t cvl_msg_level = 0;
static const char *cvl_msg_level_names[] = { "DBG", "INF", "WRN", "ERR", "REQ" };
static int cvl_msg_fmt_columns = 0;

/**
 * \param format	The program name.
 * \param ...		Variable argument list.
 *
 * Sets an optional program name for messages.
 * The program name (if one is set) will appear in front of messages.
 */
void cvl_msg_set_program_name(const char *format, ...)
{
    va_list args; 
    va_start(args, format);
    vsnprintf(cvl_msg_program_name, CVL_MSG_NAMESIZE, format, args);
    va_end(args);
}

/**
 * \param format	The command name.
 * \param ...		Variable argument list.
 *
 * Sets an optional command name for messages.
 * The command name (if one is set) will appear in front of messages, in 
 * addition to the program name.
 */
void cvl_msg_set_command_name(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(cvl_msg_command_name, CVL_MSG_NAMESIZE, format, args);
    va_end(args);
}

/**
 * \param level		The level.
 * 
 * Sets minimum importance level for messages.
 */
void cvl_msg_set_output_level(cvl_msg_level_t level)
{
    cvl_msg_level = level;
}

/* Intern: Get prefix for line */
static char *cvl_msg_get_line_prefix(cvl_msg_level_t level)
{
    char *line_prefix;
    
    if (*cvl_msg_program_name && *cvl_msg_command_name)
    {
	line_prefix = xasprintf("%s: [%s] %s: ", 
		cvl_msg_program_name, cvl_msg_level_names[level], cvl_msg_command_name);
    }
    else if (*cvl_msg_program_name && !*cvl_msg_command_name)
    {
	line_prefix = xasprintf("%s: [%s] ", 
		cvl_msg_program_name, cvl_msg_level_names[level]);
    }
    else if (!*cvl_msg_program_name && *cvl_msg_command_name)
    {
	line_prefix = xasprintf("[%s] %s: ", 
		cvl_msg_level_names[level], cvl_msg_command_name);
    }
    else
    {
	line_prefix = xasprintf("[%s] ", 
		cvl_msg_level_names[level]);
    }

    return line_prefix;
}


/* This function does all the work for cvl_msg(). */
#ifdef __GNUC__
static void cvl_msg_vf(FILE *f, cvl_msg_level_t level, const char *format, va_list args)
    __attribute__ ((format (printf, 3, 0)));
#endif
static void cvl_msg_vf(FILE *f, cvl_msg_level_t level, const char *format, va_list args)
{
    char *line_prefix;
    
    if (level < cvl_msg_level)
    {
	return;
    }
    if (f == stderr)
    {
	line_prefix = cvl_msg_get_line_prefix(level);
	fputs(line_prefix, f);
	free(line_prefix);
    }
    vfprintf(f, format, args);
    fputc('\n', f);
}

/**
 * \param f		The stream.
 * \param level		Level of the message.
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Prints a message to the given stream (usually stderr).
 * The message will only be printed if the level is at least as high as the
 * minimum level that was set before, with cvl_msg_set_output_level(). 
 * If the stream \a f is stderr, the message will be prefixed with the program
 * name and optionally with a command name.
 */
void cvl_msg(FILE *f, cvl_msg_level_t level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_vf(f, level, format, args);
    va_end(args);
}

/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg() with stream stderr and level CVL_MSG_DBG.
 */
void cvl_msg_dbg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_vf(stderr, CVL_MSG_DBG, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg() with stream stderr and level CVL_MSG_INF.
 */
void cvl_msg_inf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_vf(stderr, CVL_MSG_INF, format, args);
    va_end(args);
}

/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg() with stream stderr and level CVL_MSG_WRN.
 */
void cvl_msg_wrn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_vf(stderr, CVL_MSG_WRN, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg() with stream stderr and level CVL_MSG_ERR.
 */
void cvl_msg_err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_vf(stderr, CVL_MSG_ERR, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg() with stream stderr and level CVL_MSG_REQ.
 */
void cvl_msg_req(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_vf(stderr, CVL_MSG_REQ, format, args);
    va_end(args);
}
    
    
/**
 * \param columns	The number of columns.
 * 
 * Sets the recommended maximum number of output columns for cvl_msg_fmt().
 * cvl_msg_fmt() will try to follow this recommendation. If this function is not
 * used or \a columns is <= 0, then cvl_msg_fmt() will use a default value.
 */
void cvl_msg_fmt_set_columns(int columns)
{
    cvl_msg_fmt_columns = columns;
}

/* This function does all the work for cvl_msg_fmt(). */
#ifdef __GNUC__
static void cvl_msg_fmt_vf(FILE *f, cvl_msg_level_t level, const char *format, va_list args)
    __attribute__ ((format (printf, 3, 0)));
#endif
static void cvl_msg_fmt_vf(FILE *f, cvl_msg_level_t level, const char *format, va_list args)
{
    char *line_prefix;
    size_t line_prefix_len;
    char *text;
    
    if (level < cvl_msg_level)
    {
	return;
    }
    if (cvl_msg_fmt_columns <= 0)
    {
	cvl_msg_fmt_columns = 80;
    }
    line_prefix = cvl_msg_get_line_prefix(level);
    line_prefix_len = strlen(line_prefix);
    
    text = xvasprintf(format, args);
    
    int line_len = 0;
    int first_unprinted = 0;
    int last_blank = -1;
    bool end_of_text = false;
    for (int text_index = 0; !end_of_text; text_index++)
    {
	if (text[text_index] == '\0')
    	{
	    text[text_index] = '\n';
	    end_of_text = true;
	}
	if (text[text_index] == '\n')
	{
	    // output from first_unprinted to text_index
	    fputs(line_prefix, f);
    	    fwrite(text + first_unprinted, sizeof(char), text_index - first_unprinted + 1, f);
	    first_unprinted = text_index + 1;
	    last_blank = -1;
	    line_len = 0;
	}
	else
	{
	    if (isblank((unsigned char)text[text_index]))
	    {
		last_blank = text_index;
	    }
	    if (line_len >= cvl_msg_fmt_columns - (int)line_prefix_len)
	    {
		// output from first_unprinted to last_blank (which is replaced
		// by '\n'), then update first_unprinted.
		if (last_blank == -1)
		{
		    // word is too long for line; search next blank and use that
		    do
		    {
			text_index++;
		    }
		    while (!isblank((unsigned char)text[text_index])
			    && text[text_index] != '\n'
			    && text[text_index] != '\0');
		    if (text[text_index] == '\0')
		    {
			end_of_text = true;
		    }
		    last_blank = text_index;
		}
		text[last_blank] = '\n';
		fputs(line_prefix, f);
		fwrite(text + first_unprinted, sizeof(char), last_blank - first_unprinted + 1, f);
		first_unprinted = last_blank + 1;
		last_blank = -1;
		line_len = text_index - first_unprinted + 1;
	    }
	    else
	    {
		line_len++;
	    }
	}
    }
    free(line_prefix);
    free(text);
}

/**
 * \param f		The stream-
 * \param level		Level of the text.
 * \param format	The text.
 * \param ...		Varargs.
 *
 * Prints a text to the given stream (usually stderr). 
 * If the stream \a f is stderr, the message will be prefixed with the program
 * name and optionally with a command name.
 * Lines will be wrapped so that no line is longer than the amount of characters
 * that was previously set with cvl_msg_fmt_set_columns() (if that
 * is possible). The text may contain its own newline characters; these will be
 * respected. A newline will be appended to the text.
 * The text will only be printed if the level is at least as high as the
 * minimum level that was set before, with cvl_msg_set_output_level(). 
 */
void cvl_msg_fmt(FILE *f, cvl_msg_level_t level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_fmt_vf(f, level, format, args);
    va_end(args);
}

/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg_fmt() with stream stderr and level CVL_MSG_DBG.
 */
void cvl_msg_fmt_dbg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_fmt_vf(stderr, CVL_MSG_DBG, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg_fmt() with stream stderr and level CVL_MSG_INF.
 */
void cvl_msg_fmt_inf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_fmt_vf(stderr, CVL_MSG_INF, format, args);
    va_end(args);
}

/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg_fmt() with stream stderr and level CVL_MSG_WRN.
 */
void cvl_msg_fmt_wrn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_fmt_vf(stderr, CVL_MSG_WRN, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg_fmt() with stream stderr and level CVL_MSG_ERR.
 */
void cvl_msg_fmt_err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_fmt_vf(stderr, CVL_MSG_ERR, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for cvl_msg_fmt() with stream stderr and level CVL_MSG_REQ.
 */
void cvl_msg_fmt_req(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    cvl_msg_fmt_vf(stderr, CVL_MSG_REQ, format, args);
    va_end(args);
}
