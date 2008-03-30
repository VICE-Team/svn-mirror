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


static void via_set_ca2(int state)
{
}

static void via_set_cb2(int state)
{
}

/* see interrupt.h; ugly, but more efficient... */
static void via_set_int(via_context_t *via_context, unsigned int int_num,
                        int value)
{
    drive_context_t *drive_context;

    drive_context = (drive_context_t *)(via_context->context);

    interrupt_set_irq(drive_context->cpu.int_status, int_num, value,
                      *(via_context->clk_ptr));
}

#define myclk           (*(via_context->clk_ptr))
#define myvia           (via_context->via)
#define myviaifr        (via_context->ifr)
#define myviaier        (via_context->ier)
#define myviatal        (via_context->tal)
#define myviatbl        (via_context->tbl)
#define myviatau        (via_context->tau)
#define myviatbu        (via_context->tbu)
#define myviatai        (via_context->tai)
#define myviatbi        (via_context->tbi)
#define myviapb7        (via_context->pb7)
#define myviapb7x       (via_context->pb7x)
#define myviapb7o       (via_context->pb7o)
#define myviapb7xx      (via_context->pb7xx)
#define myviapb7sx      (via_context->pb7sx)
#define oldpa           (via_context->oldpa)
#define oldpb           (via_context->oldpb)
#define myvia_ila       (via_context->ila)
#define myvia_ilb       (via_context->ilb)
#define ca2_state       (via_context->ca2_state)
#define cb2_state       (via_context->cb2_state)
#define myvia_t1_alarm  (via_context->t1_alarm)
#define myvia_t2_alarm  (via_context->t2_alarm)

#define via_read_clk    (via_context->read_clk)
#define via_read_offset (via_context->read_offset)
#define via_last_read   (via_context->last_read)
#define snap_module_name (via_context->my_module_name)

#define myvia_init      via2d_init
#define myvia_int_num   (via_context->int_num)
#define MYVIA_NAME      (via_context->myname)
#define MYVIA_INT       (via_context->irq_line)

#define mycpu_rmw_flag  (*(via_context->rmw_flag))

#define myvia_reset     via2d_reset

#define myvia_store     via2dx_store
#define myvia_read      via2dx_read
#define myvia_peek      via2dx_peek

void REGPARM3 myvia_store(via_context_t *via_context, WORD addr, BYTE data);
BYTE REGPARM2 myvia_read(via_context_t *via_context, WORD addr);
BYTE REGPARM2 myvia_peek(via_context_t *via_context, WORD addr);

void REGPARM3 via2d_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    myvia_store(&(ctxptr->via2), addr, data);
}

BYTE REGPARM2 via2d_read(drive_context_t *ctxptr, WORD addr)
{
    return myvia_read(&(ctxptr->via2), addr);
}

BYTE REGPARM2 via2d_peek(drive_context_t *ctxptr, WORD addr)
{
    return myvia_peek(&(ctxptr->via2), addr);
}

#define myvia_log       (via_context->log)
#define myvia_signal    via2d_signal
#define myvia_prevent_clk_overflow via2_prevent_clk_overflow
#define myvia_snapshot_read_module via2d_snapshot_read_module
#define myvia_snapshot_write_module via2d_snapshot_write_module



void via2d_setup_context(drive_context_t *ctxptr)
{
    drivevia2_context_t *via2p;

    ctxptr->via2.prv = lib_malloc(sizeof(drivevia2_context_t));

    via2p = (drivevia2_context_t *)(ctxptr->via2.prv);
    via2p->number = ctxptr->mynumber;
    via2p->drive_ptr = ctxptr->drive_ptr;

    ctxptr->via2.context = (void *)ctxptr;

    ctxptr->via2.rmw_flag = &(ctxptr->cpu.rmw_flag);
    ctxptr->via2.clk_ptr = ctxptr->clk_ptr;

    sprintf(ctxptr->via2.myname, "Drive%dVia2", via2p->number);
    sprintf(ctxptr->via2.my_module_name, "VIA2D%d", via2p->number);
    ctxptr->via2.read_clk = 0;
    ctxptr->via2.read_offset = 0;
    ctxptr->via2.last_read = 0;
    ctxptr->via2.irq_line = IK_IRQ;
    ctxptr->via2.int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status,
                                       ctxptr->via2.myname);
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
    if (byte != myvia[VIA_PCR]) {
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
        if ((byte & 0x20) != (myvia[addr] & 0x20)) {
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

static void res_via(via_context_t *via_context)
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

    byte = ((via2p->drive_ptr->GCR_read & ~myvia[VIA_DDRA])
           | (myvia[VIA_PRA] & myvia[VIA_DDRA]));

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
           | drive_write_protect_sense(via2p->drive_ptr)) & ~myvia[VIA_DDRB])
           | (myvia[VIA_PRB] & myvia[VIA_DDRB]);


    via2p->drive_ptr->byte_ready_level = 0;

    return byte;
}

/* These callbacks and the data initializations have to be done here */

static void clk_overflow_callback(via_context_t *, CLOCK, void *);
static void int_myviat1(via_context_t *, CLOCK);
static void int_myviat2(via_context_t *, CLOCK);

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&(drive0_context.via2), sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&(drive1_context.via2), sub, data);
}

static void int_via2d0t1(CLOCK c)
{
    int_myviat1(&(drive0_context.via2), c);
}

static void int_via2d0t2(CLOCK c)
{
    int_myviat2(&(drive0_context.via2), c);
}

static void int_via2d1t1(CLOCK c)
{
    int_myviat1(&(drive1_context.via2), c);
}

static void int_via2d1t2(CLOCK c)
{
    int_myviat2(&(drive1_context.via2), c);
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


#define VIA_SHARED_CODE

#include "viacore.c"

