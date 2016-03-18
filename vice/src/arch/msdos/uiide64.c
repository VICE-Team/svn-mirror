/*
 * uiide64.c - IDE64 UI interface for MS-DOS.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "ide64.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uiide64.h"

TUI_MENU_DEFINE_RADIO(IDE64version)
TUI_MENU_DEFINE_TOGGLE(IDE64RTCSave)
TUI_MENU_DEFINE_TOGGLE(IDE64USBServer)
TUI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize1)
TUI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize2)
TUI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize3)
TUI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize4)
TUI_MENU_DEFINE_FILENAME(IDE64Image1, "IDE64 primary master")
TUI_MENU_DEFINE_FILENAME(IDE64Image2, "IDE64 primary slave")
TUI_MENU_DEFINE_FILENAME(IDE64Image3, "IDE64 secondary master")
TUI_MENU_DEFINE_FILENAME(IDE64Image4, "IDE64 secondary slave")

static TUI_MENU_CALLBACK(ui_set_cylinders_callback)
{
    int num = (int)param;

    if (been_activated) {
        int current_cyls, value;
        char buf[10];

        resources_get_int_sprintf("IDE64Cylinders%i", &current_cyls, num);
        sprintf(buf, "%d", current_cyls);

        if (tui_input_string("Cylinders", "Enter the amount of cylinders (1-1024):", buf, 10) == 0) {
            value = atoi(buf);
            if (value > 65535) {
                value = 65535;
            } else if (value < 1) {
                value = 1;
            }
            resources_set_int_sprintf("IDE64Cylinders%i", value, num);
        } else {
            return NULL;
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(ui_set_heads_callback)
{
    int num = (int)param;

    if (been_activated) {
        int current_heads, value;
        char buf[10];

        resources_get_int_sprintf("IDE64Heads%i", &current_heads, num);
        sprintf(buf, "%d", current_heads);

        if (tui_input_string("Heads", "Enter the amount of heads (1-16):", buf, 10) == 0) {
            value = atoi(buf);
            if (value > 16) {
                value = 16;
            } else if (value < 1) {
                value = 1;
            }
            resources_set_int_sprintf("IDE64Heads", value, num);
        } else {
            return NULL;
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(ui_set_sectors_callback)
{
    int num = (int)param;

    if (been_activated) {
        int current_sectors, value;
        char buf[10];

        resources_get_int_sprintf("IDE64Sectors", &current_sectors, num);
        sprintf(buf, "%d", current_sectors);

        if (tui_input_string("Sectors", "Enter the amount of sectors (1-63):", buf, 10) == 0) {
            value = atoi(buf);
            if (value > 63) {
                value = 63;
            } else if (value < 1) {
                value = 1;
            }
            resources_set_int_sprintf("IDE64Sectors", value, num);
        } else {
            return NULL;
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(ui_ide64_usbserver_address_callback)
{
    if (been_activated) {
      const char *current_address;
      char buf[44];

      resources_get_string("IDE64USBServerAddress", &current_address);

      strncpy(buf, current_address, 40);

      if (tui_input_string("Address", "Enter the address of USB server:", buf, 40) == 0) {
          resources_set_string("IDE64USBServerAddress", buf);
          tui_message("Address set to : %s", buf);
      } else {
          return NULL;
      }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(ide64_version_submenu_callback)
{
    int value;
    static char s[10];

    resources_get_int("IDE64version", &value);
    switch (value) {
        default:
            case IDE64_VERSION_3:
                strcpy(s, "V3");
                break;
            case IDE64_VERSION_4_1:
                strcpy(s, "V4.1");
                break;
            case IDE64_VERSION_4_2:
                strcpy(s, "V4.2");
                break;
    }
    return s;
}

static tui_menu_item_def_t ide64_hd1_menu_items[] = {
    { "IDE64 primary master image file:", "Select the IDE64 primary master image file",
      filename_IDE64Image1_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Autodetect geometry:", "Autodetect the geometry",
      toggle_IDE64AutodetectSize1_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cylinders",
      "Set the amount of cylinders",
      ui_set_cylinders_callback, (void *)1, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Heads",
      "Set the amount of heads",
      ui_set_heads_callback, (void *)1, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Sectors",
      "Set the amount of sectors",
      ui_set_sectors_callback, (void *)1, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t ide64_hd2_menu_items[] = {
    { "IDE64 primary slave image file:", "Select the IDE64 primary slave image file",
      filename_IDE64Image2_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Autodetect geometry:", "Autodetect the geometry",
      toggle_IDE64AutodetectSize2_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cylinders",
      "Set the amount of cylinders",
      ui_set_cylinders_callback, (void *)2, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Heads",
      "Set the amount of heads",
      ui_set_heads_callback, (void *)2, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Sectors",
      "Set the amount of sectors",
      ui_set_sectors_callback, (void *)2, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t ide64_hd3_menu_items[] = {
    { "IDE64 secondary master image file:", "Select the IDE64 secondary master image file",
      filename_IDE64Image3_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Autodetect geometry:", "Autodetect the geometry",
      toggle_IDE64AutodetectSize3_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cylinders",
      "Set the amount of cylinders",
      ui_set_cylinders_callback, (void *)3, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Heads",
      "Set the amount of heads",
      ui_set_heads_callback, (void *)3, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Sectors",
      "Set the amount of sectors",
      ui_set_sectors_callback, (void *)3, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t ide64_hd4_menu_items[] = {
    { "IDE64 secondary slave image file:", "Select the IDE64 secondary slave image file",
      filename_IDE64Image4_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "_Autodetect geometry:", "Autodetect the geometry",
      toggle_IDE64AutodetectSize4_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Cylinders",
      "Set the amount of cylinders",
      ui_set_cylinders_callback, (void *)4, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Heads",
      "Set the amount of heads",
      ui_set_heads_callback, (void *)4, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Sectors",
      "Set the amount of sectors",
      ui_set_sectors_callback, (void *)4, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

TUI_MENU_DEFINE_TOGGLE(SBDIGIMAX)
TUI_MENU_DEFINE_RADIO(SBDIGIMAXbase)

static TUI_MENU_CALLBACK(digimax_address_submenu_callback)
{
    int value;
    static char s[10];

    resources_get_int("SBDIGIMAXbase", &value);
    switch (value) {
        default:
        case 0xde40:
            strcpy(s, "$DE40");
            break;
        case 0xde48:
            strcpy(s, "$DE48");
            break;
    }
    return s;
}

static tui_menu_item_def_t digimax_address_submenu[] = {
    { "$DE4_0", NULL, radio_SBDIGIMAXbase_callback,
      (void *)0xde40, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "$DE4_8", NULL, radio_SBDIGIMAXbase_callback,
      (void *)0xde48, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t ide64_shortbus_menu_items[] = {
    { "_DigiMAX device:", "Enable DigiMAX device",
      toggle_SBDIGIMAX_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Digimax _address:", "Select base address of the DigiMAX device",
      digimax_address_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, digimax_address_submenu,
      "IDE64 revision" },
    { NULL }
};

static tui_menu_item_def_t ide64_version_submenu[] = {
    { "V_3", NULL, radio_IDE64version_callback,
      (void *)IDE64_VERSION_3, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "V4._1", NULL, radio_IDE64version_callback,
      (void *)IDE64_VERSION_4_1, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "V4._2", NULL, radio_IDE64version_callback,
      (void *)IDE64_VERSION_4_2, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t ide64_menu_items[] = {
    { "IDE64 _revision:", "Select revision of IDE64",
      ide64_version_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, ide64_version_submenu,
      "IDE64 revision" },
    { "Save IDE64 RTC data when changed:", "Save IDE64 RTC data when changed",
      toggle_IDE64RTCSave_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#ifdef HAVE_NETWORK
    { "_USB server:", "Enable USB server",
      toggle_IDE64USBServer_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "USB server _address",
      "Set USB server address",
      ui_ide64_usbserver_address_callback, NULL, 30,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
#endif
    { "Shortbus devices:", "Shortbus devices",
      NULL, NULL, 11,
      TUI_MENU_BEH_CONTINUE, ide64_shortbus_menu_items,
      "Shortbus devices" },
    { "IDE64 primary master settings:", "Primary master settings",
      NULL, NULL, 11,
      TUI_MENU_BEH_CONTINUE, ide64_hd1_menu_items,
      "Primary master settings" },
    { "IDE64 primary slave settings:", "Primary slave settings",
      NULL, NULL, 11,
      TUI_MENU_BEH_CONTINUE, ide64_hd2_menu_items,
      "Primary slave settings" },
    { "IDE64 secondary master settings:", "Secondary master settings",
      NULL, NULL, 11,
      TUI_MENU_BEH_CONTINUE, ide64_hd3_menu_items,
      "Secondary master settings" },
    { "IDE64 secondary slave settings:", "Secondary slave settings",
      NULL, NULL, 11,
      TUI_MENU_BEH_CONTINUE, ide64_hd4_menu_items,
      "Secondary slave settings" },
    { NULL }
};

void uiide64_init(struct tui_menu *parent_submenu)
{
    tui_menu_t ui_ide64_submenu;

    ui_ide64_submenu = tui_menu_create("IDE64 settings", 1);

    tui_menu_add(ui_ide64_submenu, ide64_menu_items);

    tui_menu_add_submenu(parent_submenu, "_IDE64 settings...",
                         "IDE64 settings",
                         ui_ide64_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}
