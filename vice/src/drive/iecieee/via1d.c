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
#include "lib.h"
#include "log.h"
#include "parallel.h"
#include "rotation.h"
#include "types.h"
#include "via.h"
#include "viad.h"


#define parieee_is_out  (via1p->v_parieee_is_out)
#define iec_info        (via1p->v_iec_info)


void REGPARM3 via1d_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    viacore_store(&(ctxptr->via1), addr, data);
}

BYTE REGPARM2 via1d_read(drive_context_t *ctxptr, WORD addr)
{
    return viacore_read(&(ctxptr->via1), addr);
}

BYTE REGPARM2 via1d_peek(drive_context_t *ctxptr, WORD addr)
{
    return viacore_peek(&(ctxptr->via1), addr);
}

static void set_ca2(int state)
{
}

static void set_cb2(int state)
{
}

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

    interrupt_restore_irq(drive_context->cpu.int_status, int_num, value);
}

#define iec_drivex_write(a)             (((drive_context_t *)(via_context->context))->func.iec_write(a))
#define iec_drivex_read()               (((drive_context_t *)(via_context->context))->func.iec_read())
#define parallel_cable_drivex_write(a,b) (((drive_context_t *)(via_context->context))->func.parallel_cable_write(a,b))
#define parallel_drivex_set_bus(a)      (((drive_context_t *)(via_context->context))->func.parallel_set_bus(a))
#define parallel_drivex_set_eoi(a)      (((drive_context_t *)(via_context->context))->func.parallel_set_eoi(a))
#define parallel_drivex_set_dav(a)      (((drive_context_t *)(via_context->context))->func.parallel_set_dav(a))
#define parallel_drivex_set_ndac(a)     (((drive_context_t *)(via_context->context))->func.parallel_set_ndac(a))
#define parallel_drivex_set_nrfd(a)     (((drive_context_t *)(via_context->context))->func.parallel_set_nrfd(a))


void drive_via_set_atn(via_context_t *via_context, int state)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
        viacore_signal(via_context, VIA_SIG_CA1, state ? VIA_SIG_RISE : 0);
        parallel_drivex_set_nrfd((BYTE)(((!parieee_is_out)
                                 && (!(via_context->oldpb & 0x02)))
                                 || (parallel_atn
                                 && (!(via_context->oldpb & 0x01)))
                                 || ((!parallel_atn)
                                 && (via_context->oldpb & 0x01))));
        parallel_drivex_set_ndac((BYTE)(((!parieee_is_out)
                                 && (!(via_context->oldpb & 0x04)))
                                 || (parallel_atn
                                 && (!(via_context->oldpb & 0x01)))
                                 || ((!parallel_atn)
                                 && (via_context->oldpb & 0x01))));
    }
}

static void undump_pra(via_context_t *via_context, BYTE byte)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    iec_info = iec_get_drive_port();
    if (via1p->drive_ptr->type == DRIVE_TYPE_1571) {
        drive_sync_set_1571(byte & 0x20, via1p->number);
        drive_set_1571_side((byte >> 2) & 1, via1p->number);
    } else
    if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
        parallel_drivex_set_bus((BYTE)(parieee_is_out ? byte : 0xff));
    }
    if (via1p->drive_ptr->parallel_cable_enabled
        && (via1p->drive_ptr->type == DRIVE_TYPE_1541
        || via1p->drive_ptr->type == DRIVE_TYPE_1541II))
        parallel_cable_drivex_write(byte, 0);
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE oldpa_value, WORD addr)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (via1p->drive_ptr->type == DRIVE_TYPE_1571) {
        if ((oldpa_value ^ byte) & 0x20)
            drive_sync_set_1571(byte & 0x20, via1p->number);
        if ((oldpa_value ^ byte) & 0x04)
            drive_set_1571_side((byte >> 2) & 1, via1p->number);
        if ((oldpa_value ^ byte) & 0x02)
            iec_fast_drive_direction(byte & 2, via1p->number);
    } else
    if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
