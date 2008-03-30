/*
 * via1d2031.c - VIA1 emulation in the 2031 disk drive.
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
#include "interrupt.h"
#include "lib.h"
#include "parallel.h"
#include "rotation.h"
#include "types.h"
#include "via.h"
#include "via1d2031.h"
#include "viad.h"


#define parieee_is_out  (via1p->v_parieee_is_out)

typedef struct drivevia1_context_s {
    unsigned int number;
    struct drive_s *drive;
    int parallel_id;
    int v_parieee_is_out;         /* init to 1 */
    struct iec_info_s *v_iec_info;
} drivevia1_context_t;


void REGPARM3 via1d2031_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    viacore_store(ctxptr->via1d2031, addr, data);
}

BYTE REGPARM2 via1d2031_read(drive_context_t *ctxptr, WORD addr)
{
    return viacore_read(ctxptr->via1d2031, addr);
}

BYTE REGPARM2 via1d2031_peek(drive_context_t *ctxptr, WORD addr)
{
    return viacore_peek(ctxptr->via1d2031, addr);
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


#define parallel_drivex_set_bus(a)      (((drive_context_t *)(via_context->context))->func->parallel_set_bus(a))
#define parallel_drivex_set_eoi(a)      (((drive_context_t *)(via_context->context))->func->parallel_set_eoi(a))
#define parallel_drivex_set_dav(a)      (((drive_context_t *)(via_context->context))->func->parallel_set_dav(a))
#define parallel_drivex_set_ndac(a)     (((drive_context_t *)(via_context->context))->func->parallel_set_ndac(a))
#define parallel_drivex_set_nrfd(a)     (((drive_context_t *)(via_context->context))->func->parallel_set_nrfd(a))


void via1d2031_set_atn(via_context_t *via_context, int state)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (via1p->drive->type == DRIVE_TYPE_2031) {
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

    parallel_drivex_set_bus((BYTE)(parieee_is_out ? byte : 0xff));
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE oldpa_value, WORD addr)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    parallel_drivex_set_bus((BYTE)(parieee_is_out ? byte : 0xff));
}

static void undump_prb(via_context_t *via_context, BYTE byte)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

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
}

inline static void store_prb(via_context_t *via_context, BYTE byte,
                             BYTE p_oldpb, WORD addr)
{
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    if (byte != p_oldpb) {
        BYTE tmp = ~byte;

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

    parallel_drivex_set_ndac(0);
    parallel_drivex_set_nrfd(0);
    parallel_drivex_set_dav(0);
    parallel_drivex_set_eoi(0);
    parallel_drivex_set_bus(0xff);

    parieee_is_out = 1;

#if 0
    iec_info = iec_get_drive_port();
    if (iec_info) {
        iec_info->drive_bus = 0xff;
        iec_info->drive_data = 0xff;
        iec_info = NULL;
    }
#endif
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    BYTE byte;
    drivevia1_context_t *via1p;

    via1p = (drivevia1_context_t *)(via_context->prv);

    byte = parieee_is_out ? 0xff : parallel_bus;

    return (byte & ~(via_context->via[VIA_DDRA]))
           | (via_context->via[VIA_PRA] & via_context->via[VIA_DDRA]);
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

    return byte;
}


/* These callbacks and the data initializations have to be done here */
static void int_via1d0t1(CLOCK c)
{
    viacore_intt1(drive0_context.via1d2031, c);
}

static void int_via1d0t2(CLOCK c)
{
    viacore_intt2(drive0_context.via1d2031, c);
}

static void int_via1d1t1(CLOCK c)
{
    viacore_intt1(drive1_context.via1d2031, c);
}

static void int_via1d1t2(CLOCK c)
{
    viacore_intt2(drive1_context.via1d2031, c);
}

static via_initdesc_t via_desc[2] = {
    { NULL, int_via1d0t1, int_via1d0t2 },
    { NULL, int_via1d1t1, int_via1d1t2 }
};

void via1d2031_init(drive_context_t *ctxptr)
{
    via_desc[0].via_ptr = drive0_context.via1d2031;
    via_desc[1].via_ptr = drive1_context.via1d2031;

    viacore_init(&via_desc[ctxptr->mynumber], ctxptr->cpu->alarm_context,
                 ctxptr->cpu->int_status, ctxptr->cpu->clk_guard);
}

void via1d2031_setup_context(drive_context_t *ctxptr)
{
    drivevia1_context_t *via1p;
    via_context_t *via;

    ctxptr->via1d2031 = lib_malloc(sizeof(via_context_t));
    via = ctxptr->via1d2031;

    via->prv = lib_malloc(sizeof(drivevia1_context_t));
    via1p = (drivevia1_context_t *)(via->prv);
    via1p->number = ctxptr->mynumber;

    via->context = (void *)ctxptr;

    via->rmw_flag = &(ctxptr->cpu->rmw_flag);
    via->clk_ptr = ctxptr->clk_ptr;

    via->myname = lib_msprintf("2031Drive%dVia1", ctxptr->mynumber);
    via->my_module_name = lib_msprintf("2031VIA1D%d", ctxptr->mynumber);

    viacore_setup_context(via);

    via->my_module_name_alt1 = lib_msprintf("VIA1D%d", ctxptr->mynumber);

    via->irq_line = IK_IRQ;

    via1p->drive = ctxptr->drive;
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

