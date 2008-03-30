/*
 * vicii-clock-stretch.c - 8502 clock cycle strechting routines.
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

#include "maincpu.h"
#include "vicii.h"
#include "viciitypes.h"

CLOCK vicii_clock_add(CLOCK clock, int amount)
{
  CLOCK tmp_clock=clock;

  if (vicii.fastmode!=0)
  {
    if (amount>0)
    {
      tmp_clock+=(amount>>1);
      vicii.half_cycles+=amount&1;
      if (vicii.half_cycles>1)
      {
        tmp_clock++;
        vicii.half_cycles=0;
      }
    }
    else
    {
      tmp_clock-=((-amount)>>1);
      vicii.half_cycles-=(-amount)&1;
      if (vicii.half_cycles<0)
      {
        tmp_clock--;
        vicii.half_cycles=1;
      }
    }
  }
  else
  {
    tmp_clock+=amount;
  }
  return tmp_clock;
}


/* if half cycle is 0, add extra half cycle to stretch */
void vicii_clock_read_stretch(void)
{
  if (vicii.fastmode!=0 && vicii.half_cycles==0)
  {
    vicii.half_cycles=1;
    maincpu_stretch=1;
  }
}

/* add 1 full cycle for 2 cycle stretch if rmw,
   otherwise add half cycle for 1 cycle stretch */
void vicii_clock_write_stretch(void)
{
  if (vicii.fastmode!=0)
  {
    if (maincpu_rmw_flag==1)
    {
      maincpu_clk++;
    }
    else
    {
      if (vicii.half_cycles==1)
      {
        maincpu_clk++;
        vicii.half_cycles=0;
      }
    }
  }
}

int vicii_get_half_cycle(void)
{
  if (vicii.fastmode!=0)
    return vicii.half_cycles;
  return -1;
}
