/*
 * uisettings.c - GTK3 settings dialog
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
 */


/* The settings_grid is supposed to become this:
 *
 * +--------------+---------------------------+
 * | treeview     |                           |
 * |  with        |                           |
 * |   settings   |    central widget,        |
 * |  more        |    depending on which     |
 * |   foo        |    item is selected in    |
 * |   bar        |    the treeview           |
 * |    whatever  |                           |
 * | burp         |                           |
 * +--------------+---------------------------+
 *
 * And this is handled by the dialog itself:
 * +------------------------------------------+
 * | load | save | load... | save... | close  |
 * +------------------------------------------+
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "vsync.h"

#include "debug_gtk3.h"
#include "resourcecheckbutton.h"
#include "widgethelpers.h"
#include "openfiledialog.h"
#include "savefiledialog.h"

#include "ui.h"
#include "settings_speed.h"
#include "uikeyboard.h"
#include "settings_sound.h"
#include "settings_autostart.h"
#include "uidrivesettings.h"
#include "uimodel.h"
#include "uimisc.h"
#include "ramresetwidget.h"
#include "settings_video.h"
#include "uisamplersettings.h"
#include "uiprintersettings.h"
#include "uicontrolport.h"
#include "uijoystick.h"
#include "uimousesettings.h"
#include "uisoundchipsettings.h"

/* I/O extension widgets */
#include "ioextensionswidget.h"
#include "c64memoryexpansionhackswidget.h"
#include "georamwidget.h"
#include "reuwidget.h"
#include "ramcartwidget.h"
#include "dqbbwidget.h"
#include "expertwidget.h"
#include "isepicwidget.h"
#include "easyflashwidget.h"
#include "gmod2widget.h"
#include "mmcrwidget.h"
#include "mmc64widget.h"
#include "ide64widget.h"
#include "retroreplaywidget.h"
#include "ethernetcartwidget.h"
#include "rrnetmk3widget.h"
#include "c128functionromwidget.h"
#include "ieee488widget.h"
#include "digimaxwidget.h"
#include "magicvoicewidget.h"
#include "midiwidget.h"
#include "sfxsoundexpanderwidget.h"
#include "ds12c887widget.h"
#include "userportdeviceswidget.h"
#include "tapeportdeviceswidget.h"
#include "sidcartwidget.h"
#include "v364speechwidget.h"
#include "sfxsoundsamplerwidget.h"
#include "megacartwidget.h"
#include "petreuwidget.h"
#include "petcolourgraphicswidget.h"
#include "petdwwwidget.h"
#include "supersnapshotwidget.h"
#include "cpmwidget.h"
#include "burstmodewidget.h"
#include "c128fullbankswidget.h"
#include "plus4aciawidget.h"
#include "plus4digiblasterwidget.h"
#include "finalexpansionwidget.h"
#include "vicflashwidget.h"
#include "ultimemwidget.h"
#include "vicieee488widget.h"
#include "vicioramwidget.h"
#include "vfliwidget.h"
#include "petdiagpinwidget.h"
#include "pethrewidget.h"

#include "snapshotwidget.h"
#include "monitorsettingswidget.h"
#include "romsetwidget.h"

#include "uisettings.h"


/** \brief  Number of columns in the tree model
 */
#define NUM_COLUMNS 3


/** \brief  Column indici for the tree model
 */
enum {
    COLUMN_NAME = 0,    /**< name */
    COLUMN_ID,          /**< id */
    COLUMN_CALLBACK     /**< callback function */
};


#define DIALOG_WIDTH 800
#define DIALOG_HEIGHT 560


#define DIALOG_WIDTH_MAX 1024
#define DIALOG_HEIGHT_MAX 640


/** \brief  Enum used for the "response" callback of the settings dialog
 *
 * All values must be positive since Gtk reserves standard responses in its
 * GtkResponse enum as negative values.
 */
enum {
    RESPONSE_LOAD = 1,  /**< "Load" -> load settings from default file */
    RESPONSE_SAVE,      /**< "Save" -> save settings from default file */
    RESPONSE_LOAD_FILE, /**< "Load ..." -> load settings via dialog */
    RESPONSE_SAVE_FILE  /**< "Save ..." -> save settings via dialog */
};


