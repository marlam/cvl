/*
 * cvl_math.c
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
 * \file cvl_math.h
 * \brief Math related functions.
 *
 * Miscellaneous math related functions: Rounding, minimum and maximum, angles,
 * sorting, random numbers, and so on.
 */

#include "config.h"

#include <stdint.h>
#include <limits.h>

#include <gsl/gsl_rng.h>

#include "cvl/cvl_msg.h"
#include "cvl/cvl_math.h"


/**
 * \param a		The first factor.
 * \param b		The second factor.
 * \return		Whether the product fits into an int.
 *
 * Checks if the product of two ints fits into an int.
 * Used to prevent arithmetic overflows.
 */ 
bool cvl_product_fits_in_int(int a, int b)
{
    /* Adapted from the comp.lang.c FAQ, see http://c-faq.com/misc/sd26.html */
    if (a < 0)
    {
	a = -a;
    }
    if (b < 0)
    { 
	b = -b; 
    }
    return (b == 0 || !(INT_MAX / b < a));
}

/**
 * \param a		The first factor.
 * \param b		The second factor.
 * \return		Whether the product fits into an int.
 *
 * Checks if the product of two size_t's into a size_t. 
 * Used to prevent arithmetic overflows.
 */ 
bool cvl_product_fits_in_size_t(size_t a, size_t b)
{
    return (b == 0 || !(SIZE_MAX / b < a));
}


/**
 * \param a		A number.
 * \param b		A number.
 * \return 		The greatest common divisor of a and b.
 *
 * Returns the greatest common divisor of a and b.
 */
int cvl_gcd(int a, int b)
{
    int t;
    
    while (b != 0) 
    {
	t = b;
	b = a % b;
	a = t;
    }
    return a;
}

/**
 * \param a		A number.
 * \param b		A number.
 * \return 		The least common multiple of a and b.
 *
 * Returns the least common multiple of a and b.
 */
int cvl_lcm(int a, int b)
{
    int gcd = cvl_gcd(a,b);
    return (gcd == 0 ? 0 : (a / gcd) * b);
}


/* The random number generator used throughout CVL */
gsl_rng *cvl_rng = NULL;

/**
 * \param seed		The seed value.
 * 
 * Seeds the GSL random number generator (after initializing it if necessary).
 * The special seed 0 disabled seeding of the RNG; this function then just makes
 * sure that the RNG is initialized. This initialization seeds the RNG with a
 * default value which can be changed by setting the environment variable
 * GSL_RNG_SEED. See http://www.gnu.org/software/gsl/manual/gsl-ref_17.html#SEC268.
 */
void cvl_rng_init(unsigned long int seed)
{
    if (!cvl_rng)
    {
	gsl_rng_env_setup();
	cvl_rng = gsl_rng_alloc(gsl_rng_default);
	cvl_msg_dbg("initializing GSL random number generator %s", 
		gsl_rng_name(cvl_rng));
	if (seed == 0)
	{
	    seed = gsl_rng_default_seed;
	}
    }
    if (seed != 0)
    {
	gsl_rng_set(cvl_rng, seed);
	cvl_msg_dbg("seeding GSL random number generator %s with seed %lu", 
		gsl_rng_name(cvl_rng), gsl_rng_default_seed);
    }
}
