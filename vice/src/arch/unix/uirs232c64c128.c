/*
 * uirs232c64c128.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "uiacia1.h"
#include "uimenu.h"
#include "uirs232.h"
#include "uirs232c64c128.h"
#include "uirs232user.h"


UI_MENU_DEFINE_RADIO(Acia1Irq)

static ui_menu_entry_t uiacia1_irq_submenu[] = {
    { N_("*No IRQ/NMI"),
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)0, NULL },
    { N_("*IRQ"),
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)1, NULL },
    { N_("*NMI"),
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia1Mode)

static ui_menu_entry_t uiacia1_mode_submenu[] = {
    { N_("*Normal"),
      (ui_callback_t)radio_Acia1Mode, (ui_callback_data_t)0, NULL },
    { N_("*Swiftlink"),
      (ui_callback_t)radio_Acia1Mode, (ui_callback_data_t)1, NULL },
    { N_("*Turbo232"),
      (ui_callback_t)radio_Acia1Mode, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_TOGGLE(Acia1Enable)
UI_MENU_DEFINE_TOGGLE(RsUserEnable)

ui_menu_entry_t uirs232c64c128_submenu[] = {
    { N_("*ACIA $DExx RS232 interface emulation"),
      (ui_callback_t)toggle_Acia1Enable, NULL, NULL },
    { N_("ACIA $DExx device"),
      NULL, NULL, uiacia1_device_submenu },
    { N_("ACIA $DExx Interrupt"),
      NULL, NULL, uiacia1_irq_submenu },
    { N_("ACIA $DExx Emulation Mode"),
      NULL, NULL, uiacia1_mode_submenu },
    { "--" },
    { N_("*Userport RS232 emulation"),
      (ui_callback_t)toggle_RsUserEnable, NULL, NULL },
    { N_("Userport RS232 baud rate"),
      NULL, NULL, rs232user_baudrate_submenu },
    { N_("Userport RS232 device"),
      NULL, NULL, rs232user_device_submenu },
    { "--" },
    { N_("Serial 1 device..."), (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"),
      NULL, NULL, ser1_c64c128_baud_submenu },
    { "--" },
    { "Serial 2 device...", (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"),
      NULL, NULL, ser2_c64c128_baud_submenu },
    { "--" },
    { N_("Dump filename..."), (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--" },
    { N_("Program name to exec..."), (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};
