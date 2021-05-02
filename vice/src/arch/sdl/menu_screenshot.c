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
    { "Ignore",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaTEDLumHandling_callback,
      (ui_callback_data_t)NATIVE_SS_TED_LUM_IGNORE },
    { "Best cell colors",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaTEDLumHandling_callback,
      (ui_callback_data_t)NATIVE_SS_TED_LUM_DITHER },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_undersize_handling_menu[] = {
    { "Scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaUndersizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_UNDERSIZE_SCALE },
    { "Borderize",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaUndersizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_UNDERSIZE_BORDERIZE },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_oversize_handling_menu[] = {
    { "Scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_SCALE },
    { "Crop left top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_TOP },
    { "Crop middle top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_TOP },
    { "Crop right top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP },
    { "Crop left center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER },
    { "Crop middle center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER },
    { "Crop right center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER },
    { "Crop left bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM },
    { "Crop middle bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM },
    { "Crop right bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_KoalaOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_tedlum_handling_menu[] = {
    { "Ignore",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintTEDLumHandling_callback,
      (ui_callback_data_t)NATIVE_SS_TED_LUM_IGNORE },
    { "Best cell colors",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintTEDLumHandling_callback,
      (ui_callback_data_t)NATIVE_SS_TED_LUM_DITHER },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_undersize_handling_menu[] = {
    { "Scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintUndersizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_UNDERSIZE_SCALE },
    { "Borderize",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintUndersizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_UNDERSIZE_BORDERIZE },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_oversize_handling_menu[] = {
    { "Scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_SCALE },
    { "Crop left top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_TOP },
    { "Crop middle top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_TOP },
    { "Crop right top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP },
    { "Crop left center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER },
    { "Crop middle center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER },
    { "Crop right center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER },
    { "Crop left bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM },
    { "Crop middle bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM },
    { "Crop right bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MinipaintOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_tedlum_handling_menu[] = {
    { "Ignore",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPTEDLumHandling_callback,
      (ui_callback_data_t)NATIVE_SS_TED_LUM_IGNORE },
    { "Best cell colors",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPTEDLumHandling_callback,
      (ui_callback_data_t)NATIVE_SS_TED_LUM_DITHER },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_multicolor_handling_menu[] = {
    { "Black & white",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPMultiColorHandling_callback,
      (ui_callback_data_t)NATIVE_SS_MC2HR_BLACK_WHITE },
    { "2 colors",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPMultiColorHandling_callback,
      (ui_callback_data_t)NATIVE_SS_MC2HR_2_COLORS },
    { "4 colors",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPMultiColorHandling_callback,
      (ui_callback_data_t)NATIVE_SS_MC2HR_4_COLORS },
    { "Gray scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPMultiColorHandling_callback,
      (ui_callback_data_t)NATIVE_SS_MC2HR_GRAY },
    { "Best cell colors",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPMultiColorHandling_callback,
      (ui_callback_data_t)NATIVE_SS_MC2HR_DITHER },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_undersize_handling_menu[] = {
    { "Scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPUndersizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_UNDERSIZE_SCALE },
    { "Borderize",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPUndersizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_UNDERSIZE_BORDERIZE },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_oversize_handling_menu[] = {
    { "Scale",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_SCALE },
    { "Crop left top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_TOP },
    { "Crop middle top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_TOP },
    { "Crop right top",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP },
    { "Crop left center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER },
    { "Crop middle center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER },
    { "Crop right center",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER },
    { "Crop left bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM },
    { "Crop middle bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM },
    { "Crop right bottom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_OCPOversizeHandling_callback,
      (ui_callback_data_t)NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_settings_vic_vicii_vdc_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_undersize_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_settings_ted_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_undersize_handling_menu },
    { "TED luminosity handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_tedlum_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t koala_settings_crtc_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)koala_undersize_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_settings_vic_vicii_vdc_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_undersize_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_settings_ted_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_undersize_handling_menu },
    { "TED luminosity handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_tedlum_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t minipaint_settings_crtc_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)minipaint_undersize_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_settings_vic_vicii_vdc_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_undersize_handling_menu },
    { "Multicolor handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_multicolor_handling_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t artstudio_settings_ted_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_undersize_handling_menu },
    { "Multicolor handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_multicolor_handling_menu },
    { "TED luminosity handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_tedlum_handling_menu },
    SDL_MENU_LIST_END
};


static const ui_menu_entry_t artstudio_settings_crtc_menu[] = {
    { "Oversize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_oversize_handling_menu },
    { "Undersize handling",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)artstudio_undersize_handling_menu },
    SDL_MENU_LIST_END
};


const ui_menu_entry_t screenshot_vic_vicii_vdc_menu[] = {
    /* settings */
    { "Artstudio screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)artstudio_settings_vic_vicii_vdc_menu },
    { "Koalapainter screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)koala_settings_vic_vicii_vdc_menu },
    { "Minipaint screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)minipaint_settings_vic_vicii_vdc_menu },

    SDL_MENU_ITEM_SEPARATOR,

    /* Commodore-native formats */
    { "Save Artstudio screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"ARTSTUDIO" },
    { "Save Koalapainter screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"KOALA" },
    { "Save GoDot screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"4BT" },
    { "Save Minipaint screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"MINIPAINT" },

    SDL_MENU_ITEM_SEPARATOR,

    /* PC formats */
    { "Save BMP screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"BMP" },
#ifdef HAVE_GIF
    { "Save GIF screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"GIF" },
#endif
    { "Save IFF screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"IFF" },
#ifdef HAVE_JPEG
    { "Save JPG screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"JPEG" },
#endif
    { "Save PCX screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PCX" },
#ifdef HAVE_PNG
    { "Save PNG screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PNG" },
#endif
    { "Save PPM screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PPM" },

    SDL_MENU_LIST_END
};


const ui_menu_entry_t screenshot_ted_menu[] = {
    { "Artstudio screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)artstudio_settings_ted_menu },
    { "Koalapainter screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)koala_settings_ted_menu },
    { "Minipaint screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)minipaint_settings_ted_menu },

    SDL_MENU_ITEM_SEPARATOR,

    { "Save Artstudio screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"ARTSTUDIO" },
    { "Save Koalapainter screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"KOALA" },
    { "Save GoDot screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"4BT" },
    { "Save Minipaint screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"MINIPAINT" },

    SDL_MENU_ITEM_SEPARATOR,

    { "Save BMP screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"BMP" },
#ifdef HAVE_GIF
    { "Save GIF screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"GIF" },
#endif
    { "Save IFF screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"IFF" },
#ifdef HAVE_JPEG
    { "Save JPG screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"JPEG" },
#endif
    { "Save PCX screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PCX" },
#ifdef HAVE_PNG
    { "Save PNG screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PNG" },
#endif
    { "Save PPM screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PPM" },

    SDL_MENU_LIST_END
};


const ui_menu_entry_t screenshot_crtc_menu[] = {
    { "Artstudio screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)artstudio_settings_crtc_menu },
    { "Koalapainter screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)koala_settings_crtc_menu },
    { "Minipaint screenshot settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)minipaint_settings_crtc_menu },

    SDL_MENU_ITEM_SEPARATOR,

    { "Save Artstudio screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"ARTSTUDIO" },
    { "Save Koalapainter screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"KOALA" },
    { "Save GoDot screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"4BT" },
    { "Save Minipaint screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"MINIPAINT" },

    SDL_MENU_ITEM_SEPARATOR,

    { "Save BMP screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"BMP" },
#ifdef HAVE_GIF
    { "Save GIF screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"GIF" },
#endif
    { "Save IFF screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"IFF" },
#ifdef HAVE_JPEG
    { "Save JPG screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"JPEG" },
#endif
    { "Save PCX screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PCX" },
#ifdef HAVE_PNG
    { "Save PNG screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PNG" },
#endif
    { "Save PPM screenshot",
      MENU_ENTRY_DIALOG,
      save_screenshot_callback,
      (ui_callback_data_t)"PPM" },

    SDL_MENU_LIST_END
};
