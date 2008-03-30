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

#include "drive.h"
#include "drivetypes.h"
#include "types.h"
#include "via.h"
#include "viad.h"

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)


/* see interrupt.h; ugly, but more efficient... */
#define via_set_int(a,b) interrupt_set_irq(&(ctxptr->cpu.int_status), a, b, \
                         *(ctxptr->clk_ptr))

#define myclk           (*(ctxptr->clk_ptr))
#define myvia           (ctxptr->via2.via)
#define myviaifr        (ctxptr->via2.ifr)
#define myviaier        (ctxptr->via2.ier)
#define myviatal        (ctxptr->via2.tal)
#define myviatbl        (ctxptr->via2.tbl)
#define myviatau        (ctxptr->via2.tau)
#define myviatbu        (ctxptr->via2.tbu)
#define myviatai        (ctxptr->via2.tai)
#define myviatbi        (ctxptr->via2.tbi)
#define myviapb7        (ctxptr->via2.pb7)
#define myviapb7x       (ctxptr->via2.pb7x)
#define myviapb7o       (ctxptr->via2.pb7o)
#define myviapb7xx      (ctxptr->via2.pb7xx)
#define myviapb7sx      (ctxptr->via2.pb7sx)
#define oldpa           (ctxptr->via2.oldpa)
#define oldpb           (ctxptr->via2.oldpb)
#define myvia_ila       (ctxptr->via2.ila)
#define myvia_ilb       (ctxptr->via2.ilb)
#define ca2_state       (ctxptr->via2.ca2_state)
#define cb2_state       (ctxptr->via2.cb2_state)
#define myvia_t1_alarm  (ctxptr->via2.t1_alarm)
#define myvia_t2_alarm  (ctxptr->via2.t2_alarm)

#define via_read_clk    (ctxptr->via2.read_clk)
#define via_read_offset (ctxptr->via2.read_offset)
#define via_last_read   (ctxptr->via2.last_read)
#define snap_module_name (ctxptr->via2.my_module_name)

#define myvia_init      via2d_init
#define I_MYVIAFL       (ctxptr->via2.irq_type)
#define MYVIA_NAME      (ctxptr->via2.myname)
/*#define MYVIA_INT     (ctxptr->via2.irq_line)*/
#define MYVIA_INT       IK_IRQ

#define mycpu_rmw_flag  (ctxptr->cpu.rmw_flag)
#define mycpu_int_status (ctxptr->cpu.int_status)
#define mycpu_alarm_context (ctxptr->cpu.alarm_context)
#define mycpu_clk_guard (ctxptr->cpu.clk_guard)

#define myvia_reset     via2d_reset
#define myvia_store     via2d_store
#define myvia_read      via2d_read
#define myvia_peek      via2d_peek

#define myvia_log       (ctxptr->via2.log)
#define myvia_signal    via2d_signal
#define myvia_prevent_clk_overflow via2_prevent_clk_overflow
#define myvia_snapshot_read_module via2d_snapshot_read_module
#define myvia_snapshot_write_module via2d_snapshot_write_module



void drive_via2_setup_context(drive_context_t *ctxptr)
{
    sprintf(ctxptr->via2.myname, "Drive%dVia2", ctxptr->mynumber);
    sprintf(ctxptr->via2.my_module_name, "VIA2D%d", ctxptr->mynumber);
    ctxptr->via2.read_clk = 0;
    ctxptr->via2.read_offset = 0;
    ctxptr->via2.last_read = 0;
    ctxptr->via2.irq_line = IK_IRQ;
    ctxptr->via2.irq_type = (ctxptr->mynumber == 0) ? I_VIA2D0FL : I_VIA2D1FL;
}


inline static void store_pra(drive_context_t *ctxptr, BYTE byte,
                             BYTE oldpa_value, ADDRESS addr)
{
    if (ctxptr->drive_ptr->byte_ready_active == 0x06)
        drive_rotate_disk(ctxptr->drive_ptr);
    ctxptr->drive_ptr->GCR_write_value = byte;
}

static void undump_pra(drive_context_t *ctxptr, BYTE byte)
{

}

inline static void store_prb(drive_context_t *ctxptr, BYTE byte, BYTE poldpb,
                             ADDRESS addr)
{
    ctxptr->drive_ptr->led_status = (byte & 8) ? 1 : 0;
    if (((poldpb ^ byte) & 0x3) && (byte & 0x4)) {
        /* Stepper motor */
        if ((poldpb & 0x3) == ((byte + 1) & 0x3))
            drive_move_head(-1, ctxptr->mynumber);
        else if ((poldpb & 0x3) == ((byte - 1) & 0x3))
            drive_move_head(+1, ctxptr->mynumber);
    }
    if ((poldpb ^ byte) & 0x60)     /* Zone bits */
        ctxptr->drive_ptr->rotation_table_ptr
            = ctxptr->drive_ptr->rotation_table[(byte >> 5) & 0x3];
    if ((poldpb ^ byte) & 0x04)     /* Motor on/off */
        ctxptr->drive_ptr->byte_ready_active
            = (ctxptr->drive_ptr->byte_ready_active & ~0x04)
                                     | (byte & 0x04);
}

