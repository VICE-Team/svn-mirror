/*
 * uidigimax.c
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

#include "uidigimax.h"
#include "intl.h"
#include "translate.h"

static video_canvas_t *digimax_canvas;

static int ui_digimax_enable_translate[] = {
  IDMS_DISABLED,
  IDS_ENABLED,
  0
};

static char *ui_digimax_enable[countof(ui_digimax_enable_translate)];


static const int ui_digimax_enable_values[] = {
  0,
  1,
  -1
};

static char *ui_digimax_base[] = {
  "Userport Interface",
  "$DE00",
  "$DE20",
  "$DE40",
  "$DE60",
  "$DE80",
  "$DEA0",
  "$DEC0",
  "$DEE0",
  "$DF00",
  "$DF20",
  "$DF40",
  "$DF60",
  "$DF80",
  "$DFA0",
  "$DFC0",
  "$DFE0",
  NULL
};

static const int ui_digimax_base_values[] = {
  0xdd00,   /* special case, userport interface */
  0xde00,
  0xde20,
  0xde40,
  0xde60,
  0xde80,
  0xdea0,
  0xdec0,
  0xdee0,
  0xdf00,
  0xdf20,
  0xdf40,
  0xdf60,
  0xdf80,
  0xdfa0,
  0xdfc0,
  0xdfe0,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "DIGIMAX", ui_digimax_enable, ui_digimax_enable_values },
  { NULL, MUI_TYPE_CYCLE, "DIGIMAXbase", ui_digimax_base, ui_digimax_base_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  APTR app, ui, ok, cancel;

  app = mui_get_app();

  ui = GroupObject,
    CYCLE(ui_to_from[0].object, translate_text(IDS_DIGIMAX_ENABLED), ui_digimax_enable)
    CYCLE(ui_to_from[1].object, translate_text(IDS_DIGIMAX_BASE), ui_digimax_base)
    OK_CANCEL_BUTTON
  End;

  if (ui != NULL) {
    DoMethod(cancel,
      MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(ok, MUIM_Notify, MUIA_Pressed, FALSE,
      app, 2, MUIM_Application_ReturnID, BTN_OK);
  }

  return ui;
}

void ui_digimax_settings_dialog(video_canvas_t *canvas)
{
  APTR window;

  digimax_canvas=canvas;
  intl_convert_mui_table(ui_digimax_enable_translate, ui_digimax_enable);

  window = mui_make_simple_window(build_gui(), translate_text(IDS_DIGIMAX_SETTINGS));

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
