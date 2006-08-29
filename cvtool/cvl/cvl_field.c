/*
 * cvl_field.c
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
 * \file cvl_field.h
 * \brief Fields.
 *
 * Functions to work with two-dimensional fields storing arbitrary data.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
extern int errno;

#include "xalloc.h"

#include "fseeko.h"

#include "cvl/cvl_assert.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_msg.h"
#include "cvl/cvl_field.h"

/**
 * \struct cvl_field_t
 * \brief A two-dimensional field.
 * 
 * Structure that stores a two-dimensional field. Use the functions from \link
 * cvl_field.h cvl_field.h \endlink to manipulate fields.
 */

/**
 * \param element_size	The size of the data elements of the field.
 * \param width		The width.
 * \param height	The height.
 * \return		The field.
 *
 * Creates a new field.
 * This function will abort the program if (\a width * \a height) does not fit
 * into an int or if (\a width * \a height * element_size) does not fit
 * into a size_t, to prevent arithmetic overflows.
 */
cvl_field_t *cvl_field_new(size_t element_size, int width, int height)
{
    cvl_assert(element_size >= 1);
    cvl_assert(width > 0);
    cvl_assert(height > 0);

    if (!cvl_product_fits_in_int(width, height)
	    || !cvl_product_fits_in_size_t(width * height, element_size))
    {
	cvl_msg_err("field with width=%d height=%d element_size=%zu " 
		"would cause arithmetic overflow", width, height, element_size);
	abort();
    }
    else
    {
	cvl_field_t *field = xmalloc(sizeof(cvl_field_t));
	field->_element_size = element_size;
	field->_width = width;
	field->_height = height;
	field->_p = xmalloc(width * height * element_size);
	return field;
    }
}

/**
 * \param nelem		Number of int components in one field element.
 * \param width		The width.
 * \param height	The height.
 * \return 		The field.
 */
cvl_field_t *cvl_field_i_new(int nelem, int width, int height)
{
    cvl_assert(nelem >= 1);
    cvl_assert(width > 0);
    cvl_assert(height > 0);

    if (!cvl_product_fits_in_size_t(nelem, sizeof(int)))
    {
	cvl_msg_err("field_i with nelement=%d would cause arithmetic overflow", nelem);
	abort();
    }
    else
    {
	return cvl_field_new((size_t)nelem * sizeof(int), width, height);
    }
}

/**
 * \param nelem		Number of float components in one field element.
 * \param width		The width.
 * \param height	The height.
 * \return 		The field.
 */
cvl_field_t *cvl_field_f_new(int nelem, int width, int height)
{
    cvl_assert(nelem >= 1);
    cvl_assert(width > 0);
    cvl_assert(height > 0);

    if (!cvl_product_fits_in_size_t(nelem, sizeof(float)))
    {
	cvl_msg_err("field_f with nelement=%d would cause arithmetic overflow", nelem);
	abort();
    }
    else
    {
	return cvl_field_new((size_t)nelem * sizeof(float), width, height);
    }
}


/**
 * \param field		The field to be freed.
 * 
 * Frees a field.
 * All allocated memory will be freed. \a field may be NULL; this
 * function will do nothing in this case.
 */
void cvl_field_free(cvl_field_t *field)
{
    if (field)
    {
	free(field->_p);
	free(field);
    }
}

/**
 * \param field		The field.
 * \return		The size of one element of the field.
 *
 * Returns the size of one element of a field.
 */
size_t cvl_field_element_size(const cvl_field_t *field)
{
    cvl_assert(field != NULL);

    return field->_element_size;
}

/**
 * \param field		The field.
 * \return		The width of the field.
 *
 * Returns the width of a field.
 */
int cvl_field_width(const cvl_field_t *field)
{
    cvl_assert(field != NULL);

    return field->_width;
}

/**
 * \param field		The field.
 * \return		The height of the field.
 *
 * Returns the height of a field.
 */
int cvl_field_height(const cvl_field_t *field)
{
    cvl_assert(field != NULL);

    return field->_height;
}

/**
 * \param field		The field.
 * \return		The number of elements in the field.
 *
 * Returns the size of a field (which means the number of elements in it).
 */
int cvl_field_size(const cvl_field_t *field)
{
    cvl_assert(field != NULL);

    return cvl_field_width(field) * cvl_field_height(field);
}


/**
 * \param field		The field.
 * 
 * Returns a pointer to the raw array data of the field, for direct read/write access.
 */
void *cvl_field_array(cvl_field_t *field)
{
    cvl_assert(field != NULL);

    return field->_p;
}

/**
 * \param field		The field.
 * 
 * Returns a const pointer to the raw array data of the field, for direct read access.
 */
const void *cvl_field_const_array(const cvl_field_t *field)
{
    cvl_assert(field != NULL);

    return field->_p;
}


/**
 * \param field		The field.
 * 
 * Sets all elements in \a field to zero. Note that this may result in invalid
 * element data, depending on the type of the data stored in \a field.
 */
