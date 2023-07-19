/*
 * menu_screenshot.c - SDL screenshot saving functions.
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

#include <stdlib.h>

#include "gfxoutput.h"
#include "lib.h"
#include "menu_common.h"
#include "menu_ffmpeg.h"
#include "menu_screenshot.h"
#include "resources.h"
#include "screenshot.h"
#include "types.h"
#include "ui.h"
#include "uifilereq.h"
#include "uimenu.h"
#include "util.h"
#include "videoarch.h"

UI_MENU_DEFINE_RADIO(OCPOversizeHandling)
UI_MENU_DEFINE_RADIO(OCPUndersizeHandling)
UI_MENU_DEFINE_RADIO(OCPMultiColorHandling)
UI_MENU_DEFINE_RADIO(OCPTEDLumHandling)
UI_MENU_DEFINE_RADIO(KoalaOversizeHandling)
UI_MENU_DEFINE_RADIO(KoalaUndersizeHandling)
UI_MENU_DEFINE_RADIO(KoalaTEDLumHandling)
UI_MENU_DEFINE_RADIO(MinipaintOversizeHandling)
UI_MENU_DEFINE_RADIO(MinipaintUndersizeHandling)
UI_MENU_DEFINE_RADIO(MinipaintTEDLumHandling)

static UI_MENU_CALLBACK(save_screenshot_callback)
{
    char title[32];
    char *name = NULL;
    int width;
    int height;
    gfxoutputdrv_t *selected_driver = NULL;

    if (activated) {
        sprintf(title, "Choose %s file", (char *)param);
        name = sdl_ui_file_selection_dialog(title, FILEREQ_MODE_SAVE_FILE);
        if (name != NULL) {
            width = sdl_active_canvas->draw_buffer->draw_buffer_width;
            height = sdl_active_canvas->draw_buffer->draw_buffer_height;
            memcpy(sdl_active_canvas->draw_buffer->draw_buffer, sdl_ui_get_draw_buffer(), width * height);
            selected_driver = gfxoutput_get_driver((char *)param);
            util_add_extension(&name, selected_driver->default_extension);
            if (screenshot_save((char *)param, name, sdl_active_canvas) < 0) {
                ui_error("Cannot save screenshot.");
            }
            lib_free(name);
        }
    }
    return NULL;
}

static const ui_menu_entry_t koala_tedlum_handling_menu[] = {
    {   .string   = "Ignore",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaTEDLumHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_TED_LUM_IGNORE
    },
    {   .string   = "Best cell colors",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaTEDLumHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_TED_LUM_DITHER
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_undersize_handling_menu[] = {
    {   .string   = "Scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaUndersizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_UNDERSIZE_SCALE
    },
    {   .string   = "Borderize",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaUndersizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_UNDERSIZE_BORDERIZE
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_oversize_handling_menu[] = {
    {   .string   = "Scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_SCALE
    },
    {   .string   = "Crop left top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_TOP
    },
    {   .string   = "Crop middle top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_TOP
    },
    {   .string   = "Crop right top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP
    },
    {   .string   = "Crop left center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER
    },
    {   .string   = "Crop middle center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER
    },
    {   .string   = "Crop right center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER
    },
    {   .string   = "Crop left bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM
    },
    {   .string   = "Crop middle bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM
    },
    {   .string   = "Crop right bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_KoalaOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_tedlum_handling_menu[] = {
    {   .string   = "Ignore",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintTEDLumHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_TED_LUM_IGNORE
    },
    {   .string   = "Best cell colors",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintTEDLumHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_TED_LUM_DITHER
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_undersize_handling_menu[] = {
    {   .string   = "Scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintUndersizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_UNDERSIZE_SCALE
    },
    {   .string   = "Borderize",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintUndersizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_UNDERSIZE_BORDERIZE
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_oversize_handling_menu[] = {
    {   .string   = "Scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_SCALE
    },
    {   .string   = "Crop left top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_TOP
    },
    {   .string   = "Crop middle top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_TOP
    },
    {   .string   = "Crop right top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP
    },
    {   .string   = "Crop left center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER
    },
    {   .string   = "Crop middle center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER
    },
    {   .string   = "Crop right center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER
    },
    {   .string   = "Crop left bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM
    },
    {   .string   = "Crop middle bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM
    },
    {   .string   = "Crop right bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_MinipaintOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_tedlum_handling_menu[] = {
    {   .string   = "Ignore",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPTEDLumHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_TED_LUM_IGNORE
    },
    {   .string   = "Best cell colors",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPTEDLumHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_TED_LUM_DITHER
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_multicolor_handling_menu[] = {
    {   .string   = "Black & white",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPMultiColorHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_MC2HR_BLACK_WHITE
    },
    {   .string   = "2 colors",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPMultiColorHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_MC2HR_2_COLORS
    },
    {   .string   = "4 colors",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPMultiColorHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_MC2HR_4_COLORS
    },
    {   .string   = "Gray scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPMultiColorHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_MC2HR_GRAY
    },
    {   .string   = "Best cell colors",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPMultiColorHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_MC2HR_DITHER
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_undersize_handling_menu[] = {
    {   .string   = "Scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPUndersizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_UNDERSIZE_SCALE
    },
    {   .string   = "Borderize",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPUndersizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_UNDERSIZE_BORDERIZE
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_oversize_handling_menu[] = {
    {   .string   = "Scale",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_SCALE
    },
    {   .string   = "Crop left top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_TOP
    },
    {   .string   = "Crop middle top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_TOP
    },
    {   .string   = "Crop right top",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP
    },
    {   .string   = "Crop left center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER
    },
    {   .string   = "Crop middle center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER
    },
    {   .string   = "Crop right center",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER
    },
    {   .string   = "Crop left bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM
    },
    {   .string   = "Crop middle bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM
    },
    {   .string   = "Crop right bottom",
        .type     = MENU_ENTRY_RESOURCE_RADIO,
        .callback = radio_OCPOversizeHandling_callback,
        .data     = (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_settings_vic_vicii_vdc_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_undersize_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_settings_ted_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_undersize_handling_menu
    },
    {   .string   = "TED luminosity handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_tedlum_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_settings_crtc_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)koala_undersize_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_settings_vic_vicii_vdc_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_undersize_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_settings_ted_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_undersize_handling_menu
    },
    {   .string   = "TED luminosity handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_tedlum_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_settings_crtc_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)minipaint_undersize_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_settings_vic_vicii_vdc_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_undersize_handling_menu
    },
    {   .string   = "Multicolor handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_multicolor_handling_menu
    },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_settings_ted_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_undersize_handling_menu
    },
    {   .string   = "Multicolor handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_multicolor_handling_menu
    },
    {   .string   = "TED luminosity handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_tedlum_handling_menu
    },
    SDL_MENU_LIST_END
};


static const ui_menu_entry_t artstudio_settings_crtc_menu[] = {
    {   .string   = "Oversize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_oversize_handling_menu
    },
    {   .string   = "Undersize handling",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_radio_callback,
        .data     = (ui_callback_data_t)artstudio_undersize_handling_menu
    },
    SDL_MENU_LIST_END
};


const ui_menu_entry_t screenshot_vic_vicii_vdc_menu[] = {
    /* settings */
    {   .string   = "Artstudio screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)artstudio_settings_vic_vicii_vdc_menu
    },
    {   .string   = "Koalapainter screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)koala_settings_vic_vicii_vdc_menu
    },
    {   .string   = "Minipaint screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)minipaint_settings_vic_vicii_vdc_menu
    },
    SDL_MENU_ITEM_SEPARATOR,

    /* Commodore-native formats */
    {   .string   = "Save Artstudio screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"ARTSTUDIO"
    },
    {   .string   = "Save Koalapainter screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"KOALA"
    },
    {   .string   = "Save GoDot screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"4BT"
    },
    {   .string   = "Save Minipaint screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"MINIPAINT"
    },
    SDL_MENU_ITEM_SEPARATOR,

    /* PC formats */
    {   .string   = "Save BMP screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"BMP"
    },
