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

#include "ui.h"      // ui_drive_enable_t

#ifdef __IBMC__
#include <stdarg.h>
#endif

/* Filesystem dependant operators.  */
#define FSDEVICE_DEFAULT_DIR   "."
#define FSDEV_EXT_SEP_STR      "."
#define FSDEV_EXT_SEP_CHR      '.'
#ifdef __EMX__
#define FSDEV_DIR_SEP_STR      "/"
#define FSDEV_DIR_SEP_CHR      '/'
#else
#define FSDEV_DIR_SEP_STR      "\\"
#define FSDEV_DIR_SEP_CHR      '\\'
#endif

/* Path separator.  */
#define FINDPATH_SEPARATOR_CHAR         ';'
#define FINDPATH_SEPARATOR_STRING       ";"

/* Modes for fopen().  */
#define MODE_READ              "rb"
#define MODE_READ_TEXT         "r"
#define MODE_READ_WRITE        "rb+"
#define MODE_WRITE             "wb"
#define MODE_WRITE_TEXT        "w"
#define MODE_APPEND            "wb+"
#define MODE_APPEND_READ_WRITE "ab+"

/* ------------------------------------------------------------------------- */
//void ui_display_speed(float spd, float fps, int sec);
//void ui_display_drive_led(int drive_number, int status);
//void ui_display_drive_track(int drive_number, double track_number);
//void ui_enable_drive_status(int state, int *drive_led_color);

// -------------------------- archdependent stuff -----------------------

int archdep_startup(int *argc, char **argv);
const char *archdep_program_name(void);
char *archdep_boot_path(void);
const char *archdep_default_sysfile_pathlist(const char *emu_id);
const char *archdep_default_resource_file_name(void);
const char *archdep_default_save_resource_file_name(void);
FILE *archdep_open_default_log_file(void);
int archdep_num_text_lines(void);
int archdep_num_text_columns(void);
int archdep_default_logger(const char *level_string, const char *format,
                                                                va_list ap);

void archdep_setup_signals(int do_core_dumps);
int archdep_path_is_relative(const char *path);

extern char *archdep_make_backup_filename(const char *fname);
extern int archdep_expand_path(char **return_path, const char *filename);

/* Launch program `name' (searched via the PATH environment variable)
   passing `argv' as the parameters, wait for it to exit and return its
   exit status. If `stdout_redir' or `stderr_redir' are != NULL,
   redirect stdout or stderr to the corresponding file.  */
extern int archdep_spawn(const char *name, char **argv,
                         const char *stdout_redir, const char *stderr_redir);

/* Monitor console functions.  */
extern void archdep_open_monitor_console(FILE **mon_input, FILE **mon_output);
extern void archdep_close_monitor_console(FILE *mon_input, FILE *mon_output);


/*void closeStatus();
void openStatus();
void PM_close();
void PM_open();*/

#endif

