/** \file   archdep.c
 * \brief   Wrappers for architecture/OS-specific code
 *
 * I've decided to use GLib's use of the XDG specification and the standard
 * way of using paths on Windows. So some files may not be where the older
 * ports expect them to be. For example, vicerc will be in $HOME/.config/vice
 * now, not $HOME/.vice. -- compyx
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "debug_gtk3.h"
#include "findpath.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "util.h"
#include "uiapi.h"

/* Will get fixed once the code in this file gets moved to its proper location */
#include "../shared/archdep_xdg.h"
#include "../shared/archdep_defs.h"
#include "../shared/archdep_create_user_config_dir.h"
#include "../shared/archdep_join_paths.h"
#include "../shared/archdep_get_vice_docsdir.h"

#if 0
/** \brief  Prefix used for autostart disk images
 */
#define AUTOSTART_FILENAME_PREFIX   "autostart-"


/** \brief  Suffix used for autostart disk images
 */
#define AUTOSTART_FILENAME_SUFFIX   ".d64"
#endif

/** \brief  Reference to argv[0]
 *
 * FIXME: this is only used twice I think, better pass this as an argument to
 *        the functions using it
 */
static char *argv0 = NULL;


/** \brief  Path to the preferences directory of the emu
 */
const char *archdep_pref_path = NULL;


#ifdef UNIX_COMPILE
#include "archdep_unix.c"
#endif

#ifdef WIN32_COMPILE
#include "archdep_win32.c"
#endif


#if 0
/** \brief  Generate default autostart disk image path
 *
 * The path will be "$cfgdir/autostart-$emu.d64". this needs to be freed with
 * lib_free().
 *
 * \return  path to autostart disk image
 */
char *archdep_default_autostart_disk_image_file_name(void)
{
    char *cfg;
    gchar *path;
    char *name;
    char *tmp;

    cfg = archdep_user_config_path();
    name = util_concat(AUTOSTART_FILENAME_PREFIX, machine_get_name(),
            AUTOSTART_FILENAME_SUFFIX, NULL);
    path = g_build_path(path_separator, cfg, name, NULL);
    lib_free(name);
#if 0
    lib_free(cfg);
#endif
    /* transfer ownership from non/glib to VICE */
    tmp = lib_stralloc(path);
    g_free(path);
    return tmp;
}
#endif

#if 0
/** \brief  Open the default log file
 *
 * \return  file pointer to log file ("vice.log on Windows, stdout otherwise)
 */
FILE *archdep_open_default_log_file(void)
{
#ifdef WIN32_COMPILE
    /* inspired by the SDL port */
    char *cfg = archdep_user_config_path();
    gchar *fname = g_build_filename(cfg, "vice.log", NULL);
    FILE *fp = fopen(fname, "wt");

    g_free(fname);
# if 0
    lib_free(cfg);
# endif
    return fp;
#else
    return stdout;
#endif
}
#endif

/** \brief  Create and open temp file
 *
 * \param[in]   filename    pointer to object to store name of temp file
 * \param[in]   mode        mode to open file with (see fopen(3))
 *
 * \return  pointer to new file or `NULL` on error
 */
FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
    GError *err;
    /* this function already uses the OS's tmp dir as a prefix, so no need to
     * do stuff like getenv("TMP")
     */
    int fd = g_file_open_tmp("vice.XXXXXX", filename, &err);
    if (fd < 0) {
        return NULL;
    }
    return fdopen(fd, mode);
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
#ifdef HAVE_DEBUG_GTK3UI
    const char *prg_name;
    char *cfg_path;
    char *searchpath;
    char *vice_ini;
    char *datadir;
    char *docsdir;
# if defined(ARCHDEP_OS_LINUX) || defined(ARCHDEP_OS_BSD)
    char *xdg_cache;
    char *xdg_config;
    char *xdg_data;
# endif
#endif
    argv0 = lib_stralloc(argv[0]);

    /* set argv0 for program_name()/boot_path() calls (yes this sucks) */
    archdep_program_path_set_argv0(argv[0]);

    archdep_create_user_config_dir();

#ifdef HAVE_DEBUG_GTK3UI
    /* sanity checks, to remove later: */
    prg_name = archdep_program_name();
    searchpath = archdep_default_sysfile_pathlist(machine_name);
    cfg_path = archdep_user_config_path();
    vice_ini = archdep_default_resource_file_name();
    datadir = archdep_get_vice_datadir();
    docsdir = archdep_get_vice_docsdir();

# if defined(ARCHDEP_OS_LINUX) && defined(ARCHDEP_OS_BSD)
    xdg_cache = archdep_xdg_cache_home();
    xdg_config = archdep_xdg_config_home();
    xdg_data = archdep_xdg_data_home()l
