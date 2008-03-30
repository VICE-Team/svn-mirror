/*
 * romset.c - romset file handling
 *
 * Written by
 *  Andre Fachat        (a.fachat@physik.tu-chemnitz.de)
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

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "types.h"
#include "mem.h"
#include "resources.h"
#include "sysfile.h"
#include "log.h"

extern int romset_load(const char *filename) {
    FILE *fp;
    int retval, line_num;
    int err = 0;

    fp = sysfile_open(filename, NULL);

    if(!fp) {
        log_warning(LOG_DEFAULT, "Could not open file '%s' for writing (%s)!", 
		filename,strerror(errno));    
	return -1;
    }

    log_message(LOG_DEFAULT, "Loading ROM set from file '%s'",filename);

    line_num = 0;
    do {
        retval = resources_read_item_from_file(fp);
        if (retval == -1) {
            log_error(LOG_DEFAULT,
                      "%s: Invalid resource specification at line %d.",
                      filename, line_num);
            err = 1;
        }
        line_num++;
    } while (retval != 0);
    fclose(fp);

    mem_load(); 

    return err;
}

extern int romset_dump(const char *filename, const char **resource_list) {
    va_list args;
    FILE *fp;
    char *s;

    fp = fopen(filename, "w");
    if (fp) {
        log_message(LOG_DEFAULT, "Dumping ROM set to file '%s'",filename);
	s = *resource_list++;
	while(s) {
	    resources_write_item_to_file(fp, s);
	    s = *resource_list++;
	} 
	fclose(fp);
	return 0;
    }
    log_warning(LOG_DEFAULT, "Could not open file '%s' for writing (%s)!", 
		filename,strerror(errno));
    return -1;
}

