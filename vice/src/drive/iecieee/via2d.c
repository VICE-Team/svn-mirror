/*
 * via2d.c - VIA2 emulation in the 1541, 1541II, 1571 and 2031 disk drive.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "drive.h"
#include "drivetypes.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "rotation.h"
#include "types.h"
#include "via.h"
#include "viad.h"


static void set_ca2(int state)
{
}

static void set_cb2(int state)
{
}

/* see interrupt.h; ugly, but more efficient... */
static void set_int(via_context_t *via_context, unsigned int int_num,
                    int value)
{
    drive_context_t *drive_context;

    drive_context = (drive_context_t *)(via_context->context);

    interrupt_set_irq(drive_context->cpu.int_status, int_num, value,
                      *(via_context->clk_ptr));
}

static void restore_int(via_context_t *via_context, unsigned int int_num,
                    int value)
{
    drive_context_t *drive_context;

    drive_context = (drive_context_t *)(via_context->context);

    interrupt_set_irq_noclk(drive_context->cpu.int_status, int_num, value);
}

void REGPARM3 via2d_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    viacore_store(&(ctxptr->via2), addr, data);
}

BYTE REGPARM2 via2d_read(drive_context_t *ctxptr, WORD addr)
{
    return viacore_read(&(ctxptr->via2), addr);
}

BYTE REGPARM2 via2d_peek(drive_context_t *ctxptr, WORD addr)
{
    return viacore_peek(&(ctxptr->via2), addr);
}

void viad2_update_pcr(int pcrval, drive_t *dptr)
{
    dptr->read_write_mode = pcrval & 0x20;
    dptr->byte_ready_active = (dptr->byte_ready_active & ~0x02)
                              | (pcrval & 0x02);
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE oldpa_value, WORD addr)
{
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    if (via2p->drive_ptr->byte_ready_active == 0x06)
        rotation_rotate_disk(via2p->drive_ptr);

    via2p->drive_ptr->GCR_write_value = byte;

    via2p->drive_ptr->byte_ready_level = 0;
}

static void undump_pra(via_context_t *via_context, BYTE byte)
{

}

inline static void store_prb(via_context_t *via_context, BYTE byte, BYTE poldpb,
                             WORD addr)
{
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    via2p->drive_ptr->led_status = (byte & 8) ? 1 : 0;
    if (((poldpb ^ byte) & 0x3) && (byte & 0x4)) {
        /* Stepper motor */
        if ((poldpb & 0x3) == ((byte + 1) & 0x3))
            drive_move_head(-1, via2p->number);
        else if ((poldpb & 0x3) == ((byte - 1) & 0x3))
            drive_move_head(+1, via2p->number);
    }
    if ((poldpb ^ byte) & 0x60)     /* Zone bits */
        rotation_speed_zone_set((byte >> 5) & 0x3, via2p->number);
    if ((poldpb ^ byte) & 0x04)     /* Motor on/off */
        via2p->drive_ptr->byte_ready_active
            = (via2p->drive_ptr->byte_ready_active & ~0x04)
                                     | (byte & 0x04);

    via2p->drive_ptr->byte_ready_level = 0;
}

static void undump_prb(via_context_t *via_context, BYTE byte)
{
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    via2p->drive_ptr->led_status = (byte & 8) ? 1 : 0;
    rotation_speed_zone_set((byte >> 5) & 0x3, via2p->number);
    via2p->drive_ptr->byte_ready_active
        = (via2p->drive_ptr->byte_ready_active & ~0x04) | (byte & 0x04);
}

