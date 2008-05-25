/*
 * uivideoc128.c
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

#include "uivideoc128.h"
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

static int ui_PAL_mode_translate[] = {
  IDS_FAST_PAL,
  IDS_PAL_EMULATION,
  0
};

static char *ui_PAL_mode[countof(ui_PAL_mode_translate)];

static const int ui_PAL_mode_values[] = {
  0,
  1,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_FLOAT, "ColorGamma", NULL, NULL },
  { NULL, MUI_TYPE_FLOAT, "PALScanLineShade", NULL, NULL },
  { NULL, MUI_TYPE_FLOAT, "PALBlur", NULL, NULL },
  { NULL, MUI_TYPE_CYCLE, "PALMode", ui_PAL_mode, ui_PAL_mode_values },
  { NULL, MUI_TYPE_CYCLE, "VICIIExternalPalette", ui_external_palette_enable, ui_external_palette_enable_values },
  { NULL, MUI_TYPE_FILENAME, "VICIIPaletteFile", NULL, NULL },
  { NULL, MUI_TYPE_FLOAT, "ColorSaturation", NULL, NULL },
  { NULL, MUI_TYPE_FLOAT, "ColorContrast", NULL, NULL },
  { NULL, MUI_TYPE_FLOAT, "ColorBrightness", NULL, NULL },
  { NULL, MUI_TYPE_FILENAME, "VDCPaletteFile", NULL, NULL },
  UI_END /* mandatory */
};

static ULONG BrowseVICII( struct Hook *hook, Object *obj, APTR arg )
{
  char *fname=NULL;

  fname=BrowseFile(translate_text(IDS_EXTERNAL_PALETTE_SELECT), "#?.vpl", video_canvas);

  if (fname!=NULL)
    set(ui_to_from[5].object, MUIA_String_Contents, fname);

  return 0;
}

static ULONG BrowseVDC( struct Hook *hook, Object *obj, APTR arg )
{
  char *fname=NULL;

  fname=BrowseFile(translate_text(IDS_EXTERNAL_PALETTE_SELECT), "#?.vpl", video_canvas);

  if (fname!=NULL)
    set(ui_to_from[9].object, MUIA_String_Contents, fname);

  return 0;
}

static APTR build_gui(void)
{
  APTR app, ui, ok, browse_button1, browse_button2, cancel;

#ifdef AMIGA_MORPHOS
  static const struct Hook BrowseVICIIFileHook = { { NULL,NULL },(VOID *)HookEntry,(VOID *)BrowseVICII, NULL};
  static const struct Hook BrowseVDCFileHook = { { NULL,NULL },(VOID *)HookEntry,(VOID *)BrowseVDC, NULL};
#else
  static const struct Hook BrowseVICIIFileHook = { { NULL,NULL },(VOID *)BrowseVICII,NULL,NULL };
  static const struct Hook BrowseVDCFileHook = { { NULL,NULL },(VOID *)BrowseVDC,NULL,NULL };
#endif

  app = mui_get_app();

  ui = GroupObject,
    STRING(ui_to_from[0].object, translate_text(IDS_GAMMA_0_2), ".0123456789", 5+1)
    STRING(ui_to_from[1].object, translate_text(IDS_PAL_SHADE_0_1), ".0123456789", 5+1)
    STRING(ui_to_from[2].object, translate_text(IDS_PAL_BLUR_0_1), ".0123456789", 5+1)
    CYCLE(ui_to_from[3].object, translate_text(IDS_PAL_MODE), ui_PAL_mode)
    CYCLE(ui_to_from[4].object, translate_text(IDS_EXTERNAL_PALETTE), ui_external_palette_enable)
    FILENAME(ui_to_from[5].object, translate_text(IDS_VICII_PALETTE_FILENAME), browse_button1)
    STRING(ui_to_from[6].object, translate_text(IDS_SATURATION_0_2), ".0123456789", 5+1)
    STRING(ui_to_from[7].object, translate_text(IDS_CONTRAST_0_2), ".0123456789", 5+1)
    STRING(ui_to_from[8].object, translate_text(IDS_BRIGHTNESS_0_2), ".0123456789", 5+1)
    FILENAME(ui_to_from[9].object, translate_text(IDS_VDC_PALETTE_FILENAME), browse_button2)
    OK_CANCEL_BUTTON
  End;

  if (ui != NULL) {
    DoMethod(cancel,
      MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, BTN_OK);

    DoMethod(browse_button1, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_CallHook, &BrowseVICIIFileHook);

    DoMethod(browse_button2, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_CallHook, &BrowseVDCFileHook);
  }

  return ui;
}

void ui_video_c128_settings_dialog(video_canvas_t *canvas)
{
  APTR window;

  video_canvas=canvas;
  intl_convert_mui_table(ui_external_palette_enable_translate, ui_external_palette_enable);
  intl_convert_mui_table(ui_PAL_mode_translate, ui_PAL_mode);

  window = mui_make_simple_window(build_gui(), translate_text(IDS_VIDEO_SETTINGS));

  if (window != NULL) {
    mui_add_window(window);
    ui_get_to(ui_to_from);
    set(window, MUIA_Window_Open, TRUE);
    if (mui_run() == BTN_OK) {
      ui_get_from(ui_to_from);
    }
    set(window, MUIA_Window_Open, FALSE);
    mui_rem_window(window);
    MUI_DisposeObject(window);
  }
}
