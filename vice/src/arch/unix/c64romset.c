/*
 * c64romset.c - Locator for c64 RomSets.
 *
 * Written by
 *  Oliver Schaertel (orschaer@axis.de)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#endif

#include "c64romset.h"
#include "c64/c64mem.h"
#include "cmdline.h"
#include "archdep.h"
#include "utils.h"
#include "findpath.h"

/* ------------------------------------------------------------------------- */

/* Resources.  */

static char *default_path;

int c64romset_init(const char *emu_id)
{
    default_path = stralloc(archdep_default_sysfile_pathlist(emu_id));
    return 0;

}

int c64romset_load()
{
    const char * path = default_path;
    const char * s;
    DIR *dp;
    struct dirent *dirp;

    PATH_VAR(buf);

    for (s = path; s; path = s + 1) {
	int l;

	s = strchr(path, FINDPATH_SEPARATOR_CHAR);
	l = s? (s - path): strlen(path);
		
	strncpy(buf, path, l);
	buf[l] = '\0';
	if( (dp = opendir(buf)) == NULL ) return 0;
	while ( (dirp = readdir(dp)) != NULL) {
	    if (dirp->d_name[0] == '.') continue; 
	    if(!strncmp(dirp->d_name,"kernal-",7))
                mem_add_romset(dirp->d_name + 7);
	    else if (!strncmp(dirp->d_name,"basic-",6))
                mem_add_romset(dirp->d_name + 6);
	    else if (!strncmp(dirp->d_name,"chargen-",8))
                mem_add_romset(dirp->d_name + 8);
	    else if (!strncmp(dirp->d_name,"dos1541-",8))
                mem_add_romset(dirp->d_name + 8);
	}
	closedir(dp);
    }
    return 1;
}
