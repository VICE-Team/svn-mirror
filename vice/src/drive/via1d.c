/*
 * via1d.c - VIA1 emulation in the 1541, 1541II, 1571 and 2031 disk drive.
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

#include "clkguard.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivesync.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "log.h"
#include "parallel.h"
#include "rotation.h"
#include "types.h"
#include "utils.h"
#include "via.h"
#include "viad.h"


#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)


/* see interrupt.h; ugly, but more efficient... */
#define via_set_int(a,b) \
        interrupt_set_irq(ctxptr->cpu.int_status, a, b, *(ctxptr->clk_ptr))

#define myclk           (*(ctxptr->clk_ptr))
#define myvia           (ctxptr->via1.via)
#define myviaifr        (ctxptr->via1.ifr)
#define myviaier        (ctxptr->via1.ier)
#define myviatal        (ctxptr->via1.tal)
#define myviatbl        (ctxptr->via1.tbl)
#define myviatau        (ctxptr->via1.tau)
#define myviatbu        (ctxptr->via1.tbu)
#define myviatai        (ctxptr->via1.tai)
#define myviatbi        (ctxptr->via1.tbi)
#define myviapb7        (ctxptr->via1.pb7)
#define myviapb7x       (ctxptr->via1.pb7x)
#define myviapb7o       (ctxptr->via1.pb7o)
#define myviapb7xx      (ctxptr->via1.pb7xx)
#define myviapb7sx      (ctxptr->via1.pb7sx)
#define oldpa           (ctxptr->via1.oldpa)
#define oldpb           (ctxptr->via1.oldpb)
#define myvia_ila       (ctxptr->via1.ila)
#define myvia_ilb       (ctxptr->via1.ilb)
#define ca2_state       (ctxptr->via1.ca2_state)
#define cb2_state       (ctxptr->via1.cb2_state)
#define myvia_t1_alarm  (ctxptr->via1.t1_alarm)
#define myvia_t2_alarm  (ctxptr->via1.t2_alarm)

#define via_read_clk    (ctxptr->via1.read_clk)
#define via_read_offset (ctxptr->via1.read_offset)
#define via_last_read   (ctxptr->via1.last_read)
#define snap_module_name (ctxptr->via1.my_module_name)
#define parieee_is_out  (ctxptr->via1p.v_parieee_is_out)
#define iec_info        (ctxptr->via1p.v_iec_info)

#define myvia_init      via1d_init
#define I_MYVIAFL       (ctxptr->via1.irq_type)
#define MYVIA_NAME      (ctxptr->via1.myname)
/*#define MYVIA_INT     (ctxptr->via1.irq_line)*/
#define MYVIA_INT       IK_IRQ

#define mycpu_rmw_flag  (ctxptr->cpu.rmw_flag)
#define mycpu_int_status (ctxptr->cpu.int_status)
#define mycpu_alarm_context (ctxptr->cpu.alarm_context)
#define mycpu_clk_guard (ctxptr->cpu.clk_guard)

#define myvia_reset     via1d_reset
#define myvia_store     via1d_store
#define myvia_read      via1d_read
#define myvia_peek      via1d_peek

#define myvia_log       (ctxptr->via1.log)
#define myvia_signal    via1d_signal
#define myvia_prevent_clk_overflow via1_prevent_clk_overflow
#define myvia_snapshot_read_module via1d_snapshot_read_module
#define myvia_snapshot_write_module via1d_snapshot_write_module


#define iec_drivex_write(a)             (ctxptr->func.iec_write(a))
#define iec_drivex_read()               (ctxptr->func.iec_read())
#define parallel_cable_drivex_write(a,b) (ctxptr->func.parallel_cable_write(a,b))
#define parallel_drivex_set_bus(a)      (ctxptr->func.parallel_set_bus(a))
#define parallel_drivex_set_eoi(a)      (ctxptr->func.parallel_set_eoi(a))
#define parallel_drivex_set_dav(a)      (ctxptr->func.parallel_set_dav(a))
#define parallel_drivex_set_ndac(a)     (ctxptr->func.parallel_set_ndac(a))
#define parallel_drivex_set_nrfd(a)     (ctxptr->func.parallel_set_nrfd(a))