/** \brief  List of C64 I/O extensions (x64, x64sc)
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t c64_io_extensions[] = {
    { "Memory Expansion Hacks",
        "mem-hacks",
        c64_memory_expansion_hacks_widget_create, NULL },

    { "GEO-RAM",
        "geo-ram",
        georam_widget_create, NULL },
    { "RAM Expansion Module",
        "reu",
        reu_widget_create, NULL },
    { "RamCart",
        "ramcart",
        ramcart_widget_create, NULL },

    { "Double Quick Brown Box",
        "dqbb",
        dqbb_widget_create, NULL },
    { "Expert Cartridge",
        "expert-cart",
        expert_widget_create, NULL },
    { "ISEPIC",
        "isepic",
        isepic_widget_create, NULL },

    { "EasyFlash",
        "easyflash",
        easyflash_widget_create, NULL },
    { "GMod2",
        "gmod2",
        gmod2_widget_create, NULL },
    { "IDE64",
        "ide64",
        ide64_widget_create, NULL },
    { "MMC64",
        "mmc64",
        mmc64_widget_create, NULL },
    { "MMC Replay",
        "mmcr",
        mmcr_widget_create, NULL },
    { "Retro Replay",
        "retroreplay",
        retroreplay_widget_create, NULL },
    { "Super Snapshot V5",
        "super-snapshot",
        super_snapshot_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
        "rrnetmk3",
        rrnetmk3_widget_create, NULL },
#endif

    { "IEEE-448 Interface",
        "ieee-488",
        ieee488_widget_create, NULL },
    { "Burst Mode Modification",
        "burstmode-mode",
        burst_mode_widget_create, NULL },

    { "DigiMAX",
        "digimax",
        digimax_widget_create, NULL },
    { "Magic Voice",
        "magic-voice",
        magic_voice_widget_create, NULL },
    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "SFX Sound Expander",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },
    { "CP/M Cartridge",
        "cpm-cart",
        cpm_widget_create, NULL },

    { "DS12C887 Real Time Clock",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  List of SuperCPU64 extensions (xscpu64)
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t scpu64_io_extensions[] = {
    { "GEO-RAM",
        "geo-ram",
        georam_widget_create, NULL },
    { "RAM Expansion Module",
        "reu",
        reu_widget_create, NULL },
    { "RamCart",
        "ramcart",
        ramcart_widget_create, NULL },

    { "Double Quick Brown Box",
        "dqbb",
        dqbb_widget_create, NULL },
    { "Expert Cartridge",
        "expert-cart",
        expert_widget_create, NULL },
    { "ISEPIC",
        "isepic",
        isepic_widget_create, NULL },

    { "EasyFlash",
        "easyflash",
        easyflash_widget_create, NULL },
    { "GMod2",
        "gmod2",
        gmod2_widget_create, NULL },
    { "IDE64",
        "ide64",
        ide64_widget_create, NULL },
    { "MMC64",
        "mmc64",
        mmc64_widget_create, NULL },
    { "MMC Replay",
        "mmcr",
        mmcr_widget_create, NULL },
    { "Retro Replay",
        "retroreplay",
        retroreplay_widget_create, NULL },
    { "Super Snapshot V5",
        "super-snapshot",
        super_snapshot_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
        "rrnetmk3",
        rrnetmk3_widget_create, NULL },
#endif

    { "IEEE-448 Interface",
        "ieee-488",
        ieee488_widget_create, NULL },
    { "Burst Mode Modification",
        "burstmode-mode",
        burst_mode_widget_create, NULL },

    { "DigiMAX",
        "digimax",
        digimax_widget_create, NULL },
    { "Magic Voice",
        "magic-voice",
        magic_voice_widget_create, NULL },
    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "SFX Sound Expander",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },
    { "CP/M Cartridge",
        "cpm-cart",
        cpm_widget_create, NULL },

    { "DS12C887 Real Time Clock",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  I/O extensions for C128
 */