void cvl_field_zero(cvl_field_t *field)
{
    cvl_assert(field != NULL);

    memset(field->_p, 0, cvl_field_size(field) * cvl_field_element_size(field));
}

/**
 * \param dst		The destination field.
 * \param src		The source field.
 * 
 * Copies the contents of \a src into \a dst. Both fields must have the same
 * dimensions and the same element size.
 */
void cvl_field_copy(cvl_field_t *dst, const cvl_field_t *src)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(cvl_field_width(dst) == cvl_field_width(src));
    cvl_assert(cvl_field_height(dst) == cvl_field_height(src));
    cvl_assert(cvl_field_element_size(dst) == cvl_field_element_size(src));

    memcpy(dst->_p, src->_p, cvl_field_size(src) * cvl_field_element_size(src));
}

/**
 * \param field		The field to be cloned.
 * \return An exact copy of the given field.
 * 
 * Clones a field.
 */
cvl_field_t *cvl_field_clone(const cvl_field_t *field)
{
    cvl_assert(field != NULL);
    
    cvl_field_t *clone = cvl_field_new(cvl_field_element_size(field),
	    cvl_field_width(field), cvl_field_height(field));
    cvl_field_copy(clone, field);
    return clone;
}

/**
 * \param field		The field.
 * \param x		The x coordinate of the upper left corner of the rectangle.
 * \param y		The y coordinate of the upper left corner of the rectangle.
 * \param w		The width of the rectangle.
 * \param h		The height of the rectangle.
 * \param e		The data element that the rectangle will be filled with.
 * 
 * Fills a rectangle in a field.
 */
void cvl_field_fill_rect(cvl_field_t *field, int x, int y, int w, int h, const void *e)
{
    cvl_assert(field != NULL);
    cvl_assert(x >= 0);
    cvl_assert(y >= 0);
    cvl_assert(w >= 0);
    cvl_assert(h >= 0);
    cvl_assert(x + w <= cvl_field_width(field));
    cvl_assert(y + h <= cvl_field_height(field));
    
    for (int ry = y; ry < y + h; ry++)
    {
	for (int rx = x; rx < x + w; rx++)
	{
	    cvl_field_set(field, rx, ry, e);
	}
    }
}

/**
 * \param dst		The destination field.
 * \param dst_x		The x coordinate of the destination rectangle's upper left corner.
 * \param dst_y		The y coordinate of the destination rectangle's upper left corner.
 * \param src		The source field.
 * \param src_x		The x coordinate of the source rectangle's upper left corner.
 * \param src_y		The y coordinate of the source rectangle's upper left corner.
 * \param rwidth	The width of the rectangle.
 * \param rheight	The height of the rectangle.
 * 
 * Copies the given rectangle. The fields must have the same type.
 * The areas may be in the same field, but they must not overlap!
 */
void cvl_field_copy_rect(cvl_field_t *dst, int dst_x, int dst_y, 
	const cvl_field_t *src, int src_x, int src_y, int rwidth, int rheight)
{
    cvl_assert(dst != NULL);
    cvl_assert(src != NULL);
    cvl_assert(dst_x >= 0);
    cvl_assert(dst_x < cvl_field_width(dst));
    cvl_assert(dst_y >= 0);
    cvl_assert(dst_y < cvl_field_height(dst));
    cvl_assert(src_x >= 0);
    cvl_assert(src_x < cvl_field_width(src));
    cvl_assert(src_y >= 0);
    cvl_assert(src_y < cvl_field_height(src));
    cvl_assert(rwidth >= 0);
    cvl_assert(dst_x + rwidth <= cvl_field_width(dst));
    cvl_assert(rheight >= 0);
    cvl_assert(dst_y + rheight <= cvl_field_height(dst));

    size_t element_size = cvl_field_element_size(src);
    for (int y = 0; y < rheight; y++)
    {
	memcpy(&((unsigned char *)(dst->_p))[element_size * ((dst_y + y) * cvl_field_width(dst) + dst_x)], 
		&((unsigned char *)(src->_p))[element_size * ((src_y + y) * cvl_field_width(src) + src_x)],
		(size_t)rwidth * element_size);
    }	
}

/**
 * \param f		The stream.
 * \param element_size	The size of a field element.
 * \param width		The field width.
 * \param height	The field height.
 * \param newpos	The number of the requested field.
 * \return		Success or error.
 * 
 * Seeks to the requested field in the file \a f that stores fields with 
 * the given \a element_size, \a width, and \a height in raw binary format.
 */
bool cvl_field_seek(FILE *f, size_t element_size, int width, int height, int newpos)
{
    cvl_assert(f != NULL);
    cvl_assert(element_size >= 1);
    cvl_assert(width > 0);
    cvl_assert(height > 0);
    cvl_assert(newpos >= 0);
    cvl_assert(cvl_product_fits_in_int(width, height));
    cvl_assert(cvl_product_fits_in_size_t(width * height, element_size));

    if (fseeko(f, (off_t)newpos * (off_t)(
		    9 * sizeof(char) + sizeof(size_t) + 2 * sizeof(int)	// CVL_FIELD + ELEM_SIZE + W + H
		    + width * height * element_size), 			// The data
		SEEK_SET) != 0)
    {
	cvl_msg_err("cannot seek in field file: %s", strerror(errno));
	return false;
    }
    else
    {
	return true;
    }
}