# endif 

    debug_gtk3("program name    = \"%s\"", prg_name);
    debug_gtk3("user home dir   = \"%s\"", archdep_home_path());
    debug_gtk3("user config dir = \"%s\"", cfg_path);
    debug_gtk3("prg boot path   = \"%s\"", archdep_boot_path());
    debug_gtk3("VICE searchpath = \"%s\"", searchpath);
    debug_gtk3("VICE gui data   = \"%s\"", datadir);
    debug_gtk3("VICE docs path  = \"%s\"", docsdir);
    debug_gtk3("vice.ini path   = \"%s\"", vice_ini);

# if defined(ARCHDEP_OS_LINUX) || defined(ARCHDEP_OS_BSD)
    xdg_cache = archdep_xdg_cache_home();
    xdg_config = archdep_xdg_config_home();
    xdg_data = archdep_xdg_data_home();

    debug_gtk3("XDG_CACHE_HOME  = '%s'.", xdg_cache);
    debug_gtk3("XDG_CONFIG_HOME = '%s'.", xdg_config);
    debug_gtk3("XDG_DATA_HOME   = '%s'.", xdg_data);

    lib_free(xdg_cache);
    lib_free(xdg_config);
    lib_free(xdg_data);
# endif

    lib_free(searchpath);
    lib_free(vice_ini);
# if 0
    lib_free(cfg_path);
# endif
    lib_free(datadir);
    lib_free(docsdir);
#endif

    /* needed for early log control (parses for -silent/-verbose) */
    log_verbose_init(*argc, argv);

    return 0;
}


#if 0
/*
 * Font handling
 */

#ifdef UNIX_COMPILE
# ifdef HAVE_FONTCONFIG

#  include <fontconfig/fontconfig.h>


/** \brief    Try to register the CBM font with the OS
 *
 * This tries to use fontconfig on Unix, and uses GDI on windows.
 *
 * \return    bool as int
 */
int archdep_register_cbmfont(void)
{
    FcConfig *fc_config;
#ifdef HAVE_DEBUG_GTK3UI
    int fc_version;
#endif
    char *datadir;
    char *path;

    debug_gtk3("Initializing FontConfig.");
    if (!FcInit()) {
        debug_gtk3("failed\n");
        return 0;
    }
    debug_gtk3("OK\n");

#ifdef HAVE_DEBUG_GTK3UI
    fc_version = FcGetVersion();
#endif
    debug_gtk3("fontconfig version = %d.%d.%d.",
            fc_version / 10000, (fc_version % 10000) / 100, fc_version % 10);

    debug_gtk3("Loading font file.");
    fc_config = FcConfigGetCurrent();

    datadir = archdep_get_vice_datadir();
#if 0
    path = util_concat(datadir, "/../fonts/CBM.ttf", NULL);
#endif
    path = archdep_join_paths(datadir, "..", "fonts", "CBM.ttf", NULL);
    lib_free(datadir);

    debug_gtk3("Trying to load font '%s'.", path);

    if (!FcConfigAppFontAddFile(fc_config, (FcChar8 *)path)) {
        debug_gtk3("failed.");
        lib_free(path);
        return 0;
    }
    debug_gtk3("OK, font loaded.")

    lib_free(path);
    return 1;
}

# else     /* HAVE_FONTCONFIG */

int archdep_register_cbmfont(void)
{
    log_error(LOG_ERR, "no fontconfig support, sorry.");
    return 0;
}

# endif

#else   /* UNIX_COMPILE */

/*
 * Windows part of the API
 */
# include "windows.h"


/** \brief  Attempt to register the CBM font with the OS's font API
 *
 *
 * \return  bool
 */
int archdep_register_cbmfont(void)
{
    char *datadir;
    char *path;
    int result;

    debug_gtk3("Attempting to register font 'CBM.ttf");

    datadir = archdep_get_vice_datadir();
    path = util_concat(datadir, "\\CBM.ttf", NULL);
    lib_free(datadir);

    debug_gtk3("Adding font '%s", path);
    result = AddFontResourceEx(path, 0, 0);
    if (result == 0) {
        debug_gtk3("failed to add font");
        lib_free(path);
        return 0;
    }
    debug_gtk3("added %d fonts", result);
    lib_free(path);
    return 1;
}

#endif


/** \brief  Unregister the CBM font
 *
 * Seems like only on Windows this actually required.
 */
void archdep_unregister_cbmfont(void)
{
#ifdef WIN32_COMPILE

    char *datadir;
    char *path;

    datadir = archdep_get_vice_datadir();
    path = archdep_join_paths(datadir, "CBM.ttf", NULL);
    lib_free(datadir);

    RemoveFontResourceEx(path, 0, 0);
    lib_free(path);
#endif
}
#endif
