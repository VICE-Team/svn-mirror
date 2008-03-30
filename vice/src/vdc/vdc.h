/*
 * vdc.h - A first attempt at a MOS8563 (VDC) emulation.
 *
 * Written by
 *  Markus Brenner <markus@brenner.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _VDC_H
#define _VDC_H

#include "vice.h"

struct snapshot_s;

extern int vdc_init_resources(void);
extern int vdc_init_cmdline_options (void);
extern void *vdc_init (void);
extern void vdc_reset (void);
extern void vdc_exposure_handler (unsigned int width, unsigned int height);
extern void vdc_prepare_for_snapshot (void);
extern void vdc_powerup (void);
extern void vdc_resize (void);

extern int vdc_write_snapshot_module (struct snapshot_s *s);
extern int vdc_read_snapshot_module (struct snapshot_s *s);

extern void vdc_set_canvas_refresh(int enable);

extern void video_free (void);
#if 0				/*  def USE_VIDMODE_EXTENSION */
extern void video_setfullscreen (int v, int width, int height);
#endif

#endif

