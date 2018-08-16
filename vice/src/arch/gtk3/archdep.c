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
#include <stdbool.h>
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


/** \brief  Prefix used for autostart disk images
 */
#define AUTOSTART_FILENAME_PREFIX   "autostart-"


/** \brief  Suffix used for autostart disk images
 */
#define AUTOSTART_FILENAME_SUFFIX   ".d64"


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
/** \brief  Get the program name
 *
 * This returns the final part of argv[0], as if basename were used.
 *
 * \return  program name, heap-allocated, free with lib_free()
 */
char *archdep_program_name(void)
{
    if (program_name == NULL) {
        program_name = lib_stralloc(g_path_get_basename(argv0));
    }
    return program_name;
}
#endif

#if 0
/** \brief  Get the user's home directory
 *
 * \return  current user's home directory
 */
const char *archdep_home_path(void)
{
    return g_get_home_dir();
}
#endif

#if 0
/** \brief  Get user configuration directory
 *
 * \return  heap-allocated string, free after use with lib_free()
 */
char *archdep_user_config_path(void)
{
    char *path;
    gchar *tmp = g_build_path(path_separator, g_get_user_config_dir(),
            VICEUSERDIR, NULL);
    /* transfer ownership of string from GLib to VICE */
    path = lib_stralloc(tmp);
    g_free(tmp);
    return path;
}
#endif

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


#if 0
/** \brief  Quote \a name for use as a parameter in exec() etc calls
 *
 * Surounds \a name with double-quotes and replaces brackets with escaped
 * versions on Windows, on Unix it simply returns a heap-allocated copy.
 * Still leaves the OSX unzip bug. (See bug #920)
 *
 * \param[in]   name    string to quote
 *
 * \return  quoted string
 */
char *archdep_quote_parameter(const char *name)
{
#ifdef WIN32_COMPILE
    char *a,*b,*c;

    a = util_subst(name, "[", "\\[");
    b = util_subst(a, "]", "\\]");
    c = util_concat("\"", b, "\"", NULL);
    lib_free(a);
    lib_free(b);
    return c;
#else
    return lib_stralloc(name);
#endif
}
#endif


/** \brief  Quote \a name with double quotes
 *
 * Taken from win32/archdep.c, seems Windows needs this, but it makes unzip etc
 * fail on proper systems.
 *
 * \param[in]   name    string to quote
 *
 * \return  quoted (win32 only) and heap-allocated copy of \a name
 */
char *archdep_filename_parameter(const char *name)
{
#ifdef WIN32_COMPILE
    char *path;
    char *result;

    archdep_expand_path(&path, name);
    result = util_concat("\"", path, "\"", NULL);
    lib_free(path);
    return result;
#else
    return lib_stralloc(name);
#endif
}


/** \brief  Generate path to the default fliplist file
 *
 * On Unix, this will return "$HOME/.config/vice/fliplist-$machine.vfl", on
 * Windows this should return "%APPDATA%\\vice\\fliplist-$machine.vfl".
 *
 * \return  path to defaul fliplist file, must be freed with lib_free()
 */
char *archdep_default_fliplist_file_name(void)
{
    gchar *path;
    char *name;
    char *tmp;

    name = util_concat("fliplist-", machine_get_name(), ".vfl", NULL);
    path = g_build_path(path_separator, g_get_user_config_dir(), VICEUSERDIR,
            name, NULL);
    lib_free(name);
    /* transfer ownership of path to VICE */
    tmp = lib_stralloc(path);
    g_free(path);
    return tmp;
}


/** \brief  Create path(s) used by VICE for user-data
 *
 */
static void archdep_create_user_config_dir(void)
{
    char *path = archdep_user_config_path();

    /*
     * cannot use the log here since it hasn't been created yet, not the
     * directory it's supposed to live in
     */
    debug_gtk3("creating user config dir '%s'", path);

    /* create config dir, fail silently if it exists */
    if (g_mkdir(path, 0755) == 0) {
        debug_gtk3("OK: created user config dir.");
    } else {
        if (errno == EEXIST) {
            debug_gtk3("OK: directory already existed.");
        } else {
            debug_gtk3("Error: %d: %s.", errno, strerror(errno));
        }
    }
    lib_free(path);
}


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
    lib_free(cfg);
    /* transfer ownership from non/glib to VICE */
    tmp = lib_stralloc(path);
    g_free(path);
    return tmp;
}


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
    lib_free(cfg);
    return fp;
#else
    return stdout;
#endif
}


