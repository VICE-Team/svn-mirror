/*
 * menu_video.c - SDL video settings menus.
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
#include "types.h"

#include "fullscreenarch.h"
#include "lib.h"
#include "machine.h"
#include "menu_common.h"
#include "menu_video.h"
#include "resources.h"
#include "ui.h"
#include "uifilereq.h"
#include "uimenu.h"
#include "vicii.h"
#include "videoarch.h"

/* Border mode menu */

UI_MENU_DEFINE_RADIO(VICIIBorderMode)

static const ui_menu_entry_t vicii_border_menu[] = {
    { "Normal",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICIIBorderMode_callback,
      (ui_callback_data_t)VICII_NORMAL_BORDERS },
    { "Full",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICIIBorderMode_callback,
      (ui_callback_data_t)VICII_FULL_BORDERS },
    { "Debug",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICIIBorderMode_callback,
      (ui_callback_data_t)VICII_DEBUG_BORDERS },
    { NULL }
};

/* PAL emulation menu */

UI_MENU_DEFINE_TOGGLE(PALEmulation)
UI_MENU_DEFINE_SLIDER(PALScanLineShade, 0, 1000)
UI_MENU_DEFINE_SLIDER(PALBlur, 0, 1000)
UI_MENU_DEFINE_SLIDER(PALOddLinePhase, 0, 2000)
UI_MENU_DEFINE_SLIDER(PALOddLineOffset, 0, 2000)

