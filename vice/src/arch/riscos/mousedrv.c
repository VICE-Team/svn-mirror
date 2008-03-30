/*
 * mousedrv.x - Mouse handling for RISC OS
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*#include "log.h"*/
#include "mouse.h"
#include "mousedrv.h"
#include "ui.h"
#include "videoarch.h"

#include <ROlib.h>
#include <wimp.h>


static int lastMouseX = 0, lastMouseY = 0, lastMouseButtons = 0;


void mousedrv_sync(void)
{
  if (ActiveCanvas != NULL)
  {
    int newX, newY, newButtons;

    if (FullScreenMode != 0)
    {
      video_full_screen_mousepos(&newX, &newY, &newButtons);
    }
    else
    {
      int mblock[MouseB_Icon+1];
      int block[WindowB_WFlags+1];
      int mx, my, sx, sy;

      Wimp_GetPointerInfo(mblock);
      mx = mblock[MouseB_PosX]; my = mblock[MouseB_PosY];
      newButtons = mblock[MouseB_Buttons];
      video_canvas_get_scale(ActiveCanvas, &sx, &sy);
      block[WindowB_Handle] = ActiveCanvas->window->Handle;
      Wimp_GetWindowState(block);
      /* y direction inverted */
      newX = ((mx - (block[WindowB_VMinX] - block[WindowB_ScrollX])) >> ScreenMode.eigx) / sx;
      newY = (((block[WindowB_VMaxY] - block[WindowB_ScrollY]) - my) >> ScreenMode.eigy) / sy;
    }

    if ((newX >= 0) && (newX < ActiveCanvas->width) &&
        (newY >= 0) && (newY < ActiveCanvas->height))
    {
      lastMouseX = newX;
      lastMouseY = newY;
      /*log_message(LOG_DEFAULT, "POS%d: %d,%d", FullScreenMode, newX, newY);*/
    }
    else
      newButtons = 0;

    if (((newButtons ^ lastMouseButtons) & 1) != 0)
      mouse_button_right((newButtons & 1));
    if (((newButtons ^ lastMouseButtons) & 4) != 0)
      mouse_button_left((newButtons & 4));

    lastMouseButtons = newButtons;
  }
}

void mousedrv_mouse_changed(void)
{
}

int mousedrv_resources_init(void)
{
  return 0;
}

int mousedrv_cmdline_options_init(void)
{
  return 0;
}

void mousedrv_init(void)
{
}

BYTE mousedrv_get_x(void)
{
  return lastMouseX;
}

BYTE mousedrv_get_y(void)
{
  return lastMouseY;
}