void drive_via1_setup_context(drive_context_t *ctxptr)
{
    sprintf(ctxptr->via1.myname, "Drive%dVia1", ctxptr->mynumber);
    sprintf(ctxptr->via1.my_module_name, "VIA1D%d", ctxptr->mynumber);
    ctxptr->via1.read_clk = 0;
    ctxptr->via1.read_offset = 0;
    ctxptr->via1.last_read = 0;
    ctxptr->via1.irq_line = IK_IRQ;
    ctxptr->via1.log = LOG_ERR;
    ctxptr->via1p.v_parieee_is_out = 1;
    if (ctxptr->mynumber == 0) {
      ctxptr->via1.irq_type = I_VIA1D0FL;
      ctxptr->via1p.parallel_id = PARALLEL_DRV0;
    } else {
      ctxptr->via1.irq_type = I_VIA1D1FL;
      ctxptr->via1p.parallel_id = PARALLEL_DRV1;
   }
}

void drive_via_set_atn(drive_context_t *ctxptr, int state)
{
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
        via1d_signal(ctxptr, VIA_SIG_CA1, state ? VIA_SIG_RISE : 0);
        parallel_drivex_set_nrfd((BYTE)(((!parieee_is_out) && (!(oldpb & 0x02)))
                               || (parallel_atn && (!(oldpb & 0x01)))
                               || ((!parallel_atn) && (oldpb & 0x01))));
        parallel_drivex_set_ndac((BYTE)(((!parieee_is_out) && (!(oldpb & 0x04)))
                               || (parallel_atn && (!(oldpb & 0x01)))
                               || ((!parallel_atn) && (oldpb & 0x01))));
    }
}

static void undump_pra(drive_context_t *ctxptr, BYTE byte)
{
    iec_info = iec_get_drive_port();
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1571) {
        drive_sync_set_1571(byte & 0x20, ctxptr->mynumber);
        drive_set_1571_side((byte >> 2) & 1, ctxptr->mynumber);
    } else
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
        parallel_drivex_set_bus((BYTE)(parieee_is_out ? byte : 0xff));
    }
    if (ctxptr->drive_ptr->parallel_cable_enabled
        && (ctxptr->drive_ptr->type == DRIVE_TYPE_1541
        || ctxptr->drive_ptr->type == DRIVE_TYPE_1541II))
        parallel_cable_drivex_write(byte, 0);
}

