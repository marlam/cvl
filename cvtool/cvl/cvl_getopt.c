/*
 * cvl_getopt.c
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
 * \file cvl_getopt.h
 * \brief Handling command line options and arguments.
 *
 * Convenient handling of command line options and arguments that are typical
 * for programs using CVL. 
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

#include "cvl/cvl_msg.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_color.h"
#include "cvl/cvl_assert.h"
#include "cvl/cvl_getopt.h"


/**
 * This is the "End of option list" marker.
 */
const cvl_option_t cvl_option_null = { NULL, '\0', 0, NULL, false };

/**
 * Maximum allowed value for k values that define mask sizes or kernels so that
 * the maximum real mask/kernel size (2k+1 * 2k+1 * 2k+1) will not overflow an
 * int. This is not the highest possible value, but it is good enough. 
 */
const int CVL_MASKSIZE_K_MAX = (1 << (((sizeof(int) * CHAR_BIT - 1) / 3) - 1));

/**
 * \typedef cvl_option_type_t
 *
 * Type of an option.
 */
/** \var CVL_OPTION_BOOL
 *  Boolean option: yes/no, on/off, 1/0, true/false. */
/** \var CVL_OPTION_INT
 *  Integer option. */
/** \var CVL_OPTION_DOUBLE
 *  Double option. */
/** \var CVL_OPTION_INT_ARRAY
 *  Array of integers option: a (possibly multidimensional) array with int values. */
/** \var CVL_OPTION_DOUBLE_ARRAY
 *  Array of doubles option: a (possibly multidimensional) array with double values. */
/** \var CVL_OPTION_NAME
 *  Name option: a string from a set of valid names. */
/** \var CVL_OPTION_STRING
 *  String option: may contain everything */
/** \var CVL_OPTION_FILE
 *  File option: a filename. */
/** \var CVL_OPTION_COLOR
 *  Color option. */
/** \var CVL_OPTION_RATIO
 *  Ratio option: a ratio of two integers: "x1:x2" */
/** \var CVL_OPTION_INFO
 *  Info option: If this option is given, a function will be called.
 *  This is intended to be used for "--help" and "--version". */

/**
 * \struct cvl_option_bool_t
 * \brief A boolean option. 
 * 
 * A boolean option. 
 */
/** \var cvl_option_bool_t::value
 *  The default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_bool_t::default_value
 *  The value to use when the option has no explicit argument. */

/**
 * \struct cvl_option_int_t
 * \brief An integer option.
 * 
 * An integer option.
 */
/** \var cvl_option_int_t::value
 *  The default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_int_t::min_value
 *  Minimum allowed value. */
/** \var cvl_option_int_t::max_value
 *  Maximum allowed value. */

/**
 * \struct cvl_option_double_t
 * \brief A double option.
 * 
 * A double option.
 */
/** \var cvl_option_double_t::value
 *  The default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_double_t::lower_bound
 *  Lower bound for the value. */
/** \var cvl_option_double_t::lower_bound_inclusive
 *  Whether the lower bound is inclusive or not */
/** \var cvl_option_double_t::higher_bound
 *  Higher bound for the value. */
/** \var cvl_option_double_t::higher_bound_inclusive
 *  Whether the higher bound is inclusive or not */

/**
 * \struct cvl_option_int_array_t
 * \brief An integer array option.
 * 
 * An integer array option.
 */
/** \var cvl_option_int_array_t::value
 *  The default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_int_array_t::value_dimensions
 *  The number of dimensions of the default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_int_array_t::value_sizes
 *  The sizes of the default value in each dimension. May be overwritten by cvl_getopt(). */
/** \var cvl_option_int_array_t::dimensions
 *  Number of dimensions that the array must have. Usually 1, 2, or 3. If set to
 *  zero, the user can choose. */
/** \var cvl_option_int_array_t::sizes
 *  Sizes in each dimension that the array must have. For example {2, 2} for a
 *  2x2 matrix. If set to NULL, the user can choose. */

/**
 * \struct cvl_option_double_array_t
 * \brief A double array option.
 * 
 * A double array option.
 */
/** \var cvl_option_double_array_t::value
 *  The default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_double_array_t::value_dimensions
 *  The number of dimensions of the default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_double_array_t::value_sizes
 *  The sizes of the default value in each dimension. May be overwritten by cvl_getopt(). */
/** \var cvl_option_double_array_t::dimensions
 *  Number of dimensions that the array must have. Usually 1, 2, or 3. If set to
 *  zero, the user can choose. */
/** \var cvl_option_double_array_t::sizes
 *  Sizes in each dimension that the array must have. For example {2, 2} for a
 *  2x2 matrix. If set to NULL, the user can choose. */

