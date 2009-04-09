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

UI_MENU_DEFINE_RADIO(SDLLimitMode)
UI_MENU_DEFINE_INT(SDLLimitWidth)
UI_MENU_DEFINE_INT(SDLLimitHeight)

static const ui_menu_entry_t limits_menu[] = {
    SDL_MENU_ITEM_TITLE("Resolution limit mode"),
    { "Off",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SDLLimitMode_callback,
      (ui_callback_data_t)SDL_LIMIT_MODE_OFF },
    { "Max",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SDLLimitMode_callback,
      (ui_callback_data_t)SDL_LIMIT_MODE_MAX },
    { "Fixed",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_SDLLimitMode_callback,
      (ui_callback_data_t)SDL_LIMIT_MODE_FIXED },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Limits"),
    { "Width",
      MENU_ENTRY_RESOURCE_INT,
      int_SDLLimitWidth_callback,
      (ui_callback_data_t)"Set width limit"},
    { "Height",
      MENU_ENTRY_RESOURCE_INT,
      int_SDLLimitHeight_callback,
      (ui_callback_data_t)"Set height limit"},
    { NULL }
};

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

UI_MENU_DEFINE_TOGGLE(PALEmulation)
UI_MENU_DEFINE_INT(PALScanLineShade)
UI_MENU_DEFINE_INT(PALBlur)
UI_MENU_DEFINE_INT(PALOddLinePhase)
UI_MENU_DEFINE_INT(PALOddLineOffset)

