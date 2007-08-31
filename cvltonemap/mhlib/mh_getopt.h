/*
 * mh_getopt.h
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2005, 2006, 2007  Martin Lambers <marlam@marlam.de>
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MH_GETOPT_H
#define MH_GETOPT_H

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    MH_OPTION_BOOL,
    MH_OPTION_INT,
    MH_OPTION_FLOAT,
    MH_OPTION_DOUBLE,
    MH_OPTION_INT_ARRAY,
    MH_OPTION_FLOAT_ARRAY,
    MH_OPTION_DOUBLE_ARRAY,
    MH_OPTION_NAME,
    MH_OPTION_STRING,
    MH_OPTION_FILE,
    MH_OPTION_RATIO,
    MH_OPTION_INFO
} mh_option_type_t;

typedef struct
{
    bool value;
    bool default_value;
} mh_option_bool_t;

typedef struct
{
    int value;
    int min_value;
    int max_value;
} mh_option_int_t;

typedef struct
{
    float value;
    float lower_bound;
    bool lower_bound_inclusive;
    float higher_bound;
    bool higher_bound_inclusive;
} mh_option_float_t;

typedef struct
{
    double value;
    double lower_bound;
    bool lower_bound_inclusive;
    double higher_bound;
    bool higher_bound_inclusive;
} mh_option_double_t;

typedef struct
{
    int *value;
    int value_dimensions;
    int *value_sizes;
    int dimensions;
    const int *sizes;
} mh_option_int_array_t;

typedef struct
{
    float *value;
    int value_dimensions;
    int *value_sizes;
    int dimensions;
    const int *sizes;
} mh_option_float_array_t;

typedef struct
{
    double *value;
    int value_dimensions;
    int *value_sizes;
    int dimensions;
    const int *sizes;
} mh_option_double_array_t;

typedef struct
{
    int value;
    const char **valid_values;
} mh_option_name_t;

typedef struct
{
    char *value;
    bool (*is_valid)(const char *s);
} mh_option_string_t;
 
typedef struct
{
    FILE *value;
    const char *mode;
    bool dash_means_stdinout;
} mh_option_file_t;

typedef struct
{
    int value1;
    int value2;
} mh_option_ratio_t;

typedef struct
{
    bool value;
    void (*function)(void);
} mh_option_info_t;

typedef struct
{
    const char *long_name;
    char short_name;
    mh_option_type_t type;
    void *option_struct;
    bool mandatory;
} mh_option_t;

extern const int MH_MASKSIZE_K_MAX;
extern const mh_option_t mh_option_null;

bool mh_getopt(int argc, char *argv[], mh_option_t *options, 
	int min_nonopt_args, int max_nonopt_args, int *argument_ind);

#ifdef __cplusplus
}
#endif

#endif
