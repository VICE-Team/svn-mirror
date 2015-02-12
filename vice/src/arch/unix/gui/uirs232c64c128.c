/*
 * uirs232c64c128.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#if defined(HAVE_RS232DEV) || defined(HAVE_RS232NET)

#include <stdio.h>

#include "uiacia1.h"
#include "uimenu.h"
#include "uirs232.h"
#include "uirs232c64c128.h"
#include "uirs232user.h"

UI_MENU_DEFINE_RADIO(Acia1Irq)

static ui_menu_entry_t uiacia1_irq_submenu[] = {
    { N_("No IRQ/NMI"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)0, NULL },
    { "IRQ", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)1, NULL },
    { "NMI", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia1Mode)

static ui_menu_entry_t uiacia1_mode_submenu[] = {
    { N_("Normal"), UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Mode, (ui_callback_data_t)0, NULL },
    { "Swiftlink", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Mode, (ui_callback_data_t)1, NULL },
    { "Turbo232", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Mode, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia1Base)

static ui_menu_entry_t uiacia1_c64_base_submenu[] = {
    { "$DE00", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0xde00, NULL },
    { "$DF00", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0xdf00, NULL },
    { NULL }
};

static ui_menu_entry_t uiacia1_c128_base_submenu[] = {
    { "$D700", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0xd700, NULL },
    { "$DE00", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0xde00, NULL },
    { "$DF00", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0xdf00, NULL },
    { NULL }
};

static ui_menu_entry_t uiacia1_vic20_base_submenu[] = {
    { "$9800", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0x9800, NULL },
    { "$9C00", UI_MENU_TYPE_TICK,
      (ui_callback_t)radio_Acia1Base, (ui_callback_data_t)0x9c00, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Acia1Enable)
UI_MENU_DEFINE_TOGGLE(RsUserEnable)

ui_menu_entry_t uirs232_c64_submenu[] = {
    { N_("ACIA RS232 interface emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Acia1Enable, NULL, NULL },
    { N_("ACIA device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_device_submenu },
    { N_("ACIA base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_c64_base_submenu },
    { N_("ACIA interrupt"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_irq_submenu },
    { N_("ACIA emulation mode"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_mode_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport RS232 emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RsUserEnable, NULL, NULL },
    { N_("Userport RS232 baud rate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_baudrate_submenu },
    { N_("Userport RS232 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_device_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Serial 1 device"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser1_c64c128_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { "Serial 2 device", UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser2_c64c128_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Dump filename"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Program name to exec"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};

ui_menu_entry_t uirs232_c128_submenu[] = {
    { N_("ACIA RS232 interface emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Acia1Enable, NULL, NULL },
    { N_("ACIA device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_device_submenu },
    { N_("ACIA base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_c128_base_submenu },
    { N_("ACIA interrupt"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_irq_submenu },
    { N_("ACIA emulation mode"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_mode_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport RS232 emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RsUserEnable, NULL, NULL },
    { N_("Userport RS232 baud rate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_baudrate_submenu },
    { N_("Userport RS232 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_device_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Serial 1 device"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser1_c64c128_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { "Serial 2 device", UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser2_c64c128_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Dump filename"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Program name to exec"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};

ui_menu_entry_t uirs232_vic20_submenu[] = {
    { N_("ACIA RS232 interface emulation (MasC=uerade)"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Acia1Enable, NULL, NULL },
    { N_("ACIA device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_device_submenu },
    { N_("ACIA base address"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_vic20_base_submenu },
    { N_("ACIA interrupt"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_irq_submenu },
    { N_("ACIA emulation mode"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiacia1_mode_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport RS232 emulation"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_RsUserEnable, NULL, NULL },
    { N_("Userport RS232 baud rate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_baudrate_submenu },
    { N_("Userport RS232 device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, rs232user_device_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Serial 1 device"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser1_c64c128_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { "Serial 2 device", UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, ser2_c64c128_baud_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Dump filename"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Program name to exec"), UI_MENU_TYPE_DOTS, (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};

#endif
