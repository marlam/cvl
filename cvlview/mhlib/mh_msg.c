/*
 * mh_msg.c
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

/** 
 * \file mh_msg.h
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

#include "mh_alloc.h"
#include "mh_msg.h"


/**
 * \typedef mh_msg_level_t
 * Importance level of a message. 
 */
/** \var MH_MSG_DBG
 * Debugging message. */
/** \var MH_MSG_INF
 * Informational message. */
/** \var MH_MSG_WRN
 * Non-critical warning. */
/** \var MH_MSG_ERR
 * Error message. */
/** \var MH_MSG_REQ
 * Requested information. */


#define MH_MSG_NAMESIZE	128
static FILE *mh_msg_default_stream = NULL;
static char mh_msg_program_name[MH_MSG_NAMESIZE] = "";
static char mh_msg_command_name[MH_MSG_NAMESIZE] = "";
static mh_msg_level_t mh_msg_level = MH_MSG_INF;
static const char *mh_msg_level_names[] = { "DBG", "INF", "WRN", "ERR", "REQ" };
static int mh_msg_fmt_columns = 0;

/**
 * \param f		The default stream.
 *
 * Set the default stream for output. If this function is not called, the
 * default stream will be stderr.
 */
void mh_msg_set_default_stream(FILE *f)
{
    mh_msg_default_stream = f;
}

/**
 * \param format	The program name.
 * \param ...		Variable argument list.
 *
 * Sets an optional program name for messages.
 * The program name (if one is set) will appear in front of messages.
 */
void mh_msg_set_program_name(const char *format, ...)
{
    va_list args; 
    va_start(args, format);
    vsnprintf(mh_msg_program_name, MH_MSG_NAMESIZE, format, args);
    va_end(args);
}

/**
 * \param format	The command name.
 * \param ...		Variable argument list.
 *
 * Sets an optional command name for messages.
 * The command name (if one is set) will appear in front of messages, in 
 * addition to the program name. An empty \a format argument unsets the command
 * name.
 */
void mh_msg_set_command_name(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(mh_msg_command_name, MH_MSG_NAMESIZE, format, args);
    va_end(args);
}

/**
 * \param level		The level.
 * 
 * Sets minimum importance level for messages.
 */
void mh_msg_set_output_level(mh_msg_level_t level)
{
    mh_msg_level = level;
}

/* Intern: Get prefix for line */
static char *mh_msg_get_line_prefix(mh_msg_level_t level)
{
    char *line_prefix;
    
    if (*mh_msg_program_name && *mh_msg_command_name)
    {
	line_prefix = mh_asprintf("%s: [%s] %s: ", 
		mh_msg_program_name, mh_msg_level_names[level], mh_msg_command_name);
    }
    else if (*mh_msg_program_name && !*mh_msg_command_name)
    {
	line_prefix = mh_asprintf("%s: [%s] ", 
		mh_msg_program_name, mh_msg_level_names[level]);
    }
    else if (!*mh_msg_program_name && *mh_msg_command_name)
    {
	line_prefix = mh_asprintf("[%s] %s: ", 
		mh_msg_level_names[level], mh_msg_command_name);
    }
    else
    {
	line_prefix = mh_asprintf("[%s] ", 
		mh_msg_level_names[level]);
    }

    return line_prefix;
}


/* This function does all the work for mh_msg(). */
#ifdef __GNUC__
static void mh_msg_vf(FILE *f, mh_msg_level_t level, const char *format, va_list args)
    __attribute__ ((format (printf, 3, 0)));
