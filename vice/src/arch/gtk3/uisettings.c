/** \file   uisettings.c
 * \brief   GTK3 main settings dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES SaveResourcesOnExit     all
 * $VICERES ConfirmOnExit           all
 * $VICERES PauseOnSettings         all
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

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "util.h"
#include "machine.h"
#include "resources.h"
#include "vsync.h"

#include "vice_gtk3.h"

#include "ui.h"
#include "settings_speed.h"
#include "settings_keyboard.h"
#include "settings_sound.h"
#include "settings_autofire.h"
#include "settings_autostart.h"
#include "settings_drive.h"
#include "settings_fsdevice.h"
#include "settings_model.h"
#include "settings_misc.h"
#include "settings_ramreset.h"
#include "settings_video.h"
#include "settings_sampler.h"
#include "settings_printer.h"
#include "settings_controlport.h"
#include "settings_joystick.h"
#include "settings_soundchip.h"
#include "settings_monitor.h"
#include "settings_romset.h"
#include "settings_snapshot.h"
#ifdef HAVE_RAWNET
# include "settings_ethernet.h"
#endif
#include "settings_rs232.h"
#include "scpu64settingswidget.h"
#include "c64dtvflashsettingswidget.h"

/* I/O extension widgets */
#include "settings_io.h"
#include "c64memhackswidget.h"
#include "georamwidget.h"
#include "reuwidget.h"
#include "ramcartwidget.h"
#include "dqbbwidget.h"
#include "expertwidget.h"
#include "isepicwidget.h"
#include "easyflashwidget.h"
#include "gmod2widget.h"
#include "gmod3widget.h"
#include "mmcrwidget.h"
#include "mmc64widget.h"
#include "ide64widget.h"
#include "retroreplaywidget.h"
#include "ltkernalwidget.h"
#include "ramlinkwidget.h"
#include "rexramfloppywidget.h"
#include "netplaywidget.h"
#include "settings_default_cart.h"

#ifdef HAVE_RAWNET
# include "ethernetcartwidget.h"
# include "rrnetmk3widget.h"
#endif

#include "c128functionromwidget.h"
#include "ieee488widget.h"
#include "digimaxwidget.h"
#include "magicvoicewidget.h"
#ifdef HAVE_MIDI
# include "midiwidget.h"
#endif
#include "sfxsoundexpanderwidget.h"
#include "ds12c887widget.h"
#include "userportdeviceswidget.h"
#include "tapeportdeviceswidget.h"
#include "sidcartwidget.h"
#include "sfxsoundsamplerwidget.h"
#include "megacartwidget.h"
#include "petreuwidget.h"
#include "petcolourgraphicswidget.h"
#include "petdwwwidget.h"
#include "supersnapshotwidget.h"
#include "cpmwidget.h"
#include "burstmodewidget.h"
#include "c128fullbankswidget.h"
#include "plus4digiblasterwidget.h"
#include "finalexpansionwidget.h"
#include "vicflashwidget.h"
#include "ultimemwidget.h"
#include "vicieee488widget.h"
#include "ieeeflash64widget.h"
#include "vicioramwidget.h"
#include "vfliwidget.h"
#include "petdiagpinwidget.h"
#include "pethrewidget.h"
#include "settings_crt.h"
#include "uimachinewindow.h"

/* TODO: move up and sort headers */
#include "settings_peripherals_generic.h"
#include "settings_host_display.h"

/* VSID stuff */
#include "hvscsettingswidget.h"

/* CWD widget: moved from machine->host to host
 *
 * XXX: Probably at some point create settings_paths.{c,h} and have that use
 * the CWD widget.
 */
#include "cwdwidget.h"

/* JAM action widget
 */
#include "jamactionwidget.h"


#include "uisettings.h"


/** \brief  CSS to allow collapsing/expanding tree nodes with crsr keys
 *
 * Note the 'treeview' rule, before Gtk+ 3.20 this was 'GtkTreeView'
 *
 *
 */
#define TREEVIEW_CSS \
    "@binding-set SettingsTreeViewBinding\n" \
    "{\n" \
    "    bind \"Left\"  { \"select-cursor-parent\" ()\n" \
    "                     \"expand-collapse-cursor-row\" (0,0,0) };\n" \
    "    bind \"Right\" { \"expand-collapse-cursor-row\" (0,1,0) };\n" \
    "}\n" \
    "\n" \
    "treeview\n" \
    "{\n" \
    "    -gtk-key-bindings: SettingsTreeViewBinding;\n" \
    "}\n" \
    "treeview .separator\n" \
    "{\n" \
    "    color: darker (@theme_bg_color);\n" \
    "}\n"


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


/** \brief  Initial dialog width
 *
 * This is not how wide the dialog will actually become, that is determined by
 * the Gtk theme applied. But it's a rough estimate.
 */
#define DIALOG_WIDTH 800


/** \brief  Initial dialog height
 *
 * This is not how tall the dialog will actually become, that is determined by
 * the Gtk theme applied. But it's a rough estimate.
 */
#define DIALOG_HEIGHT 560


/** \brief  Maximum width the UI can be
 *
 * This again is not a really a fixed value, but more of an indicator when the
 * UI might get too large after any decorations are applied. The idea is to
 * have a UI that works on a 1280x768 resolution without requiring scrollbars.
 */
#define DIALOG_WIDTH_MAX 1024


/** \brief  Maximum height the UI can be
 *
 * This again is not a really a fixed value, but more of an indicator when the
 * UI might get too large after any decorations are applied. The idea is to
 * have a UI that works on a 1280x768 resolution without requiring scrollbars.
 */
#define DIALOG_HEIGHT_MAX 640


/** \brief  Enum used for the "response" callback of the settings dialog
 *
 * All values must be positive since Gtk reserves standard responses in its
 * GtkResponse enum as negative values.
 */
enum {
    RESPONSE_RESET = 1, /**< reset current central widget */
    RESPONSE_FACTORY,   /**< set current central widget's resources to their
                             factory settings */
    RESPONSE_DEFAULT    /**< Restore default settings */
};


/*
 * I/O extensions per emulator
 */

/* {{{ c64_cartridges */
/** \brief  List of C64 cartidge settings (x64, x64sc)
 */
