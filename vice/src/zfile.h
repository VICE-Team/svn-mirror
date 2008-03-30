/*
 * zfile.h - Transparent handling of compressed files.
 *
 * Written by
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

#ifndef _ZFILE_H
#define _ZFILE_H

#include <stdio.h>

/* actions to be done when a zfile is closed */
typedef enum {
	ZFILE_KEEP,		/* nothing, keep original file (default) */
	ZFILE_REQUEST,		/* request the user what to do */
	ZFILE_DEL		/* remove original file */
} zfile_action_t;

FILE *zfopen(const char *name, const char *mode);
int zfclose(FILE *stream);
int zclose_all(void);

int zfile_close_action(const char *filename, zfile_action_t action,
				const char *request_string);

#endif /* _ZFILE_H */

