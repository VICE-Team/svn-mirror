/*
 * digimax.c - Digimax DAC cartridge emulation.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c64io.h"
#include "cmdline.h"
#include "digimax.h"
#include "lib.h"
#include "maincpu.h"
#include "resources.h"
#include "sid.h"
#include "sound.h"
#include "ui.h"

#ifdef HAS_TRANSLATION
#include "translate.h"
#endif

/* Flag: Do we enable the external DIGIMAX cartridge?  */
int digimax_enabled;

/* DIGIMAX address */
int digimax_address;

/* DIGIMAX userport address latch */
static BYTE digimax_userport_address;

/* DIGIMAX userport direction latches */
static BYTE digimax_userport_direction_A;
static BYTE digimax_userport_direction_B;

static int set_digimax_enabled(int val, void *param)
{
  if (sid_sound_machine_cycle_based()==1 && val)
  {
    ui_error("Digimax cannot be used with ReSID\nPlease switch SID Engine to FastSID");
    return -1;
  }
  digimax_enabled=val;
  return 0;
}

static int set_digimax_base(int val, void *param)
{
  if (val==digimax_address)
    return 0;

  switch (val)
  {
    case 0xdd00:   /* special case, userport interface */
    case 0xde00:
    case 0xde20:
    case 0xde40:
    case 0xde60:
    case 0xde80:
    case 0xdea0:
    case 0xdec0:
    case 0xdee0:
    case 0xdf00:
    case 0xdf20:
    case 0xdf40:
    case 0xdf60:
    case 0xdf80:
    case 0xdfa0:
    case 0xdfc0:
    case 0xdfe0:
      break;
    default:
      return -1;
  }

  digimax_address=val;

  return 0;
}

static const resource_int_t resources_int[] = {
  { "DIGIMAX", 0, RES_EVENT_STRICT, (resource_value_t)0,
    &digimax_enabled, set_digimax_enabled, NULL },
  { "DIGIMAXbase", 0xde00, RES_EVENT_NO, NULL,
    &digimax_address, set_digimax_base, NULL },
  { NULL }
};

int digimax_resources_init(void)
{
  return resources_register_int(resources_int);
}

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-digimax", SET_RESOURCE, 0, NULL, NULL, "DIGIMAX", (resource_value_t)1,
      0, IDCLS_ENABLE_DIGIMAX },
    { "+digimax", SET_RESOURCE, 0, NULL, NULL, "DIGIMAX", (resource_value_t)0,
      0, IDCLS_DISABLE_DIGIMAX },
    { "-digimaxbase", SET_RESOURCE, 1, NULL, NULL, "DIGIMAXbase", NULL,
      IDCLS_P_BASE_ADDRESS, IDCLS_DIGIMAX_BASE },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-digimax", SET_RESOURCE, 0, NULL, NULL, "DIGIMAX", (resource_value_t)1,
      NULL, N_("Enable the digimax cartridge") },
    { "+digimax", SET_RESOURCE, 0, NULL, NULL, "DIGIMAX", (resource_value_t)0,
      NULL, N_("Disable the digimax cartridge") },
    { "-digimaxbase", SET_RESOURCE, 1, NULL, NULL, "DIGIMAXbase", NULL,
      N_("<base address>"), N_("Base address of the digimax cartridge") },
    { NULL }
};
#endif

int digimax_cmdline_options_init(void)
{
  return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

static BYTE digimax_sound_data[4];

struct digimax_sound_s
{
  BYTE voice0;
  BYTE voice1;
  BYTE voice2;
  BYTE voice3;
};

static struct digimax_sound_s snd;

int digimax_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
                                                   int interleave, int *delta_t)
{
  int i;

  if (sid_sound_machine_cycle_based()==0 && digimax_enabled)
  {
    for (i=0; i<nr; i++)
    {
      pbuf[i*interleave]+=(snd.voice0+snd.voice1+snd.voice2+snd.voice3)<<6;
    }
  }
  return 0;
}

int digimax_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
  snd.voice0=0;
  snd.voice1=0;
  snd.voice2=0;
  snd.voice3=0;

  return 1;
}

void digimax_sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
  switch (addr&3)
  {
    case 0:
      snd.voice0=val;
      break;
    case 1:
      snd.voice1=val;
      break;
    case 2:
      snd.voice2=val;
      break;
    case 3:
      snd.voice3=val;
      break;
  }
}

BYTE digimax_sound_machine_read(sound_t *psid, WORD addr)
{
  return digimax_sound_data[addr&3];
}

void digimax_sound_reset(void)
{
  snd.voice0=0;
  snd.voice1=0;
  snd.voice2=0;
  snd.voice3=0;
  digimax_sound_data[0]=0;
  digimax_sound_data[1]=0;
  digimax_sound_data[2]=0;
  digimax_sound_data[3]=0;
}

/* ---------------------------------------------------------------------*/

void REGPARM2 digimax_sound_store(WORD addr, BYTE value)
{
  digimax_sound_data[addr]=value;
  sound_store((WORD)(addr+0x20), value, 0);
}

BYTE REGPARM1 digimax_sound_read(WORD addr)
{
  BYTE value;

  io_source=IO_SOURCE_DIGIMAX;
  value=sound_read((WORD)(addr+0x20), 0);

  return value;
}

static void digimax_userport_sound_store(BYTE value)
{
  WORD addr=0;

  switch ((digimax_address & digimax_userport_direction_A)&0xc)
  {
    case 0:
      addr=2;
      break;
    case 4:
      addr=3;
      break;
    case 8:
      addr=0;
      break;
    case 12:
      addr=1;
      break;
  }

  digimax_sound_store(addr,(BYTE)(value & digimax_userport_direction_B));
}

void digimax_userport_store(WORD addr, BYTE value)
{
  switch (addr&0x1f)
  {
    case 0:
      digimax_userport_address=value;
      break;
    case 1:
      if (digimax_enabled && digimax_address==0xdd00)
      {
        digimax_userport_sound_store(value);
      }
      break;
    case 2:
      digimax_userport_direction_A=value;
      break;
    case 3:
      digimax_userport_direction_B=value;
      break;
  }
}