inline static void store_pra(drive_context_t *ctxptr, BYTE byte,
                             BYTE oldpa_value, ADDRESS addr)
{
    {
        if (ctxptr->drive_ptr->type == DRIVE_TYPE_1571) {
            if ((oldpa_value ^ byte) & 0x20)
                drive_sync_set_1571(byte & 0x20, ctxptr->mynumber);
            if ((oldpa_value ^ byte) & 0x04)
                drive_set_1571_side((byte >> 2) & 1, ctxptr->mynumber);
        } else
        if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
/*
            if(parallel_debug) {
                printf("store_pra(byte=%02x, ~byte=%02x)\n",byte, 0xff^byte);
            }
*/
            parallel_drivex_set_bus((BYTE)(parieee_is_out ? byte : 0xff));
        } else
        if (ctxptr->drive_ptr->parallel_cable_enabled
            && (ctxptr->drive_ptr->type == DRIVE_TYPE_1541
            || ctxptr->drive_ptr->type == DRIVE_TYPE_1541II))
            parallel_cable_drivex_write(byte,
                                        (((addr == VIA_PRA)
                                        && ((myvia[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0));
    }
}

static void undump_prb(drive_context_t *ctxptr, BYTE byte)
{
    if (iec_info != NULL) {
        BYTE *drive_bus, *drive_data;
        if (ctxptr->mynumber == 0) {
            drive_bus = &(iec_info->drive_bus);
            drive_data = &(iec_info->drive_data);
        } else {
            drive_bus = &(iec_info->drive2_bus);
            drive_data = &(iec_info->drive2_data);
        }
        *drive_data = ~byte;
        *drive_bus = ((((*drive_data) << 3) & 0x40)
            | (((*drive_data) << 6)
            & ((~(*drive_data) ^ iec_info->cpu_bus) << 3) & 0x80));
        iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus
            & iec_info->drive2_bus; /* two &s, don't need to differentiate */
        iec_info->drive_port
            = iec_info->drive2_port = (((iec_info->cpu_port >> 4) & 0x4)
            | (iec_info->cpu_port >> 7)
            | ((iec_info->cpu_bus << 3) & 0x80));
    } else {
        if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
            parieee_is_out = byte & 0x10;
            parallel_drivex_set_bus((BYTE)(parieee_is_out ? oldpa : 0xff));

            parallel_drivex_set_eoi((BYTE)(parieee_is_out && !(byte & 0x08)));
            parallel_drivex_set_dav((BYTE)(parieee_is_out && !(byte & 0x40)));
            parallel_drivex_set_ndac((BYTE)(((!parieee_is_out)
                                    && (!(byte & 0x04)))
                                    || (parallel_atn && (!(byte & 0x01)))
                                    || ((!parallel_atn) && (byte & 0x01))));
            parallel_drivex_set_nrfd((BYTE)(((!parieee_is_out)
                                    && (!(byte & 0x02)))
                                    || (parallel_atn && (!(byte & 0x01)))
                                    || ((!parallel_atn) && (byte & 0x01))));
        } else {
            iec_drivex_write((BYTE)(~byte));
        }
    }
}

inline static void store_prb(drive_context_t *ctxptr, BYTE byte, BYTE p_oldpb,
                             ADDRESS addr)
{
    if (byte != p_oldpb) {
        if (iec_info != NULL) {
            BYTE *drive_data, *drive_bus;
            if (ctxptr->mynumber == 0) {
                drive_data = &(iec_info->drive_data);
                drive_bus = &(iec_info->drive_bus);
            } else {
                drive_data = &(iec_info->drive2_data);
                drive_bus = &(iec_info->drive2_bus);
            }
            *drive_data = ~byte;
            *drive_bus = ((((*drive_data) << 3) & 0x40)
                | (((*drive_data) << 6)
                & ((~(*drive_data) ^ iec_info->cpu_bus) << 3) & 0x80));
            iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus
                & iec_info->drive2_bus;
            iec_info->drive_port
                = iec_info->drive2_port = (((iec_info->cpu_port >> 4) & 0x4)
                | (iec_info->cpu_port >> 7)
                | ((iec_info->cpu_bus << 3) & 0x80));
        } else
        if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
            BYTE tmp = ~byte;
/*
            if(parallel_debug) {
                printf("store_prb(byte=%02x, ~byte=%02x, prb=%02x, ddrb=%02x)\n",
                        byte, tmp, myvia[VIA_PRB],myvia[VIA_DDRB]);
                printf("  -> is_out=%d, eoi=%d, dav=%d\n",byte & 0x10,
                        !(byte & 0x08), !(byte & 0x40));
            }
*/
            parieee_is_out = byte & 0x10;
            parallel_drivex_set_bus((BYTE)(parieee_is_out ? oldpa : 0xff));

            if (parieee_is_out) {
                parallel_drivex_set_eoi((BYTE)(tmp & 0x08));
                parallel_drivex_set_dav((BYTE)(tmp & 0x40));
            } else {
                parallel_drivex_set_eoi(0);
                parallel_drivex_set_dav(0);
            }
            parallel_drivex_set_nrfd((BYTE)(((!parieee_is_out) && (tmp & 0x02))
                                || (parallel_atn && (tmp & 0x01))
                                || ((!parallel_atn) && (byte & 0x01))));
            parallel_drivex_set_ndac((BYTE)(((!parieee_is_out) && (tmp & 0x04))
                                || (parallel_atn && (tmp & 0x01))
                                || ((!parallel_atn) && (byte & 0x01))));
        } else {
            iec_drivex_write((BYTE)(~byte));
        }
    }
}

static void undump_pcr(drive_context_t *ctxptr, BYTE byte)
{
    if (ctxptr->mynumber != 0)
        drive_update_viad2_pcr(byte, &drive[0]);
}

inline static BYTE store_pcr(drive_context_t *ctxptr, BYTE byte, ADDRESS addr)
{
    return byte;
}

static void undump_acr(drive_context_t *ctxptr, BYTE byte)
{
}

inline static void store_acr(drive_context_t *ctxptr, BYTE byte)
{
}

inline static void store_sr(drive_context_t *ctxptr, BYTE byte)
{
}

inline static void store_t2l(drive_context_t *ctxptr, BYTE byte)
{
}

static void res_via(drive_context_t *ctxptr)
{
    parallel_drivex_set_ndac(0);
    parallel_drivex_set_nrfd(0);
    parallel_drivex_set_dav(0);
    parallel_drivex_set_eoi(0);
    parallel_drivex_set_bus(0xff);

    parieee_is_out = 1;

    iec_info = iec_get_drive_port();
    if (iec_info && ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
        iec_info->drive_bus = 0xff;
        iec_info->drive_data = 0xff;
        iec_info = NULL;
    }
}

inline static BYTE read_pra(drive_context_t *ctxptr, ADDRESS addr)
{
    BYTE byte;
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (ctxptr->drive_ptr->byte_ready_active == 0x6)
            rotation_rotate_disk(ctxptr->drive_ptr);
        tmp = (ctxptr->drive_ptr->byte_ready_level ? 0 : 0x80)
            | (ctxptr->drive_ptr->current_half_track == 2 ? 0 : 1);
        return (tmp & ~myvia[VIA_DDRA])
            | (myvia[VIA_PRA] & myvia[VIA_DDRA]);
    }
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
/*
        if (parallel_debug) {
            printf("read_pra(is_out=%d, parallel_bus=%02x, ddra=%02x\n",
                   parieee_is_out, parallel_bus, myvia[VIA_DDRA]);
        }
*/
        byte = parieee_is_out ? 0xff : parallel_bus;
        return (byte & ~myvia[VIA_DDRA]) | (myvia[VIA_PRA] & myvia[VIA_DDRA]);
    }
    byte = (ctxptr->drive_ptr->parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                          (myvia[VIA_PCR] & 0xe) == 0xa))
                                        ? 1 : 0)
            : ((myvia[VIA_PRA] & myvia[VIA_DDRA])
               | (0xff & ~myvia[VIA_DDRA])));
    return byte;
}

