/*
 * cvl_getopt.h
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

#ifndef CVL_GETOPT_H
#define CVL_GETOPT_H

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "cvl_color.h"


typedef enum
{
    CVL_OPTION_BOOL,
    CVL_OPTION_INT,
    CVL_OPTION_DOUBLE,
    CVL_OPTION_INT_ARRAY,
    CVL_OPTION_DOUBLE_ARRAY,
    CVL_OPTION_NAME,
    CVL_OPTION_STRING,
    CVL_OPTION_FILE,
    CVL_OPTION_COLOR,
    CVL_OPTION_RATIO,
    CVL_OPTION_INFO
} cvl_option_type_t;

typedef struct
{
    bool value;
    bool default_value;
} cvl_option_bool_t;

typedef struct
{
    int value;
    int min_value;
    int max_value;
} cvl_option_int_t;

typedef struct
{
    double value;
    double lower_bound;
    bool lower_bound_inclusive;
    double higher_bound;
    bool higher_bound_inclusive;
} cvl_option_double_t;

typedef struct
{
    int *value;
    int value_dimensions;
    int *value_sizes;
    int dimensions;
    const int *sizes;
} cvl_option_int_array_t;

typedef struct
{
    double *value;
    int value_dimensions;
    int *value_sizes;
    int dimensions;
    const int *sizes;
} cvl_option_double_array_t;

typedef struct
{
    int value;
    const char **valid_values;
} cvl_option_name_t;

typedef struct
{
    char *value;
    bool (*is_valid)(const char *s);
} cvl_option_string_t;
 
typedef struct
{
    FILE *value;
    const char *mode;
    bool dash_means_stdinout;
} cvl_option_file_t;

typedef struct
{
    cvl_color_t value;
} cvl_option_color_t;

typedef struct
{
    int value1;
    int value2;
} cvl_option_ratio_t;

typedef struct
{
    bool value;
    void (*function)(void);
} cvl_option_info_t;

typedef struct
{
    const char *long_name;
    char short_name;
    cvl_option_type_t type;
    void *option_struct;
    bool mandatory;
} cvl_option_t;

extern const int CVL_MASKSIZE_K_MAX;
extern const cvl_option_t cvl_option_null;

bool cvl_getopt(int argc, char *argv[], cvl_option_t *options, 
	int min_nonopt_args, int max_nonopt_args, int *argument_ind);

#endif