static const ui_menu_entry_t pal_controls_menu[] = {
    { "PAL emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_PALEmulation_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("PAL controls"),
    { "Scanline shade",
      MENU_ENTRY_RESOURCE_INT,
      slider_PALScanLineShade_callback,
      (ui_callback_data_t)"Set PAL shade (0-1000)" },
    { "Blur",
      MENU_ENTRY_RESOURCE_INT,
      slider_PALBlur_callback,
      (ui_callback_data_t)"Set PAL blur (0-1000)" },
    { "Oddline phase",
      MENU_ENTRY_RESOURCE_INT,
      slider_PALOddLinePhase_callback,
      (ui_callback_data_t)"Set PAL oddline phase (0-2000)" },
    { "Oddline offset",
      MENU_ENTRY_RESOURCE_INT,
      slider_PALOddLineOffset_callback,
      (ui_callback_data_t)"Set PAL oddline offset (0-2000)" },
    { NULL }
};

/* Color menu */

UI_MENU_DEFINE_SLIDER(ColorGamma, 0, 2000)
UI_MENU_DEFINE_SLIDER(ColorTint, 0, 2000)
UI_MENU_DEFINE_SLIDER(ColorSaturation, 0, 2000)
UI_MENU_DEFINE_SLIDER(ColorContrast, 0, 2000)
UI_MENU_DEFINE_SLIDER(ColorBrightness, 0, 2000)

static const ui_menu_entry_t color_controls_menu[] = {
    { "Gamma",
      MENU_ENTRY_RESOURCE_INT,
      slider_ColorGamma_callback,
      (ui_callback_data_t)"Set gamma (0-2000)" },
    { "Tint",
      MENU_ENTRY_RESOURCE_INT,
      slider_ColorTint_callback,
      (ui_callback_data_t)"Set tint (0-2000)" },
    { "Saturation",
      MENU_ENTRY_RESOURCE_INT,
      slider_ColorSaturation_callback,
      (ui_callback_data_t)"Set saturation (0-2000)" },
    { "Contrast",
      MENU_ENTRY_RESOURCE_INT,
      slider_ColorContrast_callback,
      (ui_callback_data_t)"Set contrast (0-2000)" },
    { "Brightness",
      MENU_ENTRY_RESOURCE_INT,
      slider_ColorBrightness_callback,
      (ui_callback_data_t)"Set brightness (0-2000)" },
    { NULL }
};

/* Size menu template */

UI_MENU_DEFINE_INT(SDLCustomWidth)
UI_MENU_DEFINE_INT(SDLCustomHeight)
UI_MENU_DEFINE_RADIO(SDLLimitMode)

#define VICE_SDL_SIZE_MENU_ITEMS(chip)              \
    { "Double size",                                \
      MENU_ENTRY_RESOURCE_TOGGLE,                   \
      toggle_##chip##DoubleSize_callback,           \
      NULL },                                       \
    { "Double scan",                                \
      MENU_ENTRY_RESOURCE_TOGGLE,                   \
      toggle_##chip##DoubleScan_callback,           \
      NULL },                                       \
    { "Fullscreen",                                 \
      MENU_ENTRY_RESOURCE_TOGGLE,                   \
      toggle_##chip##Fullscreen_callback,           \
      NULL },                                       \
    SDL_MENU_ITEM_SEPARATOR,                        \
    SDL_MENU_ITEM_TITLE("Fullscreen mode"),         \
    { "Automatic",                                  \
      MENU_ENTRY_RESOURCE_RADIO,                    \
      radio_##chip##SDLFullscreenMode_callback,     \
      (ui_callback_data_t)FULLSCREEN_MODE_AUTO },   \
    { "Custom",                                     \
      MENU_ENTRY_RESOURCE_RADIO,                    \
      radio_##chip##SDLFullscreenMode_callback,     \
      (ui_callback_data_t)FULLSCREEN_MODE_CUSTOM }, \
    SDL_MENU_ITEM_SEPARATOR,                        \
    SDL_MENU_ITEM_TITLE("Custom resolution"),       \
    { "Width",                                      \
      MENU_ENTRY_RESOURCE_INT,                      \
      int_SDLCustomWidth_callback,                  \
      (ui_callback_data_t)"Set width" },            \
    { "Height",                                     \
      MENU_ENTRY_RESOURCE_INT,                      \
      int_SDLCustomHeight_callback,                 \
      (ui_callback_data_t)"Set height" },           \
    SDL_MENU_ITEM_SEPARATOR,                        \
    SDL_MENU_ITEM_TITLE("Resolution limit mode"),   \
    { "Off",                                        \
      MENU_ENTRY_RESOURCE_RADIO,                    \
      radio_SDLLimitMode_callback,                  \
      (ui_callback_data_t)SDL_LIMIT_MODE_OFF },     \
    { "Max",                                        \
      MENU_ENTRY_RESOURCE_RADIO,                    \
      radio_SDLLimitMode_callback,                  \
      (ui_callback_data_t)SDL_LIMIT_MODE_MAX },     \
    { "Fixed",                                      \
      MENU_ENTRY_RESOURCE_RADIO,                    \
      radio_SDLLimitMode_callback,                  \
      (ui_callback_data_t)SDL_LIMIT_MODE_FIXED },

#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE(VICIIHwScale)
UI_MENU_DEFINE_TOGGLE(VDCHwScale)
UI_MENU_DEFINE_TOGGLE(CrtcHwScale)
UI_MENU_DEFINE_TOGGLE(TEDHwScale)
UI_MENU_DEFINE_TOGGLE(VICHwScale)
UI_MENU_DEFINE_TOGGLE(SDLGLAspectMode)
UI_MENU_DEFINE_STRING(AspectRatio)
UI_MENU_DEFINE_TOGGLE(SDLGLFlipX)
UI_MENU_DEFINE_TOGGLE(SDLGLFlipY)

#define VICE_SDL_SIZE_MENU_OPENGL_ITEMS(chip)  \
    SDL_MENU_ITEM_SEPARATOR,                   \
    SDL_MENU_ITEM_TITLE("OpenGL"),             \
    { "OpenGL free scaling",                   \
      MENU_ENTRY_RESOURCE_TOGGLE,              \
      toggle_##chip##HwScale_callback,         \
      NULL },                                  \
    { "Fixed aspect ratio",                    \
      MENU_ENTRY_RESOURCE_TOGGLE,              \
      toggle_SDLGLAspectMode_callback,         \
      NULL },                                  \
    { "Aspect ratio",                          \
      MENU_ENTRY_RESOURCE_STRING,              \
      string_AspectRatio_callback,             \
      (ui_callback_data_t)"Set aspect ratio (0.5 - 2.0)" }, \
    { "Flip X",                                \
      MENU_ENTRY_RESOURCE_TOGGLE,              \
      toggle_SDLGLFlipX_callback,              \
      NULL },                                  \
    { "Flip Y",                                \
      MENU_ENTRY_RESOURCE_TOGGLE,              \
      toggle_SDLGLFlipY_callback,              \
      NULL },
#endif

/* VICII size menu */

UI_MENU_DEFINE_TOGGLE(VICIIDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICIIDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICIIFullscreen)
UI_MENU_DEFINE_RADIO(VICIISDLFullscreenMode)

static const ui_menu_entry_t vicii_size_menu[] = {
    VICE_SDL_SIZE_MENU_ITEMS(VICII)
#ifdef HAVE_HWSCALE
    VICE_SDL_SIZE_MENU_OPENGL_ITEMS(VICII)
#endif
    { NULL }
};

/* VDC size menu */

UI_MENU_DEFINE_TOGGLE(VDCDoubleSize)
UI_MENU_DEFINE_TOGGLE(VDCDoubleScan)
UI_MENU_DEFINE_TOGGLE(VDCFullscreen)
UI_MENU_DEFINE_RADIO(VDCSDLFullscreenMode)

static const ui_menu_entry_t vdc_size_menu[] = {
    VICE_SDL_SIZE_MENU_ITEMS(VDC)
#ifdef HAVE_HWSCALE
    VICE_SDL_SIZE_MENU_OPENGL_ITEMS(VDC)
#endif
    { NULL }
};

/* Crtc size menu */

UI_MENU_DEFINE_TOGGLE(CrtcFullscreen)
UI_MENU_DEFINE_RADIO(CrtcSDLFullscreenMode)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleSize)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleScan)