/* Helper: Read the header of a CVL field. */
static bool cvl_field_read_header(FILE *f, size_t *element_size, int *width, int *height)
{
    cvl_assert(f != NULL);

    char magic[9];

    if (fread(magic, sizeof(char), 9, f) != 9)
    {
	cvl_msg_err("unexpected EOF or input error in CVL field");
	return false;
    }
    if (strncmp(magic, "CVL_FIELD", 9) != 0)
    {
	cvl_msg_err("missing CVL_FIELD magic");
	return false;
    }
    if (fread(element_size, sizeof(size_t), 1, f) != 1
	    || fread(width, sizeof(int), 1, f) != 1
	    || fread(height, sizeof(int), 1, f) != 1)
    {
	cvl_msg_err("unexpected EOF or input error in CVL field");
	return false;
    }
    if (*element_size == 0 || *width <= 0 || *height <= 0
	    || !cvl_product_fits_in_int(*width, *height)
	    || !cvl_product_fits_in_size_t(*width * *height, *element_size))
    {
	cvl_msg_err("invalid type or dimensions of CVL field");
	return false;
    }
    return true;
}

/**
 * \param f		The stream to read from.
 * \param field		The field will be stored here.
 * \return		Success or error.
 * 
 * Reads a field from a stream.
 */
bool cvl_field_read(FILE *f, cvl_field_t **field)
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);

    size_t element_size;
    int width, height;

    if (!cvl_field_read_header(f, &element_size, &width, &height))
    {
	return false;
    }
    *field = cvl_field_new(element_size, width, height);
    if (fread(cvl_field_array(*field), element_size, width * height, f) != (size_t)(width * height))
    {
	cvl_msg_err("unexpected EOF or input error in CVL field");
	return false;
    }
    return true;
}

/**
 * \param f		The stream to read from.
 * \param field		The field will be stored here.
 * \param element_size	The size of elements in the field.
 * \return		Success or error.
 * 
 * Reads a field from a stream. It is an error if the size of elements in the field does not
 * match the expected \a element_size.
 */
bool cvl_field_read_knowntype(FILE *f, cvl_field_t **field, size_t element_size)
{
    cvl_assert(f != NULL);
    cvl_assert(element_size > 0);
    cvl_assert(field != NULL);

    size_t tmp_element_size;
    int width, height;

    if (!cvl_field_read_header(f, &tmp_element_size, &width, &height))
    {
	return false;
    }
    if (tmp_element_size != element_size)
    {
	cvl_msg_err("wrong type of CVL field");
	return false;
    }
    *field = cvl_field_new(element_size, width, height);
    if (fread(cvl_field_array(*field), element_size, width * height, f) != (size_t)(width * height))
    {
	cvl_msg_err("unexpected EOF or input error in CVL field");
	return false;
    }
    return true;
}

/**
 * \param f		The stream to read from.
 * \param field		The preallocated field.
 * \return		Success or error.
 * 
 * Reads a field from a stream. It is an error if the field from the stream does
 * not match the element size, width, or height of the preallocated field \a field.
 */
bool cvl_field_read_known(FILE *f, cvl_field_t *field)
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);

    size_t element_size;
    int width, height;

    if (!cvl_field_read_header(f, &element_size, &width, &height))
    {
	return false;
    }
    if (cvl_field_element_size(field) != element_size
	    || cvl_field_width(field) != width
	    || cvl_field_height(field) != height)
    {
	cvl_msg_err("wrong type or dimensions of CVL field");
	cvl_msg_err("es %d %d", element_size, cvl_field_element_size(field));
	cvl_msg_err("w %d %d", width, cvl_field_width(field));
	cvl_msg_err("h %d %d", height, cvl_field_height(field));
	return false;
    }
    if (fread(cvl_field_array(field), element_size, width * height, f) != (size_t)(width * height))
    {
	cvl_msg_err("unexpected EOF or input error in CVL field");
	return false;
    }
    return true;
}

/**
 * \param f		The stream to write the field to.
 * \param field		The field.
 * \return		Success or error.
 *
 * Writes a field to a stream.
 */
bool cvl_field_write(FILE *f, const cvl_field_t *field)
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);

    const char *magic = "CVL_FIELD";
    size_t element_size = cvl_field_element_size(field);
    int width = cvl_field_width(field);
    int height = cvl_field_height(field);

    if (fwrite(magic, sizeof(char), 9, f) != 9
	    || fwrite(&element_size, sizeof(size_t), 1, f) != 1
	    || fwrite(&width, sizeof(int), 1, f) != 1
	    || fwrite(&height, sizeof(int), 1, f) != 1
	    || fwrite(cvl_field_const_array(field), element_size, 
		width * height, f) != (size_t)(width * height))
    {
	cvl_msg_err("output error");
	return false;
    }

    return true;
}
