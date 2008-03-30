/*
 * archapi.h - Common system-specific API.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

/* Do not include this header file, include `archdep.h' instead.  */

#ifndef _ARCHAPI
#define _ARCHAPI

#include <stdarg.h>

extern int archdep_startup(int *argc, char **argv);

extern const char *archdep_program_name(void);
extern const char *archdep_boot_path(void);
extern const char *archdep_default_sysfile_pathlist(const char *emu_id);
extern int archdep_path_is_relative(const char *path);
extern int archdep_expand_path(char **return_path, const char *filename);

/* Resource handling.  */
extern const char *archdep_default_resource_file_name(void);
extern const char *archdep_default_save_resource_file_name(void);

/* Logfile stuff.  */
extern FILE *archdep_open_default_log_file(void);
extern int archdep_default_logger(const char *level_string, const char *format,
                                  va_list ap);

/* Launch program `name' (searched via the PATH environment variable)
   passing `argv' as the parameters, wait for it to exit and return its
   exit status. If `stdout_redir' or `stderr_redir' are != NULL,
   redirect stdout or stderr to the corresponding file.  */
extern int archdep_spawn(const char *name, char **argv,
                         const char *stdout_redir, const char *stderr_redir);

extern int archdep_num_text_lines(void);
extern int archdep_num_text_columns(void);

extern void archdep_setup_signals(int do_core_dumps);

#endif

