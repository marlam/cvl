/*
 * cvl_taglist.h
 * 
 * This file is part of CVL, a computer vision library.
 *
 * Copyright (C) 2007, 2008, 2009, 2010
 * Martin Lambers <marlam@marlam.de>
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

#ifndef CVL_TAGLIST_H
#define CVL_TAGLIST_H

typedef struct
{
    int length;
    int size;
    char **names;
    char **values;
} cvl_taglist_t;

extern CVL_EXPORT cvl_taglist_t *cvl_taglist_new(void);
extern CVL_EXPORT void cvl_taglist_clear(cvl_taglist_t *taglist);
extern CVL_EXPORT void cvl_taglist_free(cvl_taglist_t *taglist);
extern CVL_EXPORT int cvl_taglist_length(const cvl_taglist_t *taglist);
extern CVL_EXPORT void cvl_taglist_get_i(const cvl_taglist_t *taglist, int i, const char **name, const char **value);
extern CVL_EXPORT const char *cvl_taglist_get(const cvl_taglist_t *taglist, const char *name);
extern CVL_EXPORT void cvl_taglist_put(cvl_taglist_t *taglist, const char *name, const char *value);
extern CVL_EXPORT cvl_taglist_t *cvl_taglist_copy(const cvl_taglist_t *taglist);

#endif
