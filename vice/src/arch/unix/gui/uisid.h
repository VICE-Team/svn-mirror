/*
 * uisid.h
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef VICE_UI_SID_H
#define VICE_UI_SID_H

#include "uimenu.h"

#define SID_D4XX_MENU(menu, callback)                                                          \
static ui_menu_entry_t menu[] = {                                                              \
    { "$D420", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd420, NULL }, \
    { "$D440", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd440, NULL }, \
    { "$D460", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd460, NULL }, \
    { "$D480", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd480, NULL }, \
    { "$D4A0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd4a0, NULL }, \
    { "$D4C0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd4c0, NULL }, \
    { "$D4D0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd4e0, NULL }, \
    { NULL }                                                                                   \
};

#define SID_D5XX_MENU(menu, callback)                                                          \
static ui_menu_entry_t menu[] = {                                                              \
    { "$D500", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd500, NULL }, \
    { "$D520", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd520, NULL }, \
    { "$D540", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd540, NULL }, \
    { "$D560", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd560, NULL }, \
    { "$D580", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd580, NULL }, \
    { "$D5A0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd5a0, NULL }, \
    { "$D5C0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd5c0, NULL }, \
    { "$D5D0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd5e0, NULL }, \
    { NULL }                                                                                   \
};

#define SID_D6XX_MENU(menu, callback)                                                          \
static ui_menu_entry_t menu[] = {                                                              \
    { "$D600", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd600, NULL }, \
    { "$D620", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd620, NULL }, \
    { "$D640", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd640, NULL }, \
    { "$D660", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd660, NULL }, \
    { "$D680", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd680, NULL }, \
    { "$D6A0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd6a0, NULL }, \
    { "$D6C0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd6c0, NULL }, \
    { "$D6D0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd6e0, NULL }, \
    { NULL }                                                                                   \
};

#define SID_D7XX_MENU(menu, callback)                                                          \
static ui_menu_entry_t menu[] = {                                                              \
    { "$D700", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd700, NULL }, \
    { "$D720", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd720, NULL }, \
    { "$D740", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd740, NULL }, \
    { "$D760", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd760, NULL }, \
    { "$D780", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd780, NULL }, \
    { "$D7A0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd7a0, NULL }, \
    { "$D7C0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd7c0, NULL }, \
    { "$D7D0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xd7e0, NULL }, \
    { NULL }                                                                                   \
};

#define SID_DEXX_MENU(menu, callback)                                                          \
static ui_menu_entry_t menu[] = {                                                              \
    { "$DE00", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xde00, NULL }, \
    { "$DE20", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xde20, NULL }, \
    { "$DE40", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xde40, NULL }, \
    { "$DE60", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xde60, NULL }, \
    { "$DE80", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xde80, NULL }, \
    { "$DEA0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdea0, NULL }, \
    { "$DEC0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdec0, NULL }, \
    { "$DED0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdee0, NULL }, \
    { NULL }                                                                                   \
};

#define SID_DFXX_MENU(menu, callback)                                                          \
static ui_menu_entry_t menu[] = {                                                              \
    { "$DF00", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdf00, NULL }, \
    { "$DF20", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdf20, NULL }, \
    { "$DF40", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdf40, NULL }, \
    { "$DF60", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdf60, NULL }, \
    { "$DF80", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdf80, NULL }, \
    { "$DFA0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdfa0, NULL }, \
    { "$DFC0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdfc0, NULL }, \
    { "$DFD0", UI_MENU_TYPE_TICK, (ui_callback_t)callback, (ui_callback_data_t)0xdfe0, NULL }, \
    { NULL }                                                                                   \
};

extern struct ui_menu_entry_s sid_model_submenu[];
extern struct ui_menu_entry_s sid_resid_sampling_submenu[];

extern struct ui_menu_entry_s sid_extra_sids_submenu[];

extern UI_CALLBACK(set_sid_resid_passband);

extern void uisid_model_menu_create(void);
extern void uisid_model_menu_shutdown(void);

#endif
