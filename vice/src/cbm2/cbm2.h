/*
 * cbm2.h
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef VICE_CBM2_H
#define VICE_CBM2_H

/* except for the exact CYCLES_PER_SEC those values are reasonable default
   values. they get overwritten when writing to the CRTC */

/* FIXME: handshake with a 2031 disk drive has a timing issue, where
   the CBM-II is slightly too fast. Reducing speed for about 1.3% from
   the real 2MHz seems to help here...

#define C610_PAL_CYCLES_PER_SEC         2000000
*/
#define C610_PAL_CYCLES_PER_SEC         1974000

#define C610_PAL_CYCLES_PER_LINE        128
#define C610_PAL_SCREEN_LINES           312
#define C610_PAL_CYCLES_PER_RFSH (C610_PAL_SCREEN_LINES \
                                 * C610_PAL_CYCLES_PER_LINE)
#define C610_PAL_RFSH_PER_SEC    (1.0 / ((double)C610_PAL_CYCLES_PER_RFSH    \
                                        / (double)C610_PAL_CYCLES_PER_SEC))

#define C610_NTSC_CYCLES_PER_SEC        C610_PAL_CYCLES_PER_SEC
/*
#define C610_NTSC_CYCLES_PER_LINE       C610_PAL_CYCLES_PER_LINE
#define C610_NTSC_SCREEN_LINES          C610_PAL_SCREEN_LINES
#define C610_NTSC_CYCLES_PER_RFSH       C610_PAL_CYCLES_PER_RFSH
#define C610_NTSC_RFSH_PER_SEC          C610_PAL_RFSH_PER_SEC
*/

#define C500_PAL_CYCLES_PER_SEC         985248
#define C500_PAL_CYCLES_PER_LINE        63
#define C500_PAL_SCREEN_LINES           312
#define C500_PAL_CYCLES_PER_RFSH (C500_PAL_SCREEN_LINES \
                                 * C500_PAL_CYCLES_PER_LINE)
#define C500_PAL_RFSH_PER_SEC    (1.0 / ((double)C500_PAL_CYCLES_PER_RFSH    \
                                        / (double)C500_PAL_CYCLES_PER_SEC))

struct snapshot_s;

extern int cbm2_is_c500(void);
extern int cbm2_isC500;
extern int cbm2_c500_snapshot_write_module(struct snapshot_s *p);
extern int cbm2_c500_snapshot_read_module(struct snapshot_s *p);

struct cia_context_s;
struct tpi_context_s;

typedef struct machine_context_s {
    struct cia_context_s *cia1;
    struct tpi_context_s *tpi1;
    struct tpi_context_s *tpi2;
} machine_context_t;

extern machine_context_t machine_context;

#endif

