/*
 * joystick.c - Joystick support for Acorn.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#include "ROlib.h"
#include "joystick.h"
#include "kbd.h"
#include "config.h"
#include "types.h"
#include "resources.h"
#include "vsync.h"



int ajoyfd[2] = {-1, -1};
int djoyfd[2] = {-1, -1};

int joystick_port_map[2];
static BYTE old_joy[2];



static int set_joystick_port1(resource_value_t val)
{
  joystick_port_map[0] = (int)val;
  return 0;
}

static int set_joystick_port2(resource_value_t val)
{
  joystick_port_map[1] = (int)val;
  return 0;
}


static resource_t resources[] = {
  {"JoyDevice1", RES_INTEGER, (resource_value_t)JOYDEV_NONE,
    (resource_value_t)&joystick_port_map[0], set_joystick_port1},
  {"JoyDevice2", RES_INTEGER, (resource_value_t)JOYDEV_KBD1,
    (resource_value_t)&joystick_port_map[1], set_joystick_port2},
  {NULL}
};


void joystick_init(void)
{
  old_joy[0] = 0; old_joy[1] = 0;
}


void joystick_close(void)
{
}


void joystick(void)
{
  int port, state;
  BYTE code;

  if (EmuWindowHasInputFocus == 0) return;

  for (port=0; port<2; port++)
  {
    state = -1;
    if (joystick_port_map[port] == JOYDEV_JOY1) state = 0;
    else if (joystick_port_map[port] == JOYDEV_JOY2) state = 1;

    if (state >= 0)
    {
      state = Joystick_Read(state);
      if (state != -2)
      {
        if (state == -1) code = old_joy[port];
        else
        {
          code = 0;
          if ((state & (JoyButton1 + JoyButton2)) != 0) code |= 0x10;
          if ((state & 0x80) == 0)
          {
            if ((state & 0xff) >= JoyDir_Thresh) code |= 1;
          }
          else
          {
            if ((256 - (state & 0xff)) >= JoyDir_Thresh) code |= 2;
          }
          if ((state & 0x8000) == 0)
          {
            if ((state & 0xff00) >= (JoyDir_Thresh << 8)) code |= 8;
          }
          else
          {
            if ((0x10000 - (state & 0xff00)) >= (JoyDir_Thresh << 8)) code |= 4;
          }
          old_joy[port] = code;
        }
        joystick_value[port+1] = code;
      }
    }
  }
}


int joystick_init_resources(void)
{
  return resources_register(resources);
}


int joystick_init_cmdline_options(void)
{
  return 0;
}
