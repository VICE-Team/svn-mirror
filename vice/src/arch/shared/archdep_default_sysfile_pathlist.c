/** \file   archdep_default_sysfile_pathlist.c
 * \brief   Get a list of paths of required data files
 */

/*
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
#include "archdep_defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "lib.h"
#include "log.h"
#include "util.h"

#include "archdep_boot_path.h"
#include "archdep_get_vice_datadir.h"
#include "archdep_join_paths.h"
#include "archdep_user_config_path.h"
#include "archdep_xdg.h"


#include "archdep_default_sysfile_pathlist.h"

/** \brief  Total number of pathnames to store in the pathlist
 *
 * 16 seems to be enough, but it can always be increased to support more.
 */
#define TOTAL_PATHS 16


/** \brief  Reference to the sysfile pathlist
 *
 * This keeps a copy of the generated sysfile pathlist so we don't have to
 * generate it each time it is needed.
 */
static char *sysfile_path = NULL;


/** \brief  Generate a list of search paths for VICE system files
 *
 * \param[in]   emu_id  emulator ID (ie 'C64 or 'VSID')
 *
 * \return  heap-allocated string, to be freed by the caller
 */
char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    const char *boot_path = NULL;
    char *datadir = NULL;
#if !defined(ARCHDEP_OS_WINDOWS) && !defined(ARCHDEP_OS_BEOS)
    char *home_path = NULL;
#endif

    char *datadir_root = NULL;
    char *datadir_machine_roms = NULL;
    char *datadir_drive_roms = NULL;
    char *datadir_printer_roms = NULL;
    char *boot_root = NULL;
    char *boot_machine_roms = NULL;
    char *boot_drive_roms = NULL;
    char *boot_printer_roms = NULL;
    char *home_root = NULL;
    char *home_machine_roms = NULL;
    char *home_drive_roms = NULL;
    char *home_printer_roms = NULL;

    const char *paths[TOTAL_PATHS + 1];
    int i;


    if (sysfile_path != NULL) {
        /* sysfile.c appears to free() this */
        return lib_strdup(sysfile_path);
    }

    boot_path = archdep_boot_path();
    datadir = archdep_get_vice_datadir();
#if !defined(ARCHDEP_OS_WINDOWS) && !defined(ARCHDEP_OS_BEOS)

# ifdef USE_NATIVE_GTK3
    char *xdg_home = archdep_xdg_data_home();
    home_path = archdep_join_paths(xdg_home, "vice", NULL);
    lib_free(xdg_home);
# else
    home_path = archdep_user_config_path();
# endif
#endif

    /* zero out the array of paths to join later */
    for (i = 0; i <= TOTAL_PATHS; i++) {
        paths[i] = NULL;
    }


#ifdef ARCHDEP_OS_UNIX

    datadir_machine_roms = archdep_join_paths(datadir, emu_id, NULL);
    datadir_drive_roms = archdep_join_paths(datadir, "DRIVES", NULL);
    datadir_printer_roms = archdep_join_paths(datadir, "PRINTER", NULL);

    boot_machine_roms = archdep_join_paths(boot_path, emu_id, NULL);
    boot_drive_roms = archdep_join_paths(boot_path, "DRIVES", NULL);
    boot_printer_roms = archdep_join_paths(boot_path, "PRINTER", NULL);

    /* home path based paths */
    home_machine_roms = archdep_join_paths(home_path, emu_id, NULL);
    home_drive_roms = archdep_join_paths(home_path, "DRIVES", NULL);
    home_printer_roms = archdep_join_paths(home_path, "PRINTER", NULL);

#elif defined(ARCHDEP_OS_WINDOWS) || defined(ARCHDEP_OS_OS2) \
    || defined(ARCHDEP_OS_MSDOS)
    boot_machine_roms = archdep_join_paths(boot_path, emu_id, NULL);
    boot_drive_roms = archdep_join_paths(boot_path, "DRIVES", NULL);
    boot_printer_roms = archdep_join_paths(boot_path, "PRINTER", NULL);
