/*
 * options.c
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
extern int errno;
#include <getopt.h>
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;

#include "xalloc.h"

#include <cvl/cvl.h>

#include "options.h"


const option_t null_option = { NULL, '\0', 0, NULL, false };


/*
 * Error message helper.
 */
void cvtool_getopt_msg_invalid_arg(option_t *options, int optval, bool option_shortname)
{
    if (option_shortname)
    {
	cvl_msg_err("invalid argument for -%c", options[optval].short_name);
    }
    else
    {
	cvl_msg_err("invalid argument for --%s", options[optval].long_name);
    }
}

/*
 * Helper for array parsing, independent of int or double.
 */
bool cvtool_getopt_parse_array_info(char *s, 
	int allowed_dimensions, const int *allowed_sizes, 
	char **p, int *number_of_values,
	int *dimensions, int **sizes)
{
    char *q;
    bool error = false;
    
    /* An array has the form [[Nx]Nx...:]val1,val2,... */
    *p = strchr(s, ':');
    
    // 1. Determine the number of dimensions
    if (*p)
    {
	// the number of dimensions is 1 + (number of 'x' characters).
	*dimensions = 1;
	q = s;
	while ((q = strchr(q, 'x')))
	{
	    (*dimensions)++;
	    q++;
	}
    }
    else
    {
	// we have no first part; the array is just a list of values
	*dimensions = (allowed_dimensions == 0) ? 1 : allowed_dimensions;
    }
    if (allowed_dimensions != 0 && *dimensions != allowed_dimensions)
    {
	error = true;
    }
    if (!error)
    {
	*sizes = xmalloc(*dimensions * sizeof(int));
    }
    
    // 2. Determine the size in each dimension
    if (!error)
    {
	if (*p)
	{
	    int sizes_index = *dimensions - 1;
	    char *r;
	    long value;
	    
	    q = s;
	    while (!error && sizes_index >= 0)
	    {
		// expect "Nx" or, if this is the last dimension, "N:"
		errno = 0;
		value = strtol(q, &r, 0);
		if (r == q || *r != (sizes_index == 0 ? ':' : 'x') 
			|| errno == ERANGE || value < 1 || value > INT_MAX)
		{
		    error = true;
		}
		else
		{
		    (*sizes)[sizes_index--] = value;
		    q = r + 1;
		}
	    }
	}
	else
	{
	    if (allowed_sizes)
	    {
		memcpy(*sizes, allowed_sizes, *dimensions * sizeof(int));
	    }
	    else if (*dimensions == 1)
	    {
		(*sizes)[0] = 1;
		q = s;
		while ((q = strchr(q, ',')))
		{
		    (*sizes)[0]++;
		    q++;
		}
	    }
	    else
	    {
		error = true;
	    }
	}
    }
    if (!error)
    {
	*number_of_values = 1;
	for (int i = 0; i < *dimensions; i++)
	{
	    if (allowed_sizes)
	    {
		if ((*sizes)[i] == 0)
		{
		    (*sizes)[i] = allowed_sizes[i];
		}
		else if ((*sizes)[i] != allowed_sizes[i])
		{
		    error = true;
		    break;
		}
	    }
	    /* Make sure that the total number of elements does not overflow an int */
	    if (!cvl_product_fits_in_int(*number_of_values, (*sizes)[i]))
	    {
		error = true;
		break;
	    }
	    else
	    {
		*number_of_values *= (*sizes)[i];
	    }
	}
    }
    if (error)
    {
	free(*sizes);
	*sizes = NULL;
    }
    *p = *p ? *p + 1 : s;

    return !error;
}


/*
 * cvtool_getopt()
 *
 * see options.h
 */

