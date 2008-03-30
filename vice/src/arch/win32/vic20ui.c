/*
 * vic20ui.c - VIC20-specific user interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli (ettore@comm2000.it)
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
#include <windows.h>
#include <windowsx.h>

#include "ui.h"

#include "cartridge.h"
#include "res.h"
#include "uilib.h"
#include "vic20ui.h"
#include "winmain.h"

/* Probably one should simply remove the size numbers from the IDM_* stuff */
void vic20_ui_specific(WPARAM wparam, HWND hwnd)
{
    switch (wparam) {
      case IDM_CART_VIC20_8KB_2000:
      case IDM_CART_VIC20_16KB_4000:
      case IDM_CART_VIC20_8KB_6000:
      case IDM_CART_VIC20_8KB_A000:
      case IDM_CART_VIC20_4KB_B000:
        {
            char *s;
            int type;

            switch (wparam) {
              case IDM_CART_VIC20_8KB_2000:
                type = CARTRIDGE_VIC20_16KB_2000;
                if ((s = ui_select_file("Attach 4/8/16KB cartridge image at $2000",
                    "8KB cartridge image files (*.*)\0*.*\0"
                    "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                    if (cartridge_attach_image(type, s) < 0)
                        ui_error("Invalid cartridge image");
                    free(s);
                }
                break;
              case IDM_CART_VIC20_16KB_4000:
                type = CARTRIDGE_VIC20_16KB_4000;
                if ((s = ui_select_file("Attach 4/8/16KB cartridge image at $4000",
                    "cartridge image files (*.*)\0*.*\0"
                    "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                    if (cartridge_attach_image(type, s) < 0)
                        ui_error("Invalid cartridge image");
                    free(s);
                }
                break;
              case IDM_CART_VIC20_8KB_6000:
                type = CARTRIDGE_VIC20_16KB_6000;
                if ((s = ui_select_file("Attach 4/8/16KB cartridge image at $6000",
                    "8KB cartridge image files (*.*)\0*.*\0"
                    "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                    if (cartridge_attach_image(type, s) < 0)
                        ui_error("Invalid cartridge image");
                    free(s);
                }
                break;
              case IDM_CART_VIC20_8KB_A000:
                type = CARTRIDGE_VIC20_8KB_A000;
                if ((s = ui_select_file("Attach 8KB cartridge image at $A000",
                    "8KB cartridge image files (*.*)\0*.*\0"
                    "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                    if (cartridge_attach_image(type, s) < 0)
                        ui_error("Invalid cartridge image");
                    free(s);
                }
                break;
              case IDM_CART_VIC20_4KB_B000:
                type = CARTRIDGE_VIC20_4KB_B000;
                if ((s = ui_select_file("Attach 4KB cartridge image at $B000",
                    "4KB cartridge image files (*.*)\0*.*\0"
                    "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                    if (cartridge_attach_image(type, s) < 0)
                        ui_error("Invalid cartridge image");
                    free(s);
                }
                break;
            }
        }
        break;
      case IDM_CART_SET_DEFAULT:
        cartridge_set_default();
        break;
      case IDM_CART_DETACH:
        cartridge_detach_image();
        break;
    }
}

int vic20_ui_init(void)
{
    ui_register_machine_specific(vic20_ui_specific);
    return 0;
}