/*
        if(parallel_debug) {
            printf("store_pra(byte=%02x, ~byte=%02x)\n",byte, 0xff^byte);
        }
*/
        parallel_drivex_set_bus((BYTE)(parieee_is_out ? byte : 0xff));
    } else
    if (via1p->drive_ptr->parallel_cable_enabled
        && (via1p->drive_ptr->type == DRIVE_TYPE_1541
        || via1p->drive_ptr->type == DRIVE_TYPE_1541II))
        parallel_cable_drivex_write(byte,
                                    (((addr == VIA_PRA)
                                    && ((via_context->via[VIA_PCR]
                                    & 0xe) == 0xa)) ? 1 : 0));
}

static void undump_prb(via_context_t *via_context, BYTE byte)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (iec_info != NULL) {
        BYTE *drive_bus, *drive_data;
        if (via1p->number == 0) {
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
        if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
            parieee_is_out = byte & 0x10;
            parallel_drivex_set_bus((BYTE)(parieee_is_out
                                    ? via_context->oldpa : 0xff));

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

inline static void store_prb(via_context_t *via_context, BYTE byte,
                             BYTE p_oldpb, WORD addr)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (byte != p_oldpb) {
        if (iec_info != NULL) {
            BYTE *drive_data, *drive_bus;
            if (via1p->number == 0) {
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
        if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
            BYTE tmp = ~byte;
/*
            if(parallel_debug) {
                printf("store_prb(byte=%02x, ~byte=%02x, prb=%02x, ddrb=%02x)\n",
                        byte, tmp, via_context->via[VIA_PRB],
                        via_context->via[VIA_DDRB]);
                printf("  -> is_out=%d, eoi=%d, dav=%d\n",byte & 0x10,
                        !(byte & 0x08), !(byte & 0x40));
            }
*/
            parieee_is_out = byte & 0x10;
            parallel_drivex_set_bus((BYTE)(parieee_is_out
                                    ? via_context->oldpa : 0xff));

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

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    /* FIXME: Is this correct? */
    if (via1p->number != 0)
        viad2_update_pcr(byte, &drive[0]);
}

inline static BYTE store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
    return byte;
}

static void undump_acr(via_context_t *via_context, BYTE byte)
{
}

inline static void store_acr(via_context_t *via_context, BYTE byte)
{
}

inline static void store_sr(via_context_t *via_context, BYTE byte)
{
}

inline static void store_t2l(via_context_t *via_context, BYTE byte)
{
}

static void reset(via_context_t *via_context)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    parallel_drivex_set_ndac(0);
    parallel_drivex_set_nrfd(0);
    parallel_drivex_set_dav(0);
    parallel_drivex_set_eoi(0);
    parallel_drivex_set_bus(0xff);

    parieee_is_out = 1;

    iec_info = iec_get_drive_port();
    if (iec_info && via1p->drive_ptr->type == DRIVE_TYPE_2031) {
        iec_info->drive_bus = 0xff;
        iec_info->drive_data = 0xff;
        iec_info = NULL;
    }
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    BYTE byte;
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (via1p->drive_ptr->type == DRIVE_TYPE_1571) {
        BYTE tmp;
        if (via1p->drive_ptr->byte_ready_active == 0x6)
            rotation_rotate_disk(via1p->drive_ptr);
        tmp = (via1p->drive_ptr->byte_ready_level ? 0 : 0x80)
            | (via1p->drive_ptr->current_half_track == 2 ? 0 : 1);
        return (tmp & ~(via_context->via[VIA_DDRA]))
            | (via_context->via[VIA_PRA] & via_context->via[VIA_DDRA]);
    }
    if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
        byte = parieee_is_out ? 0xff : parallel_bus;
        return (byte & ~(via_context->via[VIA_DDRA]))
               | (via_context->via[VIA_PRA] & via_context->via[VIA_DDRA]);
    }
    byte = (via1p->drive_ptr->parallel_cable_enabled
            ? parallel_cable_drive_read((((addr == VIA_PRA) &&
                                        (via_context->via[VIA_PCR]
                                        & 0xe) == 0xa)) ? 1 : 0)
            : ((via_context->via[VIA_PRA] & via_context->via[VIA_DDRA])
               | (0xff & ~(via_context->via[VIA_DDRA]))));
    return byte;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;
    BYTE orval;
    BYTE andval;
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    /* 0 for drive0, 0x20 for drive 1 */
    orval = (via1p->number << 5);
    /* 0xfe for drive0, 0xff for drive 1 */
    andval = (0xfe | via1p->number);

    if (iec_info != NULL) {
        byte = (((via_context->via[VIA_PRB] & 0x1a)
               | iec_info->drive_port) ^ 0x85) | orval;
    } else {
        if (via1p->drive_ptr->type == DRIVE_TYPE_2031) {
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
           byte = (byte & ~(via_context->via[VIA_DDRB]))
                  | (via_context->via[VIA_PRB] & via_context->via[VIA_DDRB]);
           if (!(via_context->ca2_state)) {
               byte &= andval /* 0xff */;  /* byte & 3 + 8 -> device-no */
               byte &= 0xfd /* 0xff */;  /* device-no switche */
           }
        } else {
           byte = (((via_context->via[VIA_PRB] & 0x1a)
                  | iec_drivex_read()) ^ 0x85) | orval;
        }
    }
    return byte;
}


/* These callbacks and the data initializations have to be done here */
static void clk0_overflow_callback(CLOCK sub, void *data)
{
    viacore_clk_overflow_callback(&(drive0_context.via1), sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    viacore_clk_overflow_callback(&(drive1_context.via1), sub, data);
}

static void int_via1d0t1(CLOCK c)
{
    viacore_intt1(&(drive0_context.via1), c);
}

static void int_via1d0t2(CLOCK c)
{
    viacore_intt2(&(drive0_context.via1), c);
}

static void int_via1d1t1(CLOCK c)
{
    viacore_intt1(&(drive1_context.via1), c);
}

static void int_via1d1t2(CLOCK c)
{
    viacore_intt2(&(drive1_context.via1), c);
}

static const via_initdesc_t via1_initdesc[2] = {
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

    sprintf(buffer, "%sT1", vd->via_ptr->myname);
    vd->via_ptr->t1_alarm = alarm_new(ctxptr->cpu.alarm_context, buffer,
                            vd->int_t1);
    sprintf(buffer, "%sT2", vd->via_ptr->myname);
    vd->via_ptr->t2_alarm = alarm_new(ctxptr->cpu.alarm_context, buffer,
                            vd->int_t2);

    clk_guard_add_callback(ctxptr->cpu.clk_guard, vd->clk, NULL);
}

void via1d_setup_context(drive_context_t *ctxptr)
{
    drivevia1_context_t *via1p;
    via_context_t *via;

    via = &(ctxptr->via1);

    via->prv = lib_malloc(sizeof(drivevia1_context_t));
    via1p = (drivevia1_context_t *)(via->prv);
    via1p->number = ctxptr->mynumber;

    via->context = (void *)ctxptr;

    via->rmw_flag = &(ctxptr->cpu.rmw_flag);
    via->clk_ptr = ctxptr->clk_ptr;

    sprintf(via->myname, "Drive%dVia1", via1p->number);
    sprintf(via->my_module_name, "VIA1D%d", via1p->number);
    via->read_clk = 0;
    via->read_offset = 0;
    via->last_read = 0;
    via->irq_line = IK_IRQ;
    via->log = LOG_ERR;
    via->int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status,
                                       via->myname);

    via1p->drive_ptr = ctxptr->drive_ptr;
    via1p->v_parieee_is_out = 1;
    if (via1p->number == 0) {
        via1p->parallel_id = PARALLEL_DRV0;
    } else {
        via1p->parallel_id = PARALLEL_DRV1;
    }

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

