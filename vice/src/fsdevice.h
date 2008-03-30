/*
 * fsdevice.h - file system device.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Jarkko Sonninen     (sonninen@lut.fi)
 *  Jouko Valta         (jopi@stekt.oulu.fi)
 *  Olaf Seibert        (rhialto@mbfys.kun.nl)
 *  Andre Fachat        (a.fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
 *
 * Patches by
 *  Dan Miner           (dminer@nyx10.cs.du.edu)
 *  Germano Caronni     (caronni@tik.ethz.ch)
 *  Daniel Fandrich     (dan@fch.wimsey.bc.ca)  /DF/
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

#ifndef _FSDEVICE_H
#define _FSDEVICE_H

extern int fsdevice_init_resources(void);
extern int fsdevice_init_cmdline_options(void);

extern void fs_error(int code);
extern void flush_fs(void *flp, int secondary);
extern int write_fs(void *flp, BYTE data, int secondary);
extern int read_fs(void *flp, BYTE *data, int secondary);
extern int open_fs(void *flp, char *name, int length, int secondary);
extern int close_fs(void *flp, int secondary);

extern int attach_fsdevice(int device, char *var, char *name);
extern void fsdevice_set_directory(char *filename, int unit);

#endif	/* _FSDEVICE_H */

