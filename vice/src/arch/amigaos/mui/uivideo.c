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

#include "uireu.h"
#include "intl.h"
#include "translate.h"

static video_canvas_t *video_canvas = NULL;
static char *video_palette_filename_text = NULL;

static int ui_video_enable_translate[] = {
    IDMS_DISABLED,
    IDS_ENABLED,
    0
};

static char *ui_video_enable[countof(ui_video_enable_translate)];

static const int ui_video_enable_values[] = {
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

static const int ui_render_filter_values[] = {
    0,
    1,
    2,
    -1
};

static int ui_range_0_4[] = {
    0,
    4000
};

static int ui_range_0_2[] = {
    0,
    2000
};

static int ui_range_0_1[] = {
    0,
    1000
};

static ui_to_from_t ui_to_from_palette[] = {
    { NULL, MUI_TYPE_CYCLE, NULL, ui_video_enable, ui_video_enable_values, NULL },
    { NULL, MUI_TYPE_FILENAME, NULL, NULL, NULL, NULL },
    UI_END /* mandatory */
};

static ui_to_from_t ui_to_from_colors[] = {
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_4, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_2, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_2, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_2, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_2, NULL },
    UI_END /* mandatory */
};

static ui_to_from_t ui_to_from_render_filter[] = {
    { NULL, MUI_TYPE_CYCLE, NULL, ui_render_filter, ui_render_filter_values, NULL },
    UI_END /* mandatory */
};

static ui_to_from_t ui_to_from_crt[] = {
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_1, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_1, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_2, NULL },
    { NULL, MUI_TYPE_FLOAT, NULL, NULL, ui_range_0_2, NULL },
    UI_END /* mandatory */
};

static ULONG Browse_palette(struct Hook *hook, Object *obj, APTR arg)
{
    char *fname = NULL;

    fname = BrowseFile(translate_text(IDS_EXTERNAL_PALETTE_SELECT), "#?.vpl", video_canvas);

    if (fname != NULL) {
        set(ui_to_from_palette[1].object, MUIA_String_Contents, fname);
    }

    return 0;
}

static APTR build_gui_palette(void)
{
    APTR app, ui, ok, browse_button, cancel;

#ifdef AMIGA_MORPHOS
    static const struct Hook BrowseFileHook = { { NULL, NULL }, (VOID *)HookEntry, (VOID *)Browse_palette, NULL };
#else
    static const struct Hook BrowseFileHook = { { NULL, NULL }, (VOID *)Browse_palette, NULL, NULL };
#endif

    app = mui_get_app();

    ui = GroupObject,
           CYCLE(ui_to_from_palette[0].object, ui_to_from_palette[0].resource, ui_video_enable)
           FILENAME(ui_to_from_palette[1].object, video_palette_filename_text, browse_button)
           OK_CANCEL_BUTTON
         End;

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);

        DoMethod(browse_button, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_CallHook, &BrowseFileHook);
    }

    return ui;
}

static APTR build_gui_colors(void)
{
    APTR app, ui, ok, cancel;

    app = mui_get_app();

    ui = GroupObject,
           NSTRING(ui_to_from_colors[0].object, translate_text(IDS_GAMMA_0_4), ".0123456789", 5+1)
           NSTRING(ui_to_from_colors[1].object, translate_text(IDS_TINT_0_2), ".0123456789", 5+1)
           NSTRING(ui_to_from_colors[2].object, translate_text(IDS_SATURATION_0_2), ".0123456789", 5+1)
           NSTRING(ui_to_from_colors[3].object, translate_text(IDS_CONTRAST_0_2), ".0123456789", 5+1)
           NSTRING(ui_to_from_colors[4].object, translate_text(IDS_BRIGHTNESS_0_2), ".0123456789", 5+1)
           OK_CANCEL_BUTTON
         End;

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);
    }

    return ui;
}

static APTR build_gui_render_filter(void)
{
    APTR app, ui, ok, cancel;

    app = mui_get_app();

    ui = GroupObject,
           CYCLE(ui_to_from_render_filter[0].object, ui_to_from_render_filter[0].resource, ui_render_filter)
           OK_CANCEL_BUTTON
         End;

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);
    }

    return ui;
}