static ui_settings_tree_node_t c64_cartridges[] = {

    { "Default cartridge",
      "default-cart",
      settings_default_cart_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,

    { "GEO-RAM",
      "geo-ram",
      georam_widget_create, NULL },
    { "RAM Expansion Module",
      "reu",
      reu_widget_create, NULL },
    { "RamCart",
      "ramcart",
      ramcart_widget_create, NULL },


    UI_SETTINGS_SEPARATOR,

    { "Double Quick Brown Box",
      "dqbb",
      dqbb_widget_create, NULL },
    { "Expert Cartridge",
      "expert-cart",
      expert_widget_create, NULL },
    { "ISEPIC",
      "isepic",
      isepic_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,

    { "EasyFlash",
      "easyflash",
      easyflash_widget_create, NULL },
    { "GMod2",
      "gmod2",
      gmod2_widget_create, NULL },
    { "GMod3",
      "gmod3",
      gmod3_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,

    { "IDE64",
      "ide64",
      ide64_widget_create, NULL },
    { "Lt. Kernal Host Adapter",
      "ltkernal",
      ltkernal_widget_create, NULL },
    { "RAMLink",
      "ramlink",
      ramlink_widget_create, NULL },
    { "REX Ram-Floppy",
      "rexramfloppy",
      rexramfloppy_widget_create, NULL },
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

    UI_SETTINGS_SEPARATOR,
    { "Ethernet Cartridge",
      "ethernet-cart",
      ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
      "rrnetmk3",
      rrnetmk3_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,
#endif
    { "IEEE-448 Interface",
      "ieee-488",
      ieee488_widget_create, NULL },
    { "IEEE Flash! 64 Interface",
      "ieee-flash-64",
      ieeeflash64_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,

    { "DigiMAX",
      "digimax",
      digimax_widget_create, NULL },
    { "Magic Voice",
      "magic-voice",
      magic_voice_widget_create, NULL },
#ifdef HAVE_MIDI
    { "MIDI emulation",
      "midi",
      midi_widget_create, NULL },
#endif
    { "SFX Sound Expander",
      "sfx-expander",
      sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
      "sfx-sampler",
      sfx_sound_sampler_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,

    { "CP/M Cartridge",
      "cpm-cart",
      cpm_widget_create, NULL },

    UI_SETTINGS_SEPARATOR,

    { "DS12C887 Real Time Clock",
      "ds12c887-rtc",
      ds12c887_widget_create, NULL },
     UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ scpu64_cartridges */
/** \brief  List of SuperCPU64 extensions (xscpu64)
 */
static ui_settings_tree_node_t scpu64_cartridges[] = {
    { "Default cartridge",
      "default-cart",
      settings_default_cart_widget_create, NULL },
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
    { "GMod3",
      "gmod3",
      gmod3_widget_create, NULL },
    { "IDE64",
      "ide64",
      ide64_widget_create, NULL },
    { "REX Ram-Floppy",
      "rexramfloppy",
      rexramfloppy_widget_create, NULL },
    { "MMC64",
      "mmc64",
      mmc64_widget_create, NULL },
    { "MMC Replay",
      "mmcr",
      mmcr_widget_create, NULL },
    { "RAMLink",
      "ramlink",
      ramlink_widget_create, NULL },
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
    { "DigiMAX",
      "digimax",
      digimax_widget_create, NULL },
    { "Magic Voice",
      "magic-voice",
      magic_voice_widget_create, NULL },
#ifdef HAVE_MIDI
    { "MIDI emulation",
      "midi",
      midi_widget_create, NULL },
#endif
    { "SFX Sound Expander",
      "sfx-expander",
      sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
      "sfx-sampler",
      sfx_sound_sampler_widget_create, NULL },
    { "DS12C887 Real Time Clock",
      "ds12c887-rtc",
      ds12c887_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ c128_cartridges */
/** \brief  I/O extensions for C128
 */
static ui_settings_tree_node_t c128_cartridges[] = {
    { "Default cartridge",
      "default-cart",
      settings_default_cart_widget_create, NULL },
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
    { "GMod3",
      "gmod3",
      gmod3_widget_create, NULL },
    { "IDE64",
      "ide64",
      ide64_widget_create, NULL },
    { "Lt. Kernal Host Adapter",
      "ltkernal",
      ltkernal_widget_create, NULL },
    { "RAMLink",
      "ramlink",
      ramlink_widget_create, NULL },
    { "REX Ram-Floppy",
      "rexramfloppy",
      rexramfloppy_widget_create, NULL },
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
    { "DigiMAX",
      "digimax",
      digimax_widget_create, NULL },
    { "Magic Voice",
      "magic-voice",
      magic_voice_widget_create, NULL },
#ifdef HAVE_MIDI
    { "MIDI emulation",
      "midi",
      midi_widget_create, NULL },
#endif
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

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ vic20_cartridges */
/** \brief  List of VIC-20 I/O extensions
 */
static ui_settings_tree_node_t vic20_cartridges[] = {
    { "Default cartridge",
      "default-cart",
      settings_default_cart_widget_create, NULL },
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
    { "VIC-1112 IEEE-488 interface",
      "ieee-488",
      vic_ieee488_widget_create, NULL },
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
#ifdef HAVE_MIDI
    { "MIDI emulation",
      "midi",
      midi_widget_create, NULL },
#endif

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ plus4_io_extensions */
/** \brief  List of Plus4 I/O extensions
 */
static ui_settings_tree_node_t plus4_io_extensions[] = {
    { "Default cartridge",
      "default-cart",
      settings_default_cart_widget_create, NULL },
    { "Digiblaster add-on",
      "digiblaster",
      plus4_digiblaster_widget_create, NULL },
    { "SID Card",
      "sid-card",
      sidcart_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ pet_io_extensions */
/** \brief  List of PET I/O extensions
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
    { "SID Card",
      "sid-card",
      sidcart_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ cbm5x0_io_extensions */
/** \brief  List of CBM 5x0 I/O extensions
 */
#if 0
static ui_settings_tree_node_t cbm5x0_io_extensions[] = {
    UI_SETTINGS_TERMINATOR
};
#endif
/* }}} */

/* {{{ cbm6x0_io_extensions */
/** \brief  List of CBM 6x0 I/O extensions
 */
#if 0
static ui_settings_tree_node_t cbm6x0_io_extensions[] = {
    UI_SETTINGS_TERMINATOR
};
#endif
/* }}} */

/*
 * Main tree nodes per emulator
 */

/*****************************************************************************
 *                  VSID tree nodes for the settings UI                      *
 ****************************************************************************/

/* {{{ main_nodes_vsid */
/** \brief  Main tree nodes for VSID
 */
static ui_settings_tree_node_t main_nodes_vsid[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Speed",
      "speed",
       settings_speed_widget_create, NULL },
    /* XXX: basically a selection between 'PAL'/'NTSC' (50/60Hz) */
    { "Model",
      "model",
      settings_model_widget_create, NULL },
#if 0
    /* XXX: do we need this? Sidplay allows ROM selection for Basic, Kernal and
     *      Chargen, perhaps move into model settings, plenty of space there */
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
#endif
#if 0
    /* XXX: perhaps required for VSID-specific things */
    { "Emulator",
      "misc",
      settings_misc_widget_create, NULL },
#endif
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "HVSC",
      "hvsc",
      hvsc_settings_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */



/* {{{ host_nodes_generic */
/** \brief  Child nodes for the 'Host' node for all machines
 */
static ui_settings_tree_node_t host_nodes_generic[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Current directory",
      "cwd",
      cwd_widget_create, NULL },
    { "CPU JAM action",
      "jam-action",
      jam_action_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  C64 tree nodes for the settings UI                       *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_c64 */
/** \brief  Child nodes for the C64 'Host' node
 */
static ui_settings_tree_node_t host_nodes_c64[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Current directory",
      "cwd",
      cwd_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_c64 */
/** \brief  Child nodes for the C64 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_c64[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "Memory Expansion Hacks",
      "mem-hacks",
      c64_memhacks_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    { "Burst Mode Modification",
      "burstmode-mode",
      burst_mode_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_c64 */
/** \brief  Child nodes for the C64 'Display' node
 */
static ui_settings_tree_node_t display_nodes_c64[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "VIC-II",
      "vicii",
      settings_video_create, NULL },
    { "CRT",
      "crt",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_c64 */
/** \brief  Child nodes for the C64 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_c64[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_c64 */
/** \brief  Child nodes for the C64 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_c64[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_c64 */
/** \brief  Child nodes for the C64 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_c64[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    /* "Output devices? drive is also input */
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
   { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
    { "Tape port devices",
      "tapeport-devices",
      tapeport_devices_widget_create, NULL },
#ifdef HAVE_RAWNET
    { "Ethernet",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_c64 */
/** \brief  Main tree nodes for x64/x64sc
 */
static ui_settings_tree_node_t main_nodes_c64[] = {
    { "Host",
      "host",
      NULL, host_nodes_generic },
    { "Machine",
      "machine",
      NULL, machine_nodes_c64 },
    { "Display",
      "display",
      NULL, display_nodes_c64 },
    { "Audio",
      "audio",
      NULL, audio_nodes_c64 },
    { "Input devices",
      "input",
      NULL, input_nodes_c64 },
    { "Peripheral devices",
      "peripheral", /* I'll misspell this many times */
      NULL, peripheral_nodes_c64 },
    { "Cartridges",
      "cartridges",
      NULL, c64_cartridges },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  C64DTV tree nodes for the settings UI                    *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_c64dtv */
/** \brief  Child nodes for the C64DTV 'Host' node
 */
static ui_settings_tree_node_t host_nodes_c64dtv[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_c64dtv */
/** \brief  Child nodes for the C64DTV 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_c64dtv[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "Flash",
      "flash",
      c64dtv_flash_settings_widget_create, NULL },
#if 0
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
#endif

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_c64dtv */
/** \brief  Child nodes for the C64DTV 'Display' node
 */
static ui_settings_tree_node_t display_nodes_c64dtv[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "VIC-II",
      "vicii",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_c64dtv */
/** \brief  Child nodes for the C64DTV 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_c64dtv[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_c64dtv */
/** \brief  Child nodes for the C64DTV 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_c64dtv[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_c64dtv */
/** \brief  Child nodes for the C64DTV 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_c64dtv[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_c64dtv */
/** \brief  Main tree nodes for x64dtv
 */
static ui_settings_tree_node_t main_nodes_c64dtv[] = {
    { "Host",       "host",     NULL,   host_nodes_generic },
    { "Machine",    "machine",  NULL,   machine_nodes_c64dtv },
    { "Display",    "display",  NULL,   display_nodes_c64dtv },
    { "Audio",      "audio",    NULL,   audio_nodes_c64dtv },
    { "Input devices",      "input",    NULL,   input_nodes_c64dtv },
    { "Peripheral devices", "peripheral", NULL, peripheral_nodes_c64dtv },
#if 0
    { "Emulator",
      "misc",
      settings_misc_widget_create, NULL },
#endif


    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                      C128 tree nodes for the settings UI                  *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_c128 */
/** \brief  Child nodes for the C128 'Host' node
 */
static ui_settings_tree_node_t host_nodes_c128[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_c128 */
/** \brief  Child nodes for the C128 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_c128[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "Function ROM",
      "function-rom",
      c128_function_rom_widget_create, NULL },
    { "Banks 2 & 3",
      "banks-23",
      c128_full_banks_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_c128 */
/** \brief  Child nodes for the C128 'Display' node
 */
static ui_settings_tree_node_t display_nodes_c128[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "VIC-II",
      "vicii",
      settings_video_create, NULL },
    { "VDC",
      "vdc",
      settings_video_create_vdc, NULL },
    { "CRT",
      "crt",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_c128 */
/** \brief  Child nodes for the C128 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_c128[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_c128 */
/** \brief  Child nodes for the C128 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_c128[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_c128 */
/** \brief  Child nodes for the C128 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_c128[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    /* "Output devices? drive is also input */
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
    { "Tape port devices",
      "tapeport-devices",
      tapeport_devices_widget_create, NULL },
#ifdef HAVE_RAWNET
    { "Ethernet",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_c128 */
/** \brief  Main tree nodes for x128
 */
static ui_settings_tree_node_t main_nodes_c128[] = {
    { "Host",
      "host",
      NULL, host_nodes_generic },
    { "Machine",
      "machine",
      NULL, machine_nodes_c128 },
    { "Display",
      "display",
      NULL, display_nodes_c128 },
    { "Audio",
      "audio",
      NULL, audio_nodes_c128 },
    { "Input devices",
      "input",
      NULL, input_nodes_c128 },
    { "Peripheral devices",
      "peripheral", /* I'll misspell this many times */
      NULL, peripheral_nodes_c128 },
    { "Cartridges",
      "cartridges",
      NULL, c128_cartridges },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  SCPU64 tree nodes for the settings UI                    *
 ****************************************************************************/
#if 0
/* {{{ host_nodes_scpu64 */
/** \brief  Child nodes for the SCPU64 'Host' node
 */
static ui_settings_tree_node_t host_nodes_scpu64[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_scpu64 */
/** \brief  Child nodes for the SCPU64 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_scpu64[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL }, 
    { "SCPU64",
      "scpu64",
      scpu64_settings_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    { "Burst Mode Modification",
      "burstmode-mode",
      burst_mode_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_scpu64 */
/** \brief  Child nodes for the SCPU64 'Display' node
 */
static ui_settings_tree_node_t display_nodes_scpu64[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "VIC-II",
      "vicii",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_scpu64 */
/** \brief  Child nodes for the SCPU64 'Peripheral devices' node
 */
static ui_settings_tree_node_t audio_nodes_scpu64[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_scpu64 */
/** \brief  Child nodes for the SCPU64 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_scpu64[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_scpu64 */
/** \brief  Child nodes for the SCPU64 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_scpu64[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
#ifdef HAVE_RAWNET
    { "Ethernet",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_scpu64 */
/** \brief  Main tree nodes for xscpu64
 */
static ui_settings_tree_node_t main_nodes_scpu64[] = {
    { "Host",               "host",         NULL,   host_nodes_generic },
    { "Machine",            "machine",      NULL,   machine_nodes_scpu64 },
    { "Display",            "display",      NULL,   display_nodes_scpu64 },
    { "Audio",              "audio",        NULL,   audio_nodes_scpu64 },
    { "Input devices",      "input",        NULL,   input_nodes_scpu64 },
    { "Peripheral devices", "peripheral",   NULL,   peripheral_nodes_scpu64 },
    { "Cartridges",
      "cartridges",
      NULL, scpu64_cartridges },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  VIC-20 tree nodes for the settings UI                    *
 ****************************************************************************/
#if 0
/* {{{ host_nodes_vic20 */
/** \brief  Child nodes for the VIC-20 'Host' node
 */
static ui_settings_tree_node_t host_nodes_vic20[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_vic20 */
/** \brief  Child nodes for the VIC20 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_vic20[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    { "I/O RAM",
      "io-ram",
      vic_ioram_widget_create, NULL },
    { "VFLI modification",
      "vfli",
      vfli_widget_create, NULL },
    { "SID Card",
      "sid-card",
      sidcart_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_vic20 */
/** \brief  Child nodes for the VIC20 'Display' node
 */
static ui_settings_tree_node_t display_nodes_vic20[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "VIC",
      "vic",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_vic20 */
/** \brief  Child nodes for the VIC20 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_vic20[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_vic20 */
/** \brief  Child nodes for the VIC20 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_vic20[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_vic20 */
/** \brief  Child nodes for the VIC20 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_vic20[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
    { "Tapeport devices",
      "tapeport-devices",
      tapeport_devices_widget_create, NULL },
#ifdef HAVE_RAWNET
    { "Ethernet",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_vic20 */
/** \brief  Main tree nodes for xvic
 */
static ui_settings_tree_node_t main_nodes_vic20[] = {
    { "Host",               "host",         NULL,   host_nodes_generic },
    { "Machine",            "machine",      NULL,   machine_nodes_vic20 },
    { "Display",            "display",      NULL,   display_nodes_vic20 },
    { "Audio",              "audio",        NULL,   audio_nodes_vic20 },
    { "Input devices",      "input",        NULL,   input_nodes_vic20 },
    { "Peripheral devices", "peripheral",   NULL,   peripheral_nodes_vic20 },
    { "Cartridges",         "cartridges",
      NULL, vic20_cartridges },
#if 0
    { "Emulator",
      "misc",
      settings_misc_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/*****************************************************************************
 *                  Plus4/C16 tree nodes for the settings UI                 *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_plus4 */
/** \brief  Child nodes for the Plus4/C16 'Host' node
 */
static ui_settings_tree_node_t host_nodes_plus4[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_plus4 */
/** \brief  Child nodes for the Plus4 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_plus4[] = {
    { "Model",
      "model",
       settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
       settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_plus4 */
/** \brief  Child nodes for the Plus4 'Display' node
 */
static ui_settings_tree_node_t display_nodes_plus4[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "TED",
      "ted",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_plus4 */
/** \brief  Child nodes for the Plus4 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_plus4[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_plus4 */
/** \brief  Child nodes for the Plus4 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_plus4[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_plus4 */
/** \brief  Child nodes for the Plus4 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_plus4[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
    { "Tape port devices",
      "tapeport-devices",
      tapeport_devices_widget_create, NULL },
#ifdef HAVE_RAWNET
    { "Ethernet",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_plus4 */
/** \brief  Main tree nodes for xplus4
 */
static ui_settings_tree_node_t main_nodes_plus4[] = {
    { "Host",               "host",         NULL,   host_nodes_generic },
    { "Machine",            "machine",      NULL,   machine_nodes_plus4 },
    { "Display",            "display",      NULL,   display_nodes_plus4 },
    { "Audio",              "audio",        NULL,   audio_nodes_plus4 },
    { "Input devices",      "input",        NULL,   input_nodes_plus4 },
    { "Peripheral devices", "peripheral",   NULL,   peripheral_nodes_plus4 },
    { "I/O Extensions",
      "io-extensions",
      NULL, plus4_io_extensions },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                      PET tree nodes for the settings UI                   *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_pet */
/** \brief  Child nodes for the PET 'Host' node
 */
static ui_settings_tree_node_t host_nodes_pet[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_pet */
/** \brief  Child nodes for the PET 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_pet[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "PET userport diagnostic pin",
        "pet-diagpin",
        pet_diagpin_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_pet */
/** \brief  Child nodes for the PET 'Display' node
 */
static ui_settings_tree_node_t display_nodes_pet[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "CRTC",
      "crtc",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_pet */
/** \brief  Child nodes for the PET 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_pet[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_pet */
/** \brief  Child nodes for the PET 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_pet[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_pet */
/** \brief  Child nodes for the PET 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_pet[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
    { "Tape port devices",
      "tapeport-devices",
      tapeport_devices_widget_create, NULL },
#if 0 /* not implemented/supported */
#ifdef HAVE_RAWNET
    { "Ethernet",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_pet */
/** \brief  Main tree nodes for xpet
 */
static ui_settings_tree_node_t main_nodes_pet[] = {
    { "Host",               "host",         NULL,   host_nodes_generic },
    { "Machine",            "machine",      NULL,   machine_nodes_pet },
    { "Display",            "display",      NULL,   display_nodes_pet },
    { "Audio",              "audio",        NULL,   audio_nodes_pet },
    { "Input devices",      "input",        NULL,   input_nodes_pet },
    { "Peripheral devices", "peripheral",   NULL,   peripheral_nodes_pet },
    { "I/O extensions", "io-extensions",
      NULL, pet_io_extensions },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  CBM5x0 tree nodes for the settings UI                    *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_cbm5x0 */
/** \brief  Child nodes for the CBM5x0 'Host' node
 */
static ui_settings_tree_node_t host_nodes_cbm5x0[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_cbm5x0 */
/** \brief  Child nodes for the CBM5x0 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_cbm5x0[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_cbm5x0 */
/** \brief  Child nodes for the CBM5x0 'Display' node
 */
static ui_settings_tree_node_t display_nodes_cbm5x0[] = {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "VIC-II",
      "vicii",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_cbm5x0 */
/** \brief  Child nodes for the CBM5x0 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_cbm5x0[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_cbm5x0 */
/** \brief  Child nodes for the CBM5x0 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_cbm5x0[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_cbm5x0 */
/** \brief  Child nodes for the CBM5x0 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_cbm5x0[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Tape port devices",
      "tapeport-devices",
      tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_cbm5x0 */
/** \brief  Main tree nodes for xcbm5x0
 */
static ui_settings_tree_node_t main_nodes_cbm5x0[] = {
    { "Host",               "host",         NULL,   host_nodes_generic },
    { "Machine",            "machine",      NULL,   machine_nodes_cbm5x0 },
    { "Display",            "display",      NULL,   display_nodes_cbm5x0 },
    { "Audio",              "audio",        NULL,   audio_nodes_cbm5x0 },
    { "Input devices",      "input",        NULL,   input_nodes_cbm5x0 },
    { "Peripheral devices", "peripheral",   NULL,   peripheral_nodes_cbm5x0 },
#if 0
    { "I/O extensions", "io-extensions",    NULL, cbm5x0_io_extensions },
#endif
#if 0
    { "Emulator",
      "misc",
      settings_misc_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  CBM-II tree nodes for the settings UI                    *
 ****************************************************************************/

#if 0
/* {{{ host_nodes_cbm6x0 */
/** \brief  Child nodes for the CBM6x0 'Host' node
 */
static ui_settings_tree_node_t host_nodes_cbm6x0[] = {
    { "Autostart",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Monitor",
      "monitor",
      settings_monitor_widget_create, NULL },
    { "Netplay",
      "netplay",
      netplay_widget_create, NULL },
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */
#endif

/* {{{ machine_nodes_cbm6x0 */
/** \brief  Child nodes for the CBM6x0 'Machine' node
 */
static ui_settings_tree_node_t machine_nodes_cbm6x0[] = {
    { "Model",
      "model",
      settings_model_widget_create, NULL },
    { "ROM",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "I/O settings",
      "io-settings",
      settings_io_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_cbm6x0 */
/** \brief  Child nodes for the CBM6x0 'Display' node
 */
static ui_settings_tree_node_t display_nodes_cbm6x0[]= {
    { "Host display",
      "host-display",
      settings_host_display_widget_create, NULL },
    { "CRTC",
      "crtc",
      settings_video_create, NULL },
    { "CRT",
      "CRT",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_cbm6x0 */
/** \brief  Child nodes for the CBM6x0 'Audio' node
 */
static ui_settings_tree_node_t audio_nodes_cbm6x0[] = {
    { "Sound",
      "sound",
      settings_sound_create, NULL },
    { "SID",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Sampler",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_cbm6x0 */
/** \brief  Child nodes for the CBM6x0 'Input devices' node
 */
static ui_settings_tree_node_t input_nodes_cbm6x0[] = {
    { "Keyboard",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Autofire",
      "autofire",
      settings_autofire_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral nodes_cbm6x0 */
/** \brief  Child nodes for the CBM6x0 'Peripheral devices' node
 */
static ui_settings_tree_node_t peripheral_nodes_cbm6x0[] = {
    { "Generic",
      "generic",
      settings_peripherals_generic_widget_create, NULL },
    { "Drive",
      "drive",
      settings_drive_widget_create, NULL },
    { "Filesystem Device",
      "fsdevice",
      settings_fsdevice_widget_create, NULL },
    { "Printer",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
    { "Userport devices",
      "userport-devices",
      userport_devices_widget_create, NULL },
    { "Tape port devices",
      "tapeport_devices",
      tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ main_nodes_cbm6x0 */
/** \brief  Main tree nodes for xcbm6x0
 */
static ui_settings_tree_node_t main_nodes_cbm6x0[] = {
    { "Host",               "host",         NULL,   host_nodes_generic },
    { "Machine",            "machine",      NULL,   machine_nodes_cbm6x0 },
    { "Display",            "display",      NULL,   display_nodes_cbm6x0 },
    { "Audio",              "audio",        NULL,   audio_nodes_cbm6x0 },
    { "Input devices",      "input",        NULL,   input_nodes_cbm6x0 },
    { "Peripheral devices", "peripheral",   NULL,   peripheral_nodes_cbm6x0 },
#if 0
    { "I/O extensions",     "io-extensions",NULL, cbm6x0_io_extensions },
#endif
    { "Snapshot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
#if 0
    { "Emulator",
      "misc",
      settings_misc_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/** \brief  Reference to the current 'central' widget in the settings dialog
 */
static void ui_settings_set_central_widget(GtkWidget *widget);


/** \brief  Old pause state when popping up the dialog
 *
 * Used for the PauseOnSettings resource: if true, exiting the dialog will set
 * the emulators pause state to this.
 */
static int settings_old_pause_state;


/** \brief  Reference to the settings dialog
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


/** \brief  Scroll window container for the settings treeview
 */
static GtkWidget *scrolled_window = NULL;


/** \brief  Widget containing the treeview and the settings 'page'
 *
 * Allows resizing both the treeview and the setting with a 'grip'.
 */
static GtkWidget *paned_widget = NULL;


/** \brief  Path to the last used settings page
 */
static GtkTreePath *last_node_path = NULL;


/** \brief  Handler for the "destroy" event of the main dialog
 *
 * \param[in]   widget      main dialog (unused)
 * \param[in]   data        extra event data (unused)
 */
static void on_settings_dialog_destroy(GtkWidget *widget, gpointer data)
{
    settings_window = NULL;
}


/** \brief  Handler for the double click event of a tree node
 *
 * Expands or collapses the node and its children (if any)
 *
 * \param[in,out]   tree_view   tree view instance
 * \param[in]       path        tree view path
 * \param[in]       column      tree view column (unused)
 * \param[in]       user_data   extra event data (unused)
 */
static void on_row_activated(GtkTreeView *tree_view,
                            GtkTreePath *path,
                            GtkTreeViewColumn *column,
                            gpointer user_data)
{
    if (gtk_tree_view_row_expanded(tree_view, path)) {
        gtk_tree_view_collapse_row(tree_view, path);
    } else {
        /*
         * Only expand the immediate children. A no-op at the moment since
         * we only have two levels of nodes in the tree, but perhaps useful
         * for later.
         */
        gtk_tree_view_expand_row(tree_view, path, FALSE);
    }
}


/** \brief  Create the widget that is initially shown in the settings UI
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
static GtkWidget *ui_settings_inital_widget(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(64, 64);
    label = gtk_label_new(NULL);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_markup(GTK_LABEL(label),
            "This is the first widget/dialog shown when people click on the"
            " settings UI.\n"
            "So perhaps we could show some instructions or something here.");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


#if 0
/** \brief  Paused state when popping up the UI
 */
static int old_pause_state;
#endif

/** \brief  Handler for the "changed" event of the tree view
 *
 * \param[in]   selection   GtkTreeSelection associated with the tree model
 * \param[in]   user_data   data for the event (unused for now)
 */
static void on_tree_selection_changed(GtkTreeSelection *selection,
                                      gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *name = NULL;
        gchar *parent_name = NULL;
        GtkWidget *(*callback)(void *) = NULL;
        const char *id;


        gtk_tree_model_get(model, &iter, COLUMN_NAME, &name, -1);
        gtk_tree_model_get(model, &iter, COLUMN_CALLBACK, &callback, -1);
        gtk_tree_model_get(model, &iter, COLUMN_ID, &id, -1);

        if (callback != NULL) {
            GtkTreeIter parent;
            char *title;

            /* try to get parent's name */
            if (gtk_tree_model_iter_parent(model, &parent, &iter)) {
                gtk_tree_model_get(model, &parent, COLUMN_NAME, &parent_name, -1);
            }

            if (parent_name != NULL) {
                title = lib_msprintf("%s settings :: %s :: %s",
                        machine_name, parent_name, name);
            } else {
                title = lib_msprintf("%s settings :: %s", machine_name, name);
            }
            gtk_window_set_title(GTK_WINDOW(settings_window), title);
            lib_free(title);
            /* create new central widget, using settings_window (this dialog)
             * as its parent, this will allow for proper blocking in modal
             * dialogs, while ui_get_active_window() breaks that. */
            if (last_node_path != NULL) {
                gtk_tree_path_free(last_node_path);
            }
            last_node_path = gtk_tree_model_get_path(
                    GTK_TREE_MODEL(settings_model), &iter);
            ui_settings_set_central_widget(callback(settings_window));
        }
        if (name != NULL) {
            g_free(name);
        }
        if (parent_name != NULL) {
            g_free(parent_name);
        }
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
    return vice_gtk3_resource_check_button_new("SaveResourcesOnExit",
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
    return vice_gtk3_resource_check_button_new("ConfirmOnExit",
            "Confirm on exit");
}


/** \brief  Create the 'Pause on settings dialog' checkbox
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_pause_on_settings_checkbox(void)
{
    return vice_gtk3_resource_check_button_new(
            "PauseOnSettings",
            "Pause when showing settings");
}


/** \brief  Create empty tree model for the settings tree
 */
static void create_tree_model(void)
{
    settings_model = gtk_tree_store_new(NUM_COLUMNS,
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
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
    ui_settings_tree_node_t *nodes = NULL;
    int i;

    model = settings_model;

    switch (machine_class) {
        case VICE_MACHINE_C64:  /* fall through */
        case VICE_MACHINE_C64SC:
            nodes = main_nodes_c64;
            break;
        case VICE_MACHINE_C64DTV:
            nodes = main_nodes_c64dtv;
            break;
        case VICE_MACHINE_C128:
            nodes = main_nodes_c128;
            break;
        case VICE_MACHINE_SCPU64:
            nodes = main_nodes_scpu64;
            break;
        case VICE_MACHINE_VIC20:
            nodes = main_nodes_vic20;
            break;
        case VICE_MACHINE_PLUS4:
            nodes = main_nodes_plus4;
            break;
        case VICE_MACHINE_PET:
            nodes = main_nodes_pet;
            break;
        case VICE_MACHINE_CBM5x0:
            nodes = main_nodes_cbm5x0;
            break;
        case VICE_MACHINE_CBM6x0:
            nodes = main_nodes_cbm6x0;
            break;
        case VICE_MACHINE_VSID:
            nodes = main_nodes_vsid;
            break;
        default:
            fprintf(stderr,
                    "Error: %s:%d:%s(): unsupported machine_class %d\n",
                    __FILE__, __LINE__, __func__, machine_class);
            archdep_vice_exit(1);
            break;
    }

    for (i = 0; nodes[i].name != NULL; i++) {
        gtk_tree_store_append(model, &iter, NULL);
        gtk_tree_store_set(model, &iter,
                COLUMN_NAME, nodes[i].name,
                COLUMN_ID, nodes[i].id,
                COLUMN_CALLBACK, nodes[i].callback,
                -1);

        /* this bit will need proper recursion if we need more than two
         * levels of subitems */
        if (nodes[i].children != NULL) {
            int c;
            ui_settings_tree_node_t *list = nodes[i].children;

            for (c = 0; list[c].name != NULL; c++) {
                char buffer[256];

                /* mark items without callback with 'TODO' */
                //if (list[c].callback != NULL) {
                g_snprintf(buffer, 256, "%s", list[c].name);
                //} else {
                //    g_snprintf(buffer, 256, "TODO: %s", list[c].name);
                //}

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


static gboolean row_separator_func(GtkTreeModel *model,
                                   GtkTreeIter *iter,
                                   gpointer data)
{
    gchar *name = NULL;
    gboolean is_sep;

    gtk_tree_model_get(model, iter, COLUMN_NAME, &name, -1);
    is_sep = strcmp(name, "---") == 0;
    g_free(name);

    return is_sep;
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

    create_tree_model();
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(populate_tree_model()));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);
    gtk_tree_view_set_row_separator_func(GTK_TREE_VIEW(tree),
                                         row_separator_func,
                                         NULL,
                                         NULL);

    text_renderer = gtk_cell_renderer_text_new();
    text_column = gtk_tree_view_column_new_with_attributes(
            "item-name",
            text_renderer,
            "text", 0,
            NULL);
    /*    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), obj_column); */
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), text_column);

    /* apply CSS for keyboard navigation */
    vice_gtk3_css_add(tree, TREEVIEW_CSS);
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

    child = gtk_paned_get_child2(GTK_PANED(paned_widget));
    if (child != NULL) {
        gtk_widget_destroy(child);
    }
    gtk_paned_pack2(GTK_PANED(paned_widget), widget, TRUE, FALSE);
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
 * \return  GtkGrid
 */
static GtkWidget *create_content_widget(GtkWidget *widget)
{
    GtkTreeSelection *selection;
    GtkWidget *extra;

    settings_grid = gtk_grid_new();
    settings_tree = create_treeview();

    /* pack the tree in a scrolled window to allow scrolling of the tree when
     * it gets too large for the dialog
     */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), settings_tree);

    /* pack the tree and the settings 'page' into a GtkPaned so we can resize
     * the tree */
    paned_widget = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_wide_handle(GTK_PANED(paned_widget), TRUE);

#if 0
    gtk_grid_attach(GTK_GRID(settings_grid), scrolled_window, 0, 0, 1, 1);
#else
    gtk_paned_pack1(GTK_PANED(paned_widget), scrolled_window, FALSE, FALSE);
#endif
#if 0
    gtk_grid_attach(GTK_GRID(settings_grid), scrolled_window, 0, 0, 1, 1);
#endif
    gtk_grid_attach(GTK_GRID(settings_grid), paned_widget, 0, 0, 1, 1);


    /* Remember the previously selected setting/widget and set it here */

    /* do we have a previous settings "page"? */
    if (last_node_path == NULL) {
        /* nope, display the default one */
        ui_settings_set_central_widget(ui_settings_inital_widget(widget));
    } else {
        /* try to restore the page last shown */
        GtkTreeIter iter;
        if (gtk_tree_model_get_iter(GTK_TREE_MODEL(settings_model), &iter,
                    last_node_path)) {
            GtkWidget *(*callback)(GtkWidget *) = NULL;
            gtk_tree_model_get(
                    GTK_TREE_MODEL(settings_model), &iter,
                    COLUMN_CALLBACK, &callback, -1);
            if (callback != NULL) {

                selection = gtk_tree_view_get_selection(
                        GTK_TREE_VIEW(settings_tree));

                ui_settings_set_central_widget(callback(widget));
                gtk_tree_view_expand_to_path(
                        GTK_TREE_VIEW(settings_tree),
                        last_node_path);
                gtk_tree_selection_select_path(selection, last_node_path);

            }
        }
    }

    /* create container for generic settings */
    extra = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(extra), 8);
    gtk_grid_set_row_spacing(GTK_GRID(extra), 8);
    g_object_set(extra, "margin", 16, NULL);

    gtk_grid_attach(GTK_GRID(extra), create_save_on_exit_checkbox(),
            0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(extra), create_confirm_on_exit_checkbox(),
            0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(extra), create_pause_on_settings_checkbox(),
            0, 2, 1, 1);

    /* add to main layout */
    gtk_grid_attach(GTK_GRID(settings_grid), extra, 0, 2, 2, 1);

    gtk_widget_show(settings_grid);
    gtk_widget_show(settings_tree);

    gtk_widget_set_size_request(scrolled_window, 250, 500);
    gtk_widget_set_size_request(settings_grid, DIALOG_WIDTH, DIALOG_HEIGHT);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(settings_tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect_unlocked(G_OBJECT(selection), "changed",
            G_CALLBACK(on_tree_selection_changed), NULL);

    /* handler for the double click event on a node */
    g_signal_connect(settings_tree, "row-activated",
            G_CALLBACK(on_row_activated), NULL);

    return settings_grid;
}


/** \brief  Handler for the 'response' event of the settings dialog
 *
 * This determines what to do based on the 'reponse ID' emitted by the dialog.
 *
 * \param[in,out]   widget      settings dialog
 * \param[in]       response_id response ID
 * \param[in]       user_data   extra data (unused)
 */
static void response_callback(GtkWidget *widget,
                              gint response_id,
                              gpointer user_data)
{
    int pause_on_settings;

    switch (response_id) {

        /* close dialog */
        case GTK_RESPONSE_DELETE_EVENT:
            gtk_widget_destroy(widget);
            settings_window = NULL;

            resources_get_int("PauseOnSettings", &pause_on_settings);
            if (pause_on_settings) {
                if (settings_old_pause_state) {
                    ui_pause_enable();
                } else {
                    ui_pause_disable();
                }
            }

            break;

        /* reset resources in current central widget to the state they were
         * in before entering the (sub)dialog */
        case RESPONSE_RESET:
            break;

        /* restore resources in (sub)dialog to factory settings */
        case RESPONSE_FACTORY:
            break;

        default:
            break;
    }
}


/** \brief  Respond to window size changes
 *
 * This allows for quickly seeing if specific dialog is getting too large.
 *
 * The DIALOG_WIDTH_MAX and DIALOG_HEIGHT_MAX I sucked out of my thumb, since
 * due to window managers using different themes, we can't use 'proper' values,
 * so I had to use approximate values.
 *
 * \param[in]   widget  a GtkWindow
 * \param[in]   event   the GDK event
 * \param[in]   data    extra event data (unused)
 *
 * \return  boolean
 */
static gboolean on_dialog_configure_event(GtkWidget *widget,
                                          GdkEvent *event,
                                          gpointer data)
{
#if 0
    if (event->type == GDK_CONFIGURE) {
        GdkEventConfigure *cfg = (GdkEventConfigure *)event;
        int width = cfg->width;
        int height = cfg->height;

        /* debug_gtk3("width %d, height %d.", width, height); */
        if (width > DIALOG_WIDTH_MAX || height > DIALOG_HEIGHT_MAX) {
            /* uncomment the following to get some 'help' while building
             * new dialogs: */
#if 0
            gtk_window_set_title(GTK_WINDOW(widget),
                    "HELP! --- DIALOG IS TOO BLOODY LARGE -- ERROR!");
#endif
            debug_gtk3("Dialog is too large: %dx%d (max: %dx%d).",
                    width, height, DIALOG_WIDTH_MAX, DIALOG_HEIGHT_MAX);
        }
#if 0
        debug_gtk3("XPOS: %d - YPOS: %d", cfg->x, cfg->y);
#endif
    }
#endif
    return FALSE;
}



/** \brief  Dialog create helper
 *
 * \return  Settings dialog
 */
static GtkWidget *dialog_create_helper(void)
{
    GtkWidget *dialog;
    GtkWidget *content;
    char title[256];

    vsync_suspend_speed_eval();

    g_snprintf(title, sizeof(title), "%s Settings", machine_name);

    dialog = gtk_dialog_new_with_buttons(
            title,
            ui_get_active_window(),
            GTK_DIALOG_MODAL,
            /*
             * Temp. disabled due to code freeze, will work on this again
             * after the code freeze -- compyx
             */
#if 0
            "Revert changes", RESPONSE_RESET,
            "Factory reset", RESPONSE_FACTORY,
#endif
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    g_object_set(content, "border-width", 8, NULL);
    gtk_container_add(GTK_CONTAINER(content), create_content_widget(dialog));

    /* set default response to Close */
    gtk_dialog_set_default_response(
            GTK_DIALOG(dialog),
            GTK_RESPONSE_DELETE_EVENT);

    gtk_window_set_resizable(GTK_WINDOW(dialog), TRUE);
    g_signal_connect_unlocked(dialog, "response", G_CALLBACK(response_callback), NULL);
    g_signal_connect_unlocked(dialog, "configure-event",
            G_CALLBACK(on_dialog_configure_event), NULL);
    g_signal_connect_unlocked(dialog, "destroy", G_CALLBACK(on_settings_dialog_destroy),
            NULL);

    return dialog;
}


#if 0
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
 *
 * \return  TRUE (avoids the key press getting passed to the emulated machine)
 */
static gboolean ui_settings_dialog_create(GtkWidget *widget, gpointer user_data)
{

    ui_settings_dialog_create_and_activate_node(NULL);

    return TRUE;
}
#endif


/** \brief  Clean up resources used on emu exit
 *
 * This function cleans up the data used to present the user with the last used
 * settings page.
 *
 * \note    Do NOT call this when exiting the settings UI, the event handlers
 *          will take care of cleaning up resources used by the UI.
 */
void ui_settings_shutdown(void)
{
    if (last_node_path != NULL) {
        gtk_tree_path_free(last_node_path);
        last_node_path = NULL;
    }
}


/** \brief  Find and activate node in the tree view via \a path
 *
 * The \a path argument is expected to be in the form 'foo/bar/bah', each
 * path item indicates a node in the tree view/model. For example:
 * "display/vdc" would select the VDC settings dialog on x128, but would fail
 * on any other machine.
 *
 * \param[in]   path    path to the node
 *
 * \return  bool
 */
gboolean ui_settings_dialog_activate_node(const char *path)
{
    GtkTreeIter iter;
    gchar **parts;
    const gchar *part;
    int column = 0;

    if (settings_window == NULL) {
        log_error(LOG_ERR, "settings dialog node activation requested without"
                " the dialog active.");
        return FALSE;
    }
    if (path == NULL || *path == '\0') {
        log_error(LOG_ERR, "NULL or empty path pased.");
        return FALSE;
    }

    /* split path into parts */
    parts = g_strsplit(path, "/", 0);
    part = parts[0];

    /* get first item in model */
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(settings_model), &iter);

    /* iterate the parts of the path, trying to find to requested node */
    while (part != NULL) {

        const gchar *node_id = NULL;

        /* iterate nodes until either 'part' is found or the nodes in the
         * current 'column' run out */
        while (TRUE) {
            gtk_tree_model_get(GTK_TREE_MODEL(settings_model), &iter,
                    COLUMN_ID, &node_id, -1);

            /* check node ID against currently sought part of the path */
            if (strcmp(node_id, part) == 0) {
                /* got the requested node */
                if (parts[column + 1] == NULL) {
                    /* got final item */
                    GtkTreeSelection *selection;
                    GtkTreePath *tree_path;

                    selection = gtk_tree_view_get_selection(
                            GTK_TREE_VIEW(settings_tree));
                    tree_path = gtk_tree_model_get_path(
                            GTK_TREE_MODEL(settings_model), &iter);
                    gtk_tree_view_expand_to_path(
                            GTK_TREE_VIEW(settings_tree), tree_path);
                    gtk_tree_selection_select_path(selection, tree_path);

                    gtk_tree_path_free(tree_path);
                    g_strfreev(parts);
                    return TRUE;
                } else {
                    /* continue searching, dive into the children of the
                     * current node, if there are any */
                    if (gtk_tree_model_iter_has_child(
                                GTK_TREE_MODEL(settings_model), &iter)) {
                        /* node has children, iterate those now */
                        GtkTreeIter child;

                        if (!gtk_tree_model_iter_nth_child(
                                    GTK_TREE_MODEL(settings_model),
                                    &child, &iter, 0)) {
                            g_strfreev(parts);
                            return FALSE;
                        }
                        /* set iterator to first child node, update the index
                         * in the path parts */
                        iter = child;
                        part = parts[++column];
                        continue;
                    } else {
                        /* oops */
                        debug_gtk3("error: path '%s' continues into '%s' but"
                                " there are no child nodes.", path, part);
                        g_strfreev(parts);
                        return FALSE;
                    }
                }
            } else {
                /* is there another node to inspect? */
                if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(settings_model),
                                              &iter)) {
                    /* couldn't find the requested node, exit */
                    debug_gtk3("failed to find node at path '%s'.", path);
                    g_strfreev(parts);
                    return FALSE;
                }
            }
        }
    }

    debug_gtk3("warning: should never get here.");
    g_strfreev(parts);
    return FALSE;
}


/** \brief  Threaded UI handler for the settings dialog constructor
 *
 * \param[in]   user_data   path to active node in the treeview
 *
 * \return  FALSE
 */
static gboolean ui_settings_dialog_create_and_activate_node_impl(gpointer user_data)
{
    const char *path = (const char *)user_data;
    GtkWidget *dialog;

    dialog = dialog_create_helper();
    settings_window = dialog;

    /* find and activate the node */
    if (path && !ui_settings_dialog_activate_node(path)) {
        debug_gtk3("failed to locate node, showing dialog anyway for now.");
    }

    gtk_widget_show_all(dialog);

    return FALSE;
}


/** \brief  Show settings main dialog and activate a node
 *
 * \param[in]   path    NULL or path to name ("foo/bar/blah")
 *
 * \return  TRUE which means nothing.
 */
gboolean ui_settings_dialog_create_and_activate_node(const char *path)
{
    /* call from ui thread without locking - creating the settings dialog is heavy */
    gdk_threads_add_timeout(0, ui_settings_dialog_create_and_activate_node_impl, (gpointer)path);

    return TRUE;
}


/** \brief  Menu callback for the settings dialog
 *
 * Opens the main settings dialog and activates the previously activate node,
 * if any.
 *
 * \param[in]   widget      unused
 * \param[in]   user_data   path to previously active node
 *
 * \return  TRUE
 */
gboolean ui_settings_dialog_create_and_activate_node_callback(
        GtkWidget *widget,
        gpointer user_data)
{
    int pause_on_settings;

    settings_old_pause_state = ui_pause_active();

    resources_get_int("PauseOnSettings", &pause_on_settings);
    if (pause_on_settings) {
        ui_pause_enable();
    }
    ui_settings_dialog_create_and_activate_node((const char *)user_data);

    return TRUE;
}
