/*
 * uivideo.c - video settings UI interface for MS-DOS.
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

#include "machine.h"
#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uivideo.h"
#include "vicii.h"

typedef struct ui_video_item_s {
    int video_id;
    char *cache_res;
    char *double_size_res;
    char *double_scan_res;
    char *scale2x_res;
    char *ext_pal_res;
    char *ext_pal_file_res;
    char *settings_name;
    char *settings_title;
    tui_menu_item_def_t *chip_menu;
    int has_video_standard;
} ui_video_item_t;

TUI_MENU_DEFINE_TOGGLE(VICIICheckSsColl)
TUI_MENU_DEFINE_TOGGLE(VICIICheckSbColl)
TUI_MENU_DEFINE_RADIO(VICIIBorderMode)

static TUI_MENU_CALLBACK(vicii_border_submenu_callback)
{
    int value;
    char *s;

    resources_get_int("VICIIBorderMode", &value);
    switch (value) {
        default:
        case VICII_NORMAL_BORDERS:
            s = "Normal";
            break;
        case VICII_FULL_BORDERS:
            s = "Full";
            break;
        case VICII_DEBUG_BORDERS:
            s = "Debug";
            break;
    }
    return s;
}

static tui_menu_item_def_t vicii_border_submenu[] = {
    { "_Normal", NULL, radio_VICIIBorderMode_callback,
      (void *)VICII_NORMAL_BORDERS, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_Full", NULL, radio_VICIIBorderMode_callback,
      (void *)VICII_FULL_BORDERS, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_Debug", NULL, radio_VICIIBorderMode_callback,
      (void *)VICII_DEBUG_BORDERS, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t vicii_menu_items[] = {
    { "Border mode:", "Select the border mode",
      vicii_border_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, vicii_border_submenu,
      "Border mode" },
    { "Sprite-_Background Collisions:",
      "Emulate sprite-background collision register",
      toggle_VICIICheckSbColl_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Sprite-_Sprite Collisions:",
      "Emulate sprite-sprite collision register",
      toggle_VICIICheckSsColl_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

TUI_MENU_DEFINE_TOGGLE(VDC64KB)
TUI_MENU_DEFINE_RADIO(VDCRevision)

static TUI_MENU_CALLBACK(vdc_revision_submenu_callback)
{
    int value;
    static char s[100];

    resources_get_int("VDCRevision", &value);
    sprintf(s, "Rev %d",value);
    return s;
}

static tui_menu_item_def_t vdc_revision_submenu[] = {
    { "Rev _0", NULL, radio_VDCRevision_callback,
      (void *)0, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Rev _1", NULL, radio_VDCRevision_callback,
      (void *)1, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Rev _2", NULL, radio_VDCRevision_callback,
      (void *)2, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t vdc_menu_items[] = {
    { "VDC _64KB video memory",
      "Emulate a VDC with 64KB video RAM",
      toggle_VDC64KB_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "VDC _revision:", "Select the revision of the VDC",
      vdc_revision_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, vdc_revision_submenu,
      "VDC revision" },
    { NULL }
};

static ui_video_item_t video_item[] = {
    { VID_NONE, NULL,
      NULL, NULL,
      NULL,
      NULL, NULL,
      NULL, NULL,
      NULL, 0 },
    { VID_VIC, "VICVideoCache",
      "VICDoubleSize", "VICDoubleScan",
      "VICScale2x",
      "VICExternalPalette", "VICPaletteFile",
      "VIC settings...", "VIC settings",
      NULL, 1 },
    { VID_VICII, "VICIIVideoCache",
      "VICIIDoubleSize", "VICIIDoubleScan",
      "VICIIScale2x",
      "VICIIExternalPalette", "VICIIPaletteFile",
      "VICII settings...", "VICII settings",
      vicii_menu_items, 1 },
    { VID_TED, "TEDVideoCache",
      "TEDDoubleSize", "TEDDoubleScan",
      "TEDScale2x",
      "TEDExternalPalette", "TEDPaletteFile",
      "TED settings...", "TED settings",
      NULL, 1 },
    { VID_VDC, NULL,
      "VDCDoubleSize", "VDCDoubleScan",
      NULL,
      "VDCExternalPalette", "VDCPaletteFile",
      "VDC settings...", "VDC settings",
      vdc_menu_items, 0 },
    { VID_CRTC, "CrtcVideoCache",
      "CrtcDoubleSize", "CrtcDoubleScan",
      NULL,
      "CrtcExternalPalette", "CrtcPaletteFile",
      "CRTC settings...", "CRTC settings",
      NULL, 0 }
};

TUI_MENU_DEFINE_RADIO(MachineVideoStandard)

static TUI_MENU_CALLBACK(video_standard_submenu_callback)
{
    int value;
    char *s;

    resources_get_int("VICIIBorderMode", &value);
    switch (value) {
        default:
        case MACHINE_SYNC_PAL:
            s = "PAL";
            break;
        case MACHINE_SYNC_NTSC:
            s = "NTSC";
            break;
    }
    return s;
}

static tui_menu_item_def_t video_standard_submenu[] = {
    { "_PAL", NULL, radio_MachineVideoStandard_callback,
      (void *)MACHINE_SYNC_PAL, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_NTSC", NULL, radio_MachineVideoStandard_callback,
      (void *)MACHINE_SYNC_NTSC, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t video_standard_menu_items[] = {
    { "V_ideo Standard:", "Select machine clock ratio",
      video_standard_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, video_standard_submenu,
      "Video standard" },
    { NULL }
};

static TUI_MENU_CALLBACK(toggle_cache_callback)
{
    int i = (int)param;

    return _tui_menu_toggle_helper(been_activated, video_item[i].cache_res);
}

static TUI_MENU_CALLBACK(toggle_doublesize_callback)
{
    int i = (int)param;

    return _tui_menu_toggle_helper(been_activated, video_item[i].double_size_res);
}

static TUI_MENU_CALLBACK(toggle_doublescan_callback)
{
    int i = (int)param;

    return _tui_menu_toggle_helper(been_activated, video_item[i].double_scan_res);
}

static TUI_MENU_CALLBACK(radio_renderfilter_callback)
{
    int i;
    int video_index;
    int render_index;
    int crtemu;
    int scale2x;

    i = (int)param;
    video_index = i >> 4;
    render_index = i & 0xf;

    if (been_activated) {
        switch (render_index) {
             default:
             case 0:
                 crtemu = 0;
                 scale2x = 0;
                 break;
             case 1:
                 crtemu = 1;
                 scale2x = 0;
                 break;
             case 2:
                 crtemu = 0;
                 scale2x = 1;
                 break;
        }
        resources_set_int("PALEmulation", crtemu);
        if (video_item[video_index].scale2x_res != NULL) {
            resources_set_int(video_item[video_index].scale2x_res, scale2x);
        }
        *become_default = 1;
    } else {
        resources_get_int("PALEmulation", &crtemu);
        if (video_item[video_index].scale2x_res != NULL) {
            resources_get_int(video_item[video_index].scale2x_res, &scale2x);
        } else {
            scale2x = 0;
        }
        if (render_index == (crtemu * 2) + scale2x) {
            *become_default = 1;
        }
    }
    return NULL;
}

static TUI_MENU_CALLBACK(toggle_external_palette_callback)
{
    char *resource = (char *)param;
    char *s;
    int val;

    if (been_activated) {
        resources_toggle(resource, NULL);
    }
    resources_get_int(resource, &val);

    return (val) ? "On" : "Off";
}

static TUI_MENU_CALLBACK(custom_palette_callback)
{
    char *resource = (char *)param;

    if (been_activated) {
        char *name;

        name = tui_file_selector("Load custom palette", NULL, "*.vpl", NULL, NULL, NULL, NULL);

        if (name != NULL) {
            if (resources_set_string(resource, name) < 0) {
                tui_error("Invalid palette file");
            }
            ui_update_menus();
            lib_free(name);
        }
    }
    return NULL;
}

/* being lazy ;) */
#define float_inputs(callback, name, range, resource, start, end)                               \
    static TUI_MENU_CALLBACK(callback)                                                          \
    {                                                                                           \
        int value;                                                                              \
        float f;                                                                                \
        char buf[44];                                                                           \
                                                                                                \
        if (been_activated) {                                                                   \
            resources_get_int(resource, &value);                                                \
                                                                                                \
            sprintf(buf, "%.3f", (float)(value / 1000.0));                                      \
                                                                                                \
            if (tui_input_string(name, "Enter the value for "name " "range":", buf, 40) == 0) { \
                f = atof(buf);                                                                  \
                value = (int)(f * 1000.0);                                                      \
                if (value < start) {                                                            \
                    value = start;                                                              \
                }                                                                               \
                if (value > end) {                                                              \
                    value = end;                                                                \
                }                                                                               \
                resources_set_int(resource, value);                                             \
                tui_message(name" set to : %f", (float)(value / 1000.0));                       \
            } else {                                                                            \
                return NULL;                                                                    \
            }                                                                                   \
        }                                                                                       \
        return NULL;                                                                            \
    }

