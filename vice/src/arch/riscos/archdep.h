/*
 * archdep.h - Architecture dependent defines.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#ifndef _ARCHDEFS_H
#define _ARCHDEFS_H

/* Name of the resource file.  */
#define RESOURCE_FILE_NAME "VICERC"

/* Filesystem dependant operators.  */
#define FSDEVICE_DEFAULT_DIR   "@"
#define FSDEV_DIR_SEP_STR      "."
#define FSDEV_DIR_SEP_CHR      '.'
#define FSDEV_EXT_SEP_STR      "/"
#define FSDEV_EXT_SEP_CHR      '/'

extern int   archdep_startup(int *argc, char **argv);
extern char *archdep_program_name(void);
extern FILE *archdep_open_default_log_file(void);
extern char *archdep_default_resource_file_name(void);
extern char *archdep_default_save_resource_file_name(void);
extern char *archdep_default_sysfile_pathlist(const char *emu_id);

extern int   archdep_num_text_lines(void);
extern int   archdep_num_text_columns(void);

int archdep_default_logger(const char *level_string, const char *format,
                                                                va_list ap);

/* Line editing */
extern char *readline(const char *prompt);
extern void add_history(const char *p);
/* Logfile */
extern FILE *open_logfile(const char *basename);

#endif
