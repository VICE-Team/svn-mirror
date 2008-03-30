/*
 * romset.h - romset file handling
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

#ifndef _ROMSET_H
#define _ROMSET_H

extern int romset_load(const char *filename);
extern int romset_dump(const char *filename, const char **resource_list);

extern int romset_load_archive(const char *filename);
extern int romset_dump_archive(const char *filename);
extern int romset_select(const char *romset_name);
extern void romset_clear(void);
extern int romset_get_number(void);
extern char *romset_get_item(int number);

#endif				/* _ROMSET_H */

