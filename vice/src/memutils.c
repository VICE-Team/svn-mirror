/*
 * memutils.c -- Miscellaneous memory-related functions.
 *
 * Written by
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "memutils.h"
#include "resources.h"
#include "file.h"
#include "utils.h"

/* ------------------------------------------------------------------------- */

static const char *create_name(const char *path, const char *name)
{
    static char  buffer[2048];
    static char *home = NULL;
    const char *p;

    if (!path && !name)
	return NULL;

    if (!name)
	name = "";
    else {
	p = name + strlen(name) -1;

	if  (*p == '/' || *p == '~' || *p == '.') {
	    fprintf (stderr, "Invalid filename '%s'\n", name);
	    return NULL;
	}
    }

    *buffer = 0;

#ifndef __MSDOS__
    if (*name == '~') {
	if (home == NULL)
	    home = getenv("HOME");
	if (home != NULL)
	    strcpy(buffer, home);
	++name;
    }
#endif

    if (path == NULL || *path == '\0')
	return name;

#ifdef __MSDOS__
    else if (*name != '/' && *name != '\\' && *name != '.')
	sprintf(buffer,"%s/", (path ? path : app_resources.directory));
#else
    else if (*name != '/' && *name != '.')
	sprintf(buffer, "%s/", (path ? path : app_resources.directory));
#endif

    strcat(buffer, name);

    return (buffer);
}

int mem_load_sys_file(const char *path, const char *name, BYTE *dest,
		      int minsize, int maxsize)
{
    FILE *fp = NULL;
    const char *bufp = NULL;
    char *paths[2];
    size_t rsize = 0;
    int	n, i;

    if (name == NULL || *name == '\0')
	return -1;

#ifdef __MSDOS__
    /* On MS-DOS, always load from the working directory or from the specified
       `path'. */
    n = 1;
    if (path != NULL)
	paths[0] = stralloc(path);
    else
	paths[0] = concat("./", app_resources.projectDir, NULL);
#else

    /* On other systems, if no `path' is specified, try to load from the
       current directory and, if not possible, try to boot from the LIBDIR. */
    if (path != NULL) {
	n = 1;
	paths[0] = stralloc(path);
    } else {
	n = 2;
	paths[0] = concat("./", app_resources.projectDir, NULL);
	paths[1] = concat(LIBDIR, "/", app_resources.projectDir, NULL);
    }
#endif

    for (i = 0; i < n; i++) {
	if ((bufp = create_name(paths[i], name)) == NULL)
	    return -2;
	if ((fp = fopen(bufp, READ)) == NULL)
	    /* perror(bufp)*/ ;
	else
	    break;
    }

    if (fp == NULL)
	return -1;

    {
	struct stat s;

	/* Check if the file is large enough before loading it. */
	if (fstat(fileno(fp), &s) == -1) {
	    perror(bufp);
	    return -1;
	}
	rsize = s.st_size - ftell(fp);

#if 0
	printf("ROM %s: size=%04x, minsize=%04x, maxsize=%04x romp=%p\n",
	       bufp, rsize, minsize, maxsize, dest);
#endif

	if (rsize < minsize) {
	    fprintf(stderr, "ROM %s: short file.\n", bufp);
	    return -1;
	}
	if (rsize == maxsize + 2) {
	    printf("ROM %s: two bytes too large - removing assumed start "
		   "address\n", bufp);
	    fread((char*)dest, 1, 2, fp);
	    rsize -= 2;
	}
	if (rsize < maxsize) {
#if 0
	    printf("ROM %s: short file, reading to the end\n", bufp);
#endif
	    dest += maxsize-rsize;
	} else if (rsize > maxsize) {
	    printf("ROM %s: long file, discarding end\n", bufp);
	    rsize = maxsize;
	}
	if ((rsize = fread((char *)dest, 1, rsize, fp)) < minsize)
	    return -1;
    }

    for (i=0; i < n; i++)
	free(paths[i]);

    (void) fclose(fp);

    return rsize;  /* return ok */
}

