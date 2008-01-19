/*
 * mh_os.c
 * 
 * This file is part of cvtool, a computer vision tool.
 *
 * Copyright (C) 2007, 2008  Martin Lambers <marlam@marlam.de>
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

/** 
 * \file mh_os.h
 * \brief Platform/OS dependant things.
 *
 * Platform/OS dependant things.
 */

#include "config.h"

#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef W32_NATIVE
# include <windows.h>
# include <io.h>
# include <fcntl.h>
# include <limits.h>
#else
# include <sys/types.h>
# include <pwd.h>
#endif

#include "mh_alloc.h"
#include "mh_os.h"



/* W32 does neither have mkstemp() nor a reliable unlink(). The following is a
 * replacement:
 * w32_mkstemp(template, false) works exactly like POSIX mkstemp(template).
 * w32_mkstemp(template, true) works like POSIX mkstemp(template) followed
 * by POSIX unlink() on the generated file. Both functions will set errno if
 * they fail. */
#ifdef W32_NATIVE
static int w32_mkstemp(char *template, int unlink)
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
	if (unlink)
	{
	    ret = _open(template, _O_CREAT | _O_EXCL | _O_RDWR 
		    | _O_BINARY | _O_TEMPORARY, _S_IREAD | _S_IWRITE);
	}
	else
	{
	    ret = _open(template, _O_CREAT | _O_EXCL | _O_RDWR 
		    | _O_BINARY, _S_IREAD | _S_IWRITE);
	}
    }

    return ret;
}
#endif /* W32_NATIVE */


/* Internal function that does the real work. Use a NULL filename to get
 * tempfile() behaviour (no name returned, file will be deleted) and a non-NULL
 * filename to get mktempfile() behaviour (name returned, file will not be
 * deleted). */
static FILE *real_mktempfile(const char *base, char **filename)
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
#ifdef W32_NATIVE
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
    template = malloc((templatelen + 1) * sizeof(char));
    if (!template)
    {
	errno = ENOMEM;		/* not every malloc() sets this */
	goto error_exit;
    }
    strncpy(template, dir, dirlen);
    if (dirlen == 0 || template[dirlen - 1] != MH_DIRSEP)
    {
	template[dirlen++] = MH_DIRSEP;
    }
    /* template is long enough */
    strncpy(template + dirlen, base, baselen);
    strcpy(template + dirlen + baselen, "XXXXXX");

    /* create the file */
#ifdef W32_NATIVE
    if ((fd = w32_mkstemp(template, filename ? 0 : 1)) == -1)
    {
	goto error_exit;
    }
#else /* POSIX */
    if ((fd = mkstemp(template)) == -1)
    {
	goto error_exit;
    }
    if (fchmod(fd, S_IRUSR | S_IWUSR) == -1)
    {
	goto error_exit;
    }
    if (!filename)
    {
	if (unlink(template) != 0)
	{
	    goto error_exit;
	}
    }
#endif

    /* get the stream from the filedescriptor */
    if (!(f = fdopen(fd, "w+")))
    {
	goto error_exit;
    }
    if (filename)
    {
	*filename = template;
    }
    else
    {
	free(template);
    }

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

/**
 * \param base		File name prefix, or NULL.
 * \param filename	Resulting file name.
 * \return 		The resulting stream.
 *
 * This function is a better mkstemp().\n
 * It creates a temporary file, only accessible by the current user (if applicable
 * given the platform; on UNIX this means mode 0600). The file will be created
 * in $TMPDIR or, if this variable is unset, in a system specific directory for
 * temporary files.\n
 * The parameter \a base is the file name prefix. It may be empty or even NULL, 
 * but if it is a string, it must contain only characters that are safe for 
 * filenames on all systems.\n
 * Return value is the resulting stream (opened with "w+") or NULL on error, in
 * which case errno will be set.\n
 * If the file can be created successfully, its name will be returned in
 * \a filename, which must not be be NULL.
 */
FILE *mh_mktempfile(const char *base, char **filename)
{
    return real_mktempfile(base, filename);
}

/**
 * \param base		File name prefix, or NULL.
 * \return 		The resulting stream.
 *
 * This function is a better tmpfile().\n
 * It creates a temporary file, only accessible by the current user (if applicable
 * given the platform; on UNIX this means mode 0600). The file will be created
 * in $TMPDIR or, if this variable is unset, in a system specific directory for
 * temporary files.\n
 * The parameter \a base is the file name prefix. It may be empty or even NULL, 
 * but if it is a string, it must contain only characters that are safe for 
 * filenames on all systems.\n
 * Return value is the resulting stream (opened with "w+") or NULL on error, in
 * which case errno will be set.\n
 * The file will be automatically deleted when it is closed or the program
 * terminates.
 */
FILE *mh_tempfile(const char *base)
{
    return real_mktempfile(base, NULL);
}


/**
 * \param program_name	Name of the program. Use PACKAGE_NAME for autotools-based  projects.
 * \returns		An appropriate name of the application rc file for this platform.
 *
 * Constructs the full path of an application rc file name that is appropriate
 * for the platform. Example for POSIX systems: "foobar" -> "$HOME/.foobarrc".
 */
char *mh_get_apprcpath(const char *program_name)
{
    /* Get the directory in which to store the application rc file.
     * On POSIX, this is the home directory. On W32, this is the "User
     * Application data directory". */

    char *apprcdir;

#ifdef W32_NATIVE

    BYTE apprcdirbuf[MAX_PATH + 1];
    HKEY hkey;
    DWORD len;
    DWORD type;
    long l;
    
    if ((apprcdir = getenv("HOME")))
    {
	apprcdir = mh_strdup(apprcdir);
    }
    else
    {
	apprcdir = NULL;
	l = RegOpenKeyEx(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\"
		"Shell Folders", 0, KEY_READ, &hkey);
	if (l == ERROR_SUCCESS)
	{
	    len = MAX_PATH;
	    l = RegQueryValueEx(hkey, "AppData", NULL, &type, apprcdirbuf, &len);
	    if (l == ERROR_SUCCESS && len < MAX_PATH)
	    {
		RegCloseKey(hkey);
		apprcdir = mh_strdup((char *)apprcdirbuf);
	    }
	}
	if (!apprcdir)
	{
	    apprcdir = mh_strdup("C:");
	}
    }
    
#else /* POSIX */

    struct passwd *pw;
    
    if ((apprcdir = getenv("HOME")))
    {
	apprcdir = mh_strdup(apprcdir);
    }
    else
    {
	pw = getpwuid(getuid());
	if (pw && pw->pw_dir)
	{
	    apprcdir = mh_strdup(pw->pw_dir);
	}
	else
	{
	    apprcdir = mh_strdup("");
	}
    }

#endif

    /* Construct the name of the application rc file. */

    char *rcfilename;

    // Convert to lower case.
    char *tmpstr = mh_strdup(program_name);
    int i = 0;
    while (tmpstr[i] != '\0')
    {
	tmpstr[i] = tolower((unsigned char)tmpstr[i]);
	i++;
    }

#ifdef W32_NATIVE

    rcfilename = mh_asprintf("%s-apprc.txt", tmpstr);
    
#else /* POSIX */

    rcfilename = mh_asprintf(".%src", tmpstr);

#endif

    free(tmpstr);

    /* Combine filename and directory. */

    char *apprcpath = mh_asprintf("%s%c%s", apprcdir, MH_DIRSEP, rcfilename);
    free(apprcdir);
    free(rcfilename);

    return apprcpath;
}
