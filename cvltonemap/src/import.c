/*
 * import.c
 *
 * This file is part of cvltonemap, a tone mapping tool using the CVL library.
 *
 * Copyright (C) 2008  Martin Lambers <marlam@marlam.de>
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#ifdef W32_NATIVE
# define WIFEXITED(s) (1)
# define WEXITSTATUS(s) (s)
#else
# include <sys/wait.h>
#endif

#include <cvl/cvl.h>

#include "mh.h"

#ifdef W32_NATIVE
# define EXEEXT ".exe"
#else
# define EXEEXT ""
#endif


/* Copies 'fin' to 'fout'. Returns 0 on success, 1 on error. */
bool copy(FILE *fin, FILE *fout)
{
    char buf[4096];

    while (!feof(fin))
    {
	size_t len = fread(buf, sizeof(char), 4096, fin);
	if (ferror(fin))
	    break;
	fwrite(buf, sizeof(char), len, fout);
	if (ferror(fout))
	    break;
    }
    return !(ferror(fin) || ferror(fout));
}

void get_extension(const char *filename, char extension[5])
{
    char *dot = strrchr(filename, '.');
    if (!dot || strlen(filename) - (dot - filename) - 1 > 4)
    {
	extension[0] = '\0';
    }
    else
    {
	char *p = dot + 1;
	int i = 0;
	while (*p)
	{
	    extension[i++] = tolower((unsigned char)(*(p++)));
	}
	extension[i] = '\0';
    }
}

void import(const char *filename, cvl_frame_t **frame)
{
    char extension[5];
    char *pfs_filename;
    char pfs_magic[4] = { '\0', '\0', '\0', '\0' };
    FILE *f;

    *frame = NULL;

    if (!(f = fopen(filename, "r")))
    {
	cvl_error_set(CVL_ERROR_IO, "Cannot open %s: %s", filename, strerror(errno));
	return;
    }
    (void)fread(pfs_magic, sizeof(char), 4, f);
    (void)fclose(f);

    if (strncmp(pfs_magic, "PFS1", 4) == 0)
    {
	pfs_filename = mh_strdup(filename);
    }
    else
    {
	get_extension(filename, extension);
	const char *import_cmd;
	
	if (strcmp(extension, "hdr") == 0 || strcmp(extension, "pic") == 0)
	{
	    import_cmd = "pfsinrgbe" EXEEXT;
	}
	else if (strcmp(extension, "tif") == 0 || strcmp(extension, "tiff") == 0)
	{
	    import_cmd = "pfsintiff" EXEEXT;
	}
	else if (strcmp(extension, "exr") == 0)
	{
	    import_cmd = "pfsinexr" EXEEXT;
	}
	else if (strcmp(extension, "pfm") == 0)
	{
	    import_cmd = "pfsinpfm" EXEEXT;
	}
	else if (strcmp(extension, "cr2") == 0)
	{
	    import_cmd = "pfsindcraw" EXEEXT;
	}
	else
	{
	    cvl_error_set(CVL_ERROR_IO, 
		    "Cannot guess file format of %s: %s extension", 
		    filename, extension[0] ? "unknown" : "missing");
	    return;
	}
	const char *clamp_cmd = "pfsclamp" EXEEXT;

	FILE *file;
	if (!(file = fopen(filename, "r")))
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot open %s: %s", filename, strerror(errno));
	    return;
	}
	FILE *tempfile;
	char *tempfilename;
	if (!(tempfile = mh_mktempfile(PACKAGE "-import-in-", &tempfilename)))
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot create temporary file: %s", strerror(errno));
	    fclose(file);
	    return;
	}

	/* Copy the original file to tempfile, so that we have the input data in
	 * a file with a safe file name that we can later pass to system(3).
	 * Yes, this is inefficient... */
	if (!copy(file, tempfile))
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot import %s: input/output error", filename);
	    fclose(file);
	    fclose(tempfile);
	    remove(tempfilename);
	    free(tempfilename);
	    return;
	}
	fclose(file);
    	fclose(tempfile);

	FILE *intermfile;
	char *intermfilename;
	if (!(intermfile = mh_mktempfile(PACKAGE "-import-intermediate-", &intermfilename)))
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot create temporary file: %s", strerror(errno));
	    remove(tempfilename);
	    free(tempfilename);
	    return;
	}
	fclose(intermfile);

	char *import_system_cmd = mh_asprintf("%s %s > %s", import_cmd, tempfilename, intermfilename);
	int import_status = system(import_system_cmd);
	free(import_system_cmd);
	remove(tempfilename);
    	free(tempfilename);
	if (import_status == -1 || !WIFEXITED(import_status) || WEXITSTATUS(import_status) != 0)
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot execute the command %s. Make sure that the pfstools "
		    "package is installed and that the programs are in your PATH.", import_cmd);
	    remove(intermfilename);
	    free(intermfilename);
	    return;
	}

	FILE *resultfile;
	char *resultfilename;
	if (!(resultfile = mh_mktempfile(PACKAGE "-import-out-", &resultfilename)))
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot create temporary file: %s", strerror(errno));
	    remove(intermfilename);
	    free(intermfilename);
	    return;
	}
	fclose(resultfile);

	char *clamp_system_cmd = mh_asprintf("%s < %s > %s", clamp_cmd, intermfilename, resultfilename);
	int clamp_status = system(clamp_system_cmd);
	free(clamp_system_cmd);
	remove(intermfilename);
    	free(intermfilename);
	if (clamp_status == -1 || !WIFEXITED(clamp_status) || WEXITSTATUS(clamp_status) != 0)
	{
	    cvl_error_set(CVL_ERROR_IO, "Cannot execute the command %s. Make sure that the pfstools "
		    "package is installed and that the programs are in your PATH.", clamp_cmd);
	    remove(resultfilename);
	    free(resultfilename);
	    return;
	}

	pfs_filename = resultfilename;
    }

    cvl_load_pfs(pfs_filename, frame);    
    if (strcmp(pfs_filename, filename) != 0)
    {
	remove(pfs_filename);
    }
    free(pfs_filename);
}
