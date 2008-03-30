/*
 * c64tpi.c - IEEE488 interface for the C64.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
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

#include "vice.h"

#include <stdio.h>

#include "c64.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64tpi.h"
#include "drive.h"
#include "drivecpu.h"
#include "lib.h"
#include "log.h"
#include "parallel.h"
#include "maincpu.h"
#include "tpi.h"
#include "types.h"


#define mytpi_init tpi_init
#define mytpi_set_int tpi_set_int


void REGPARM2 tpi_store(WORD addr, BYTE data)
{
    tpicore_store(machine_context.tpi1, addr, data);
}

BYTE REGPARM1 tpi_read(WORD addr)
{
    io_source=IO_SOURCE_IEEE488;
    return tpicore_read(machine_context.tpi1, addr);
}

BYTE REGPARM1 tpi_peek(WORD addr)
{
    return tpicore_peek(machine_context.tpi1, addr);
}

static void set_int(unsigned int int_num, int value)
{
}

static void restore_int(unsigned int int_num, int value)
{
}

static void set_ca(tpi_context_t *tpi_context, int a)
{
}

static void set_cb(tpi_context_t *tpi_context, int a)
{
}

static int ieee_is_dev = 1;
static BYTE ieee_is_out = 1;

static void reset(tpi_context_t *tpi_context)
{
    /* assuming input after reset */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_ndac(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_dav(0);
    parallel_cpu_set_eoi(0);
    parallel_cpu_set_bus(0xff);

    ieee_is_dev = 1;
    ieee_is_out = 1;
}

static void store_pa(tpi_context_t *tpi_context, BYTE byte)
{
    if (byte != tpi_context->oldpa) {
        BYTE tmp = ~byte;
        ieee_is_dev = byte & 0x01;
        ieee_is_out = byte & 0x02;

        parallel_cpu_set_bus((BYTE)(ieee_is_out ? tpi_context->oldpb : 0xff));

        if (ieee_is_out) {
            parallel_cpu_set_ndac(0);
            parallel_cpu_set_nrfd(0);
            parallel_cpu_set_dav((BYTE)(tmp & 0x10));
            parallel_cpu_set_eoi((BYTE)(tmp & 0x20));
        } else {
            parallel_cpu_set_nrfd((BYTE)(tmp & 0x80));
            parallel_cpu_set_ndac((BYTE)(tmp & 0x40));
            parallel_cpu_set_dav(0);
            parallel_cpu_set_eoi(0);
        }
        if (ieee_is_dev) {
            parallel_cpu_set_atn(0);
        } else {
            parallel_cpu_set_atn((BYTE)(tmp & 0x08));
        }
    }
}

static void store_pb(tpi_context_t *tpi_context, BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(ieee_is_out ? byte : 0xff));
}

static void undump_pa(tpi_context_t *tpi_context, BYTE byte)
{
    BYTE tmp = ~byte;
    ieee_is_dev = byte & 0x01;
    ieee_is_out = byte & 0x02;

    parallel_cpu_set_bus((BYTE)(ieee_is_out ? tpi_context->oldpb : 0xff));

    if (ieee_is_out) {
        parallel_cpu_set_ndac(0);
        parallel_cpu_set_nrfd(0);
        parallel_cpu_set_dav((BYTE)(tmp & 0x10));
        parallel_cpu_set_eoi((BYTE)(tmp & 0x20));
    } else {
        parallel_cpu_set_nrfd((BYTE)(tmp & 0x80));
        parallel_cpu_set_ndac((BYTE)(tmp & 0x40));
        parallel_cpu_set_dav(0);
        parallel_cpu_set_eoi(0);
    }
    if (ieee_is_dev) {
        parallel_cpu_restore_atn(0);
    } else {
        parallel_cpu_restore_atn((BYTE)(tmp & 0x08));
    }
}

static void undump_pb(tpi_context_t *tpi_context, BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(ieee_is_out ? byte : 0xff));
}

static void store_pc(tpi_context_t *tpi_context, BYTE byte)
{
    /* 1 = active */
    mem_set_exrom((byte & 8) ? 0 : 1);
}

static void undump_pc(tpi_context_t *tpi_context, BYTE byte)
{
}

static BYTE read_pa(tpi_context_t *tpi_context)
{
    BYTE byte;

    drivecpu_execute_all(maincpu_clk);

    byte = 0xff;
    if (ieee_is_out) {
        if (parallel_nrfd)
            byte &= 0x7f;
        if (parallel_ndac)
            byte &= 0xbf;
    } else {
        if (parallel_dav)
            byte &= 0xef;
        if (parallel_eoi)
            byte &= 0xdf;
    }
    if (ieee_is_dev) {
        if (parallel_atn)
            byte &= 0xf7;
    }

    byte = (byte & ~(tpi_context->c_tpi)[TPI_DDPA])
           | (tpi_context->c_tpi[TPI_PA] & tpi_context->c_tpi[TPI_DDPA]);

    return byte;
}

static BYTE read_pb(tpi_context_t *tpi_context)
{
    BYTE byte;

    drivecpu_execute_all(maincpu_clk);

    byte = ieee_is_out ? 0xff : parallel_bus;
    byte = (byte & ~(tpi_context->c_tpi)[TPI_DDPB])
           | (tpi_context->c_tpi[TPI_PB] & tpi_context->c_tpi[TPI_DDPB]);

    return byte;
}

static BYTE read_pc(tpi_context_t *tpi_context)
{
    BYTE byte;
    byte = (0xff & ~(tpi_context->c_tpi)[TPI_DDPC])
           | (tpi_context->c_tpi[TPI_PC] & tpi_context->c_tpi[TPI_DDPC]);
    return byte;
}

void tpi_init(tpi_context_t *tpi_context)
{
    tpi_context->log = log_open(tpi_context->myname);
}

void tpi_setup_context(machine_context_t *machine_context)
{
    tpi_context_t *tpi_context;

    machine_context->tpi1 = lib_malloc(sizeof(tpi_context_t));
    tpi_context = machine_context->tpi1;

    tpi_context->prv = NULL;

    tpi_context->context = (void *)machine_context;

    tpi_context->rmw_flag = &maincpu_rmw_flag;
    tpi_context->clk_ptr = &maincpu_clk;

    tpi_context->myname = lib_msprintf("TPI");

    tpicore_setup_context(tpi_context);

    tpi_context->store_pa = store_pa;
    tpi_context->store_pb = store_pb;
    tpi_context->store_pc = store_pc;
    tpi_context->read_pa = read_pa;
    tpi_context->read_pb = read_pb;
    tpi_context->read_pc = read_pc;
    tpi_context->undump_pa = undump_pa;
    tpi_context->undump_pb = undump_pb;
    tpi_context->undump_pc = undump_pc;
    tpi_context->reset = reset;
    tpi_context->set_ca = set_ca;
    tpi_context->set_cb = set_cb;
    tpi_context->set_int = set_int;
    tpi_context->restore_int = restore_int;
}