float_inputs(scanlineshade_callback, "Scan line shade", "(0..1)", "PALScanLineShade", 0, 1000)
float_inputs(blur_callback, "Blur", "(0..1)", "PALBlur", 0, 1000)
float_inputs(oddlinephase_callback, "Oddline phase", "(0..2)", "PALOddLinePhase", 0, 2000)
float_inputs(oddlineoffset_callback, "Oddline offset", "(0..2)", "PALOddLineOffset", 0, 2000)

tui_menu_item_def_t ui_crt_emulation_items[] = {
    { "Scan line shade",
      "Adjust the scan line shade",
      scanlineshade_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Blur",
      "Adjust the blur",
      blur_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Oddline phase",
      "Adjust the oddline phase",
      oddlinephase_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Oddline offset",
      "Adjust the oddline offset",
      oddlineoffset_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

float_inputs(gamma_callback, "Gamma", "(0..4)", "ColorGamma", 0, 4000)
float_inputs(tint_callback, "Tint", "(0..2)", "ColorTint", 0, 2000)
float_inputs(saturation_callback, "Saturation", "(0..2)", "ColorSaturation", 0, 2000)
float_inputs(contrast_callback, "Contrast", "(0..2)", "ColorContrast", 0, 2000)
float_inputs(brightness_callback, "Brightness", "(0..2)", "ColorBrightness", 0, 2000)

tui_menu_item_def_t ui_color_items[] = {
    { "Gamma",
      "Adjust the gamma",
      gamma_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Tint",
      "Adjust the tint",
      tint_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Saturation",
      "Adjust the saturation",
      saturation_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Contrast",
      "Adjust the contrast",
      contrast_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "Brightness",
      "Adjust the brightness",
      brightness_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uivideo_init(struct tui_menu *parent_submenu, int vid1, int vid2)
{
    tui_menu_t video_submenu1;
    tui_menu_t video_submenu2;
    tui_menu_t render_submenu;
    tui_menu_t crt_emulation_submenu;
    tui_menu_t colors_submenu;

    if (vid2 != VID_NONE) {
        video_submenu1 = tui_menu_create(video_item[vid1].settings_name, 1);
        video_submenu2 = tui_menu_create(video_item[vid2].settings_name, 1);
    } else {
        video_submenu1 = parent_submenu;
    }

    tui_menu_add_item(video_submenu1, "Video cache",
                      "Enable video cache",
                      toggle_cache_callback,
                      (void *)vid1, 3,
                      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_item(video_submenu1, "Double size",
                      "Enable double size",
                      toggle_doublesize_callback,
                      (void *)vid1, 3,
                      TUI_MENU_BEH_CONTINUE);

    if (vid2 != VID_NONE) {
        tui_menu_add_item(video_submenu2, "Double size",
                          "Enable double size",
                          toggle_doublesize_callback,
                          (void *)vid2, 3,
                          TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_item(video_submenu1, "Double scan",
                      "Enable double scan",
                      toggle_doublescan_callback,
                      (void *)vid1, 3,
                      TUI_MENU_BEH_CONTINUE);

    if (vid2 != VID_NONE) {
        tui_menu_add_item(video_submenu2, "Double scan",
                          "Enable double scan",
                          toggle_doublesize_callback,
                          (void *)vid2, 3,
                          TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_separator(video_submenu1);

    if (vid2 != VID_NONE) {
        tui_menu_add_separator(video_submenu2);
    }

    if (video_item[vid1].chip_menu != NULL) {
        tui_menu_add(video_submenu1, video_item[vid1].chip_menu);
        if (machine_class != VICE_MACHINE_C64SC && video_item[vid1].has_video_standard == 1) {
            tui_menu_add(video_submenu1, video_standard_menu_items);
        }
        tui_menu_add_separator(video_submenu1);
    }

    if (vid2 != VID_NONE) {
        if (video_item[vid2].chip_menu != NULL) {
            tui_menu_add(video_submenu2, video_item[vid2].chip_menu);
            tui_menu_add_separator(video_submenu2);
        }
    }

    tui_menu_add_item(video_submenu1, "External palette",
                      "Use external palette file",
                      toggle_external_palette_callback,
                      (void *)video_item[vid1].ext_pal_res, 3,
                      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_item(video_submenu1, "Choose external palette",
                      "Load a custom palette",
                      custom_palette_callback,
                      (void *)video_item[vid1].ext_pal_file_res, 0,
                      TUI_MENU_BEH_RESUME);

    if (vid2 != VID_NONE) {
        tui_menu_add_item(video_submenu2, "External palette",
                          "Use external palette file",
                          toggle_external_palette_callback,
                          (void *)video_item[vid2].ext_pal_res, 3,
                          TUI_MENU_BEH_CONTINUE);

        tui_menu_add_item(video_submenu2, "Choose external palette",
                          "Load a custom palette",
                          custom_palette_callback,
                          (void *)video_item[vid2].ext_pal_file_res, 0,
                          TUI_MENU_BEH_RESUME);
    }

    if (vid2 != VID_NONE) {
        tui_menu_add_submenu(parent_submenu, video_item[vid1].settings_name,
                         video_item[vid1].settings_title,
                         video_submenu1,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

        tui_menu_add_submenu(parent_submenu, video_item[vid2].settings_name,
                         video_item[vid2].settings_title,
                         video_submenu2,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
    }

    render_submenu = tui_menu_create("Render filter", 1);

    tui_menu_add_item(render_submenu, "None",
                      "No render filter",
                      radio_renderfilter_callback,
                      (void *)((vid1 << 4) | 0), 3,
                      TUI_MENU_BEH_CONTINUE);

    tui_menu_add_item(render_submenu, "CRT emulation",
                      "CRT emulation",
                      radio_renderfilter_callback,
                      (void *)((vid1 << 4) | 1), 3,
                      TUI_MENU_BEH_CONTINUE);

    if (video_item[vid1].scale2x_res != NULL) {
        tui_menu_add_item(render_submenu, "Scale2x",
                          "CRT emulation",
                          radio_renderfilter_callback,
                          (void *)((vid1 << 4) | 2), 3,
                          TUI_MENU_BEH_CONTINUE);
    }

    tui_menu_add_submenu(parent_submenu, "Render filter",
                         "Render filter",
                         render_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    crt_emulation_submenu = tui_menu_create("CRT emulation", 1);
    tui_menu_add(crt_emulation_submenu, ui_crt_emulation_items);
    tui_menu_add_submenu(parent_submenu, "CRT emulation",
                         "CRT emulation",
                         crt_emulation_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    colors_submenu = tui_menu_create("Colors", 1);
    tui_menu_add(colors_submenu, ui_color_items);
    tui_menu_add_submenu(parent_submenu, "Colors",
                         "Colors",
                         colors_submenu,
                         NULL, 0,
                         TUI_MENU_BEH_CONTINUE);
}
