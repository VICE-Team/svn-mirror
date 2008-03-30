/*
 * archdep.h - Miscellaneous system-specific stuff.
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

#ifndef _ARCHDEP_H
#define _ARCHDEP_H

#include <stdio.h>
#include <stdarg.h>

/* Filesystem dependant operators.  */
#define FSDEVICE_DEFAULT_DIR   "."
#define FSDEV_DIR_SEP_STR      "/"
#define FSDEV_DIR_SEP_CHR      '/'
#define FSDEV_EXT_SEP_STR      "."
#define FSDEV_EXT_SEP_CHR      '.'

/* Modes for fopen().  */
#define MODE_READ   "r"
#define MODE_WRITE  "w"
#define MODE_APPEND "w+"

/* Define the default system directory (where the ROMs are).  */
#define LIBDIR          PREFIX "/lib/vice"
#define DOCDIR          LIBDIR "/doc"
#define VICEUSERDIR     ".vice"

int archdep_startup(int *argc, char **argv);
const char *archdep_program_name(void);
const char *archdep_boot_path(void);
const char *archdep_home_path(void);
const char *archdep_default_sysfile_pathlist(const char *emu_id);
const char *archdep_default_resource_file_name(void);
const char *archdep_default_save_resource_file_name(void);
int archdep_default_logger(const char *level_string, const char *format, 
                           va_list ap);
FILE *archdep_open_default_log_file(void);
int archdep_num_text_lines(void);
int archdep_num_text_columns(void);

int archdep_path_is_relative(const char *path);

void archdep_setup_signals(int do_core_dumps);

#endif

