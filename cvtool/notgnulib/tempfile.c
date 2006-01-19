/*
 * tempfile.c
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

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif /* _WIN32 */

#include "xalloc.h"

#include "tempfile.h"

#ifdef _WIN32
# define PATH_SEP '\\'
#else
# define PATH_SEP '/'
#endif


/* A Win32 mkstemp(), POSIX conformant. */
#ifdef _WIN32
#define mkstemp w32_mkstemp
int w32_mkstemp(char *template)
{
    size_t templatelen;
    char *X;
    int i;
    int try;
    int ret;
    const char alnum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 

    templatelen = strlen(template);
    if (templatelen < 6)
    {
	errno = EINVAL;
	return -1;
    }
    X = template + templatelen - 6;
    if (strcmp(X, "XXXXXX") != 0)
    {
	errno = EINVAL;
	return -1;
    }

    /* We have 62^6 possible filenames. We try 62^2=3844 times. */
    ret = -1;
    for (try = 0; ret == -1 && try < 3844; try++)
    {
	for (i = 0; i < 6; i++)
	{
	    X[i] = alnum[rand() % 62];
	}
	ret = _open(template, _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY, _S_IREAD | _S_IWRITE);
    }

    return ret;
}
#endif /* _WIN32 */


/*
 * tempfile()
 *
 * Create a temporary file, only accessible by the current user (if applicable
 * given the platform; on UNIX this means mode 0600). The file will be created
 * in $TMPDIR or, if this variable is unset, in a system specific directory for
 * temporary files.
 * 'base' is the file name prefix. It may be empty or even NULL, but if it
 * is a string, it must contain only characters that are safe for filenames on
 * all systems.
 * Return value is the resulting stream (opened with "w+") or NULL on error, in
 * which case errno will be set.
 * If the file can be created successfully, its name will be returned in
 * 'filename'.
 */
FILE *tempfile(const char *base, char **filename)
{
    FILE *f;
    size_t baselen;
    const char *dir;
    size_t dirlen;
    char *template = NULL;
    size_t templatelen;
    int fd = -1;
    int saved_errno;

    if (!base)
    {
	base = "tmp";
    }
    /* the directory for the temp file */
    if (!(dir = getenv("TMPDIR")))
    {
	/* system dependent default location */
#ifdef _WIN32
	if (!(dir = getenv("TEMP")))
	{
	    if (!(dir = getenv("TMP")))
	    {
		dir = "C:";
	    }		    
	}
#else /* UNIX */
#ifdef P_tmpdir
	dir = P_tmpdir;
#else
	dir = "/tmp";
#endif
#endif /* UNIX */
    }    
    dirlen = strlen(dir);

    /* the proposed file name */
    baselen = strlen(base);
    
    /* build the template */
    templatelen = dirlen + 1 + baselen + 6;
    template = xmalloc((templatelen + 1) * sizeof(char));
    strncpy(template, dir, dirlen);
    if (dirlen == 0 || template[dirlen - 1] != PATH_SEP)
    {
	template[dirlen++] = PATH_SEP;
    }
    /* template is long enough */
    strncpy(template + dirlen, base, baselen);
    strcpy(template + dirlen + baselen, "XXXXXX");

    /* create the file */
    if ((fd = mkstemp(template)) == -1)
    {
	goto error_exit;
    }

    /* UNIX only: set the permissions (not every mkstemp() sets them to 0600) */
#ifndef _WIN32
    if (fchmod(fd, S_IRUSR | S_IWUSR) == -1)
    {
	goto error_exit;
    }
#endif

    /* get the stream from the filedescriptor */
    if (!(f = fdopen(fd, "w+")))
    {
	goto error_exit;
    }
    *filename = template;

    return f;

error_exit:
    saved_errno = errno;
    if (fd >= 0)
    {
	close(fd);
    }
    if (template)
    {
	(void)remove(template);
	free(template);
    }
    errno = saved_errno;
    return NULL;
}
