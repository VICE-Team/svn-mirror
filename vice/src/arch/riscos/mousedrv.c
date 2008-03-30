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

#include "mouse.h"
#include "mousedrv.h"
#include "ui.h"
#include "videoarch.h"

#include <ROlib.h>
#include <wimp.h>


static int lastMouseX = 0, lastMouseY = 0;


void mousedrv_sync(void)
{
  if (ActiveCanvas != NULL)
  {
    mouse_desc mdesc = {0, 0, 0, 0};

    ReadMouseState(&mdesc);
    if (FullScreenMode == 0)
    {
      video_full_screen_mousepos(&mdesc, &lastMouseX, &lastMouseY);
    }
    else
    {
      int block[WindowB_WFlags+1];
      block[WindowB_Handle] = ActiveCanvas->window->Handle;
      Wimp_GetWindowState(block);
      /* y direction inverted */
      lastMouseX = (mdesc.x - (block[WindowB_VMinX] - block[WindowB_ScrollX])) >> ScreenMode.eigx;
      lastMouseY = ((block[WindowB_VMaxY] - block[WindowB_ScrollY]) - mdesc.y) >> ScreenMode.eigy;
    }

    if (lastMouseX < 0)
      lastMouseX = 0;
    if (lastMouseX >= ActiveCanvas->width)
      lastMouseX = ActiveCanvas->width - 1;
    if (lastMouseY < 0)
      lastMouseY = 0;
    if (lastMouseY >= ActiveCanvas->height)
      lastMouseY = ActiveCanvas->height - 1;
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
