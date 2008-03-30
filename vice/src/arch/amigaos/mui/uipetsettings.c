/*
 * uipetsettings.c
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

#include "uipetsettings.h"
#include "intl.h"
#include "translate.h"

static char *ui_pet_ram_size[] = {
  "4K",
  "8K",
  "16K",
  "32K",
  "96K",
  "128K",
  NULL
};

static const int ui_pet_ram_size_values[] = {
  4,
  8,
  16,
  32,
  96,
  128,
  -1
};

static int ui_pet_video_translate[] = {
  IDS_AUTO_FROM_ROM,
  IDS_40_COLUMNS,
  IDS_80_COLUMNS,
  0
};

static char *ui_pet_video[countof(ui_pet_video_translate)];

static const int ui_pet_video_values[] = {
  0,
  40,
  80,
  -1
};

static char *pet_model[] = {
  "2001",
  "3008",
  "3016",
  "3032",
  "3032B",
  "4016",
  "4032",
  "4032B",
  "8032",
  "8096",
  "8296",
  "SuperPET"
};

static int ui_pet_model_translate[] = {
  IDS_CURRENT_MODEL,
  -1
};

static char *ui_pet_model[] = {
  NULL,		/* "Current model" place-holder */
  "PET 2001-8N",
  "PET 3008",
  "PET 3016",
  "PET 3032",
  "PET 3032B",
  "PET 4016",
  "PET 4032",
  "PET 4032B",
  "PET 8032",
  "PET 8096",
  "PET 8296",
  "SuperPET",
  NULL
};

static int ui_pet_io_size_translate[] = {
  IDS_256_BYTE,
  IDS_2_KBYTE,
  0
};

static char *ui_pet_io_size[countof(ui_pet_io_size_translate)];

static const int ui_pet_io_size_values[] = {
  0x100,
  0x800,
  -1
};

static int ui_pet_enable_translate[] = {
  IDMS_DISABLED,
  IDS_ENABLED,
  0
};

static char *ui_pet_enable[countof(ui_pet_enable_translate)];

static const int ui_pet_enable_values[] = {
  0,
  1,
  -1
};

static int ui_pet_keyboard_translate[] = {
  IDS_GRAPHICS,
  IDS_BUSINESS_UK,
  0
};

static char *ui_pet_keyboard[countof(ui_pet_keyboard_translate)];

static const int ui_pet_keyboard_values[] = {
  2,
  0,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "RamSize", ui_pet_ram_size, ui_pet_ram_size_values },
  { NULL, MUI_TYPE_CYCLE, "VideoSize", ui_pet_video, ui_pet_video_values },
  { NULL, MUI_TYPE_CYCLE, "IOSize", ui_pet_io_size, ui_pet_io_size_values },
  { NULL, MUI_TYPE_CYCLE, "KeymapIndex", ui_pet_keyboard, ui_pet_keyboard_values },
  { NULL, MUI_TYPE_CYCLE, "Crtc", ui_pet_enable, ui_pet_enable_values },
  { NULL, MUI_TYPE_CYCLE, "SuperPET", ui_pet_enable, ui_pet_enable_values },
  { NULL, MUI_TYPE_CYCLE, "Ram9", ui_pet_enable, ui_pet_enable_values },
  { NULL, MUI_TYPE_CYCLE, "RamA", ui_pet_enable, ui_pet_enable_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  APTR app, ui, ok, cancel;

  app = mui_get_app();

  ui = GroupObject,
    CYCLE(ui_to_from[0].object, translate_text(IDS_MEMORY), ui_pet_ram_size)
    CYCLE(ui_to_from[1].object, translate_text(IDS_VIDEO_SIZE), ui_pet_video)
    CYCLE(ui_to_from[2].object, translate_text(IDS_IO_SIZE), ui_pet_io_size)
    CYCLE(ui_to_from[3].object, translate_text(IDS_KEYBOARD), ui_pet_keyboard)
    CYCLE(ui_to_from[4].object, translate_text(IDS_CRTC_CHIP), ui_pet_enable)
    CYCLE(ui_to_from[5].object, translate_text(IDS_SUPERPET_IO), ui_pet_enable)
    CYCLE(ui_to_from[6].object, translate_text(IDS_9xxx_AS_RAM), ui_pet_enable)
    CYCLE(ui_to_from[7].object, translate_text(IDS_Axxx_AS_RAM), ui_pet_enable)
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

static APTR model;

static APTR build_pet_model_gui(void)
{
  APTR app, ui, ok, cancel;

  app = mui_get_app();

  ui = GroupObject,
    CYCLE(model, translate_text(IDS_PET_MODEL), ui_pet_model)
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

void ui_pet_model_dialog(void)
{
  APTR window;
  int val;

  intl_convert_mui_table(ui_pet_model_translate, ui_pet_model);

  window = mui_make_simple_window(build_pet_model_gui(), translate_text(IDS_SET_PET_MODEL));

  if (window != NULL) {
    mui_add_window(window);
    set(model, MUIA_Cycle_Active, 0);
    set(window, MUIA_Window_Open, TRUE);
    if (mui_run() == BTN_OK)
    {
      get(model, MUIA_Cycle_Active, (APTR)&val);
      if (val!=0)
      {
        pet_set_model(pet_model[val-1], NULL);
      }
    }
    set(window, MUIA_Window_Open, FALSE);
    mui_rem_window(window);
    MUI_DisposeObject(window);
  }
}

void ui_pet_settings_dialog(void)
{
  APTR window;

  intl_convert_mui_table(ui_pet_video_translate, ui_pet_video);
  intl_convert_mui_table(ui_pet_io_size_translate, ui_pet_io_size);
  intl_convert_mui_table(ui_pet_enable_translate, ui_pet_enable);
  intl_convert_mui_table(ui_pet_keyboard_translate, ui_pet_keyboard);

  window = mui_make_simple_window(build_gui(), translate_text(IDS_PET_SETTINGS));

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