static void undump_prb(drive_context_t *ctxptr, BYTE byte)
{
    ctxptr->drive_ptr->led_status = (byte & 8) ? 1 : 0;
    ctxptr->drive_ptr->rotation_table_ptr
        = ctxptr->drive_ptr->rotation_table[(byte >> 5) & 0x3];
    ctxptr->drive_ptr->byte_ready_active
        = (ctxptr->drive_ptr->byte_ready_active & ~0x04) | (byte & 0x04);
}

inline static BYTE store_pcr(drive_context_t *ctxptr, BYTE byte, ADDRESS addr)
{
    /* FIXME: this should use VIA_SET_CA2() and VIA_SET_CB2() */
    if(byte != myvia[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if((tmp & 0xc0) != 0xc0)
            tmp |= 0x20;
        /* insert_your_favourite_drive_function_here(tmp);
        bit 5 is the write output to the analog circuitry:
        0 = writing, 0x20 = reading */
        drive_update_viad2_pcr(tmp, ctxptr->drive_ptr);
        if ((byte & 0x20) != (myvia[addr] & 0x20)) {
            if (ctxptr->drive_ptr->byte_ready_active == 0x06)
                drive_rotate_disk(ctxptr->drive_ptr);
            ctxptr->drive_ptr->finish_byte = 1;
        }
    }
    return byte;
}

static void undump_pcr(drive_context_t *ctxptr, BYTE byte)
{
    drive_update_viad2_pcr(byte, ctxptr->drive_ptr);
}

static void undump_acr(drive_context_t *ctxptr, BYTE byte)
{
}

inline void static store_acr(drive_context_t *ctxptr, BYTE byte)
{
}

inline void static store_sr(drive_context_t *ctxptr, BYTE byte)
{
}

inline void static store_t2l(drive_context_t *ctxptr, BYTE byte)
{
}

static void res_via(drive_context_t *ctxptr)
{
    ctxptr->drive_ptr->led_status = 1;
    drive_update_ui_status();
}

inline static BYTE read_pra(drive_context_t *ctxptr, ADDRESS addr)
{
    BYTE byte;

    if (ctxptr->drive_ptr->attach_clk != (CLOCK)0) {
        if (*(ctxptr->clk_ptr) - ctxptr->drive_ptr->attach_clk
            < DRIVE_ATTACH_DELAY)
            ctxptr->drive_ptr->GCR_read = 0;
        else
            ctxptr->drive_ptr->attach_clk = (CLOCK)0;
    } else if (ctxptr->drive_ptr->attach_detach_clk != (CLOCK)0) {
        if (*(ctxptr->clk_ptr) - ctxptr->drive_ptr->attach_detach_clk
            < DRIVE_ATTACH_DETACH_DELAY)
            ctxptr->drive_ptr->GCR_read = 0;
        else
            ctxptr->drive_ptr->attach_detach_clk = (CLOCK)0;
    } else {
        if (ctxptr->drive_ptr->byte_ready_active == 0x06)
            drive_rotate_disk(ctxptr->drive_ptr);
    }

    byte = ((ctxptr->drive_ptr->GCR_read & ~myvia[VIA_DDRA])
           | (myvia[VIA_PRA] & myvia[VIA_DDRA] ));

    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1571)
        if (ctxptr->drive_ptr->byte_ready)
            ctxptr->drive_ptr->byte_ready = 0;

    return byte;
}

inline static BYTE read_prb(drive_context_t *ctxptr)
{
    BYTE byte;

    byte = (drive_read_viad2_prb(ctxptr->drive_ptr) & ~myvia[VIA_DDRB])
           | (myvia[VIA_PRB] & myvia[VIA_DDRB] );

    return byte;
}

/* These callbacks and the data initializations have to be done here */

static void clk_overflow_callback(drive_context_t *, CLOCK, void *);
static void int_myviat1(drive_context_t *, CLOCK);
static void int_myviat2(drive_context_t *, CLOCK);

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&drive0_context, sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&drive1_context, sub, data);
}

static void int_via2d0t1(CLOCK c)
{
    int_myviat1(&drive0_context, c);
}

static void int_via2d0t2(CLOCK c)
{
    int_myviat2(&drive0_context, c);
}

static void int_via2d1t1(CLOCK c)
{
    int_myviat1(&drive1_context, c);
}

static void int_via2d1t2(CLOCK c)
{
    int_myviat2(&drive1_context, c);
}

static via_initdesc_t via2_initdesc[2] = {
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
#define VIACONTEXT drive_context_t

#include "viacore.c"