/**
 * \struct cvl_option_name_t
 * \brief A name option.
 * 
 * A name option.
 */
/** \var cvl_option_name_t::value
 *  The default value. May be overwritten by cvl_getopt().
 *  The value is an index into the array \a cvl_option_name_t::valid_values. */
/** \var cvl_option_name_t::valid_values
 *  A NULL-terminated array of valid strings. */

/**
 * \struct cvl_option_string_t
 * \brief A string option.
 * 
 * A string option.
 */
/** \var cvl_option_string_t::value
 *  The default value. May be overwritten by cvl_getopt(). */
/** \var cvl_option_string_t::is_valid
 *  Function to check whether a string is valid for this option. If NULL, every
 *  string will be accepted. */

/**
 * \struct cvl_option_file_t
 * \brief A file option.
 * 
 * A file option.
 */
/** \var cvl_option_file_t::value
 *  The default value. May be overwritten by cvl_getopt(). This value will
 *  usually be NULL, but can also be stdin or stdout for example. */
/** \var cvl_option_file_t::mode
 *  Mode to open the file with, see fopen(). */
/** \var cvl_option_file_t::dash_means_stdinout
 *  If true, the filename "-" means stdout (if \a cvl_option_file_t::mode contains 'w')
 *  or stdin (if \a cvl_option_file_t::mode contains 'r'). */

/**
 * \struct cvl_option_color_t
 * \brief A color option.
 * 
 * A color option.
 */
/** \var cvl_option_color_t::value
 *  The default value. May be overwritten by cvl_getopt(). */

/**
 * \struct cvl_option_ratio_t
 * \brief A ratio option.
 * 
 * A ratio option.
 */
/** \var cvl_option_ratio_t::value1
 *  The default value 1. May be overwritten by cvl_getopt(). */
/** \var cvl_option_ratio_t::value2
 *  The default value 2. May be overwritten by cvl_getopt(). */

/**
 * \struct cvl_option_info_t.
 * \brief An info option.
 *
 * An info option.
 */
/** \var cvl_option_info_t::value
 *  Whether this option was given. Should be initialized to false, may be
 *  overwritten by cvl_getopt(). Once some informational option was found
 *  on the command line, error messages about missing mandatory options or
 *  about the number of arguments are not printed anymore. The program is
 *  expected to test if a CVL_OPTION_INFO was used and exit in this case. */
/** \var cvl_option_info_t::function
 *  Function to be called when the option is found. The function will only be
 *  called at most once. */

/**
 * \struct cvl_option_t
 * \brief An option description.
 *
 * An option description.
 */
/** \var cvl_option_t::long_name
 *  Long name of the option, or NULL if no long name exists. */
/** \var cvl_option_t::short_name
 *  Short name of the option, or \\0 if no short name exists. */
/** \var cvl_option_t::type
 *  Type of the option. */
/** \var cvl_option_t::option_struct
 *  Pointer to the appropriate struct for this option type. */
/** \var cvl_option_t::mandatory
 *  Whether this option is mandatory (or optional). */


/*
 * Error message helper.
 */
