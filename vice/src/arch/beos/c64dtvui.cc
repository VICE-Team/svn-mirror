/*
 * c64dtvui.cc - C64DTV-specific user interface.
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

#include <Alert.h>
#include <Application.h>
#include <FilePanel.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <TextView.h>
#include <View.h>
#include <Window.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__BEOS__) && defined(WORDS_BIGENDIAN)
#include <string.h>
#endif

extern "C" {
#include "archdep.h"
#include "cartridge.h"
#include "constants.h"
#include "keyboard.h"
#include "resources.h"
#include "statusbar.h"
#include "types.h"
#include "ui.h"
#include "ui_file.h"
#include "ui_vicii.h"
#include "util.h"
#include "viceapp.h"
#include "vicewindow.h"
}

extern ViceWindow *windowlist[];

ui_menu_toggle  c64dtv_ui_menu_toggles[]={
    { "VICIIDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "c64dtvromrw", MENU_TOGGLE_C64DTV_WRITE_ENABLE },
    { "HummerUserportJoy", MENU_TOGGLE_HUMMER_USERPORT_JOY },
    { "Mouse", MENU_TOGGLE_MOUSE },
    { "ps2mouse", MENU_TOGGLE_PS2MOUSE },
    { "PALEmulation", MENU_TOGGLE_FASTPAL },
    { "VICIIScale2x", MENU_TOGGLE_SCALE2X },
    { NULL, 0 }
};

ui_res_possible_values C64DTVRevision[] = {
        {2, MENU_C64DTV_REVISION_2},
        {3, MENU_C64DTV_REVISION_3},
        {-1, 0}
};

ui_res_possible_values HummerJoyPort[] = {
        {1, MENU_HUMMER_JOY_PORT_1},
        {2, MENU_HUMMER_JOY_PORT_2},
        {-1, 0}
};
ui_res_possible_values c64dtvviciiBorders[] = {
        {0, MENU_VICII_BORDERS_NORMAL},
        {1, MENU_VICII_BORDERS_FULL},
        {2, MENU_VICII_BORDERS_DEBUG},
        {-1, 0}
};

ui_res_value_list c64dtv_ui_res_values[] = {
    {"DtvRevision", C64DTVRevision},
    {"HummerUserportJoyPort", HummerJoyPort},
    {"VICIIBorderMode", c64dtvviciiBorders},
    {NULL,NULL}
};

void c64dtv_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
        case MENU_VICII_SETTINGS:
            ui_vicii();
            break;
        case MENU_C64DTV_ROM_FILE:
            ui_select_file(windowlist[0]->savepanel,C64DTV_ROM_FILE,(void*)0);
            break;
    	default: ;
    }
}

extern "C" {
int c64dtvui_init(void)
{
    ui_register_machine_specific(c64dtv_ui_specific);
    ui_register_menu_toggles(c64dtv_ui_menu_toggles);
    ui_register_res_values(c64dtv_ui_res_values);
    ui_update_menus();
    return 0;
}

void c64dtvui_shutdown(void)
{
}

}