static ui_settings_tree_node_t c128_io_extensions[] = {
    { "Function ROM",
        "function-rom",
        c128_function_rom_widget_create, NULL },
    { "Banks 2 & 3",
        "banks-23",
        c128_full_banks_widget_create, NULL },

    { "GEO-RAM",
        "geo-ram",
        georam_widget_create, NULL },
    { "RAM Expansion Module",
        "reu",
        reu_widget_create, NULL },
    { "RamCart",
        "ramcart",
        ramcart_widget_create, NULL },

    { "Double Quick Brown Box",
        "dqbb",
        dqbb_widget_create, NULL },
    { "Expert Cartridge",
        "expert-cart",
        expert_widget_create, NULL },
    { "ISEPIC",
        "isepic",
        isepic_widget_create, NULL },

    { "EasyFlash",
        "easyflash",
        easyflash_widget_create, NULL },
    { "GMod2",
        "gmod2",
        gmod2_widget_create, NULL },
    { "IDE64",
        "ide64",
        ide64_widget_create, NULL },
    { "MMC64",
        "mmc64",
        mmc64_widget_create, NULL },
    { "MMC Replay",
        "mmcr",
        mmcr_widget_create, NULL },
    { "Retro Replay",
        "retroreplay",
        retroreplay_widget_create, NULL },
    { "Super Snapshot V5",
        "super-snapshot",
        super_snapshot_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
        "rrnetmk3",
        rrnetmk3_widget_create, NULL },
#endif

    { "IEEE-448 Interface",
        "ieee-488",
        ieee488_widget_create, NULL },
    { "Burst Mode Modification",
        "burstmode-mode",
        burst_mode_widget_create, NULL },

    { "DigiMAX",
        "digimax",
        digimax_widget_create, NULL },
    { "Magic Voice",
        "magic-voice",
        magic_voice_widget_create, NULL },
    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "SFX Sound Expander",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },
    { "CP/M Cartridge",
        "cpm-cart",
        cpm_widget_create, NULL },

    { "DS12C887 Real Time Clock",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  List of VIC-20 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t vic20_io_extensions[] = {
    { "Mega Cart",
        "mega-cart",
        mega_cart_widget_create, NULL },
    { "Final Expansion",
        "final-expansion",
        final_expansion_widget_create, NULL },
    { "Vic Flash Plugin",
        "vic-flash-plugin",
        vic_flash_widget_create, NULL },
    { "UltiMem",
        "ultimem",
        ultimem_widget_create, NULL },
    { "SID Cartridge",
        "sid-cart",
        sidcart_widget_create, NULL },
    { "VIC-1112 IEEE-488 interface",
        "ieee-488",
        vic_ieee488_widget_create, NULL },
    { "I/O RAM",
        "io-ram",
        vic_ioram_widget_create, NULL },
    { "VFLI modification",
        "vfli",
        vfli_widget_create, NULL },

    { "DigiMAX (MasC=uerade",
        "digimax",
        digimax_widget_create, NULL },
    { "DS12C887 RTC (MasC=uerade)",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "GEO-RAM (MasC=uerade)",
        "geo-ram",
        georam_widget_create, NULL },
    { "SFX Sound Expander (MasC=uerade)",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler (MasC=uerade)",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge (MasC=uerade)",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
#endif

    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tapeport devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  List of Plus4 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t plus4_io_extensions[] = {
    { "ACIA",
        "acia",
        plus4_acia_widget_create, NULL },
    { "Digiblaster add-on",
        "digiblaster",
        plus4_digiblaster_widget_create, NULL },
    { "SID Cartridge",
        "sid-cart",
        sidcart_widget_create, NULL },
    { "V364 Speech",
        "v364",
        v364_speech_widget_create, NULL },

    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  List of PET I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t pet_io_extensions[] = {
    { "PET RAM Expansion Unit",
        "pet-reu",
        pet_reu_widget_create, NULL },
    { "PET Colour graphics",
        "pet-colour",
        pet_colour_graphics_widget_create, NULL },
    { "PET DWW hi-res graphics",
        "pet-dww",
        pet_dww_widget_create, NULL },
    { "PET HRE hi-res graphics",
        "pet-hre",
        pet_hre_widget_create, NULL },
    { "SID Cartridge",
        "sid-cart",
        sidcart_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },
    { "PET userport diagnostic pin",
        "pet-diagpin",
        pet_diagpin_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};


/** \brief  List of CBM 5x0 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t cbm5x0_io_extensions[] = {
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};


/** \brief  List of CBM 6x0 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t cbm6x0_io_extensions[] = {
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport_devices",
        tapeport_devices_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};


/** \brief  No I/O extensions (temporary)
 */
static ui_settings_tree_node_t no_io_extensions[] = {
    { "NOT IMPLEMENTED",
       "not-implented",
       NULL, NULL },

    UI_SETTINGS_TERMINATOR
};



/** \brief  Index in the main nodes of the I/O extension sub nodes
 *
 * FIXME:   This is a hack, similar to how the gtk2 UI handled dynamic
 *          menus. The proper way is to implement functions to build the
 *          tree model, which is a TODO at the moment.
 */
#define IO_EXTENSIONS_INDEX 16


/** \brief  Main tree nodes
 *
 *
 * TODO: When creating the SID model/SID settings widget: The fastSiD/ReSID
 *       selection ("SidEngine" resource) should be moved to 'SID Settings',
 *       while the SID model (6581/8580(D), "SidModel" resource should be in
 *       the SID Model widget, with the reSID/fastSID options removed.
 *
 * -- compyx 2017-09-24
 */
static ui_settings_tree_node_t main_nodes[] = {
    { "Speed settings",
       "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
       "keyboard",
       uikeyboard_create_central_widget, NULL },
    { "Sound settings",
       "sound",
       settings_sound_create, NULL },
    { "Sampler settings",
       "sampler",
       uisamplersettings_widget_create, NULL },
    { "Autostart settings",
       "autostart",
       settings_autostart_widget_create, NULL },
    { "Drive settings",
       "drive",
       uidrivesettings_widget_create, NULL },
    { "Printer settings",
        "printer",
        uiprintersettings_widget_create, NULL },
    { "Control port settings",
        "control-port",
        uicontrolport_widget_create, NULL },
    { "Joystick settings",
        "joystick",
        uijoystick_widget_create, NULL },
    { "Mouse settings",
        "mouse",
        uimousesettings_widget_create, NULL },
    { "Model settings",
        "model",
        uimodel_create_central_widget, NULL },
    { "RAM reset pattern",
        "ram-reset",
        create_ram_reset_central_widget, NULL },
    { "ROM settings",
        "rom-settings",
        romset_widget_create, NULL },
    { "Miscellaneous",
        "misc",
        uimisc_create_central_widget, NULL },
    { "Video settings",
        "video",
        settings_video_create, NULL },
    { "SID settings",
        "sid",
        uisoundchipsettings_widget_create, NULL },

    /* the `c64_io_extensions` is a placeholder: it will get replaced by the
     * proper per-machine list. Unfortunately with a fixed index into this list
     * until I refactor the tree model code into something more flexible
     * -- compyx*/
    { "I/O extensions",
        "io-extensions",
        ioextensions_widget_create, c64_io_extensions },

    { "Snaphot/event/media recording",
        "snapshot",
        snapshot_widget_create, NULL },
    { "Monitor settings",
        "monitor",
        monitor_settings_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};



/** \brief  Reference to the current 'central' widget in the settings dialog
 */
static void ui_settings_set_central_widget(GtkWidget *widget);


/** \brief  Reference to the settings dialog
 *
 * Used to show/hide the widget without rebuilding it each time. Clean up
 * with ui_settings_dialog_shutdown()
 */
static GtkWidget *settings_window = NULL;


/** \brief  Reference to the 'content area' widget of the settings dialog
 */
static GtkWidget *settings_grid = NULL;


/** \brief  Reference to the tree model for the settings tree
 */
static GtkTreeStore *settings_model = NULL;


/** \brief  Reference to the tree view for the settings tree
 */
static GtkWidget *settings_tree = NULL;



/** \brief  Handler for the "changed" event of the tree view
 *
 * \param[in]   selection   GtkTreeSelection associated with the tree model
 * \param[in]   user_data   data for the event (unused for now)
 *
 */
static void on_tree_selection_changed(
        GtkTreeSelection *selection,
        gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *name;
        GtkWidget *(*callback)(void *) = NULL;

        gtk_tree_model_get(model, &iter, COLUMN_NAME, &name, -1);
        debug_gtk3("item '%s' clicked\n", name);
        gtk_tree_model_get(model, &iter, COLUMN_CALLBACK, &callback, -1);
        if (callback != NULL) {
            ui_settings_set_central_widget(callback(NULL));
        }
        g_free(name);
    }
}


/** \brief  Create the 'Save on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_save_on_exit_checkbox(void)
{
    return vice_gtk3_resource_check_button_create("SaveResourcesOnExit",
            "Save settings on exit");
}


/** \brief  Create the 'Confirm on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_confirm_on_exit_checkbox(void)
{
    return vice_gtk3_resource_check_button_create("ConfirmOnExit",
            "Confirm on exit");
}


/** \brief  Create empty tree model for the settings tree
 */
static void create_tree_model(void)
{
    settings_model = gtk_tree_store_new(NUM_COLUMNS,
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
}


/** \brief  Get iterator into the tree model by \a path, setting \a iter
 *
 * Sets \a iter to the position in the settings tree model as requested by
 * \a path. If \a path is `NULL` or "" an iter to the very first element in
 * the tree model will be set (no idea if this useful).
 *
 * \param[in]   path    xpath-like expression ("foo/bar/huppel" for now)
 * \param[out]  iter    tree model iterator target
 *
 * \return  boolean (probably best not to touch \a iter when `false`)
 */
bool ui_settings_iter_by_xpath(const char *path, GtkTreeIter *iter)
{
    GtkTreeModel *model = GTK_TREE_MODEL(settings_model);
    gchar **elements;
    gchar **curr;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(settings_model), iter);
    if (path == NULL || *path == '\0') {
        return true;
    }

    /* split the path into elements */
    debug_gtk3("splitting '%s' into elements ...\n", path);
    elements = g_strsplit(path, "/", 0);
    for (curr = elements; *curr != NULL; curr++) {
        debug_gtk3("element '%s'\n", *curr);
        do {
            const char *id;

            gtk_tree_model_get(model, iter, COLUMN_ID, &id, -1);
            debug_gtk3("checking ID string '%s'=='%s'\n", id, *curr);
            if (strcmp(id, *curr) == 0) {
                const char *name;
                gtk_tree_model_get(model, iter, COLUMN_NAME, &name, -1);
                debug_gtk3("got the bastard! '%s'\n", name);
                /* clean up */
                g_strfreev(elements);
                return true;
            }

        } while (gtk_tree_model_iter_next(model, iter));
    }
    /* TODO: figure out if Gtk supports setting an iter to an invalid state
     *       that avoids weird behaviour */
    g_strfreev(elements);
    return false;
}


bool ui_settings_append_by_xpath(const char *path,
                                 ui_settings_tree_node_t *nodes)
{
#if 0
    GtkTreeModel *model = GTK_TREE_MODEL(settings_model);
    GtkTreeIter iter;

    if (ui_settings_iter_by_xpath(path, &iter)) {
        /* found the proper node, add node */
        return true;
    }
#endif
    return false;
}


/** \brief  Create tree store containing settings items and children
 *
 * \return  GtkTreeStore
 */
static GtkTreeStore *populate_tree_model(void)
{
    GtkTreeStore *model;
    GtkTreeIter iter;
    GtkTreeIter child;
    int i;

    model = settings_model;

    for (i = 0; main_nodes[i].name != NULL; i++) {
        gtk_tree_store_append(model, &iter, NULL);
        gtk_tree_store_set(model, &iter,
                COLUMN_NAME, main_nodes[i].name,
                COLUMN_ID, main_nodes[i].id,
                COLUMN_CALLBACK, main_nodes[i].callback,
                -1);
        /* this bit will need proper recursion if we need more than two
         * levels of subitems */
        if (main_nodes[i].children != NULL) {
            int c;
            ui_settings_tree_node_t *list = main_nodes[i].children;

            for (c = 0; list[c].name != NULL; c++) {
                char buffer[256];

                /* mark items without callback with 'TODO' */
                if (list[c].callback != NULL) {
                    g_snprintf(buffer, 256, "%s", list[c].name);
                } else {
                    g_snprintf(buffer, 256, "TODO: %s", list[c].name);
                }

                gtk_tree_store_append(model, &child, &iter);
                gtk_tree_store_set(model, &child,
                        COLUMN_NAME, buffer,
                        COLUMN_ID, list[c].id,
                        COLUMN_CALLBACK, list[c].callback,
                        -1);
            }
        }
    }
    return model;
}


/** \brief  Create treeview for settings side-menu
 *
 * Reads items from `main_nodes` and adds them to the tree view.
 *
 * \return  GtkTreeView
 *
 * TODO:    Handle nested items, and write up somewhere how the hell I finally
 *          got the callbacks working.
 *          Split into a function creating the tree view and functions adding,
 *          altering or removing nodes.
 */
static GtkWidget *create_treeview(void)
{
    GtkWidget *tree;
    GtkCellRenderer *text_renderer;
    GtkTreeViewColumn *text_column;

    ui_settings_tree_node_t *io_nodes = NULL;

    /* hack: set I/O extension sub-nodes */
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:
            io_nodes = c64_io_extensions;
            break;

        case VICE_MACHINE_SCPU64:
            io_nodes = scpu64_io_extensions;
            break;

        case VICE_MACHINE_C128:
            io_nodes = c128_io_extensions;
            break;

        case VICE_MACHINE_VIC20:
            io_nodes = vic20_io_extensions;
            break;

        case VICE_MACHINE_C64DTV:
            io_nodes = NULL;
            break;

        case VICE_MACHINE_PLUS4:
            io_nodes = plus4_io_extensions;
            break;

        case VICE_MACHINE_PET:
            io_nodes = pet_io_extensions;
            break;

        case VICE_MACHINE_CBM5x0:
            io_nodes = cbm5x0_io_extensions;
            break;

        case VICE_MACHINE_CBM6x0:
            io_nodes = cbm6x0_io_extensions;
            break;

        default:
            io_nodes = no_io_extensions;
            break;
    }
    main_nodes[IO_EXTENSIONS_INDEX].children = io_nodes;

    create_tree_model();
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(populate_tree_model()));

    text_renderer = gtk_cell_renderer_text_new();
    text_column = gtk_tree_view_column_new_with_attributes(
            "item-name",
            text_renderer,
            "text", 0,
            NULL);
    /*    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), obj_column); */
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), text_column);
    return tree;
}


/** \brief  Set the 'central'/action widget for the settings dialog
 *
 * Destroys the old 'central' widget and sets the new one.
 *
 *  \param[in,out]  widget  widget to use as the new 'central' widget
 */
static void ui_settings_set_central_widget(GtkWidget *widget)
{
    GtkWidget *child;

    child = gtk_grid_get_child_at(GTK_GRID(settings_grid), 1, 0);
    if (child != NULL) {
        gtk_widget_destroy(child);
    }
    gtk_grid_attach(GTK_GRID(settings_grid), widget, 1, 0, 1, 1);
    /* add a little space around the widget */
    g_object_set(widget, "margin", 16, NULL);
}


/** \brief  Create the 'content widget' of the settings dialog
 *
 * This creates the widget in the dialog used to display the treeview and room
 * for the widget connected to that tree's currently selected item.
 *
 * \param[in]   widget  parent widget
 *
 * \return  GtkGrid (as a GtkWidget)
 */
static GtkWidget *create_content_widget(GtkWidget *widget)
{
    GtkTreeSelection *selection;
    GtkWidget *scroll;
    GtkWidget *extra;

    debug_gtk3("called\n");

    settings_grid = gtk_grid_new();
    settings_tree = create_treeview();
    g_print("tree created\n");

    /* pack the tree in a scrolled window to allow scrolling of the tree when
     * it gets too large for the dialog
     */
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), settings_tree);

    gtk_grid_attach(GTK_GRID(settings_grid), scroll, 0, 0, 1, 1);

    /* TODO: remember the previously selected setting/widget and set it here */
    ui_settings_set_central_widget(settings_speed_widget_create(widget));

    /* create container for generic settings */
    extra = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(extra), 8);
    gtk_grid_set_row_spacing(GTK_GRID(extra), 8);
    g_object_set(extra, "margin", 16, NULL);

    gtk_grid_attach(GTK_GRID(extra), create_save_on_exit_checkbox(),
            0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(extra), create_confirm_on_exit_checkbox(),
            1, 0, 1, 1);

    /* add to main layout */
    gtk_grid_attach(GTK_GRID(settings_grid), extra, 0, 2, 2, 1);

    gtk_widget_show(settings_grid);
    gtk_widget_show(settings_tree);

    gtk_widget_set_size_request(scroll, 300, 500);
    gtk_widget_set_size_request(settings_grid, DIALOG_WIDTH, DIALOG_HEIGHT);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(settings_tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selection), "changed",
            G_CALLBACK(on_tree_selection_changed), NULL);


    return settings_grid;
}