static void cvl_getopt_msg_invalid_arg(cvl_option_t *options, int optval, bool option_shortname)
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
static bool cvl_getopt_parse_array_info(char *s, 
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


/**
 * \param argc			Argument count.
 * \param argv			Argument vector.
 * \param options		Array of option definitions.
 * \param min_nonopt_args	Minimum number of required non-option arguments.
 * \param max_nonopt_args	Maximum number of required non-option arguments, or -1.
 * \param argument_ind		Storage space for the index of the first non-option argument, or NULL.
 * \return 			Success or error.
 *
 * Parses the command line given by \a argc and \a argv and processes the
 * options defined in the array \a options, which must be terminated with \a
 * cvl_option_null. The minimum and maximum number of required non-option
 * arguments can be given in \a min_nonopt_args and \a max_nonopt_args. If \a
 * max_nonopt_args is -1, any number of non-option arguments is accepted. If
 * \a argument_ind is not NULL, then the index of the first non-option argument
 * will be stored in it, allowing easy access to all non-option arguments
 * (since GNU getopt() is used, the command line arguments are reordered so 
 * that all non-option arguments appear after the option arguments).
 * If the command line is invalid, this function prints an informative error
 * message and returns false. The contents of \a argument_ind are undefined in
 * this case. If the command line is valid, the option values will be filled
 * into \a options, and true is returned.
 */
bool cvl_getopt(int argc, char *argv[], cvl_option_t *options,
	int min_nonopt_args, int max_nonopt_args, int *argument_ind)
{
    cvl_assert(argc >= 0);
    cvl_assert(argv != NULL);
    cvl_assert(options != NULL);
    cvl_assert(min_nonopt_args >= 0);
    cvl_assert(max_nonopt_args >= -1);

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
    bool info_option_was_seen;	/* Whether an option of type CVL_OPTION_INFO was seen */
    bool error;
    
    info_option_was_seen = false;
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
	if (options[longopt_ind].type == CVL_OPTION_BOOL)
	{
	    longopts[longopt_ind].has_arg = optional_argument;
	}
	else if (options[longopt_ind].type == CVL_OPTION_INFO)
	{
	    longopts[longopt_ind].has_arg = no_argument;
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
	    if (options[longopt_ind].type == CVL_OPTION_BOOL)
	    {
		// optional_argument
		shortopts[shortopts_ind++] = ':';
		shortopts[shortopts_ind++] = ':';
	    }		
	    else if (options[longopt_ind].type == CVL_OPTION_INFO)
	    {
		// no_argument
	    }
	    else
	    {
		// required_argument
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
	if (options[optval].type == CVL_OPTION_BOOL)
	{
	    cvl_option_bool_t *option_struct = options[optval].option_struct;
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == CVL_OPTION_INT)
	{
	    cvl_option_int_t *option_struct = options[optval].option_struct;
	    long value;
	    char *endptr;

	    errno = 0;
	    value = strtol(optarg, &endptr, 0);
	    if (endptr == optarg || *endptr != '\0'
		    || errno == ERANGE
		    || value < (long)option_struct->min_value
		    || value > (long)option_struct->max_value)
	    {
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	    else
	    {
		option_struct->value = (int)value;
	    }
	}
	else if (options[optval].type == CVL_OPTION_DOUBLE)
	{
	    cvl_option_double_t *option_struct = options[optval].option_struct;
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	    else
	    {
		option_struct->value = value;
	    }
	}
	else if (options[optval].type == CVL_OPTION_INT_ARRAY)
	{
	    cvl_option_int_array_t *option_struct = options[optval].option_struct;
	    int number_of_values;
	    char *p, *q;
	    
    	    free(option_struct->value);
	    option_struct->value = NULL;
    	    free(option_struct->value_sizes);
	    option_struct->value_sizes = NULL;
	    error = !cvl_getopt_parse_array_info(optarg,
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		option_struct->value = NULL;
		free(option_struct->value_sizes);
		option_struct->value_sizes = NULL;
	    }
	}
	else if (options[optval].type == CVL_OPTION_DOUBLE_ARRAY)
	{
	    cvl_option_double_array_t *option_struct = options[optval].option_struct;
	    int number_of_values;
	    char *p, *q;
	    
	    free(option_struct->value);
    	    option_struct->value = NULL;
    	    free(option_struct->value_sizes);
	    option_struct->value_sizes = NULL;
	    error = !cvl_getopt_parse_array_info(optarg,
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		option_struct->value = NULL;
		free(option_struct->value_sizes);
		option_struct->value_sizes = NULL;
	    }
	}
	else if (options[optval].type == CVL_OPTION_NAME)
	{
	    cvl_option_name_t *option_struct = options[optval].option_struct;
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == CVL_OPTION_STRING)
	{
	    cvl_option_string_t *option_struct = options[optval].option_struct;
	    if (option_struct->is_valid && !option_struct->is_valid(optarg))
	    {
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	    else
	    {
		option_struct->value = optarg;
	    }
	}
	else if (options[optval].type == CVL_OPTION_FILE)
	{
	    cvl_option_file_t *option_struct = options[optval].option_struct;
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == CVL_OPTION_COLOR)
	{
	    cvl_option_color_t *option_struct = options[optval].option_struct;
	    if (!cvl_color_from_string(optarg, &(option_struct->value)))
	    {
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
		error = true;
	    }
	}
	else if (options[optval].type == CVL_OPTION_RATIO)
	{
	    cvl_option_ratio_t *option_struct = options[optval].option_struct;
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
		cvl_getopt_msg_invalid_arg(options, optval, option_shortname);
	    }
	}
	else if (options[optval].type == CVL_OPTION_INFO)
	{
	    cvl_option_info_t *option_struct = options[optval].option_struct;
	    if (!option_struct->value)
	    {
		// do not call the function more than once
		option_struct->function();
	    }
	    option_struct->value = true;
	    info_option_was_seen = true;
	}
    }
    
    /* Test if all mandatory options were seen */
    if (!error && !info_option_was_seen)
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
    if (!error && !info_option_was_seen)
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
