/*
 * cvl_taglist.c
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007  Martin Lambers <marlam@marlam.de>
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

/** 
 * \file cvl_taglist.h
 * \brief Manipulation of tag lists.
 *
 * Manipulation of tag lists.
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "cvl_intern.h"
#include "cvl/cvl_error.h"
#include "cvl/cvl_taglist.h"


/*
 * We use a linear unsorted list for the tags.
 * This results in bad performance, but it has the advantage that the order of tags
 * does not change. If you read a frame with tags and then write it, the two
 * files will not differ, as they would if the order of tags changed.
 * Let's hope nobody uses lots of tags per frame.
 */

/**
 * \struct cvl_taglist_t
 * \brief A tag list.
 *
 * A tag list. A tag is a pair of a name and its associated value. Both name and
 * value are strings. The name must be unique.
 */

/**
 * \return 	A new empty tag list.
 *
 * Creates a new empty tag list.
 */
cvl_taglist_t *cvl_taglist_new(void)
{
    if (cvl_error())
	return NULL;

    cvl_taglist_t *taglist;
    
    if (!(taglist = malloc(sizeof(cvl_taglist_t))))
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	return NULL;
    }
    taglist->length = 0;
    taglist->size = 0;
    taglist->names = NULL;
    taglist->values = NULL;
    return taglist;
}

/**
 * \param taglist	The tag list.
 *
 * Empties a tag list.
 */
void cvl_taglist_clear(cvl_taglist_t *taglist)
{
    cvl_assert(taglist != NULL);
    if (cvl_error())
	return;

    for (int i = 0; i < taglist->length; i++)
    {
	free(taglist->names[i]);
	free(taglist->values[i]);
    }
    taglist->length = 0;
    taglist->size = 0;
    free(taglist->names);
    taglist->names = NULL;
    free(taglist->values);
    taglist->values = NULL;
}

/**
 * \param taglist	The tag list.
 *
 * Frees a tag list.
 */
void cvl_taglist_free(cvl_taglist_t *taglist)
{
    if (taglist)
    {
	cvl_taglist_clear(taglist);
	free(taglist);
    }
}

/**
 * \param taglist	The tag list.
 * \return		The length of the tag list.
 *
 * Returns the length of the taglist. Can be used with cvl_taglist_get_i() to
 * loop over all tags in the list.
 */
int cvl_taglist_length(const cvl_taglist_t *taglist)
{
    cvl_assert(taglist != NULL);
    if (cvl_error())
	return 0;

    return taglist->length;
}

/**
 * \param taglist	The tag list.
 * \param i		An index.
 * \param name		Storage space for a pointer to the tag name.
 * \param value		Storage space for a pointer to the tag value.
 *
 * Returns pointers to the name and value of tag \a i in \a and \a value.
 * Use cvl_taglist_length() to find out the number of tags in a tag list.
 */
void cvl_taglist_get_i(const cvl_taglist_t *taglist, int i, const char **name, const char **value)
{
    cvl_assert(taglist != NULL);
    cvl_assert(i >= 0 && i < taglist->length);
    cvl_assert(name != NULL);
    cvl_assert(value != NULL);
    if (cvl_error())
	return;

    *name = taglist->names[i];
    *value = taglist->values[i];
}

/**
 * \param taglist	The tag list.
 * \param name		A name.
 *
 * Returns a pointer to the value associated with the tag \a name in the tag 
 * list \a taglist. If there is no such name, NULL is returned.
 */
const char *cvl_taglist_get(const cvl_taglist_t *taglist, const char *name)
{
    cvl_assert(taglist != NULL);
    cvl_assert(name != NULL);
    if (cvl_error())
	return NULL;

    for (int i = 0; i < taglist->length; i++)
    {
	if (strcmp(taglist->names[i], name) == 0)
	    return taglist->values[i];
    }
    return NULL;
}

/**
 * \param taglist	The tag list.
 * \param name		A tag name.
 * \param value		A tag value.
 *
 * Puts the given tag (\a name and \a value) into the tag list \a taglist. If
 * \a name is already in the tag list, its value is overwritten.
 */
void cvl_taglist_put(cvl_taglist_t *taglist, const char *name, const char *value)
{
    cvl_assert(taglist != NULL);
    cvl_assert(name != NULL);
    cvl_assert(value != NULL);
    if (cvl_error())
	return;

    if (taglist->length == taglist->size)
    {
	taglist->size += 20;
	taglist->names = realloc(taglist->names, taglist->size * sizeof(char *));
	taglist->values = realloc(taglist->values, taglist->size * sizeof(char *));
	if (!taglist->names || !taglist->values)
	{
	    cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	    return;
	}
    }
    taglist->names[taglist->length] = strdup(name);
    taglist->values[taglist->length] = strdup(value);
    if (!taglist->names[taglist->length] || !taglist->values[taglist->length])
    {
	cvl_error_set(CVL_ERROR_MEM, "%s", strerror(ENOMEM));
	return;
    }
    taglist->length++;
}

/**
 * \param taglist	The tag list.
 * \return		A copy of the tag list.
 *
 * Copies the tag list \a taglist.
 */
cvl_taglist_t *cvl_taglist_copy(const cvl_taglist_t *taglist)
{
    cvl_assert(taglist != NULL);
    if (cvl_error())
	return NULL;

    cvl_taglist_t *copy = cvl_taglist_new();
    for (int i = 0; i < cvl_taglist_length(taglist); i++)
    {
	const char *name, *value;
	cvl_taglist_get_i(taglist, i, &name, &value);
	cvl_taglist_put(copy, name, value);
    }
    return copy;
}