inline static BYTE read_prb(drive_context_t *ctxptr)
{
    BYTE byte;
    /* 0 for drive0, 0x20 for drive 1 */
    BYTE orval = (ctxptr->mynumber << 5);
    /* 0xfe for drive0, 0xff for drive 1 */
    BYTE andval = (0xfe | ctxptr->mynumber);

    if (iec_info != NULL) {
        byte = (((myvia[VIA_PRB] & 0x1a)
               | iec_info->drive_port) ^ 0x85) | orval;
    } else {
        if (ctxptr->drive_ptr->type == DRIVE_TYPE_2031) {
           byte = 0xff;
           if (parieee_is_out) {
               /* talk enable */
               if (parallel_nrfd)
                   byte &= 0xfd ;
               if (parallel_ndac)
                   byte &= 0xfb ;
           } else {
               /* listener */
               if (parallel_eoi)
                   byte &= 0xf7 ;
               if (parallel_dav)
                   byte &= 0xbf ;
           }
           if (!parallel_atn)
               byte &= 0x7f;
/*
           if (parallel_debug) {
               printf("read_prb(is_out=%d, byte=%02x, prb=%02x, ddrb=%02x\n",
                      parieee_is_out, byte, myvia[VIA_PRB], myvia[VIA_DDRB]);
           }
*/
           byte = (byte & ~myvia[VIA_DDRB])
                  | (myvia[VIA_PRB] & myvia[VIA_DDRB]);
           if (!ca2_state) {
               byte &= andval /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
/*
               if (parallel_debug) {
                   printf("read with ca2_state = 0 -> byte=%02x\n", byte);
               }
*/
           }
/*
           if (parallel_debug) {
               printf("       -> byte=%02x\n", byte);
           }
*/
        } else {
           byte = (((myvia[VIA_PRB] & 0x1a) | iec_drivex_read()) ^ 0x85)
                  | orval;
        }
    }
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

static void int_via1d0t1(CLOCK c)
{
    int_myviat1(&drive0_context, c);
}

static void int_via1d0t2(CLOCK c)
{
    int_myviat2(&drive0_context, c);
}

static void int_via1d1t1(CLOCK c)
{
    int_myviat1(&drive1_context, c);
}

static void int_via1d1t2(CLOCK c)
{
    int_myviat2(&drive1_context, c);
}

static via_initdesc_t via1_initdesc[2] = {
    { &drive0_context.via1, clk0_overflow_callback,
      int_via1d0t1, int_via1d0t2 },
    { &drive1_context.via1, clk1_overflow_callback,
      int_via1d1t1, int_via1d1t2 }
};

void via1d_init(drive_context_t *ctxptr)
{
    via_drive_init(ctxptr, via1_initdesc);
}

/* this function is shared by via1 and via2! */
void via_drive_init(drive_context_t *ctxptr, const via_initdesc_t *via_desc)
{
    char buffer[16];
    const via_initdesc_t *vd = &via_desc[ctxptr->mynumber];

    if (vd->via_ptr->log == LOG_ERR)
        vd->via_ptr->log = log_open(vd->via_ptr->my_module_name);

    vd->via_ptr->t1_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));
    vd->via_ptr->t2_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));

    sprintf(buffer, "%sT1", vd->via_ptr->myname);
    alarm_init(vd->via_ptr->t1_alarm, mycpu_alarm_context, buffer, vd->int_t1);
    sprintf(buffer, "%sT2", vd->via_ptr->myname);
    alarm_init(vd->via_ptr->t2_alarm, mycpu_alarm_context, buffer, vd->int_t2);

    clk_guard_add_callback(mycpu_clk_guard, vd->clk, NULL);
}


#define VIA_SHARED_CODE
#define VIACONTEXT drive_context_t

#include "viacore.c"

