/*
 * uiide64.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <stdlib.h>

#include "lib.h"
#include "resources.h"
#include "uiapi.h"
#include "uiide64.h"
#include "uilib.h"
#include "uimenu.h"
#include "vsync.h"

UI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize1)
UI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize2)
UI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize3)
UI_MENU_DEFINE_TOGGLE(IDE64AutodetectSize4)
UI_MENU_DEFINE_RADIO(IDE64version4)

static UI_CALLBACK(set_ide64_image_name)
{
    uilib_select_file((char *)UI_MENU_CB_PARAM, _("IDE64 interface image"), UILIB_FILTER_HD_IMAGE);
}

static UI_CALLBACK(set_cylinders)
{
    static char input_string[32];
    int num = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (CHECK_MENUS) {
        int autosize;

        resources_get_int_sprintf("IDE64AutodetectSize%i", &autosize, num);

        if (autosize) {
            ui_menu_set_sensitive(w, 0);
        } else {
            ui_menu_set_sensitive(w, 1);
        }
    } else {
        char *msg_string;
        ui_button_t button;
        int i;
        int cylinders;

        vsync_suspend_speed_eval();

        resources_get_int_sprintf("IDE64Cylinders%i", &cylinders, num);

        sprintf(input_string, "%d", cylinders);

        msg_string = lib_stralloc(_("Enter number of cylinders"));
        button = ui_input_string(_("Cylinders"), msg_string, input_string, 32);
        lib_free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            if (cylinders > 0 && cylinders <= 65535 && cylinders != i) {
                resources_set_int_sprintf("IDE64Cylinders%i", i, num);
                ui_update_menus();
            }
        }
    }
}

static UI_CALLBACK(set_heads)
{
    static char input_string[32];
    int num = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (CHECK_MENUS) {
        int autosize;

        resources_get_int_sprintf("IDE64AutodetectSize%i", &autosize, num);

        if (autosize) {
            ui_menu_set_sensitive(w, 0);
        } else {
            ui_menu_set_sensitive(w, 1);
        }
    } else {
        char *msg_string;
        ui_button_t button;
        int i;
        int heads;

        vsync_suspend_speed_eval();

        resources_get_int_sprintf("IDE64Heads%i", &heads, num);

        sprintf(input_string, "%d", heads);

        msg_string = lib_stralloc(_("Enter number of heads"));
        button = ui_input_string(_("Heads"), msg_string, input_string, 32);
        lib_free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            if (heads > 0 && heads <= 16 && heads != i) {
                resources_set_int_sprintf("IDE64Heads%i", i, num);
                ui_update_menus();
            }
        }
    }
}

static UI_CALLBACK(set_sectors)
{
    static char input_string[32];
    int num = vice_ptr_to_int(UI_MENU_CB_PARAM);

    if (CHECK_MENUS) {
        int autosize;

        resources_get_int_sprintf("IDE64AutodetectSize%i", &autosize, num);

        if (autosize) {
            ui_menu_set_sensitive(w, 0);
        } else {
            ui_menu_set_sensitive(w, 1);
        }
    } else {
        char *msg_string;
        ui_button_t button;
        int i;
        int sectors;

        vsync_suspend_speed_eval();

        resources_get_int_sprintf("IDE64Sectors%i", &sectors, num);

        sprintf(input_string, "%d", sectors);

        msg_string = lib_stralloc(_("Enter number of sectors"));
        button = ui_input_string(_("Sectors"), msg_string, input_string, 32);
        lib_free(msg_string);
        if (button == UI_BUTTON_OK) {
            i = atoi(input_string);
            if (sectors > 0 && sectors <= 63 && sectors != i) {
                resources_set_int_sprintf("IDE64Sectors%i", i, num);
                ui_update_menus();
            }
        }
    }
}

static UI_CALLBACK(usbserver_select_addr)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("IDE64 USB setting"), _("USB server address:"));
}

static ui_menu_entry_t ide64_revision_submenu[] = {
    { N_("Version 3"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_IDE64version4,
      (ui_callback_data_t)0, NULL },
    { N_("Version 4"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_IDE64version4,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static ui_menu_entry_t ide64_hd1_submenu[] = {
    { N_("Device 1 image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_ide64_image_name,
      (ui_callback_data_t)"IDE64Image1", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Autodetect image size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IDE64AutodetectSize1, NULL, NULL },
    { N_("Cylinders"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_cylinders,
      (ui_callback_data_t)1, NULL },
    { N_("Heads"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_heads,
      (ui_callback_data_t)1, NULL },
    { N_("Sectors"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_sectors,
      (ui_callback_data_t)1, NULL },
    { NULL }
};

static ui_menu_entry_t ide64_hd2_submenu[] = {
    { N_("Device 2 image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_ide64_image_name,
      (ui_callback_data_t)"IDE64Image2", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Autodetect image size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IDE64AutodetectSize2, NULL, NULL },
    { N_("Cylinders"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_cylinders,
      (ui_callback_data_t)2, NULL },
    { N_("Heads"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_heads,
      (ui_callback_data_t)2, NULL },
    { N_("Sectors"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_sectors,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

static ui_menu_entry_t ide64_hd3_submenu[] = {
    { N_("Device 3 image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_ide64_image_name,
      (ui_callback_data_t)"IDE64Image3", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Autodetect image size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IDE64AutodetectSize3, NULL, NULL },
    { N_("Cylinders"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_cylinders,
      (ui_callback_data_t)3, NULL },
    { N_("Heads"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_heads,
      (ui_callback_data_t)3, NULL },
    { N_("Sectors"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_sectors,
      (ui_callback_data_t)3, NULL },
    { NULL }
};

static ui_menu_entry_t ide64_hd4_submenu[] = {
    { N_("Device 4 image name"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_ide64_image_name,
      (ui_callback_data_t)"IDE64Image4", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Autodetect image size"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IDE64AutodetectSize4, NULL, NULL },
    { N_("Cylinders"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_cylinders,
      (ui_callback_data_t)4, NULL },
    { N_("Heads"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_heads,
      (ui_callback_data_t)4, NULL },
    { N_("Sectors"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)set_sectors,
      (ui_callback_data_t)4, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(IDE64USBServer)
UI_MENU_DEFINE_TOGGLE(IDE64RTCSave)

ui_menu_entry_t ide64_submenu[] = {
    { N_("Revision"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_revision_submenu },
    { N_("Enable USB server"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IDE64USBServer, NULL, NULL },
    { N_("Set USB server address"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)usbserver_select_addr,
      (ui_callback_data_t)"IDE64USBServerAddress", NULL },
    { N_("Enable RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IDE64RTCSave, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Device 1 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_hd1_submenu },
    { N_("Device 2 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_hd2_submenu },
    { N_("Device 3 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_hd3_submenu },
    { N_("Device 4 settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ide64_hd4_submenu },
    { NULL }
};
