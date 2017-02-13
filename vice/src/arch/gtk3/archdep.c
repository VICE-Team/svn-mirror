/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Bas Wassink <b.wassink@ziggo.nl>
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
#include <glib.h>

#include "log.h"
#include "lib.h"


static char *argv0 = NULL;

#ifdef UNIX_COMPILE
#include "archdep_unix.c"
#endif

#ifdef WIN32_COMPILE
#include "archdep_win32.c"
#endif


/** \brief  Initialize the UI, a stub for now
 *
 * Theoretically, it should not have to matter what system we run on, as long
 * as it has Gtk3.
 */
static void archdep_ui_init(int arg, char **argv)
{
    /* do nothing, just like in src/arch/x11/gnome/x11ui.c */
}


/** \brief  Get the program name
 *
 * This returns the final part of argv[0], as if basename where used.
 *
 * \return  program name, heap-allocated, free with lib_free()
 */
char *archdep_program_name(void)
{
    return lib_stralloc(g_path_get_basename(argv0));

}


/** \brief  Get the absolute path to the VICE dir
 *
 * \return  Path to VICE's directory
 */
const gchar *archdep_boot_path(void)
{
    const char *boot;
    char *prg_name = archdep_program_name();

    boot = g_path_get_dirname(g_find_program_in_path(prg_name));
    lib_free(prg_name);
    return boot;
}


/** \brief  Get the user's home directory
 *
 * \return  current user's home directory
 */
const gchar *archdep_home_path(void)
{
    return g_get_home_dir();
}


/** \brief  Determine if \a path is an absolute path
 *
 * \param[in]   path    some path
 *
 * \return  bool
 */
int archdep_path_is_relative(const char *path)
{
    return !g_path_is_absolute(path);
}



/** \brief  Arch-dependent init
 *
 * \param[in]   argc    pointer to argument count
 * \param[in]   argv    argument vector
 *
 * \return  0
 */
int archdep_init(int *argc, char **argv)
{
    char *prg_name;

    argv0 = lib_stralloc(argv[0]);

    /* sanity check, to remove later: */
    prg_name = archdep_program_name();
    printf("prg name = \"%s\"\n", prg_name);
    printf("user home dir = \"%s\"\n", archdep_home_path());
    printf("prg boot path = \"%s\"\n", archdep_boot_path());
    lib_free(prg_name);

    /* needed for early log control (parses for -silent/-verbose) */
    log_verbose_init(*argc, argv);

    /* initialize the UI */
    archdep_ui_init(*argc, argv);
    return 0;
}

