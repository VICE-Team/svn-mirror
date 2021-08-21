/** \file   hotkeys.c
 * \brief   Gtk3 custom hotkeys handling
 *
 * Provides custom keyboard shortcuts for the Gtk3 UI.
 *
 * (
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
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "archdep.h"
#include "debug_gtk3.h"
#include "cmdline.h"
#include "lib.h"
#include "resources.h"
#include "uimachinemenu.h"
#include "util.h"

#include "hotkeys.h"


/*
 * Forward declarations
 */

static int hotkey_file_set(const char *val, void *param);


/** \brief  Path to current hotkey file
 *
 * \note    Free with hotkeys_shutdown() on emulator shutdown.
 */
static char *hotkey_file = NULL;


/** \brief  Default hotkey file name
 *
 * \note    Free with hotkeys_shutdown() on emulator shutdown.
 */
static char *hotkey_file_default = NULL;


/** \brief  String type resources
 *
 * \note    Make sure "HotkeyFile" remains the first element in the list, the
 *          init code sets the factory value during runtime, using array index
 *          0.
 */
static resource_string_t resources_string[] = {
    { "HotkeyFile", NULL, RES_EVENT_NO, NULL,
      &hotkey_file, hotkey_file_set, NULL },
    RESOURCE_STRING_LIST_END
};


/** \brief  List of command line options
 */
static const cmdline_option_t cmdline_options[] = {
    { "-hotkeyfile", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "HotkeyFile", NULL,
      "<name>", "Specify name of hotkey file" },
    CMDLINE_LIST_END
};


/*
 * Resource getters/setters
 */

/** \brief  Set hotkey file and process its contents
 *
 * \param[in]   val     new hotkey file
 * \param[in]   param   extra argument (unused)
 *
 * \return  0 on success, -1 on error
 */
static int hotkey_file_set(const char *val, void *param)
{
    debug_gtk3("Got hotkey file '%s'", val);

    if (util_string_set(&hotkey_file, val) != 0) {
        /* new value was the same as the old value, don't do anything */
        return 0;
    }

    /* process hotkeys */
#if 0
    debug_gtk3("Faking hotkeys loading: swapping Alt+H/Alt+M.");
    ui_set_vice_menu_item_hotkey_by_name("monitor", "m", GDK_MOD1_MASK);
    ui_set_vice_menu_item_hotkey_by_name("toggle-mouse-grab", "h", GDK_MOD1_MASK);
#endif
    return 0;
}


/** \brief  Initialize resources used by the custom hotkeys
 *
 * \return  0 on success
 */
int hotkeys_resources_init(void)
{
    /* set the default filename */
    hotkey_file_default = archdep_default_hotkey_file_name();
    resources_string[0].factory_value = hotkey_file_default;

    /* register the resources */
    return resources_register_string(resources_string);
}


/** \brief  Initialize command line options used by the custom hotkeys
 *
 * \return  0 on success
 */
int hotkeys_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


/** \brief  Clean up resources used by the hotkeys
 *
 * \note    Call on emulator shutdown
 */
void hotkeys_shutdown(void)
{
    if (hotkey_file != NULL) {
        lib_free(hotkey_file);
        hotkey_file = NULL;
    }
    if (hotkey_file_default != NULL) {
        lib_free(hotkey_file_default);
        hotkey_file_default = NULL;
    }
}
