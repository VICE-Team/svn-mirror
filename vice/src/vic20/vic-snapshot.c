/*
 * vic-snapshot.c - Snapshot support for the VIC-I emulation.
 *
 * Written by
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

#include <stdlib.h>

#include "maincpu.h"
#include "sound.h"

#include "vic.h"
#include "vic-mem.h"

#include "vic-snapshot.h"



static char snap_module_name[] = "VIC-I";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0



int 
vic_snapshot_write_module (snapshot_t *s)
{
  int i;
  snapshot_module_t *m;

  m = snapshot_module_create (s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
  if (m == NULL)
    return -1;

  if (snapshot_module_write_byte (m, (BYTE) VIC_RASTER_CYCLE (clk)) < 0
      || snapshot_module_write_word (m, (WORD) VIC_RASTER_Y (clk)) < 0)
    goto fail;

  if (snapshot_module_write_word (m, (WORD) vic.memptr) < 0)
    goto fail;

  /* Color RAM.  */
  if (snapshot_module_write_byte_array (m, ram + 0x9400, 0x800) < 0)
    goto fail;

  for (i = 0; i < 0x10; i++)
    if (snapshot_module_write_byte (m, (BYTE) vic.regs[i]) < 0)
      goto fail;

  return snapshot_module_close (m);

fail:
  if (m != NULL)
    snapshot_module_close (m);
  return -1;
}



int 
vic_snapshot_read_module (snapshot_t *s)
{
  int i;
  snapshot_module_t *m;
  BYTE major_version, minor_version;
  WORD w;
  BYTE b;

  sound_close ();

  m = snapshot_module_open (s, snap_module_name,
			    &major_version, &minor_version);
  if (m == NULL)
    return -1;

  if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR)
    {
      log_error (vic.log, "Snapshot module version (%d.%d) newer than %d.%d.",
		 major_version, minor_version,
		 SNAP_MAJOR, SNAP_MINOR);
      goto fail;
    }

  if (snapshot_module_read_byte (m, &b) < 0)
    goto fail;
  if (b != VIC_RASTER_CYCLE (clk))
    {
      log_error (vic.log, "Cycle value (%d) incorrect; should be %d.",
		 (int) b, VIC_RASTER_CYCLE (clk));
      goto fail;
    }

  if (snapshot_module_read_word (m, &w) < 0)
    goto fail;

  if (w != VIC_RASTER_Y (clk))
    {
      log_error (vic.log, "Raster line value (%d) incorrect; should be %d.",
		 (int) w, VIC_RASTER_Y (clk));
      goto fail;
    }

  if (snapshot_module_read_word (m, &w) < 0)
    goto fail;

  vic.memptr = w;

  /* Color RAM.  */
  if (snapshot_module_read_byte_array (m, ram + 0x9400, 0x800) < 0)
    goto fail;

  for (i = 0; i < 0x10; i++)
    {
      if (snapshot_module_read_byte (m, &b) < 0)
	goto fail;

      /* XXX: This assumes that there are no side effects.  */
      store_vic (i, b);
    }

  alarm_set (&vic.raster_draw_alarm,
             clk + VIC_CYCLES_PER_LINE - VIC_RASTER_CYCLE (clk));

  raster_force_repaint (&vic.raster);
  return snapshot_module_close (m);

fail:
  if (m != NULL)
    snapshot_module_close (m);
  return -1;
}