static const ui_menu_entry_t crtc_size_menu[] = {
    VICE_SDL_SIZE_MENU_ITEMS(Crtc)
#ifdef HAVE_HWSCALE
    VICE_SDL_SIZE_MENU_OPENGL_ITEMS(Crtc)
#endif
    { NULL }
};

/* TED size menu */

UI_MENU_DEFINE_TOGGLE(TEDDoubleSize)
UI_MENU_DEFINE_TOGGLE(TEDDoubleScan)
UI_MENU_DEFINE_TOGGLE(TEDFullscreen)
UI_MENU_DEFINE_RADIO(TEDSDLFullscreenMode)

static const ui_menu_entry_t ted_size_menu[] = {
    VICE_SDL_SIZE_MENU_ITEMS(TED)
#ifdef HAVE_HWSCALE
    VICE_SDL_SIZE_MENU_OPENGL_ITEMS(TED)
#endif
    { NULL }
};

/* VIC size menu */

UI_MENU_DEFINE_TOGGLE(VICDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICFullscreen)
UI_MENU_DEFINE_RADIO(VICSDLFullscreenMode)

static const ui_menu_entry_t vic_size_menu[] = {
    VICE_SDL_SIZE_MENU_ITEMS(VIC)
#ifdef HAVE_HWSCALE
    VICE_SDL_SIZE_MENU_OPENGL_ITEMS(VIC)
#endif
    { NULL }
};

/* Misc. callbacks */

UI_MENU_DEFINE_TOGGLE(VICIIVideoCache)
UI_MENU_DEFINE_TOGGLE(VICIIScale2x)
UI_MENU_DEFINE_TOGGLE(VICIINewLuminances)
UI_MENU_DEFINE_TOGGLE(VDCVideoCache)
UI_MENU_DEFINE_TOGGLE(CrtcVideoCache)
UI_MENU_DEFINE_TOGGLE(TEDVideoCache)
UI_MENU_DEFINE_TOGGLE(TEDScale2x)
UI_MENU_DEFINE_TOGGLE(VICVideoCache)
UI_MENU_DEFINE_TOGGLE(VICScale2x)
UI_MENU_DEFINE_TOGGLE(VICExternalPalette)
UI_MENU_DEFINE_TOGGLE(VICIIExternalPalette)
UI_MENU_DEFINE_TOGGLE(TEDExternalPalette)
UI_MENU_DEFINE_RADIO(MachineVideoStandard)

static UI_MENU_CALLBACK(restore_size_callback)
{
    if (activated) {
        sdl_video_resize(0, 0);
    }
    return NULL;
}

