/*
 * tpi.h - Chip register definitions.
 *
 * Written by
 *   Andre' Fachat <a.fachat@physik.tu-chemnitz.de>
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

#ifndef _TPI_H
#define _TPI_H

#define TPI_PA          0
#define TPI_PB          1
#define TPI_PC          2
#define TPI_ILR         2
#define TPI_DDPA        3
#define TPI_DDPB        4
#define TPI_DDPC        5
#define TPI_IMR         5
#define TPI_CREG        6
#define TPI_AIR         7

typedef struct tpi_context_s {
    BYTE c_tpi[8];

    BYTE irq_previous;
    BYTE irq_stack;

    BYTE tpi_last_read;
    unsigned int tpi_int_num;

    BYTE oldpa;
    BYTE oldpb;
    BYTE oldpc;

    BYTE ca_state;
    BYTE cb_state;

    signed int log;

    char myname[12];

    int irq_line;                 /* IK_... */
    unsigned int int_num;

    CLOCK *clk_ptr;
    int *rmw_flag;

    void *prv;
    void *context;
} tpi_context_t;

#endif