static APTR build_gui_crt(void)
{
    APTR app, ui, ok, cancel;

    app = mui_get_app();

    ui = GroupObject,
           NSTRING(ui_to_from_crt[0].object, translate_text(IDS_SCANLINE_SHADE_0_1), ".0123456789", 5+1)
           NSTRING(ui_to_from_crt[1].object, translate_text(IDS_BLUR_0_1), ".0123456789", 5+1)
           NSTRING(ui_to_from_crt[2].object, translate_text(IDS_ODDLINE_PHASE_0_2), ".0123456789", 5+1)
           NSTRING(ui_to_from_crt[3].object, translate_text(IDS_ODDLINE_OFFSET_0_2), ".0123456789", 5+1)
           OK_CANCEL_BUTTON
         End;

    if (ui != NULL) {
        DoMethod(cancel, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
                 app, 2, MUIM_Application_ReturnID, BTN_OK);
    }

    return ui;
}

void ui_video_palette_settings_dialog(video_canvas_t *canvas, char *palette_enable_res, char *palette_filename_res, char *palette_filename_text)
{
    APTR window;

    video_canvas = canvas;
    ui_to_from_palette[0].resource = palette_enable_res;
    ui_to_from_palette[1].resource = palette_filename_res;
    video_palette_filename_text = palette_filename_text;
    intl_convert_mui_table(ui_video_enable_translate, ui_video_enable);

    window = mui_make_simple_window(build_gui_palette(), translate_text(IDS_PALETTE_SETTINGS));

    if (window != NULL) {
        mui_add_window(window);
        ui_get_to(ui_to_from_palette);
        set(window, MUIA_Window_Open, TRUE);
        if (mui_run() == BTN_OK) {
            ui_get_from(ui_to_from_palette);
        }
        set(window, MUIA_Window_Open, FALSE);
        mui_rem_window(window);
        MUI_DisposeObject(window);
    }
}

void ui_video_color_settings_dialog(video_canvas_t *canvas, char *gamma_res, char *tint_res, char *saturation_res, char *contrast_res, char *brightness_res)
{
    APTR window;

    video_canvas = canvas;
    ui_to_from_colors[0].resource = gamma_res;
    ui_to_from_colors[1].resource = tint_res;
    ui_to_from_colors[2].resource = saturation_res;
    ui_to_from_colors[3].resource = contrast_res;
    ui_to_from_colors[4].resource = brightness_res;

    window = mui_make_simple_window(build_gui_colors(), translate_text(IDS_COLOR_SETTINGS));

    if (window != NULL) {
        mui_add_window(window);
        ui_get_to(ui_to_from_colors);
        set(window, MUIA_Window_Open, TRUE);
        if (mui_run() == BTN_OK) {
            ui_get_from(ui_to_from_colors);
        }
        set(window, MUIA_Window_Open, FALSE);
        mui_rem_window(window);
        MUI_DisposeObject(window);
    }
}

void ui_video_render_filter_settings_dialog(video_canvas_t *canvas, char *render_filter_res)
{
    APTR window;

    video_canvas = canvas;
    ui_to_from_render_filter[0].resource = render_filter_res;

    intl_convert_mui_table(ui_render_filter_translate, ui_render_filter);

    window = mui_make_simple_window(build_gui_render_filter(), translate_text(IDS_RENDER_FILTER_SETTINGS));

    if (window != NULL) {
        mui_add_window(window);
        ui_get_to(ui_to_from_render_filter);
        set(window, MUIA_Window_Open, TRUE);
        if (mui_run() == BTN_OK) {
            ui_get_from(ui_to_from_render_filter);
        }
        set(window, MUIA_Window_Open, FALSE);
        mui_rem_window(window);
        MUI_DisposeObject(window);
    }
}

void ui_video_crt_settings_dialog(video_canvas_t *canvas, char *scanline_shade_res, char *blur_res, char *oddline_phase_res, char *oddline_offset_res)
{
    APTR window;

    video_canvas = canvas;
    ui_to_from_crt[0].resource = scanline_shade_res;
    ui_to_from_crt[1].resource = blur_res;
    ui_to_from_crt[2].resource = oddline_phase_res;
    ui_to_from_crt[3].resource = oddline_offset_res;

    window = mui_make_simple_window(build_gui_crt(), translate_text(IDS_CRT_SETTINGS));

    if (window != NULL) {
        mui_add_window(window);
        ui_get_to(ui_to_from_crt);
        set(window, MUIA_Window_Open, TRUE);
        if (mui_run() == BTN_OK) {
            ui_get_from(ui_to_from_crt);
        }
        set(window, MUIA_Window_Open, FALSE);
        mui_rem_window(window);
        MUI_DisposeObject(window);
    }
}
