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
#include "sysfile.h"

/* ------------------------------------------------------------------------- */

int mem_load_sys_file(const char *name, BYTE *dest, int minsize, int maxsize)
{
    FILE *fp = NULL;
    size_t rsize = 0;
    char *complete_path;

    fp = sysfile_open(name, &complete_path);
    if (fp == NULL)
        goto fail;

    printf("Loading file `%s'\n", complete_path);

    {
	struct stat s;

	/* Check if the file is large enough before loading it. */
	if (fstat(fileno(fp), &s) == -1) {
	    perror(complete_path);
            goto fail;
        }
	rsize = s.st_size - ftell(fp);

#if 0
	printf("ROM %s: size=%04x, minsize=%04x, maxsize=%04x romp=%p\n",
	       complete_path, rsize, minsize, maxsize, dest);
#endif

	if (rsize < minsize) {
	    fprintf(stderr, "ROM %s: short file.\n", complete_path);
            goto fail;
	}
	if (rsize == maxsize + 2) {
	    printf("ROM %s: two bytes too large - removing assumed start "
		   "address\n", complete_path);
	    fread((char*)dest, 1, 2, fp);
	    rsize -= 2;
	}
	if (rsize < maxsize) {
#if 0
	    printf("ROM %s: short file, reading to the end\n", complete_path);
#endif
	    dest += maxsize-rsize;
	} else if (rsize > maxsize) {
	    printf("ROM %s: long file, discarding end\n", complete_path);
	    rsize = maxsize;
	}
	if ((rsize = fread((char *)dest, 1, rsize, fp)) < minsize)
            goto fail;
    }

    (void) fclose(fp);
    free(complete_path);
    return rsize;  /* return ok */

fail:
    free(complete_path);
    return -1;
}