#if 0
    home_machine_roms = archdep_join_paths(home_path, emu_id, NULL);
    home_drive_roms = archdep_join_paths(home_path, "DRIVES", NULL);
    home_printer_roms = archdep_join_paths(home_path, "PRINTER", NULL);
#endif
#elif defined(ARCHDEP_OS_AMIGA) || defined(ARCHDEP_OS_BEOS)
    boot_machine_roms = archdep_join_paths(boot_path, emu_id, NULL);
    boot_drive_roms = archdep_join_paths(boot_path, "DRIVES", NULL);
    boot_printer_roms = archdep_join_paths(boot_path, "PRINTER", NULL);
#endif
    /* now join everything together */
    i = 0;

    /* VICE_DATADIR paths */
    if (datadir_root != NULL) {
        paths[i++] = datadir_root;
    }
    if (datadir_machine_roms != NULL) {
        paths[i++] = datadir_machine_roms;
    }
    if (datadir_drive_roms != NULL) {
        paths[i++] = datadir_drive_roms;
    }
    if (datadir_printer_roms != NULL) {
        paths[i++] = datadir_printer_roms;
    }
    /* boot paths */
    if (boot_root != NULL) {
        paths[i++] = boot_root;
    }
    if (boot_machine_roms != NULL) {
        paths[i++] = boot_machine_roms;
    }
    if (boot_drive_roms != NULL) {
        paths[i++] = boot_drive_roms;
    }
    if (boot_printer_roms != NULL) {
        paths[i++] = boot_printer_roms;
    }

    /* home paths */
    if (home_root != NULL) {
        paths[i++] = home_root;
    }
    if (home_machine_roms != NULL) {
        paths[i++] = home_machine_roms;
    }
    if (home_drive_roms != NULL) {
        paths[i++] = home_drive_roms;
    }
    if (home_printer_roms != NULL) {
        paths[i++] = home_printer_roms;
    }

    /* terminate list */
    paths[i] = NULL;
    sysfile_path = util_strjoin(paths, ARCHDEP_FINDPATH_SEPARATOR_STRING);

    /* cleanup */
    if (datadir != NULL) {
        lib_free(datadir);
    }

    if (datadir_root != NULL) {
        lib_free(datadir_root);
    }
    if (datadir_machine_roms != NULL) {
        lib_free(datadir_machine_roms);
    }
    if (datadir_drive_roms != NULL) {
        lib_free(datadir_drive_roms);
    }
    if (datadir_printer_roms != NULL) {
        lib_free(datadir_printer_roms);
    }
    /* boot paths */
    if (boot_root != NULL) {
        lib_free(boot_root);
    }
    if (boot_machine_roms != NULL) {
        lib_free(boot_machine_roms);
    }
    if (boot_drive_roms != NULL) {
        lib_free(boot_drive_roms);
    }
    if (boot_printer_roms != NULL) {
        lib_free(boot_printer_roms);
    }

    /* home paths */
    if (home_root != NULL) {
        lib_free(home_root);
    }
    if (home_machine_roms != NULL) {
        lib_free(home_machine_roms);
    }
    if (home_drive_roms != NULL) {
        lib_free(home_drive_roms);
    }
    if (home_printer_roms != NULL) {
        lib_free(home_printer_roms);
    }
    if (home_path != NULL) {
        lib_free(home_path);
    }

#if 0
    log_message(LOG_DEFAULT, "Search path = %s", sysfile_path);
    printf("%s(): paths = '%s'\n", __func__, sysfile_path);
#endif
    /* sysfile.c appears to free() this (ie TODO: fix sysfile.c) */
    return lib_strdup(sysfile_path);
}


/** \brief  Free the internal copy of the sysfile pathlist
 *
 * Call on emulator exit
 */

void archdep_default_sysfile_pathlist_free(void)
{
    if (sysfile_path != NULL) {
        lib_free(sysfile_path);
        sysfile_path = NULL;
    }
}
