/** \file   uidata.c
 * \brief   GTK3 binary resources handling
 *
 * Mostly a wrapper around the GResource API, which appears to be the way to
 * handle binary resources in Gtk3 applications.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * \note    WORK IN PROGRESS, leave this alone for now -- compyx
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
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "archdep.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "util.h"

#include "uidata.h"


/** \brief  Reference to the global GResource data
 */
static GResource *gresource = NULL;


/** \brief  Intialize the GResource binary blob handling
 *
 * First tries to load from src/arch/gtk3/data and then from VICEDIR/data.
 * Loading from VICEDIR/data will fail, the vice.gresource file doesn't get
 * copied there yet with a make install, nor does the VICEDIR/data dir exist.
 *
 * \return  bool
 */
bool uidata_init(void)
{
    GError *err = NULL;
#ifdef HAVE_DEBUG_GTK3UI
    char **files;
    int i;
#endif
    char *path, *dir = NULL;

    debug_gtk3("registering GResource data (trying trunk first)\n");
    gresource = g_resource_load(
            "src/arch/gtk3/data/vice.gresource", &err);
    if (gresource == NULL && err != NULL) {
        debug_gtk3("failed to load GResource data : %s\n", err->message);
        g_error_free(err);
#if 0
        debug_gtk3("trying $VICEDIR/data (%s/data)\n", VICEDIR);
        err = NULL;
        path = util_concat(VICEDIR, "/data/", "vice.geresource", NULL);
#else
        err = NULL;
        dir = archdep_get_vice_datadir();
        debug_gtk3("trying archdep_get_vice_datadir() (%s)\n", dir);
        path = util_concat(dir, "vice.geresource", NULL);
        lib_free(dir);
#endif
        gresource = g_resource_load(path, &err);
        if (gresource == NULL && err != NULL) {
            debug_gtk3("failed to load resource data '%s': %s\n",
                    path, err->message);
            g_error_free(err);
            lib_free(path);
            return false;
        }
    }
    g_resources_register(gresource);

    /* debugging: show files in the resource blob */
#ifdef HAVE_DEBUG_GTK3UI
    files = g_resource_enumerate_children(
            gresource,
            "/org/pokefinder/vice",
            G_RESOURCE_LOOKUP_FLAGS_NONE,
            &err);
    if (files == NULL && err != NULL) {
        debug_gtk3("couldn't enumerate children: %s\n", err->message);
        g_error_free(err);
        return false;
    }
    for (i = 0; files[i] != NULL; i++) {
        debug_gtk3("%d: %s\n", i, files[i]);
    }
#endif
    return true;
}


/** \brief  Clean up GResource data
 */
void uidata_shutdown(void)
{
    debug_gtk3("freeing GResource data\n");
    if (gresource != NULL) {
        g_free(gresource);
        gresource = NULL;
    }
}