/** \brief  Sanitize \a name by removing invalid characters for the current OS
 *
 * \param[in,out]   name    0-terminated string
 */
void archdep_sanitize_filename(char *name)
{
    while (*name != '\0') {
        int i = 0;
        while (illegal_name_tokens[i] != '\0') {
            if (illegal_name_tokens[i] == *name) {
                *name = '_';
                break;
            }
            i++;
        }
        name++;
    }
}


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

#if 0
/** \brief  Create directory \a pathname
 *
 * \param[in]   pathname    path/name of new directory
 * \param[in]   mode        ignored
 *
 * \return  0 on success, -1 on failure
 */
int archdep_mkdir(const char *pathname, int mode)
{
    return g_mkdir(pathname, mode); /* mode is ignored on Windows */
}
#endif

#if 0
/** \brief  Remove directory \a pathname
 *
 * \param[in]   pathname    directory to remove
 *
 * \return  0 on success, -1 on error
 */
int archdep_rmdir(const char *pathname)
{
    return g_rmdir(pathname);
}
#endif


/** \brief  Rename \a oldpath to \a newpath
 *
 * \param[in]   oldpath old path
 * \param[in]   newpath new path
 *
 * \return  0 on success, -1 on failure
 */
int archdep_rename(const char *oldpath, const char *newpath)
{
    return g_rename(oldpath, newpath);
}

#if 0
/** \brief  Log an error message
 *
 * \param[in]   format  format string
 */
void archdep_startup_log_error(const char *format, ...)
{
    char *tmp;
    va_list args;

    va_start(args, format);
    tmp = lib_mvsprintf(format, args);
    va_end(args);

    ui_error(tmp);
    lib_free(tmp);
}
#endif


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

    debug_gtk3("program name    = \"%s\"", prg_name);
    debug_gtk3("user home dir   = \"%s\"", archdep_home_path());
    debug_gtk3("user config dir = \"%s\"", cfg_path);
    debug_gtk3("prg boot path   = \"%s\"", archdep_boot_path());
    debug_gtk3("VICE searchpath = \"%s\"", searchpath);
    debug_gtk3("VICE gui data   = \"%s\"", datadir);
    debug_gtk3("VICE docs path  = \"%s\"", docsdir);
    debug_gtk3("vice.ini path   = \"%s\"", vice_ini);

    lib_free(searchpath);
    lib_free(vice_ini);
    lib_free(cfg_path);
    lib_free(datadir);
    lib_free(docsdir);
#endif

    /* needed for early log control (parses for -silent/-verbose) */
    log_verbose_init(*argc, argv);

    return 0;
}


/** \brief  Provide extra text for the application title
 *
 * Unused in Gtk3, used in SDL to generate a "Press F[10|12] for menu" message
 *
 * \return  NULL
 */
char *archdep_extra_title_text(void)
{
    return NULL;
}


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
 * \return    bool
 */
bool archdep_register_cbmfont(void)
{
    FcConfig *fc_config;
    int fc_version;
    char *datadir;
    char *path;

    debug_gtk3("Initializing FontConfig.");
    if (!FcInit()) {
        debug_gtk3("failed\n");
        return false;
    }
    debug_gtk3("OK\n");

    fc_version = FcGetVersion();
    debug_gtk3("fontconfig version = %d.%d.%d.",
            fc_version / 10000, (fc_version % 10000) / 100, fc_version % 10);

    debug_gtk3("Loading font file.");
    fc_config = FcConfigGetCurrent();

    datadir = archdep_get_vice_datadir();
    path = util_concat(datadir, "/../fonts/CBM.ttf", NULL);
    lib_free(datadir);

    debug_gtk3("Trying to load font '%s'.", path);

    if (!FcConfigAppFontAddFile(fc_config, (FcChar8 *)path)) {
        debug_gtk3("failed.");
        lib_free(path);
        return false;
    }
    debug_gtk3("OK, font loaded.")

    lib_free(path);
    return true;
}

# else     /* HAVE_FONTCONFIG */

bool archdep_register_cbmfont(void)
{
    log_error(LOG_ERR, "no fontconfig support, sorry.");
    return false;
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
bool archdep_register_cbmfont(void)
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
        return FALSE;
    }
    debug_gtk3("added %d fonts", result);
    lib_free(path);
    return TRUE;
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
    path = util_concat(datadir, "/CBM.ttf", NULL);
    lib_free(datadir);

    RemoveFontResourceEx(path, 0, 0);
    lib_free(path);
#endif
}
