/** \file   fontapi.c
 * \brief   Custom font handling, using the CBM font
 *
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

#include <gtk/gtk.h>
#include <stdbool.h>
#include <errno.h>
#include "debug_gtk3.h"
#include "lib.h"
#include "util.h"
#include "archdep.h"

#include "fontapi.h"

#ifdef HAVE_FONTCONFIG

#include <fontconfig/fontconfig.h>


/** \brief    Try to register the CBM font with fontconfig
 *
 * \return    bool
 */
bool fontapi_register_cbmfont_with_fc(void)
{
    FcConfig *fc_config;
    int fc_version;
    char *datadir;
    char *path;

    debug_gtk3("Initializing FontConfig\n");
    if (!FcInit()) {
        debug_gtk3("failed\n");
        return false;
    }
    debug_gtk3("OK\n");

    fc_version = FcGetVersion();
    debug_gtk3("fontconfig version = %d.%d.%d\n",
            fc_version / 10000, (fc_version % 10000) / 100, fc_version % 10);

    debug_gtk3("Loading font file\n");
    fc_config = FcConfigGetCurrent();

    datadir = archdep_get_vice_datadir();
    /*
     * Work around the fact that Windows' bindist script doesn't create the
     * fonts dir, nor the gui dir
     */
#ifdef WINDOWS_COMPILE
    path = util_concat(datadir, "\\..\\html\\fonts\\CBM.ttf", NULL);
#else
    path = util_concat(datadir, "/../fonts/CBM.ttf", NULL);
#endif
    lib_free(datadir);

    debug_gtk3("Trying to load font '%s'\n", path);

    if (!FcConfigAppFontAddFile(fc_config, (FcChar8 *)path)) {
        debug_gtk3("failed\n");
        lib_free(path);
        return false;
    }
    debug_gtk3("OK, font loaded.\n")

    lib_free(path);
    return true;
}

#else

bool fontapi_register_cbmfont_with_fc(void)
{
    log_error(LOG_ERR, "no fontconfig support, sorry\n");
    return false;
}

#endif