#ifdef HAVE_GIF
    {   .string   = "Save GIF screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"GIF"
    },
#endif
    {   .string   = "Save IFF screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"IFF"
    },
    {   .string   = "Save PCX screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PCX"
    },
#ifdef HAVE_PNG
    {   .string   = "Save PNG screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PNG"
    },
#endif
    {   .string   = "Save PPM screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PPM"
    },
    SDL_MENU_LIST_END
};


const ui_menu_entry_t screenshot_ted_menu[] = {
    {   .string   = "Artstudio screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)artstudio_settings_ted_menu
    },
    {   .string   = "Koalapainter screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)koala_settings_ted_menu
    },
    {   .string   = "Minipaint screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)minipaint_settings_ted_menu
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Save Artstudio screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"ARTSTUDIO"
    },
    {   .string   = "Save Koalapainter screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"KOALA"
    },
    {   .string   = "Save GoDot screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"4BT"
    },
    {   .string   = "Save Minipaint screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"MINIPAINT"
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Save BMP screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"BMP"
    },
#ifdef HAVE_GIF
    {   .string   = "Save GIF screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"GIF"
    },
#endif
    {   .string   = "Save IFF screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"IFF"
    },
    {   .string   = "Save PCX screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PCX"
    },
#ifdef HAVE_PNG
    {   .string   = "Save PNG screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PNG"
    },
#endif
    {   .string   = "Save PPM screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PPM"
    },
    SDL_MENU_LIST_END
};


const ui_menu_entry_t screenshot_crtc_menu[] = {
    {   .string   = "Artstudio screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)artstudio_settings_crtc_menu
    },
    {   .string   = "Koalapainter screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)koala_settings_crtc_menu
    },
    {   .string   = "Minipaint screenshot settings",
        .type     = MENU_ENTRY_SUBMENU,
        .callback = submenu_callback,
        .data     = (ui_callback_data_t)minipaint_settings_crtc_menu
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Save Artstudio screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"ARTSTUDIO"
    },
    {   .string   = "Save Koalapainter screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"KOALA"
    },
    {   .string   = "Save GoDot screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"4BT"
    },
    {   .string   = "Save Minipaint screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"MINIPAINT"
    },
    SDL_MENU_ITEM_SEPARATOR,

    {   .string   = "Save BMP screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"BMP"
    },
#ifdef HAVE_GIF
    {   .string   = "Save GIF screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"GIF"
    },
#endif
    {   .string   = "Save IFF screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"IFF"
    },
    {   .string   = "Save PCX screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PCX"
    },
#ifdef HAVE_PNG
    {   .string   = "Save PNG screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PNG"
    },
#endif
    {   .string   = "Save PPM screenshot",
        .type     = MENU_ENTRY_DIALOG,
        .callback = save_screenshot_callback,
        .data     = (ui_callback_data_t)"PPM"
    },
    SDL_MENU_LIST_END
};
