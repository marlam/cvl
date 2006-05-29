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
 * Functions to work with two-dimensional fields storing data of different
 * types.
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

#include "cvl/cvl_msg.h"
#include "cvl/cvl_math.h"
#include "cvl/cvl_field.h"
#include "cvl_assert.h"

#define CVL_FIELD_IO_LINEBUFSIZE 512


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
inline size_t cvl_field_element_size(const cvl_field_t *field)
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
inline int cvl_field_width(const cvl_field_t *field)
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
inline int cvl_field_height(const cvl_field_t *field)
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
inline int cvl_field_size(const cvl_field_t *field)
{
    return cvl_field_width(field) * cvl_field_height(field);
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

    memcpy(dst->_p, src->_p, cvl_field_width(src) * cvl_field_height(src) 
	    * cvl_field_element_size(src));
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
 * \param i		The index of the element.
 * \return 		A pointer to the element.
 *
 * Returns a pointer to an element in a field. This element can then be
 * manipulated directly.
 * The index refers to all lines of the field one after another, 
 * from top to bottom and left to right.
 */
inline void *cvl_field_ref_i(const cvl_field_t *field, int i)
{
    cvl_assert(field != NULL);
    cvl_assert(i >= 0);
    cvl_assert(i < cvl_field_size(field));

    return &((unsigned char *)(field->_p))[cvl_field_element_size(field) * i];
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Returns a pointer to an element in a field. This element can then be
 * manipulated directly.
 */
inline void *cvl_field_ref(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);
    cvl_assert(x >= 0);
    cvl_assert(x < cvl_field_width(field));
    cvl_assert(y >= 0);
    cvl_assert(y < cvl_field_height(field));

    return cvl_field_ref_i(field, y * cvl_field_width(field) + x);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Returns a pointer to an element in a field. This element can then be
 * manipulated directly.
 * This function uses reflective indexing: arbitrary \a x and \a y values are accepted.
 */
inline void *cvl_field_ref_r(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);

    return cvl_field_ref(field, 
	    cvl_reflect(x, cvl_field_width(field)), 
	    cvl_reflect(y, cvl_field_height(field)));
}

/**
 * \param field		The field.
 * \param i		The index of the element.
 * \return 		A pointer to the element.
 *
 * Gets an element from a field by returning a pointer to it.
 * The index refers to all lines of the field one after another, 
 * from top to bottom and left to right.
 */
inline const void *cvl_field_get_i(const cvl_field_t *field, int i)
{
    cvl_assert(field != NULL);
    cvl_assert(i >= 0);
    cvl_assert(i < cvl_field_size);

    return cvl_field_ref_i(field, i);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Gets an element from a field by returning a pointer to it.
 */
inline const void *cvl_field_get(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);
    cvl_assert(x >= 0);
    cvl_assert(x < cvl_field_width(field));
    cvl_assert(y >= 0);
    cvl_assert(y < cvl_field_height(field));

    return cvl_field_get_i(field, y * cvl_field_width(field) + x);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Gets an element from a field by returning a pointer to it.
 * This function uses reflective indexing: arbitrary \a x and \a y values are accepted.
 */
inline const void *cvl_field_get_r(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);

    return cvl_field_get(field, 
	    cvl_reflect(x, cvl_field_width(field)), 
	    cvl_reflect(y, cvl_field_height(field)));
}

/**
 * \param field		The field.
 * \param i		The index of the element.
 * \param e		The element.
 *
 * Sets the element at index \a i in \a field to \a e.
 * The index refers to all lines of the field one after another, 
 * from top to bottom and left to right.
 */
inline void cvl_field_set_i(cvl_field_t *field, int i, const void *e)
{
    cvl_assert(field != NULL);
    cvl_assert(e != NULL);
    cvl_assert(i >= 0);
    cvl_assert(i < cvl_field_size);

    memcpy(cvl_field_ref_i(field, i), e, cvl_field_element_size(field));
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \param e		The element. 
 *
 * Sets the element at \a x, \a y in \a field to \a e.
 */
inline void cvl_field_set(cvl_field_t *field, int x, int y, const void *e)
{
    cvl_assert(field != NULL);
    cvl_assert(e != NULL);
    cvl_assert(x >= 0);
    cvl_assert(x < cvl_field_width(field));
    cvl_assert(y >= 0);
    cvl_assert(y < cvl_field_height(field));

    cvl_field_set_i(field, y * cvl_field_width(field) + x, e);
}


/**
 * \param field		The field.
 * \param i		The index of the element.
 * \return 		A pointer to the element.
 *
 * Convenience interface to cvl_field_get_i() for fields that store one or more
 * floats in one element.\n
 * Gets an element from a field by returning a pointer to it.
 * The index refers to all lines of the field one after another, 
 * from top to bottom and left to right.
 */
inline const float *cvl_field_getf_i(const cvl_field_t *field, int i)
{
    cvl_assert(field != NULL);
    cvl_assert(i >= 0);
    cvl_assert(i < cvl_field_size);

    return cvl_field_get_i(field, i);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Convenience interface to cvl_field_get() for fields that store one or more
 * floats in one element.\n
 * Gets an element from a field by returning a pointer to it.
 */
inline const float *cvl_field_getf(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);
    cvl_assert(x >= 0);
    cvl_assert(x < cvl_field_width(field));
    cvl_assert(y >= 0);
    cvl_assert(y < cvl_field_height(field));

    return cvl_field_get(field, x, y);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Convenience interface to cvl_field_get_r() for fields that store one or more
 * floats in one element.\n
 * Gets an element from a field by returning a pointer to it.
 * This function uses reflective indexing: arbitrary \a x and \a y values are accepted.
 */
inline const float *cvl_field_getf_r(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);

    return cvl_field_get_r(field, x, y);
}

/**
 * \param field		The field.
 * \param i		The index of the element.
 * \return 		A pointer to the element.
 *
 * Convenience interface to cvl_field_get_i() for fields that store one or more
 * ints in one element.\n
 * Gets an element from a field by returning a pointer to it.
 * The index refers to all lines of the field one after another, 
 * from top to bottom and left to right.
 */
inline const int *cvl_field_geti_i(const cvl_field_t *field, int i)
{
    cvl_assert(field != NULL);
    cvl_assert(i >= 0);
    cvl_assert(i < cvl_field_size);

    return cvl_field_get_i(field, i);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Convenience interface to cvl_field_get() for fields that store one or more
 * ints in one element.\n
 * Gets an element from a field by returning a pointer to it.
 */
inline const int *cvl_field_geti(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);
    cvl_assert(x >= 0);
    cvl_assert(x < cvl_field_width(field));
    cvl_assert(y >= 0);
    cvl_assert(y < cvl_field_height(field));

    return cvl_field_get(field, x, y);
}

/**
 * \param field		The field.
 * \param x		The x coordinate.
 * \param y		The y coordinate.
 * \return 		A pointer to the element.
 * 
 * Convenience interface to cvl_field_get_r() for fields that store one or more
 * ints in one element.\n
 * Gets an element from a field by returning a pointer to it.
 * This function uses reflective indexing: arbitrary \a x and \a y values are accepted.
 */
inline const int *cvl_field_geti_r(const cvl_field_t *field, int x, int y)
{
    cvl_assert(field != NULL);

    return cvl_field_get_r(field, x, y);
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

/* Internal function to convert a string to an integer. */
bool cvl_field_read_getint(const char *s, int min, int max, char **p, int *x)
{
    long value;
    
    errno = 0;
    value = strtol(s, p, 10);
    if (*p == s || (**p != ' ' && **p != '\0') || errno == ERANGE 
	    || value < (long)min || value > (long)max)
    {
	return false;
    }
    else
    {
	*x = (int)value;
	return true;
    }
}

/**
 * \param f	The input stream.
 * \return 	EOF (true) or not EOF (false).
 *
 * Tests whether there is more data readable from f.
 */
bool cvl_field_stream_eof(FILE *f)
{
    int c;
    
    if ((c = fgetc(f)) == EOF && !ferror(f))
    {
	return true;
    }
    else
    {
	if (ungetc(c, f) == EOF)
	{
	    cvl_msg_err("%s: ungetc() failed!", __func__);
	}
	return false;
    }
}

/**
 * \param f		The stream to read from.
 * \param field		The field will be stored here.
 * \param element_size	The size of one data element.
 * \param read_element	Function that converts a string to one element. Must return success or error.
 * \return		Success or error.
 * 
 * Reads a field from a stream. The type of the field must be known by the
 * caller, so that the necessary conversion function can be provided.
 */
bool cvl_field_read(FILE *f, cvl_field_t **field, size_t element_size,
	bool (*read_element)(const char *s, void *element))
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);
    cvl_assert(element_size > 0);
    cvl_assert(read_element != NULL);

    char linebuf[CVL_FIELD_IO_LINEBUFSIZE];
    size_t linelen;
    char *p;
    int width, height;
    
    *field = NULL;
    if (!fgets(linebuf, CVL_FIELD_IO_LINEBUFSIZE, f))
    {
	cvl_msg_err("unexpected EOF or input error in CVL field");
	return false;
    }
    linelen = strlen(linebuf);
    if (linelen < 1 || linebuf[linelen - 1] != '\n')
    {
	cvl_msg_err("unexpected EOF in CVL field");
	return false;
    }
    linebuf[--linelen] = '\0';
    // Expect "CVL FIELD <width> <height>"
    //        "... <width> * <height> lines with one element each ..."
    if (strncmp(linebuf, "CVL FIELD ", 10) != 0
	    || !cvl_field_read_getint(linebuf + 10, 1, INT_MAX, &p, &width)
	    || !cvl_field_read_getint(p + 1, 1, INT_MAX, &p, &height)
	    || *p != '\0')
    { 
	cvl_msg_err("invalid CVL field header");
	return false;
    }
    *field = cvl_field_new(element_size, width, height);
    for (int i = 0; i < width * height; i++)
    {
	if (!fgets(linebuf, CVL_FIELD_IO_LINEBUFSIZE, f))
	{
	    cvl_msg_err("unexpected EOF or input error in CVL field");
	    cvl_field_free(*field);
	    return false;
	}
	linelen = strlen(linebuf);
    	if (linelen < 1 || linebuf[linelen - 1] != '\n')
	{
	    cvl_msg_err("unexpected EOF in CVL field");
	    cvl_field_free(*field);
	    return false;
	}
	linebuf[--linelen] = '\0';
	if (!(read_element(linebuf, 
			&((unsigned char *)((*field)->_p))[cvl_field_element_size(*field) * i])))
	{
	    cvl_msg_err("cannot read data element from CVL field input string");
	    cvl_field_free(*field);
	    return false;
	}
    }
    return true;
}

/**
 * \param f		The stream to write the field to.
 * \param field		The field.
 * \param write_element	Function to convert one element into a fixed length string. Must return success or error.
 * \return		Success or error.
 *
 * Writes a field to a stream.
 */
bool cvl_field_write(FILE *f, cvl_field_t *field,
	bool (*write_element)(char *s, size_t s_size, const void *element))
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);
    cvl_assert(write_element != NULL);

    char linebuf[CVL_FIELD_IO_LINEBUFSIZE];
    
    if (fprintf(f, "CVL FIELD %d %d\n", 
	    cvl_field_width(field),
	    cvl_field_height(field)) < 0)
    {
	cvl_msg_err("cannot write CVL field header: %s", strerror(errno));
	return false;
    }
    for (int i = 0; i < cvl_field_size(field); i++)
    {
	if (!write_element(linebuf, CVL_FIELD_IO_LINEBUFSIZE,
    		    &((unsigned char *)(field->_p))[cvl_field_element_size(field) * i]))
	{
	    cvl_msg_err("cannot write data element to CVL field output string");
	    return false;
	}
	if (fprintf(f, "%s\n", linebuf) < 0)
	{
	    cvl_msg_err("cannot write data element to CVL field: %s", strerror(errno));
	    return false;
	}
    }
    return true;
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
bool cvl_field_seek_raw(FILE *f, size_t element_size, int width, int height, int newpos)
{
    if (fseeko(f, newpos * width * height * element_size, SEEK_SET) != 0)
    {
	cvl_msg_err("cannot seek in raw field file: %s", strerror(errno));
	return false;
    }
    else
    {
	return true;
    }
}

/**
 * \param f		The stream to read from.
 * \param field		A preallocated field to read the data into.
 * \return		Success or error.
 * 
 * Reads a field in raw binary data form from a stream into the preallocated
 * field \a field. Exactly width*height*element_size bytes are read.
 */
bool cvl_field_read_raw(FILE *f, cvl_field_t *field)
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);

    size_t count = cvl_field_size(field);
    if (fread(field->_p, cvl_field_element_size(field), count, f) != count)
    {
	cvl_msg_err("cannot read raw CVL field data: %s", strerror(errno));
	return false;
    }
    return true;
}

/**
 * \param f		The stream to write to.
 * \param field		The field to write.
 * \return		Success or error.
 * 
 * Writes the field \a field into \a f in raw binary data form.
 * Exactly width*height*element_size bytes are read.
 */
bool cvl_field_write_raw(FILE *f, cvl_field_t *field)
{
    cvl_assert(f != NULL);
    cvl_assert(field != NULL);

    size_t count = cvl_field_size(field);
    if (fwrite(field->_p, cvl_field_element_size(field), count, f) != count)
    {
	cvl_msg_err("cannot write raw CVL field data: %s", strerror(errno));
	return false;
    }
    return true;
}
