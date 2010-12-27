/*
 * uivideo.c
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
#ifdef AMIGA_M68K
#define _INLINE_MUIMASTER_H
#endif
#include "mui.h"

#include "uivideo.h"
#include "intl.h"
#include "translate.h"

static video_canvas_t *video_canvas;

static int ui_external_palette_enable_translate[] = {
    IDMS_DISABLED,
    IDS_ENABLED,
    0
};

static char *ui_external_palette_enable[countof(ui_external_palette_enable_translate)];

static const int ui_external_palette_enable_values[] = {
    0,
    1,
    -1
};

static int ui_render_filter_translate[] = {
    IDS_NONE,
    IDS_CRT_EMULATION,
    IDS_SCALE2X,
    0
};

static char *ui_render_filter[countof(ui_render_filter_translate)];

static int ui_render_filter_without_scale2x_translate[] = {
    IDS_NONE,
    IDS_CRT_EMULATION,
    0
};

static char *ui_render_filter_without_scale2x[countof(ui_render_filter_without_scale2x_translate)];

static ui_to_from_t ui_to_from[] = {
    { NULL, MUI_TYPE_FLOAT, "ColorGamma", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "ColorTint", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "ColorSaturation", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "ColorContrast", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "ColorBrightness", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "PALScanLineShade", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "PALBlur", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "PALOddLinePhase", NULL, NULL, NULL },
    { NULL, MUI_TYPE_FLOAT, "PALOddLineOffset", NULL, NULL, NULL },
    { NULL, MUI_TYPE_CYCLE, NULL, ui_external_palette_enable, ui_external_palette_enable_values, NULL }, /* resource placeholder for palette 1 */
    { NULL, MUI_TYPE_FILENAME, NULL, NULL, NULL, NULL }, /* resource placeholder for palette 1 file */
    { NULL, MUI_TYPE_CYCLE, NULL, ui_external_palette_enable, ui_external_palette_enable_values, NULL }, /* resource placeholder for palette 2 */
    { NULL, MUI_TYPE_FILENAME, NULL, NULL, NULL, NULL }, /* resource placeholder for palette 2 file */
    UI_END /* mandatory */
};

static ULONG Browse1(struct Hook *hook, Object *obj, APTR arg)
{
    char *fname = NULL;

    fname = BrowseFile(translate_text(IDS_EXTERNAL_PALETTE_SELECT), "#?.vpl", video_canvas);

    if (fname != NULL) {
        set(ui_to_from[10].object, MUIA_String_Contents, fname);
    }

    return 0;
}

static ULONG Browse2( struct Hook *hook, Object *obj, APTR arg )
{
    char *fname = NULL;

    fname = BrowseFile(translate_text(IDS_EXTERNAL_PALETTE_SELECT), "#?.vpl", video_canvas);

    if (fname != NULL) {
        set(ui_to_from[12].object, MUIA_String_Contents, fname);
    }

    return 0;
}

static APTR render_filter;

