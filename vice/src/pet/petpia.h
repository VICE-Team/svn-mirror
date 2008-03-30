/*
 * pia.h -- PIA chip emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _PIA_H
#define _PIA_H

/* Signal values (for signaling edges on the control lines) */

#define PIA_SIG_CA1     0
#define PIA_SIG_CA2     1
#define PIA_SIG_CB1     2
#define PIA_SIG_CB2     3

#define PIA_SIG_FALL    0
#define PIA_SIG_RISE    1

/* ------------------------------------------------------------------------- */

extern int pia1_init_resources(void);
extern int pia1_init_cmdline_options(void);

extern void reset_pia1(void);
extern void signal_pia1(int line, int edge);
extern void REGPARM2 store_pia1(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_pia1(ADDRESS addr);
extern BYTE REGPARM1 peek_pia1(ADDRESS addr);

extern int pia1_read_snapshot_module(snapshot_t *);
extern int pia1_write_snapshot_module(snapshot_t *);

extern void reset_pia2(void);
extern void signal_pia2(int line, int edge);
extern void REGPARM2 store_pia2(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_pia2(ADDRESS addr);
extern BYTE REGPARM1 peek_pia2(ADDRESS addr);

extern int pia2_read_snapshot_module(snapshot_t *);
extern int pia2_write_snapshot_module(snapshot_t *);

#endif