#endif
static void mh_msg_vf(FILE *f, mh_msg_level_t level, const char *format, va_list args)
{
    char *line_prefix;
    
    if (level < mh_msg_level)
    {
	return;
    }
    if (f == stderr)
    {
	line_prefix = mh_msg_get_line_prefix(level);
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
 * minimum level that was set before, with mh_msg_set_output_level(). 
 * The message will be prefixed with the program name, optionally with a command
 * name, and with the message level.
 */
void mh_msg(FILE *f, mh_msg_level_t level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_vf(f, level, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg() with the default stream and level MH_MSG_DBG.
 */
void mh_msg_dbg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_DBG, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg() with the default stream and level MH_MSG_INF.
 */
void mh_msg_inf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_INF, format, args);
    va_end(args);
}

/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg() with the default stream and level MH_MSG_WRN.
 */
void mh_msg_wrn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_WRN, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg() with the default stream and level MH_MSG_ERR.
 */
void mh_msg_err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_ERR, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg() with the default stream and level MH_MSG_REQ.
 */
void mh_msg_req(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_REQ, format, args);
    va_end(args);
}


/**
 * \param columns	The number of columns.
 * 
 * Sets the recommended maximum number of output columns for mh_msg_fmt().
 * mh_msg_fmt() will try to follow this recommendation. If this function is not
 * used or \a columns is <= 0, then mh_msg_fmt() will use a default value.
 */
void mh_msg_fmt_set_columns(int columns)
{
    mh_msg_fmt_columns = columns;
}
    
/**
 * Uses the environment variable COLUMNS to set the recommended maximum number 
 * of output columns for mh_msg_fmt(). If COLUMNS is not set or invalid, this
 * function will do nothing. See also mh_msg_fmt_set_columns().
 */
void mh_msg_fmt_set_columns_from_env(void)
{
    char *column_string;
    if ((column_string = getenv("COLUMNS")))
    {
    	int columns = atoi(column_string);
	if (columns > 0)
   	{
	    mh_msg_fmt_set_columns(columns);
     	}
    }
}


/* This function does all the work for mh_msg_fmt(). */
#ifdef __GNUC__
static void mh_msg_fmt_vf(FILE *f, mh_msg_level_t level, const char *format, va_list args)
    __attribute__ ((format (printf, 3, 0)));
#endif
static void mh_msg_fmt_vf(FILE *f, mh_msg_level_t level, const char *format, va_list args)
{
    char *line_prefix = NULL;
    size_t line_prefix_len = 0;
    char *text;
    
    if (level < mh_msg_level)
    {
	return;
    }
    if (mh_msg_fmt_columns <= 0)
    {
	mh_msg_fmt_columns = 80;
    }
    if (f == stderr)
    {
	line_prefix = mh_msg_get_line_prefix(level);
	line_prefix_len = strlen(line_prefix);
    }
    
    text = mh_vasprintf(format, args);
    
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
	    if (f == stderr)
		fputs(line_prefix, f);
    	    fwrite(text + first_unprinted, sizeof(char), text_index - first_unprinted + 1, f);
	    first_unprinted = text_index + 1;
	    last_blank = -1;
	    line_len = 0;
	}
	else
	{
	    if (text[text_index] == ' ' || text[text_index] == '\t')
	    {
		last_blank = text_index;
	    }
	    if (line_len >= mh_msg_fmt_columns - (int)line_prefix_len)
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
		    while (text[text_index] != ' '
			    && text[text_index] != '\t'
			    && text[text_index] != '\n'
			    && text[text_index] != '\0');
		    if (text[text_index] == '\0')
		    {
			end_of_text = true;
		    }
		    last_blank = text_index;
		}
		text[last_blank] = '\n';
		if (line_prefix)
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
 * that was previously set with mh_msg_fmt_set_columns() (if that
 * is possible). The text may contain its own newline characters; these will be
 * respected. A newline will be appended to the text.
 * The text will only be printed if the level is at least as high as the
 * minimum level that was set before, with mh_msg_set_output_level(). 
 */
void mh_msg_fmt(FILE *f, mh_msg_level_t level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_fmt_vf(f, level, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg_fmt() with the default stream and level MH_MSG_DBG.
 */
void mh_msg_fmt_dbg(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_fmt_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_DBG, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg_fmt() with the default stream and level MH_MSG_INF.
 */
void mh_msg_fmt_inf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_fmt_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_INF, format, args);
    va_end(args);
}

/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg_fmt() with the default stream and level MH_MSG_WRN.
 */
void mh_msg_fmt_wrn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_fmt_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_WRN, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg_fmt() with the default stream and level MH_MSG_ERR.
 */
void mh_msg_fmt_err(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_fmt_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_ERR, format, args);
    va_end(args);
}
    
/**
 * \param format	The message.
 * \param ...		Variable argument list.
 *
 * Shortcut for mh_msg_fmt() with the default stream and level MH_MSG_REQ.
 */
void mh_msg_fmt_req(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    mh_msg_fmt_vf(mh_msg_default_stream ? mh_msg_default_stream : stderr, MH_MSG_REQ, format, args);
    va_end(args);
}