/** \brief  Properly destroy the settings window if required
 */
void ui_settings_dialog_shutdown(void)
{
    if (settings_window != NULL && GTK_IS_WIDGET(settings_window)) {
        gtk_widget_destroy(settings_window);
    }
}



/** \brief  Handler for the "response" event of the settings dialog
 *
 * This determines what to do based on the 'reponse ID' emitted by the dialog.
 *
 * \param[in]   widget      widget triggering the event (button pushed)
 * \param[in]   response_id response ID
 * \param[in]   user_data   extra data (unused)
 */
static void response_callback(GtkWidget *widget, gint response_id,
                              gpointer user_data)
{
    gchar *filename;

    switch (response_id) {

        /* close dialog */
        case GTK_RESPONSE_DELETE_EVENT:
            debug_gtk3("destroying settings widget\n");
            gtk_widget_destroy(widget);
            break;

        /* load vicerc from default location */
        case RESPONSE_LOAD:
            debug_gtk3("loading resources from default file\n");
            if(resources_load(NULL) != 0) {
                debug_gtk3("failed\n");
            }
            break;

        /* load vicerc from a user-specified location */
        case RESPONSE_LOAD_FILE:
            filename = ui_open_file_dialog(widget, "Load settings file",
                    NULL, NULL, NULL);
            if (filename!= NULL) {
                debug_gtk3("loading settings from '%s'\n", filename);
                if (resources_load(filename) != 0) {
                    debug_gtk3("failed\n");
                }
            }
            break;

        /* save settings to default location */
        case RESPONSE_SAVE:
            debug_gtk3("saving vicerc to default location\n");
            if (resources_save(NULL) != 0) {
                debug_gtk3("failed!\n");
            }
            break;

        /* save settings to a user-specified location */
        case RESPONSE_SAVE_FILE:
            filename = ui_save_file_dialog(widget, "Save settings as ...",
                    NULL, TRUE, NULL);
            if (filename != NULL) {
                debug_gtk3("saving setting as '%s'\n",
                        filename ? filename : "NULL");
                if (resources_save(filename) != 0) {
                    debug_gtk3("failed!\n");
                }
                g_free(filename);
            }
            break;

        default:
            break;
    }
}


