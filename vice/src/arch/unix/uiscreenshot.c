/*
 * uiscreenshot.c - Screenshot UI.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "screenshot.h"
#include "uimenu.h"

UI_CALLBACK(save_screenshot)
{
    /* FIXME: A nice dialog is needed.  */
    int retval = 0;
    int saveval = (int)UI_MENU_CB_PARAM;

    switch (saveval & 15) {
      case 0:
        retval = screenshot_save("BMP", "otto.bmp", saveval / 16);
        break;
#if HAVE_PNG
      case 1:
        retval = screenshot_save("PNG", "otto.png", saveval / 16);
        break;
#endif
    }

    if (retval < 0)
        ui_error("Failed saving screenshot.");
}