UI_MENU_DEFINE_FILE_STRING(VICIIPaletteFile)
UI_MENU_DEFINE_FILE_STRING(VDCPaletteFile)
UI_MENU_DEFINE_FILE_STRING(CrtcPaletteFile)
UI_MENU_DEFINE_FILE_STRING(TEDPaletteFile)
UI_MENU_DEFINE_FILE_STRING(VICPaletteFile)

/* C128 video menu */

static UI_MENU_CALLBACK(radio_VideoOutput_c128_callback)
{
    int value = vice_ptr_to_int(param);

    if (activated) {
        sdl_video_canvas_switch(value);
    } else {
        if (value == sdl_active_canvas->index) {
            return sdl_menu_text_tick;
        }
    }
    return NULL;
}

const ui_menu_entry_t c128_video_menu[] = {
    SDL_MENU_ITEM_TITLE("Video output"),
    { "VICII (40 cols)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VideoOutput_c128_callback,
      (ui_callback_data_t)0 },
    { "VDC (80 cols)",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VideoOutput_c128_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    { "VICII size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_size_menu },
    { "VDC size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vdc_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "VICII Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    { "VICII New luminances",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIINewLuminances_callback,
      NULL },
    { "VICII Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "VICII Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
    { "External VICII palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIExternalPalette_callback,
      NULL },
    { "VICII external palette file",
      MENU_ENTRY_DIALOG,
      file_string_VICIIPaletteFile_callback,
      (ui_callback_data_t)"Choose VICII palette file" },
    SDL_MENU_ITEM_SEPARATOR,
    { "VDC Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VDCVideoCache_callback,
      NULL },
    { "VDC external palette file",
      MENU_ENTRY_DIALOG,
      file_string_VDCPaletteFile_callback,
      (ui_callback_data_t)"Choose VDC palette file" },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Video Standard"),
    { "PAL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_PAL },
    { "NTSC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSC },
    { NULL }
};

/* C64 video menu */

const ui_menu_entry_t c64_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "New luminances",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIINewLuminances_callback,
      NULL },
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "External palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIExternalPalette_callback,
      NULL },
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_VICIIPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Video Standard"),
    { "PAL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_PAL },
    { "NTSC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSC },
    { "NTSC old",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSCOLD },
    { NULL }
};

/* C64DTV video menu */

const ui_menu_entry_t c64dtv_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Colorfix",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIINewLuminances_callback,
      NULL },
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
#if 0   /* disabled until there are external DTV palette files available */
    { "External palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIExternalPalette_callback,
      NULL },
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_VICIIPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
#endif
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Video Standard"),
    { "PAL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_PAL },
    { "NTSC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSC },
    { NULL }
};

/* CBM-II 5x0 video menu */

const ui_menu_entry_t cbm5x0_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "External palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIExternalPalette_callback,
      NULL },
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_VICIIPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Video Standard"),
    { "PAL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_PAL },
    { "NTSC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSC },
    { NULL }
};

/* CBM-II 6x0/7x0 video menu */

const ui_menu_entry_t cbm6x0_7x0_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)crtc_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_CrtcPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    { NULL }
};

/* PET video menu */

const ui_menu_entry_t pet_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)crtc_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_CrtcPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    { NULL }
};

/* PLUS4 video menu */

const ui_menu_entry_t plus4_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ted_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDScale2x_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "External palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDExternalPalette_callback,
      NULL },
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_TEDPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Video Standard"),
    { "PAL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_PAL },
    { "NTSC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSC },
    { NULL }
};

/* VIC-20 video menu */

static UI_MENU_CALLBACK(radio_MachineVideoStandard_vic20_callback)
{
    if (activated) {
        int value = vice_ptr_to_int(param);
        resources_set_int("MachineVideoStandard", value);
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        return sdl_menu_text_exit_ui;
    }
    return sdl_ui_menu_radio_helper(activated, param, "MachineVideoStandard");
}

const ui_menu_entry_t vic20_video_menu[] = {
    { "Size settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vic_size_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICVideoCache_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICScale2x_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "External palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICExternalPalette_callback,
      NULL },
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_VICPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Video Standard"),
    { "PAL",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_vic20_callback,
      (ui_callback_data_t)MACHINE_SYNC_PAL },
    { "NTSC",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_MachineVideoStandard_vic20_callback,
      (ui_callback_data_t)MACHINE_SYNC_NTSC },
    { NULL }
};
