/** \file   archdep_cbmfont.c
 * \brief   CBM font handling
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
#include "debug_gtk3.h"
#include "archdep_defs.h"
#include "archdep_join_paths.h"
#include "archdep_get_vice_datadir.h"
#include "lib.h"

#include "archdep_cbmfont.h"

#ifdef ARCHDEP_OS_UNIX
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
    path = archdep_join_paths(datadir, "CBM.ttf", NULL);
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
 * Seems like only on Windows this is actually required.
 */
void archdep_unregister_cbmfont(void)
{
#ifdef ARCHDEP_OS_WINDOWS

    char *datadir;
    char *path;

    datadir = archdep_get_vice_datadir();
    path = archdep_join_paths(datadir, "CBM.ttf", NULL);
    lib_free(datadir);

    RemoveFontResourceEx(path, 0, 0);
    lib_free(path);
#endif
}