inline static BYTE store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    /* FIXME: this should use via_set_ca2() and via_set_cb2() */
    if (byte != via_context->via[VIA_PCR]) {
        BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if ((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if ((tmp & 0xc0) != 0xc0)
            tmp |= 0x20;
        /* insert_your_favourite_drive_function_here(tmp);
        bit 5 is the write output to the analog circuitry:
        0 = writing, 0x20 = reading */
        viad2_update_pcr(tmp, via2p->drive_ptr);
        if ((byte & 0x20) != (via_context->via[addr] & 0x20)) {
            if (via2p->drive_ptr->byte_ready_active == 0x06)
                rotation_rotate_disk(via2p->drive_ptr);
            rotation_change_mode(via2p->number);
        }
    }
    return byte;
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    viad2_update_pcr(byte, via2p->drive_ptr);
}

static void undump_acr(via_context_t *via_context, BYTE byte)
{
}

inline void static store_acr(via_context_t *via_context, BYTE byte)
{
}

inline void static store_sr(via_context_t *via_context, BYTE byte)
{
}

inline void static store_t2l(via_context_t *via_context, BYTE byte)
{
}

static void reset(via_context_t *via_context)
{
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    via2p->drive_ptr->led_status = 1;
    drive_update_ui_status();
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    BYTE byte;
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    rotation_byte_read(via2p->drive_ptr);

    byte = ((via2p->drive_ptr->GCR_read & ~(via_context->via[VIA_DDRA]))
           | (via_context->via[VIA_PRA] & via_context->via[VIA_DDRA]));

    via2p->drive_ptr->byte_ready_level = 0;

    return byte;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;
    drivevia2_context_t *via2p;

    via2p = (drivevia2_context_t *)(via_context->prv);

    if (via2p->drive_ptr->byte_ready_active == 0x06)
        rotation_rotate_disk(via2p->drive_ptr);

    byte = ((rotation_sync_found(via2p->drive_ptr)
           | drive_write_protect_sense(via2p->drive_ptr))
           & ~(via_context->via[VIA_DDRB]))
           | (via_context->via[VIA_PRB] & via_context->via[VIA_DDRB]);


    via2p->drive_ptr->byte_ready_level = 0;

    return byte;
}

/* These callbacks and the data initializations have to be done here */
static void clk0_overflow_callback(CLOCK sub, void *data)
{
    viacore_clk_overflow_callback(&(drive0_context.via2), sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    viacore_clk_overflow_callback(&(drive1_context.via2), sub, data);
}

static void int_via2d0t1(CLOCK c)
{
    viacore_intt1(&(drive0_context.via2), c);
}

static void int_via2d0t2(CLOCK c)
{
    viacore_intt2(&(drive0_context.via2), c);
}

static void int_via2d1t1(CLOCK c)
{
    viacore_intt1(&(drive1_context.via2), c);
}

static void int_via2d1t2(CLOCK c)
{
    viacore_intt2(&(drive1_context.via2), c);
}

static const via_initdesc_t via2_initdesc[2] = {
    { &drive0_context.via2, clk0_overflow_callback,
      int_via2d0t1, int_via2d0t2 },
    { &drive1_context.via2, clk1_overflow_callback,
      int_via2d1t1, int_via2d1t2 }
};

void via2d_init(drive_context_t *ctxptr)
{
    via_drive_init(ctxptr, via2_initdesc);
}

void via2d_setup_context(drive_context_t *ctxptr)
{
    drivevia2_context_t *via2p;
    via_context_t *via;

    via = &(ctxptr->via2);

    via->prv = lib_malloc(sizeof(drivevia2_context_t));

    via2p = (drivevia2_context_t *)(via->prv);
    via2p->number = ctxptr->mynumber;
    via2p->drive_ptr = ctxptr->drive_ptr;

    via->context = (void *)ctxptr;

    via->rmw_flag = &(ctxptr->cpu.rmw_flag);
    via->clk_ptr = ctxptr->clk_ptr;

    sprintf(via->myname, "Drive%dVia2", via2p->number);
    sprintf(via->my_module_name, "VIA2D%d", via2p->number);
    via->read_clk = 0;
    via->read_offset = 0;
    via->last_read = 0;
    via->irq_line = IK_IRQ;
    via->int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status, via->myname);

    via->undump_pra = undump_pra;
    via->undump_prb = undump_prb;
    via->undump_pcr = undump_pcr;
    via->undump_acr = undump_acr;
    via->store_pra = store_pra;
    via->store_prb = store_prb;
    via->store_pcr = store_pcr;
    via->store_acr = store_acr;
    via->store_sr = store_sr;
    via->store_t2l = store_t2l;
    via->read_pra = read_pra;
    via->read_prb = read_prb;
    via->set_int = set_int;
    via->restore_int = restore_int;
    via->set_ca2 = set_ca2;
    via->set_cb2 = set_cb2;
    via->reset = reset;
}

