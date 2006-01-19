/*
 * options.h
 * 
 * This file is part of cvtool, a computer vision tool.
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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include <cvl/cvl.h>


/* The maximum allowed value for k values that defines mask sizes or kernels so that the
 * maximum real mask/kernel size (2k+1 x 2k+1 x 2k+1) will not overflow an int.
 * This is not the highest possible value, but it's good enough. */
#define MASKSIZE_K_MAX (1 << (((sizeof(int) * CHAR_BIT - 1) / 3) - 1))

/* An option can have one of the following types: */

enum option_type_t
{
    OPTION_BOOL,		/* yes/no, on/off, 1/0, true/false */
    OPTION_INT,			/* integer */
    OPTION_DOUBLE,		/* floating point (double) */
    OPTION_NAME,		/* a string from a set of valid names */
    OPTION_FILE,		/* name of a file */
    OPTION_COLOR,		/* name of a color, or 0xrrggbb */
    OPTION_INT_ARRAY,		/* a (possibly multidimensional) array with int values */
    OPTION_DOUBLE_ARRAY,	/* a (possibly multidimensional) array with double values */
    OPTION_RATIO,		/* a ratio of two integers: "x1:x2" */
    OPTION_STRING		/* may contain everything */
};


/* Each option type is further specified by one of the following structs.
 * The value of the option is always stored in the field 'value'. */

typedef struct
{
    bool value;
    bool default_value;		/* used when the option has no argument */
} option_bool_t;

typedef struct
{
    int value;
    int min_value;		/* minimum allowed value */
    int max_value;		/* maximum allowed value */
} option_int_t;

typedef struct
{
    double value;
    double lower_bound;		/* lower bound */
    bool lower_bound_inclusive;	/* whether lower bound is included or not */
    double higher_bound;	/* maximum allowed value */
    bool higher_bound_inclusive;/* whether higher bound is included or not */
} option_double_t;

typedef struct
{
    int value;			/* the value is an index in the following array */
    const char **valid_values;	/* the set of valid names, terminated by NULL */
} option_name_t;

typedef struct
{
    FILE *value;
    const char *mode;		/* mode to open the file with, see fopen() */
    bool dash_means_stdinout;	/* let filename "-" mean stdout (if mode contains 'w') or 
				   stdin (if mode contains 'r') */
} option_file_t;

typedef struct
{
    cvl_color_t value;
} option_color_t;

typedef struct
{
    int *value;
    int dimensions;		/* Number of dimensions: usually 1D, 2D, or 3D. If set to zero, 
				   the user can choose. */
    int *sizes;			/* Size in each dimension: for example 2x2 for a 2D transformation matrix. */
    				/* If set to zero, the user can choose. */
} option_int_array_t;

typedef struct
{
    double *value;
    int dimensions;		/* Number of dimensions: usually 1D, 2D, or 3D. If set to zero, 
				   the user can choose. */
    int *sizes;			/* Size in each dimension: for example 2x2 for a 2D transformation matrix. */
    				/* If set to zero, the user can choose. */
} option_double_array_t;

typedef struct
{
    int value1;
    int value2;
} option_ratio_t;

typedef struct
{
    char *value;
    bool (*is_valid)(const char *s);	/* function to check whether the argument is valid */
} option_string_t;
 
typedef struct
{
    const char *long_name;	/* long option, not NULL */
    char short_name;		/* short option or '\0' */
    int type;			/* one of the types defined above */
    void *option_struct;	/* pointer to the appropriate option structure for the type */
    bool mandatory;		/* is this option mandatory? */
} option_t;

extern const option_t null_option;

/*
 * cvtool_getopt()
 *
 * Parses the options for a cvtool command. 'options' must be an array of
 * option_t's terminated by 'null_option'. 'min_nonopt_args' gives the minimum
 * number of non-option arguments. 'max_nonopt_args' gives the maximum number of
 * non-option arguments, with -1 meaning that there is no limit. If 
 * 'argument_ind' is not NULL, it will point to the first non-option argument.
 * If the options and arguments are invalid in some way, this function prints an
 * approriate error message. The contents of 'argument_ind' are undefined in
 * this case.
 * The return value is true on success and false on error.
 */

bool cvtool_getopt(int argc, char *argv[], option_t *options, 
	int min_nonopt_args, int max_nonopt_args, int *argument_ind);

#endif