bool cvtool_getopt(int argc, char *argv[], option_t *options, 
	int min_nonopt_args, int max_nonopt_args, int *argument_ind)
{
    int longopt_count;		/* Number of long options == number of options */
    int longopt_ind;		/* Index in options and longoptions */
    int shortopt_count;		/* Number of short options <= number of options */
    int optval_base;		/* First safe option identifier */
    struct option *longopts;	/* long option description for getopt_long() */
    char *shortopts;		/* short option description for getopt_long() */
    int shortopts_ind;		/* index in short options */
    int *shortopt_opt_ind;	/* Mapping from index of short options to index of options */
    bool option_shortname;	/* Whether the short name of an option was used */
    bool *option_was_seen;	/* Whether option i was used */
    bool error;
    
    longopt_count = 0;
    shortopt_count = 0;
    optval_base = 0;
    while (options[longopt_count].long_name != NULL)
    {
	if (options[longopt_count].short_name != '\0')
	{
	    if ((int)(options[longopt_count].short_name) >= optval_base)
	    {
		optval_base = (int)(options[longopt_count].short_name) + 1;
	    }
	    shortopt_count++;
	}	    
	longopt_count++;
    }
    /* Construct an array of type 'struct option', 'longopts', and a short 
     * option description string 'shortopts' for use with getopt_long().
     * The indices of options in the given array 'options' and the constructed
     * array 'longopts' are the same. This is not true for the 'shortopts' 
     * string. We use an index array 'shortopt_longopt_ind' in the following 
     * way: If 'shortopts' is ":i::", then option i has index 1 in 'shortopts',
     * and 'options[shortopt_longopt_ind[1]]' is the corresponding option 
     * description in 'options'.
     * The maximum length of 'shortopts' is 3*number_of_options+1: This is the
     * case when all options described in 'options' have corresponding short
     * options, and all have optional arguments. A leading colon in shortopts is
     * always needed. */
    longopts = xmalloc((longopt_count + 1) * sizeof(struct option));
    shortopts = xmalloc((1 + 3 * shortopt_count + 1) * sizeof(char));
    shortopts[0] = ':';
    shortopt_opt_ind = xmalloc((1 + 3 * shortopt_count) * sizeof(int));
    shortopts_ind = 1;
    for (longopt_ind = 0; longopt_ind < longopt_count; longopt_ind++)
    {
	longopts[longopt_ind].name = options[longopt_ind].long_name;
	if (options[longopt_ind].type == OPTION_BOOL)
	{
	    longopts[longopt_ind].has_arg = optional_argument;
	}
	else
	{
	    longopts[longopt_ind].has_arg = required_argument;
	}
	longopts[longopt_ind].flag = NULL;
	longopts[longopt_ind].val = optval_base + longopt_ind;
	if (options[longopt_ind].short_name != '\0')
	{
	    shortopts[shortopts_ind] = options[longopt_ind].short_name;
	    shortopt_opt_ind[shortopts_ind] = longopt_ind;
	    shortopts_ind++;
	    shortopts[shortopts_ind++] = ':';
	    if (options[longopt_ind].type == OPTION_BOOL)
	    {
		shortopts[shortopts_ind++] = ':';
	    }		
	}
    }
    shortopts[shortopts_ind] = '\0';
    longopts[longopt_count].name = NULL;
    longopts[longopt_count].has_arg = 0;
    longopts[longopt_count].flag = 0;
    longopts[longopt_count].val = 0;

    /* The getopt_long() loop */
    option_was_seen = xmalloc(longopt_count * sizeof(bool));
    for (int i = 0; i < longopt_count; i++)
    {
	option_was_seen[i] = false;
    }
    error = false;
    opterr = 0;
    while (!error)
    {
	int optval = getopt_long(argc, argv, shortopts, longopts, NULL);
	if (optval == -1)
	{
	    break;
	}
	else if (optval == '?')
	{
	    if (optopt == 0)
	    {
		cvl_msg_err("invalid option %s", argv[optind - 1]);
	    }
	    else
	    {
		cvl_msg_err("invalid option -%c", optopt);
	    }
	    error = true;
	    break;
	}
	else if (optval == ':')
	{
	    cvl_msg_err("option %s requires an argument", argv[optind - 1]);
	    error = true;
	    break;
	}
	else if (optval < optval_base)
	{
	    option_shortname = true;
	    optval = shortopt_opt_ind[(strchr(shortopts, optval) - shortopts)];
	}
	else
	{
	    option_shortname = false;
	    optval -= optval_base;
	}
	/* options[optval] is the original description of the current option. */
	option_was_seen[optval] = true;
	if (options[optval].type == OPTION_BOOL)
	{
	    option_bool_t *option_struct = options[optval].option_struct;
	    if (!optarg)
	    {
		option_struct->value = option_struct->default_value;
	    }
	    else if (strcmp(optarg, "yes") == 0 || strcmp(optarg, "on") == 0
		    || strcmp(optarg, "1") == 0 || strcmp(optarg, "true") == 0)
	    {
		option_struct->value = true;
	    }
	    else if (strcmp(optarg, "no") == 0 || strcmp(optarg, "off") == 0
		    || strcmp(optarg, "0") == 0 || strcmp(optarg, "false") == 0)
	    {
		option_struct->value = false;
	    }
	    else
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == OPTION_INT)
	{
	    option_int_t *option_struct = options[optval].option_struct;
	    long value;
	    char *endptr;

	    errno = 0;
	    value = strtol(optarg, &endptr, 0);
	    if (endptr == optarg || *endptr != '\0'
		    || errno == ERANGE
		    || value < (long)option_struct->min_value
		    || value > (long)option_struct->max_value)
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	    else
	    {
		option_struct->value = (int)value;
	    }
	}
	else if (options[optval].type == OPTION_DOUBLE)
	{
	    option_double_t *option_struct = options[optval].option_struct;
	    double value;
	    char *endptr;

	    errno = 0;
	    value = strtod(optarg, &endptr);
	    if (endptr == optarg || *endptr != '\0'
		    || errno == ERANGE
		    || (option_struct->lower_bound_inclusive && value < option_struct->lower_bound)
		    || (!option_struct->lower_bound_inclusive && value <= option_struct->lower_bound)
		    || (option_struct->higher_bound_inclusive && value > option_struct->higher_bound)
		    || (!option_struct->higher_bound_inclusive && value >= option_struct->higher_bound))
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	    else
	    {
		option_struct->value = value;
	    }
	}
	else if (options[optval].type == OPTION_NAME)
	{
	    option_name_t *option_struct = options[optval].option_struct;
	    bool found = false;
	    int i = 0;
	    while (!found && option_struct->valid_values[i])
	    {
		if (strcmp(optarg, option_struct->valid_values[i]) == 0)
		{
		    option_struct->value = i;
		    found = true;
		}
		i++;
	    }
	    if (!found)
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == OPTION_FILE)
	{
	    option_file_t *option_struct = options[optval].option_struct;
	    if (option_struct->dash_means_stdinout && strcmp(optarg, "-") == 0)
	    {
		if (strchr(option_struct->mode, 'w'))
		{
		    option_struct->value = stdout;
		}
		else // assume that mode contains 'r'
		{
		    option_struct->value = stdin;
		}
	    }
	    else if (!(option_struct->value = fopen(optarg, option_struct->mode)))
	    {
		cvl_msg_err("%s: %s", optarg, strerror(errno));
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == OPTION_COLOR)
	{
	    option_color_t *option_struct = options[optval].option_struct;
	    if (!cvl_color_from_string(optarg, &(option_struct->value)))
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == OPTION_INT_ARRAY)
	{
	    option_int_array_t *option_struct = options[optval].option_struct;
	    int number_of_values;
	    char *p, *q;
	    
    	    free(option_struct->value);
	    option_struct->value = NULL;
    	    free(option_struct->value_sizes);
	    option_struct->value_sizes = NULL;
	    error = !cvtool_getopt_parse_array_info(optarg,
		    option_struct->dimensions, option_struct->sizes,
		    &p, &number_of_values,
		    &(option_struct->value_dimensions),
		    &(option_struct->value_sizes));
	    if (!error)
	    {
	    	option_struct->value = xmalloc(number_of_values * sizeof(int));
		for (int i = 0; i < number_of_values; i++)
		{
		    long value;
		    errno = 0;
		    value = strtol(p, &q, 0);
		    if (q == p || *q != (i == number_of_values - 1 ? '\0' : ',') 
			    || errno == ERANGE || value < INT_MIN || value > INT_MAX)
		    {
			error = true;
			free(option_struct->value);
			break;
		    }
		    option_struct->value[i] = value;
		    p = q + 1;
		}
	    }
	    if (error)
    	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		option_struct->value = NULL;
		free(option_struct->value_sizes);
		option_struct->value_sizes = NULL;
	    }
	}
	else if (options[optval].type == OPTION_DOUBLE_ARRAY)
	{
	    option_double_array_t *option_struct = options[optval].option_struct;
	    int number_of_values;
	    char *p, *q;
	    
	    free(option_struct->value);
    	    option_struct->value = NULL;
    	    free(option_struct->value_sizes);
	    option_struct->value_sizes = NULL;
	    error = !cvtool_getopt_parse_array_info(optarg,
		    option_struct->dimensions, option_struct->sizes,
		    &p, &number_of_values,
		    &(option_struct->value_dimensions),
		    &(option_struct->value_sizes));
	    if (!error)
	    {
	    	option_struct->value = xmalloc(number_of_values * sizeof(double));
		for (int i = 0; i < number_of_values; i++)
		{
		    double value;
		    errno = 0;
		    value = strtod(p, &q);
		    if (q == p || *q != (i == number_of_values - 1 ? '\0' : ',') || errno == ERANGE) 
		    {
			error = true;
			free(option_struct->value);
			break;
		    }
		    option_struct->value[i] = value;
		    p = q + 1;
		}
	    }
	    if (error)
    	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		option_struct->value = NULL;
		free(option_struct->value_sizes);
		option_struct->value_sizes = NULL;
	    }
	}
	else if (options[optval].type == OPTION_RATIO)
	{
	    option_ratio_t *option_struct = options[optval].option_struct;
	    char *p, *s = optarg;
	    long v1, v2;
	    error = true;
	    
	    errno = 0;
	    v1 = strtol(s, &p, 0);
	    if (p != s && *p == ':' && errno != ERANGE && v1 <= INT_MAX && v1 >= 0)
	    {
		s = p + 1;
		errno = 0; 
		v2 = strtol(s, &p, 0);
		if (p != s && *p == '\0' && errno != ERANGE && v2 <= INT_MAX && v2 >= 0)
		{
		    option_struct->value1 = v1;
		    option_struct->value2 = v2;
		    error = false;
		}
	    }
	    if (error)
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
	    }
	}
	else if (options[optval].type == OPTION_STRING)
	{
	    option_string_t *option_struct = options[optval].option_struct;
	    if (option_struct->is_valid && !option_struct->is_valid(optarg))
	    {
		cvtool_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	    else
	    {
		option_struct->value = optarg;
	    }
	}
    }
    
    /* Test if all mandatory options were seen */
    if (!error)
    {
	for (int i = 0; i < longopt_count; i++)
	{
	    if (options[i].mandatory && !option_was_seen[i])
	    {
		if (options[i].short_name != '\0')
		{
		    cvl_msg_err("option --%s (-%c) is mandatory", 
			    options[i].long_name, options[i].short_name);
		}
		else
		{
		    cvl_msg_err("option --%s is mandatory", 
			    options[i].long_name);
		}
		error = true;
		/* Do not break the loop. Print an error message for every missing option. */
	    }
	}
    }
    
    /* Test if number of non-options arguments is ok */
    if (!error)
    {
	int number_of_nonopt_args = argc - optind;
	if (number_of_nonopt_args < min_nonopt_args)
	{
	    cvl_msg_err("too few arguments");
	    error = true;
	}
	else if (max_nonopt_args >= 0 && number_of_nonopt_args > max_nonopt_args)
	{
	    cvl_msg_err("too many arguments");
	    error = true;
	}
    }
	
    /* Return index of first non-option argument */
    if (argument_ind)
    {
	*argument_ind = optind;
    }

    free(longopts);
    free(shortopts);
    free(shortopt_opt_ind);
    free(option_was_seen);
    return !error;
}
