/*
 * c128ui.cc - C128-specific user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include "mouse.h"
#include "resources.h"
#include "statusbar.h"
#include "types.h"
#include "ui.h"
#include "ui_file.h"
#include "ui_ide64.h"
#include "ui_vicii.h"
#include "util.h"
#include "viceapp.h"
#include "vicewindow.h"
}

extern ViceWindow *windowlist[];

ui_menu_toggle  c128_ui_menu_toggles[] = {
    { "VICIIDoubleSize", MENU_TOGGLE_DOUBLESIZE },
    { "VICIIDoubleScan", MENU_TOGGLE_DOUBLESCAN },
    { "VICIIVideoCache", MENU_TOGGLE_VIDEOCACHE },
    { "REU", MENU_TOGGLE_REU },
    { "REUImageWrite", MENU_TOGGLE_REU_SWC },
    { "GEORAM", MENU_TOGGLE_GEORAM },
    { "GEORAMImageWrite", MENU_TOGGLE_GEORAM_SWC },
    { "RAMCART", MENU_TOGGLE_RAMCART },
    { "RAMCARTImageWrite", MENU_TOGGLE_RAMCART_SWC },
    { "DIGIMAX", MENU_TOGGLE_DIGIMAX },
    { "IEEE488", MENU_TOGGLE_IEEE488 },
    { "Mouse", MENU_TOGGLE_MOUSE },
    { "VDCDoubleSize", MENU_TOGGLE_VDC_DOUBLESIZE },
    { "VDCDoubleScan", MENU_TOGGLE_VDC_DOUBLESCAN },
    { "VDC64KB", MENU_TOGGLE_VDC64KB },
    { "C128FullBanks", MENU_TOGGLE_C128FULLBANKS },
    { "SFXSoundExpander", MENU_TOGGLE_SFX_SE },
    { "SFXSoundSampler", MENU_TOGGLE_SFX_SS },
    { "EasyFlashJumper", MENU_TOGGLE_EASYFLASH_JUMPER },
    { "EasyFlashWriteCRT", MENU_TOGGLE_EASYFLASH_AUTOSAVE },
    { "MagicVoiceCartridgeEnabled", MENU_TOGGLE_MAGICVOICE },
    { "IDE64version4", MENU_TOGGLE_IDE64_V4 },
    { "IDE64AutodetectSize", MENU_TOGGLE_IDE64_AUTODETECT },
    { "ExpertCartridgeEnabled", MENU_TOGGLE_EXPERT },
    { "ExpertImageWrite", MENU_TOGGLE_EXPERT_SWC },
    { NULL, 0 }
};

ui_res_possible_values c128ReuSize[] = {
    { 128, MENU_REU_SIZE_128 },
    { 256, MENU_REU_SIZE_256 },
    { 512, MENU_REU_SIZE_512 },
    { 1024, MENU_REU_SIZE_1024 },
    { 2048, MENU_REU_SIZE_2048 },
    { 4096, MENU_REU_SIZE_4096 },
    { 8192, MENU_REU_SIZE_8192 },
    { 16384, MENU_REU_SIZE_16384 },
    { -1, 0 }
};

ui_res_possible_values c128GeoRAMSize[] = {
    { 64, MENU_GEORAM_SIZE_64 },
    { 128, MENU_GEORAM_SIZE_128 },
    { 256, MENU_GEORAM_SIZE_256 },
    { 512, MENU_GEORAM_SIZE_512 },
    { 1024, MENU_GEORAM_SIZE_1024 },
    { 2048, MENU_GEORAM_SIZE_2048 },
    { 4096, MENU_GEORAM_SIZE_4096 },
    { -1, 0 }
};

ui_res_possible_values c128RamCartSize[] = {
    { 64, MENU_RAMCART_SIZE_64 },
    { 128, MENU_RAMCART_SIZE_128 },
    { -1, 0 }
};

ui_res_possible_values c128DigimaxBase[] = {
    { 0xdd00, MENU_DIGIMAX_BASE_DD00 },
    { 0xde00, MENU_DIGIMAX_BASE_DE00 },
    { 0xde20, MENU_DIGIMAX_BASE_DE20 },
    { 0xde40, MENU_DIGIMAX_BASE_DE40 },
    { 0xde60, MENU_DIGIMAX_BASE_DE60 },
    { 0xde80, MENU_DIGIMAX_BASE_DE80 },
    { 0xdea0, MENU_DIGIMAX_BASE_DEA0 },
    { 0xdec0, MENU_DIGIMAX_BASE_DEC0 },
    { 0xdee0, MENU_DIGIMAX_BASE_DEE0 },
    { 0xdf00, MENU_DIGIMAX_BASE_DF00 },
    { 0xdf20, MENU_DIGIMAX_BASE_DF20 },
    { 0xdf40, MENU_DIGIMAX_BASE_DF40 },
    { 0xdf60, MENU_DIGIMAX_BASE_DF60 },
    { 0xdf80, MENU_DIGIMAX_BASE_DF80 },
    { 0xdfa0, MENU_DIGIMAX_BASE_DFA0 },
    { 0xdfc0, MENU_DIGIMAX_BASE_DFC0 },
    { 0xdfe0, MENU_DIGIMAX_BASE_DFE0 },
    { -1, 0 }
};

ui_res_possible_values c128SFXSoundExpanderChip[] = {
    { 3526, MENU_SFX_SE_3526 },
    { 3812, MENU_SFX_SE_3812 },
    { -1, 0 }
};

ui_res_possible_values c128viciiBorders[] = {
    { 0, MENU_VICII_BORDERS_NORMAL },
    { 1, MENU_VICII_BORDERS_FULL} ,
    { 2, MENU_VICII_BORDERS_DEBUG },
    { -1, 0 }
};

ui_res_possible_values c128VDCrev[] = {
    { 0, MENU_VDC_REV_0 },
    { 1, MENU_VDC_REV_1 },
    { 2, MENU_VDC_REV_2 },
    { -1, 0 }
};

ui_res_possible_values c128mousetypes[] = {
    { MOUSE_TYPE_1351, MENU_MOUSE_TYPE_1351 },
    { MOUSE_TYPE_NEOS, MENU_MOUSE_TYPE_NEOS },
    { MOUSE_TYPE_AMIGA, MENU_MOUSE_TYPE_AMIGA },
    { MOUSE_TYPE_PADDLE, MENU_MOUSE_TYPE_PADDLE },
    { -1, 0 }
};

ui_res_possible_values c128mouseports[] = {
    { 1, MENU_MOUSE_PORT_JOY1 },
    { 2, MENU_MOUSE_PORT_JOY2 },
    { -1, 0 }
};

ui_res_possible_values c128ExpertModes[] = {
    { 0, MENU_EXPERT_MODE_OFF },
    { 1, MENU_EXPERT_MODE_PRG },
    { 2, MENU_EXPERT_MODE_ON },
    { -1, 0 }
};

ui_res_value_list c128_ui_res_values[] = {
    { "REUsize", c128ReuSize },
    { "GeoRAMsize", c128GeoRAMSize },
    { "RAMCARTsize", c128RamCartSize },
    { "DIGIMAXbase", c128DigimaxBase },
    { "SFXSoundExpanderChip", c128SFXSoundExpanderChip },
    { "VICIIBorderMode", c128viciiBorders },
    { "VDCRevision", c128VDCrev },
    { "Mousetype", c128mousetypes },
    { "Mouseport", c128mouseports },
    { "ExpertCartridgeMode", c128ExpertModes },
    { NULL, NULL }
};


void c128_ui_specific(void *msg, void *window)
{
    switch (((BMessage*)msg)->what) {
          case MENU_VICII_SETTINGS:
            ui_vicii();
                break;
        case MENU_REU_FILE:
            ui_select_file(windowlist[0]->savepanel,REU_FILE,(void*)0);
            break;
        case MENU_GEORAM_FILE:
            ui_select_file(windowlist[0]->savepanel,GEORAM_FILE,(void*)0);
            break;
        case MENU_RAMCART_FILE:
            ui_select_file(windowlist[0]->savepanel,RAMCART_FILE,(void*)0);
            break;
        case MENU_IDE64_FILE1:
            ui_select_file(windowlist[0]->savepanel, IDE64_FILE1, (void*)0);
            break;
        case MENU_IDE64_FILE2:
            ui_select_file(windowlist[0]->savepanel, IDE64_FILE2, (void*)0);
            break;
        case MENU_IDE64_FILE3:
            ui_select_file(windowlist[0]->savepanel, IDE64_FILE3, (void*)0);
            break;
        case MENU_IDE64_FILE4:
            ui_select_file(windowlist[0]->savepanel, IDE64_FILE4, (void*)0);
            break;
        case MENU_IDE64_CUSTOM_SIZE:
            ui_ide64();
            break;
        case MENU_EXPERT_FILE:
            ui_select_file(windowlist[0]->filepanel, EXPERT_FILE, (void*)0);
            break;
        case MENU_EASYFLASH_SAVE_NOW:
            if (cartridge_flush_image(CARTRIDGE_EASYFLASH) < 0) {
                ui_error("Error saving EasyFlash .crt file");
            }
            break;
        default:
            break;
    }
}

extern "C" {
int c128ui_init(void)
{
    ui_register_machine_specific(c128_ui_specific);
    ui_register_menu_toggles(c128_ui_menu_toggles);
    ui_register_res_values(c128_ui_res_values);
    ui_update_menus();
    return 0;
}

void c128ui_shutdown(void)
{
}

}
