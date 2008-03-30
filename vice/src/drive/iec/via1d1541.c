/*
 * via1d15xx.c - VIA1 emulation in the 1541, 1541II, 1570 and 1571 disk drive.
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
#include "drivecpu.h"
#include "drivesync.h"
#include "drivetypes.h"
#include "glue1571.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "lib.h"
#include "rotation.h"
#include "types.h"
#include "via.h"
#include "via1d1541.h"
#include "viad.h"


#define iec_info (via1p->v_iec_info)

typedef struct drivevia1_context_s {
    unsigned int number;
    struct drive_s *drive;
    int parallel_id;
    int v_parieee_is_out;         /* init to 1 */
    struct iec_info_s *v_iec_info;
} drivevia1_context_t;


void REGPARM3 via1d1541_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    viacore_store(ctxptr->via1d1541, addr, data);
}

BYTE REGPARM2 via1d1541_read(drive_context_t *ctxptr, WORD addr)
{
    return viacore_read(ctxptr->via1d1541, addr);
}

BYTE REGPARM2 via1d1541_peek(drive_context_t *ctxptr, WORD addr)
{
    return viacore_peek(ctxptr->via1d1541, addr);
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

    interrupt_set_irq(drive_context->cpu->int_status, int_num, value,
                      *(via_context->clk_ptr));
}

static void restore_int(via_context_t *via_context, unsigned int int_num,
                        int value)
{
    drive_context_t *drive_context;

    drive_context = (drive_context_t *)(via_context->context);

    interrupt_restore_irq(drive_context->cpu->int_status, int_num, value);
}

#define iec_drivex_write(a)             (((drive_context_t *)(via_context->context))->func->iec_write(a))
#define iec_drivex_read()               (((drive_context_t *)(via_context->context))->func->iec_read())
#define parallel_cable_drivex_write(a,b) (((drive_context_t *)(via_context->context))->func->parallel_cable_write(a,b))


static void undump_pra(via_context_t *via_context, BYTE byte)
{
    drivevia1_context_t *via1p;
    drive_context_t *drive_context;

    via1p = (drivevia1_context_t *)(via_context->prv);
    drive_context = (drive_context_t *)(via_context->context);

    iec_info = iec_get_drive_port();
    if (via1p->drive->type == DRIVE_TYPE_1570
        || via1p->drive->type == DRIVE_TYPE_1571
        || via1p->drive->type == DRIVE_TYPE_1571CR) {
        drive_sync_set_1571(byte & 0x20, drive_context);
        glue1571_side_set((byte >> 2) & 1, via1p->drive);
    } else

    if (via1p->drive->parallel_cable_enabled
        && (via1p->drive->type == DRIVE_TYPE_1541
        || via1p->drive->type == DRIVE_TYPE_1541II))
        parallel_cable_drivex_write(byte, 0);
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE oldpa_value, WORD addr)
{
    drivevia1_context_t *via1p;
    drive_context_t *drive_context;

    via1p = (drivevia1_context_t *)(via_context->prv);
    drive_context = (drive_context_t *)(via_context->context);

    if (via1p->drive->type == DRIVE_TYPE_1570
        || via1p->drive->type == DRIVE_TYPE_1571
        || via1p->drive->type == DRIVE_TYPE_1571CR) {
        if ((oldpa_value ^ byte) & 0x20)
            drive_sync_set_1571(byte & 0x20, drive_context);
        if ((oldpa_value ^ byte) & 0x04)
            glue1571_side_set((byte >> 2) & 1, via1p->drive);
        if ((oldpa_value ^ byte) & 0x02)
            iec_fast_drive_direction(byte & 2, via1p->number);
    } else {
        if (via1p->drive->parallel_cable_enabled
            && (via1p->drive->type == DRIVE_TYPE_1541
            || via1p->drive->type == DRIVE_TYPE_1541II))
            parallel_cable_drivex_write(byte,
                                        (((addr == VIA_PRA)
                                        && ((via_context->via[VIA_PCR]
                                        & 0xe) == 0xa)) ? 1 : 0));
    }
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
        iec_drivex_write((BYTE)(~byte));
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
        } else {
            iec_drivex_write((BYTE)(~byte));
        }
    }
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);
#if 0
    /* FIXME: Is this correct? */
    if (via1p->number != 0)
        viad2_update_pcr(byte, &drive[0]);
#endif
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

    iec_info = iec_get_drive_port();
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    BYTE byte;
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (via1p->drive->type == DRIVE_TYPE_1570
        || via1p->drive->type == DRIVE_TYPE_1571
        || via1p->drive->type == DRIVE_TYPE_1571CR) {
        BYTE tmp;
        if (via1p->drive->byte_ready_active == 0x6)
            rotation_rotate_disk(via1p->drive);
        tmp = (via1p->drive->byte_ready_level ? 0 : 0x80)
            | (via1p->drive->current_half_track == 2 ? 0 : 1);
        return (tmp & ~(via_context->via[VIA_DDRA]))
            | (via_context->via[VIA_PRA] & via_context->via[VIA_DDRA]);
    }

    byte = (via1p->drive->parallel_cable_enabled
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
        byte = (((via_context->via[VIA_PRB] & 0x1a)
               | iec_drivex_read()) ^ 0x85) | orval;
    }

    return byte;
}

void via1d1541_init(drive_context_t *ctxptr)
{
    viacore_init(ctxptr->via1d1541, ctxptr->cpu->alarm_context,
                 ctxptr->cpu->int_status, ctxptr->cpu->clk_guard);
}

void via1d1541_setup_context(drive_context_t *ctxptr)
{
    drivevia1_context_t *via1p;
    via_context_t *via;

    ctxptr->via1d1541 = lib_malloc(sizeof(via_context_t));
    via = ctxptr->via1d1541;

    via->prv = lib_malloc(sizeof(drivevia1_context_t));
    via1p = (drivevia1_context_t *)(via->prv);
    via1p->number = ctxptr->mynumber;

    via->context = (void *)ctxptr;

    via->rmw_flag = &(ctxptr->cpu->rmw_flag);
    via->clk_ptr = ctxptr->clk_ptr;

    via->myname = lib_msprintf("1541Drive%dVia1", ctxptr->mynumber);
    via->my_module_name = lib_msprintf("1541VIA1D%d", ctxptr->mynumber);

    viacore_setup_context(via);

    via->my_module_name_alt1 = lib_msprintf("VIA1D%d", ctxptr->mynumber);
    via->my_module_name_alt2 = lib_msprintf("VIA1D1541");

    via->irq_line = IK_IRQ;

    via1p->drive = ctxptr->drive;

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