static APTR build_gui(char *pal1extname, char *pal1fname, char *pal2extname, char *pal2fname, int scale2x)
{
    APTR app, ui, ok, browse_button1, browse_button2, cancel;

#ifdef AMIGA_MORPHOS
    static const struct Hook BrowseFileHook1 = { { NULL, NULL }, (VOID *)HookEntry, (VOID *)Browse1, NULL };
    static const struct Hook BrowseFileHook2 = { { NULL, NULL }, (VOID *)HookEntry, (VOID *)Browse2, NULL };
#else
    static const struct Hook BrowseFileHook1 = { { NULL, NULL }, (VOID *)Browse1, NULL, NULL };
    static const struct Hook BrowseFileHook2 = { { NULL, NULL }, (VOID *)Browse2, NULL, NULL };
#endif

    app = mui_get_app();

    if (scale2x == 1 && pal2extname != NULL) {
        ui = GroupObject,
               CYCLE(render_filter, translate_text(IDS_RENDER_FILTER), ui_render_filter)
               STRING(ui_to_from[0].object, translate_text(IDS_GAMMA_0_4), ".0123456789", 5+1)
               STRING(ui_to_from[1].object, translate_text(IDS_TINT_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[2].object, translate_text(IDS_SATURATION_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[3].object, translate_text(IDS_CONTRAST_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[4].object, translate_text(IDS_BRIGHTNESS_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[5].object, translate_text(IDS_SCANLINE_SHADE_0_1), ".0123456789", 5+1)
               STRING(ui_to_from[6].object, translate_text(IDS_BLUR_0_1), ".0123456789", 5+1)
               STRING(ui_to_from[7].object, translate_text(IDS_ODDLINE_PHASE_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[8].object, translate_text(IDS_ODDLINE_OFFSET_0_2), ".0123456789", 5+1)
               CYCLE(ui_to_from[9].object, pal1extname, ui_external_palette_enable)
               FILENAME(ui_to_from[10].object, pal1fname, browse_button1)
               CYCLE(ui_to_from[11].object, pal2extname, ui_external_palette_enable)
               FILENAME(ui_to_from[12].object, pal2fname, browse_button2)
               OK_CANCEL_BUTTON
             End;
    }

    if (scale2x == 1 && pal2extname == NULL) {
        ui = GroupObject,
               CYCLE(render_filter, translate_text(IDS_RENDER_FILTER), ui_render_filter)
               STRING(ui_to_from[0].object, translate_text(IDS_GAMMA_0_4), ".0123456789", 5+1)
               STRING(ui_to_from[1].object, translate_text(IDS_TINT_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[2].object, translate_text(IDS_SATURATION_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[3].object, translate_text(IDS_CONTRAST_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[4].object, translate_text(IDS_BRIGHTNESS_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[5].object, translate_text(IDS_SCANLINE_SHADE_0_1), ".0123456789", 5+1)
               STRING(ui_to_from[6].object, translate_text(IDS_BLUR_0_1), ".0123456789", 5+1)
               STRING(ui_to_from[7].object, translate_text(IDS_ODDLINE_PHASE_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[8].object, translate_text(IDS_ODDLINE_OFFSET_0_2), ".0123456789", 5+1)
               CYCLE(ui_to_from[9].object, pal1extname, ui_external_palette_enable)
               FILENAME(ui_to_from[10].object, pal1fname, browse_button1)
               OK_CANCEL_BUTTON
             End;
    }

    if (!scale2x) {
        ui = GroupObject,
               CYCLE(render_filter, translate_text(IDS_RENDER_FILTER), ui_render_filter_without_scale2x)
               STRING(ui_to_from[0].object, translate_text(IDS_GAMMA_0_4), ".0123456789", 5+1)
               STRING(ui_to_from[1].object, translate_text(IDS_TINT_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[2].object, translate_text(IDS_SATURATION_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[3].object, translate_text(IDS_CONTRAST_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[4].object, translate_text(IDS_BRIGHTNESS_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[5].object, translate_text(IDS_SCANLINE_SHADE_0_1), ".0123456789", 5+1)
               STRING(ui_to_from[6].object, translate_text(IDS_BLUR_0_1), ".0123456789", 5+1)
               STRING(ui_to_from[7].object, translate_text(IDS_ODDLINE_PHASE_0_2), ".0123456789", 5+1)
               STRING(ui_to_from[8].object, translate_text(IDS_ODDLINE_OFFSET_0_2), ".0123456789", 5+1)
               CYCLE(ui_to_from[9].object, pal1extname, ui_external_palette_enable)
               FILENAME(ui_to_from[10].object, pal1fname, browse_button1)
               OK_CANCEL_BUTTON
             End;
    }

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);

        DoMethod(browse_button1, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_CallHook, &BrowseFileHook1);

        if (pal2extname != NULL) {
            DoMethod(browse_button2, MUIM_Notify, MUIA_Pressed, FALSE,
                     app, 2, MUIM_CallHook, &BrowseFileHook2);
        }
    }

    return ui;
}

void ui_video_settings_dialog(video_canvas_t *canvas,
                              char *px1r, char *pf1r, char *px1n, char *pf1n,
                              char *px2r, char *pf2r, char *px2n, char *pf2n,
                              char *s2r)
{
    APTR window;
    int res_value;
    int crtemul, s2x;
    int scale2x = (s2r != NULL) ? 1 : 0;

    video_canvas = canvas;
    intl_convert_mui_table(ui_external_palette_enable_translate, ui_external_palette_enable);
    intl_convert_mui_table(ui_render_filter_translate, ui_render_filter);
    intl_convert_mui_table(ui_render_filter_without_scale2x_translate, ui_render_filter_without_scale2x);
    ui_to_from[9].resource = px1r;
    ui_to_from[10].resource = pf1r;
    ui_to_from[11].resource = px2r;
    ui_to_from[12].resource = pf2r;

    window = mui_make_simple_window(build_gui(px1n, pf1n, px2n, pf2n, scale2x), translate_text(IDS_VIDEO_SETTINGS));

    if (window != NULL) {
        mui_add_window(window);

        resources_get_int("PALEmulation", &res_value);
        if (res_value == 1) {
            set(render_filter, MUIA_Cycle_Active, 1);
        } else {
            if (scale2x) {
                resources_get_int(s2r, &res_value);
                if (res_value == 1) {
                    set(render_filter, MUIA_Cycle_Active, 2);
                } else {
                    set(render_filter, MUIA_Cycle_Active, 0);
                }
            } else {
                set(render_filter, MUIA_Cycle_Active, 0);
            }
        }
        ui_get_to(ui_to_from);
        set(window, MUIA_Window_Open, TRUE);
        if (mui_run() == BTN_OK) {
            get(render_filter, MUIA_Cycle_Active, (APTR)&res_value);
            switch (res_value) {
                default:
                case 0:
                    crtemul = 0;
                    s2x = 0;
                    break;
                case 1:
                    crtemul = 1;
                    s2x = 0;
                    break;
                case 2:
                    crtemul = 0;
                    s2x = 1;
                    break;
            }
            resources_set_int("PALEmulation", crtemul);
            if (scale2x) {
                resources_set_int(s2r, s2x);
            }
            ui_get_from(ui_to_from);
        }
        set(window, MUIA_Window_Open, FALSE);
        mui_rem_window(window);
        MUI_DisposeObject(window);
    }
}