/** \brief  Respond to window size changes
 *
 * This allows for quickly seeing if specific dialog is getting too large. The
 * DIALOG_WIDTH_MAX and DIALOG_HEIGHT_MAX I sucked out of my thumb, since due
 * to window managers using different themes, we can't use 'proper' values, so
 * I had to use approximate values.
 *
 * \param[in]   widget  a GtkWindow
 * \param[in]   event   the GDK event
 * \param[in]   data    extra event data (unused)
 *
 * \return  boolean
 */
static gboolean on_dialog_configure_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer data)
{
    if (event->type == GDK_CONFIGURE) {
        int width = ((GdkEventConfigure*)event)->width;
        int height = ((GdkEventConfigure*)event)->height;

        /* debug_gtk3("width %d, height %d\n", width, height); */
        if (width > DIALOG_WIDTH_MAX || height > DIALOG_HEIGHT_MAX) {
            gtk_window_set_title(GTK_WINDOW(widget),
                    "HELP! --- DIALOG IS TOO BLOODY LARGE -- ERROR!");
        }
    }
    return FALSE;
}



/** \brief  Callback to create the main settings dialog from the menu
 *
 * \param[in]   widget      (direct) parent widget, the menu item
 * \param[in]   user_data   data for the event (unused)
 *
 * \note    The appearance of minimize/maximize buttons seems to depend on
 *          which Window Manager is active:
 *
 *          On MATE (marco, a Metacity fork) both buttons are hidden.
 *          On KDE (KWin) the maximize button is still visible but inactive
 *          On OpenBox both min/max are visible with only minimize working
 */
void ui_settings_dialog_create(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *content;
    char title[256];
#if 0
    GtkTreeIter iter;
#endif

    g_snprintf(title, 256, "%s Settings", machine_name);

    dialog = gtk_dialog_new_with_buttons(
            title,
            ui_get_active_window(),
            GTK_DIALOG_MODAL,
            "Load", RESPONSE_LOAD,
            "Save", RESPONSE_SAVE,
            "Load file ...", RESPONSE_LOAD_FILE,
            "Save file ...", RESPONSE_SAVE_FILE,
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_content_widget(dialog));

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    g_signal_connect(dialog, "response", G_CALLBACK(response_callback), NULL);
    g_signal_connect(dialog, "configure-event",
            G_CALLBACK(on_dialog_configure_event), NULL);

#if 0
    /* XXX: used to test some tree manipulation functions, remove when stuff
     *      works
     */
    if (ui_settings_iter_by_xpath("io-extensions/expert-cart", &iter)) {
        debug_gtk3("yay, got iterator\n");
    } else {
        debug_gtk3("oops, no iterator found\n");
    }
#endif


    /* XXX: this is normal code again */
    gtk_widget_show_all(dialog);
}
