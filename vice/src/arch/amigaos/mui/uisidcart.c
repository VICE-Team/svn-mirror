/*
 * uisidcart.c
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

#include "sid.h"
#include "uisidcart.h"
#include "intl.h"
#include "translate.h"

static int ui_sidcart_enable_translate[] = {
  IDMS_DISABLED,
  IDS_ENABLED,
  0
};

static char *ui_sidcart_enable[countof(ui_sidcart_enable_translate)];

static const int ui_sidcart_enable_values[] = {
  0,
  1,
  -1
};

static char *ui_sidcart_engine[] =
{
  "Fast SID",
#ifdef HAVE_CATWEASELMKIII
  "Catweasel MK3",
#endif
#ifdef HAVE_HARDSID
  "HardSID",
#endif
  NULL
};

static const int ui_sidcart_engine_values[] =
{
  SID_ENGINE_FASTSID,
#ifdef HAVE_CATWEASELMKIII
  SID_ENGINE_CATWEASELMKIII,
#endif
#ifdef HAVE_HARDSID
  SID_ENGINE_HARDSID,
#endif
  -1
};

static int ui_sidcart_model_translate[] =
{
  IDS_6581_OLD,
  IDS_8580_NEW,
  0
};

static char *ui_sidcart_model[countof(ui_sidcart_model_translate)];

static const int ui_sidcart_model_values[] =
{
  0,
  1,
  -1
};

static char *ui_sidcart_address[] =
{
  NULL, /* placeholder for primary address */
  NULL, /* placeholder for secondary address */
  NULL
};

static const int ui_sidcart_address_values[] =
{
  0,
  1,
  -1
};

static char *ui_sidcart_clock[] =
{
  "C64",
  NULL, /* placeholder for native clock */
  NULL
};

static const int ui_sidcart_clock_values[] =
{
  0,
  1,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "SidCart", ui_sidcart_enable, ui_sidcart_enable_values },
  { NULL, MUI_TYPE_CYCLE, "SidEngine", ui_sidcart_engine, ui_sidcart_engine_values },
  { NULL, MUI_TYPE_CYCLE, "SidModel", ui_sidcart_model, ui_sidcart_model_values },
  { NULL, MUI_TYPE_CYCLE, "SidFilters", ui_sidcart_enable, ui_sidcart_enable_values },
  { NULL, MUI_TYPE_CYCLE, "SidAddress", ui_sidcart_address, ui_sidcart_address_values },
  { NULL, MUI_TYPE_CYCLE, "SidClock", ui_sidcart_clock, ui_sidcart_clock_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, translate_text(IDS_SID_CART), ui_sidcart_enable)
    CYCLE(ui_to_from[1].object, translate_text(IDS_SID_ENGINE), ui_sidcart_engine)
    CYCLE(ui_to_from[2].object, translate_text(IDS_SID_MODEL), ui_sidcart_model)
    CYCLE(ui_to_from[3].object, translate_text(IDS_SID_FILTERS), ui_sidcart_enable)
    CYCLE(ui_to_from[4].object, translate_text(IDS_SID_ADDRESS), ui_sidcart_address)
    CYCLE(ui_to_from[5].object, translate_text(IDS_SID_CLOCK), ui_sidcart_clock)
  End;
}

void ui_sidcart_settings_dialog(char *addr1, char *addr2, char *clock)
{
  intl_convert_mui_table(ui_sidcart_enable_translate, ui_sidcart_enable);
  intl_convert_mui_table(ui_sidcart_model_translate, ui_sidcart_model);
  ui_sidcart_address[0]=addr1;
  ui_sidcart_address[1]=addr2;
  ui_sidcart_clock[1]=clock;
  mui_show_dialog(build_gui(), translate_text(IDS_SIDCART_SETTINGS), ui_to_from);
}
