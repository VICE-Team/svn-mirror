/** \file   src/arch/gtk3/widgets/uicart.c
 * \brief   Widget to attach carts
 *
 * Written by
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
#include <gtk/gtk.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "cartimagewidget.h"
#include "filechooserhelpers.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "cartridge.h"
#include "vsync.h"

#include "uicart.h"


static int  (*crt_detect_func)(const char *filename) = NULL;
static int  (*crt_attach_func)(int type, const char *filename) = NULL;
static void (*crt_freeze_func)(void) = NULL;
static void (*crt_detach_func)(int type) = NULL;


/** \brief  Set function to detect a cartridge's type
 *
 * Appears to be CBM2/Plus4 only
 *
 * \param[in]   func    detect function
 */
void uicart_set_detect_func(int (*func)(const char *))
{
    crt_detect_func = func;
}


/** \brief  Set function to attach a cartridge image
 *
 * \param[in]   func    attach function
 */
void uicart_set_attach_func(int (*func)(int, const char *))
{
    crt_attach_func = func;
}


/** \brief  Set function to trigger a cartridge freeze-button click
 *
 * \param[in]   func    freeze function
 */
void uicart_set_freeze_func(void (*func)(void))
{
    crt_freeze_func = func;
}


/** \brief  Set function to detach a/all cartridges
 *
 * \param[in]   func    freeze function
 */
void uicart_set_detach_func(void (*func)(int))
{
    crt_detach_func = func;
}


/** \brief  Try to smart-attach a cartridge image
 *
 * \param[in]   widget      parent widget (unused)
 * \param[in]   user_data   extra event data (unused)
 *
 * \return  TRUE
 */
gboolean uicart_smart_attach_dialog(GtkWidget *widget, gpointer user_data)
{
    vsync_suspend_speed_eval();

    gchar *filename;

    filename = ui_open_file_dialog(
            widget,
            "Smart-attach cartridge image",
            "Cartridge images",
            file_chooser_pattern_cart,
            NULL);

    if (filename != NULL) {
        debug_gtk3("Got filename '%s'\n", filename);
        if (crt_attach_func != NULL) {
            if (crt_attach_func(CARTRIDGE_CRT, filename) < 0) {
                ui_message_error(widget, "VICE error",
                        "Failed to attach '%s' as a cartridge image",
                        filename);
            } else {
                debug_gtk3("Attached '%s' as valid cartridge image\n", filename);
            }
        }
        g_free(filename);
    }

    return TRUE;
}


/** \brief  Trigger cartridge freeze
 *
 * Called from the menu
 *
 *
 * \return  TRUE
 */
gboolean uicart_trigger_freeze(void)
{
    if (crt_freeze_func != NULL) {
        debug_gtk3("triggering cart freeze\n");
        crt_freeze_func();
    }
    return TRUE;
}


/** \brief  Detach all cartridge images
 *
 * \return  TRUE
 */
gboolean uicart_detach(void)
{
    if (crt_detach_func != NULL) {
        debug_gtk3("detaching latest cartridge image\n");
        crt_detach_func(-1);
    }
    return TRUE;
}