static const ui_menu_entry_t pal_controls_menu[] = {
    { "PAL emulation",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_PALEmulation_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("PAL controls"),
    { "Scanline shade",
      MENU_ENTRY_RESOURCE_INT,
      int_PALScanLineShade_callback,
      (ui_callback_data_t)"Set PAL shade (0-1000)"},
    { "Blur",
      MENU_ENTRY_RESOURCE_INT,
      int_PALBlur_callback,
      (ui_callback_data_t)"Set PAL blur (0-1000)"},
    { "Oddline phase",
      MENU_ENTRY_RESOURCE_INT,
      int_PALOddLinePhase_callback,
      (ui_callback_data_t)"Set PAL oddline phase (0-2000)"},
    { "Oddline offset",
      MENU_ENTRY_RESOURCE_INT,
      int_PALOddLineOffset_callback,
      (ui_callback_data_t)"Set PAL oddline offset (0-2000)"},
    { NULL }
};

UI_MENU_DEFINE_INT(ColorGamma)
UI_MENU_DEFINE_INT(ColorTint)
UI_MENU_DEFINE_INT(ColorSaturation)
UI_MENU_DEFINE_INT(ColorContrast)
UI_MENU_DEFINE_INT(ColorBrightness)

static const ui_menu_entry_t color_controls_menu[] = {
    { "Gamma",
      MENU_ENTRY_RESOURCE_INT,
      int_ColorGamma_callback,
      (ui_callback_data_t)"Set gamma (0-2000)"},
    { "Tint",
      MENU_ENTRY_RESOURCE_INT,
      int_ColorTint_callback,
      (ui_callback_data_t)"Set tint (0-2000)"},
    { "Saturation",
      MENU_ENTRY_RESOURCE_INT,
      int_ColorSaturation_callback,
      (ui_callback_data_t)"Set saturation (0-2000)"},
    { "Contrast",
      MENU_ENTRY_RESOURCE_INT,
      int_ColorContrast_callback,
      (ui_callback_data_t)"Set contrast (0-2000)"},
    { "Brightness",
      MENU_ENTRY_RESOURCE_INT,
      int_ColorBrightness_callback,
      (ui_callback_data_t)"Set brightness (0-2000)"},
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VICIIFullscreen)
UI_MENU_DEFINE_RADIO(VICIISDLFullscreenMode)

static const ui_menu_entry_t vicii_fullscreen_menu[] = {
    { "Fullscreen",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIFullscreen_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Fullscreen mode"),
    { "Automatic",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICIISDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_AUTO },
    { "Custom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICIISDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_CUSTOM },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VDCFullscreen)
UI_MENU_DEFINE_RADIO(VDCSDLFullscreenMode)

static const ui_menu_entry_t vdc_fullscreen_menu[] = {
    { "Fullscreen",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VDCFullscreen_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Fullscreen mode"),
    { "Automatic",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDCSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_AUTO },
    { "Custom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VDCSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_CUSTOM },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(CrtcFullscreen)
UI_MENU_DEFINE_RADIO(CrtcSDLFullscreenMode)

static const ui_menu_entry_t crtc_fullscreen_menu[] = {
    { "Fullscreen",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcFullscreen_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Fullscreen mode"),
    { "Automatic",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_CrtcSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_AUTO },
    { "Custom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_CrtcSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_CUSTOM },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(TEDFullscreen)
UI_MENU_DEFINE_RADIO(TEDSDLFullscreenMode)

static const ui_menu_entry_t ted_fullscreen_menu[] = {
    { "Fullscreen",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDFullscreen_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Fullscreen mode"),
    { "Automatic",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_TEDSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_AUTO },
    { "Custom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_TEDSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_CUSTOM },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(VICFullscreen)
UI_MENU_DEFINE_RADIO(VICSDLFullscreenMode)

static const ui_menu_entry_t vic_fullscreen_menu[] = {
    { "Fullscreen",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICFullscreen_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    SDL_MENU_ITEM_TITLE("Fullscreen mode"),
    { "Automatic",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_AUTO },
    { "Custom",
      MENU_ENTRY_RESOURCE_RADIO,
      radio_VICSDLFullscreenMode_callback,
      (ui_callback_data_t)FULLSCREEN_MODE_CUSTOM },
    { NULL }
};


UI_MENU_DEFINE_TOGGLE(VICIIDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICIIDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICIIVideoCache)
UI_MENU_DEFINE_TOGGLE(VICIIScale2x)
UI_MENU_DEFINE_TOGGLE(VDCDoubleSize)
UI_MENU_DEFINE_TOGGLE(VDCDoubleScan)
UI_MENU_DEFINE_TOGGLE(VDCVideoCache)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleSize)
UI_MENU_DEFINE_TOGGLE(CrtcDoubleScan)
UI_MENU_DEFINE_TOGGLE(CrtcVideoCache)
UI_MENU_DEFINE_TOGGLE(TEDDoubleSize)
UI_MENU_DEFINE_TOGGLE(TEDDoubleScan)
UI_MENU_DEFINE_TOGGLE(TEDVideoCache)
UI_MENU_DEFINE_TOGGLE(TEDScale2x)
UI_MENU_DEFINE_TOGGLE(VICDoubleSize)
UI_MENU_DEFINE_TOGGLE(VICDoubleScan)
UI_MENU_DEFINE_TOGGLE(VICVideoCache)
UI_MENU_DEFINE_TOGGLE(VICScale2x)
UI_MENU_DEFINE_TOGGLE(VICExternalPalette)
UI_MENU_DEFINE_TOGGLE(VICIIExternalPalette)
UI_MENU_DEFINE_TOGGLE(TEDExternalPalette)
UI_MENU_DEFINE_RADIO(MachineVideoStandard)


#ifdef HAVE_HWSCALE
UI_MENU_DEFINE_TOGGLE(VICIIHwScale)
UI_MENU_DEFINE_TOGGLE(VDCHwScale)
UI_MENU_DEFINE_TOGGLE(CrtcHwScale)
UI_MENU_DEFINE_TOGGLE(TEDHwScale)
UI_MENU_DEFINE_TOGGLE(VICHwScale)
UI_MENU_DEFINE_TOGGLE(SDLGLAspectMode)
UI_MENU_DEFINE_STRING(AspectRatio)

static const ui_menu_entry_t c128_opengl_menu[] = {
    { "VICII OpenGL",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIHwScale_callback,
      NULL },
    { "VDC OpenGL",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VDCHwScale_callback,
      NULL },
    { "Fixed aspect ratio",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SDLGLAspectMode_callback,
      NULL },
    { "Aspect ratio",
      MENU_ENTRY_RESOURCE_STRING,
      string_AspectRatio_callback,
      (ui_callback_data_t)"Set aspect ratio (0.5 - 2.0)" },
    { NULL }
};

static const ui_menu_entry_t vicii_opengl_menu[] = {
    { "OpenGL",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIHwScale_callback,
      NULL },
    { "Fixed aspect ratio",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SDLGLAspectMode_callback,
      NULL },
    { "Aspect ratio",
      MENU_ENTRY_RESOURCE_STRING,
      string_AspectRatio_callback,
      (ui_callback_data_t)"Set aspect ratio (0.5 - 2.0)" },
    { NULL }
};

static const ui_menu_entry_t crtc_opengl_menu[] = {
    { "OpenGL",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcHwScale_callback,
      NULL },
    { "Fixed aspect ratio",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SDLGLAspectMode_callback,
      NULL },
    { "Aspect ratio",
      MENU_ENTRY_RESOURCE_STRING,
      string_AspectRatio_callback,
      (ui_callback_data_t)"Set aspect ratio (0.5 - 2.0)" },
    { NULL }
};

static const ui_menu_entry_t ted_opengl_menu[] = {
    { "OpenGL",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDHwScale_callback,
      NULL },
    { "Fixed aspect ratio",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SDLGLAspectMode_callback,
      NULL },
    { "Aspect ratio",
      MENU_ENTRY_RESOURCE_STRING,
      string_AspectRatio_callback,
      (ui_callback_data_t)"Set aspect ratio (0.5 - 2.0)" },
    { NULL }
};

static const ui_menu_entry_t vic_opengl_menu[] = {
    { "OpenGL",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICHwScale_callback,
      NULL },
    { "Fixed aspect ratio",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SDLGLAspectMode_callback,
      NULL },
    { "Aspect ratio",
      MENU_ENTRY_RESOURCE_STRING,
      string_AspectRatio_callback,
      (ui_callback_data_t)"Set aspect ratio (0.5 - 2.0)" },
    { NULL }
};
#endif

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

static const ui_menu_entry_t c128_video_vicii_menu[] = {
    { "VICII Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_fullscreen_menu },
    { "VICII Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleSize_callback,
      NULL },
    { "VICII Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleScan_callback,
      NULL },
    { "VICII Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    { "VICII Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
    { "External VICII palette",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIExternalPalette_callback,
      NULL },
    { "VICII external palette file",
      MENU_ENTRY_DIALOG,
      file_string_VICIIPaletteFile_callback,
      (ui_callback_data_t)"Choose VICII palette file" },
    { NULL }
};

static const ui_menu_entry_t c128_video_vdc_menu[] = {
    { "VDC Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vdc_fullscreen_menu },
    { "VDC Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VDCDoubleSize_callback,
      NULL },
    { "VDC Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VDCDoubleScan_callback,
      NULL },
    { "VDC Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VDCVideoCache_callback,
      NULL },
    { "VDC external palette file",
      MENU_ENTRY_DIALOG,
      file_string_VDCPaletteFile_callback,
      (ui_callback_data_t)"Choose VDC palette file" },
    { NULL }
};


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
    { "VICII settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)c128_video_vicii_menu },
    { "VDC settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)c128_video_vdc_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)c128_opengl_menu },
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

const ui_menu_entry_t c64_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_fullscreen_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleScan_callback,
      NULL },
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_opengl_menu },
#endif
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
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

const ui_menu_entry_t c64dtv_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_fullscreen_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleScan_callback,
      NULL },
    { "Video cache (broken)",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_opengl_menu },
#endif
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
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

const ui_menu_entry_t cbm5x0_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_fullscreen_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIDoubleScan_callback,
      NULL },
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIVideoCache_callback,
      NULL },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICIIScale2x_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vicii_opengl_menu },
#endif
    { "VICII border mode",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)vicii_border_menu },
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
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

const ui_menu_entry_t cbm6x0_7x0_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)crtc_fullscreen_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcDoubleScan_callback,
      NULL },
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcVideoCache_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)crtc_opengl_menu },
#endif
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_CrtcPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    { NULL }
};

const ui_menu_entry_t pet_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcFullscreen_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcDoubleScan_callback,
      NULL },
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_CrtcVideoCache_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)crtc_opengl_menu },
#endif
    { "External palette file",
      MENU_ENTRY_DIALOG,
      file_string_CrtcPaletteFile_callback,
      (ui_callback_data_t)"Choose palette file" },
    { NULL }
};

const ui_menu_entry_t plus4_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ted_fullscreen_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDDoubleScan_callback,
      NULL },
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDVideoCache_callback,
      NULL },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_TEDScale2x_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)ted_opengl_menu },
#endif
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
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

const ui_menu_entry_t vic20_video_menu[] = {
    { "Limit settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)limits_menu },
    { "Fullscreen settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vic_fullscreen_menu },
    { "Restore window size",
      MENU_ENTRY_OTHER,
      restore_size_callback,
      NULL },
    { "Double size",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICDoubleSize_callback,
      NULL },
    { "Double scan",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICDoubleScan_callback,
      NULL },
    { "Video cache",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICVideoCache_callback,
      NULL },
    { "Scale2x",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_VICScale2x_callback,
      NULL },
#ifdef HAVE_HWSCALE
    { "OpenGL settings",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)vic_opengl_menu },
#endif
    { "Color controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)color_controls_menu },
    { "PAL emulation controls",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)pal_controls_menu },
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

