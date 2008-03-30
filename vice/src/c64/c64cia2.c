/*
 * c64cia2.c - Definitions for the second MOS6526 (CIA) chip in the C64
 * ($DD00).
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
 * */


#include "vice.h"

#include <stdio.h>

#include "c64.h"
#include "c64mem.h"
#include "c64iec.h"
#include "c64cia.h"
#include "cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "maincpu.h"
#include "printer.h"
#include "types.h"
#include "vicii.h"

#ifdef HAVE_RS232
#include "rsuser.h"
#endif


void REGPARM2 cia2_store(WORD addr, BYTE data)
{
    ciacore_store(machine_context.cia2, addr, data);
}

BYTE REGPARM1 cia2_read(WORD addr)
{
    return ciacore_read(machine_context.cia2, addr);
}

BYTE REGPARM1 cia2_peek(WORD addr)
{
    return ciacore_peek(machine_context.cia2, addr);
}

static void cia_set_int_clk(cia_context_t *cia_context, int value, CLOCK clk)
{
    interrupt_set_nmi(maincpu_int_status, cia_context->int_num, value, clk);
}

static void cia_restore_int(cia_context_t *cia_context, int value)
{
    interrupt_restore_nmi(maincpu_int_status, cia_context->int_num, value);
}

#define MYCIA CIA2

/*************************************************************************
 * I/O
 */

/* Pointer to the IEC structure.  */
static iec_info_t *cia2_iec_info;

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

static const iec_cpu_write_callback_t iec_cpu_write_callback[4] = {
    iec_cpu_write_conf0, iec_cpu_write_conf1,
    iec_cpu_write_conf2, iec_cpu_write_conf3
};


static void do_reset_cia(cia_context_t *cia_context)
{
    printer_interface_userport_write_strobe(1);
    printer_interface_userport_write_data((BYTE)0xff);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)0xff);
    rsuser_set_tx_bit(1);
#endif
    cia2_iec_info = iec_get_drive_port();

    vbank = 0;
    mem_set_vbank(vbank);
}


static void pre_store(void)
{
    vicii_handle_pending_alarms_external(maincpu_num_write_cycles());
}

static void pre_read(void)
{
    vicii_handle_pending_alarms_external(0);
}

static void pre_peek(void)
{
    vicii_handle_pending_alarms_external(0);
}

static void store_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    if (cia_context->old_pa != byte) {
        BYTE tmp;
        int new_vbank;

#ifdef HAVE_RS232
        if (rsuser_enabled && ((cia_context->old_pa ^ byte) & 0x04)) {
            rsuser_set_tx_bit(byte & 4);
        }
#endif
        tmp = ~byte;
        new_vbank = tmp & 3;
        if (new_vbank != vbank) {
            vbank = new_vbank;
            mem_set_vbank(new_vbank);
        }
        iec_cpu_write_callback[iec_callback_index]((BYTE)tmp);
        printer_interface_userport_write_strobe(tmp & 0x04);
    }
}

static void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
#ifdef HAVE_RS232
    if(rsuser_enabled) {
        rsuser_set_tx_bit((int)(byte & 4));
    }
#endif
    vbank = (byte ^ 3) & 3;
    mem_set_vbank(vbank);
    iec_cpu_undump((BYTE)(byte ^ 0xff));
}


static void store_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    if (drive_context[0]->drive->parallel_cable_enabled
        || drive_context[1]->drive->parallel_cable_enabled)
        parallel_cable_cpu_write((BYTE)byte);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)byte);
#endif
}

static void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{
    if (drive_context[0]->drive->parallel_cable_enabled
        || drive_context[1]->drive->parallel_cable_enabled)
        parallel_cable_cpu_pulse();
    printer_interface_userport_write_data((BYTE)(cia_context->old_pb));
}

/* FIXME! */
static inline void undump_ciapb(cia_context_t *cia_context, CLOCK rclk,
                                BYTE byte)
{
    parallel_cable_cpu_undump((BYTE)byte);
    printer_interface_userport_write_data((BYTE)byte);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)byte);
#endif
}

/* read_* functions must return 0xff if nothing to read!!! */
static BYTE read_ciapa(cia_context_t *cia_context)
{
    BYTE byte;
    if (!(drive_context[0]->drive->enable)
        && !(drive_context[1]->drive->enable))
        return ((cia_context->c_cia[CIA_PRA] | ~(cia_context->c_cia[CIA_DDRA]))
            & 0x3f) | (cia2_iec_info->iec_fast_1541 & 0x30) << 2;

    drivecpu_execute_all(maincpu_clk);

    byte = ((cia_context->c_cia[CIA_PRA] | ~(cia_context->c_cia[CIA_DDRA]))
           & 0x3f) | cia2_iec_info->cpu_port;
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static BYTE read_ciapb(cia_context_t *cia_context)
{
    BYTE byte;
#ifdef HAVE_RS232
    byte = ((drive_context[0]->drive->parallel_cable_enabled
           || drive_context[1]->drive->parallel_cable_enabled)
            ? parallel_cable_cpu_read()
            : (rsuser_enabled
                ? rsuser_read_ctrl()
                : 0xff ));
#else
    byte = ((drive_context[0]->drive->parallel_cable_enabled
           || drive_context[1]->drive->parallel_cable_enabled)
            ? parallel_cable_cpu_read()
            : 0xff );
#endif
    byte = (byte & ~(cia_context->c_cia[CIA_DDRB]))
           | (cia_context->c_cia[CIA_PRB] & cia_context->c_cia[CIA_DDRB]);
    return byte;
}

static void read_ciaicr(cia_context_t *cia_context)
{
    if (drive_context[0]->drive->parallel_cable_enabled)
        drivecpu_execute(&drive0_context, maincpu_clk);
    if (drive_context[1]->drive->parallel_cable_enabled)
        drivecpu_execute(&drive1_context, maincpu_clk);
}

static void read_sdr(cia_context_t *cia_context)
{
}

static void store_sdr(cia_context_t *cia_context, BYTE byte)
{
}

/* Temporary!  */
void cia2_set_flagx(void)
{
    ciacore_set_flag(machine_context.cia2);
}

void cia2_set_sdrx(BYTE received_byte)
{
    ciacore_set_sdr(machine_context.cia2, received_byte);
}

static void int_cia2ta(CLOCK offset)
{
    ciacore_intta(machine_context.cia2, offset);
}

static void int_cia2tb(CLOCK offset)
{
    ciacore_inttb(machine_context.cia2, offset);
}

static void int_cia2tod(CLOCK offset)
{
    ciacore_inttod(machine_context.cia2, offset);
}

static cia_initdesc_t cia_initdesc[2] = {
    { NULL, int_cia2ta, int_cia2tb, int_cia2tod },
};

void cia2_init(cia_context_t *cia_context)
{
    cia_initdesc[0].cia_ptr = machine_context.cia2;

    ciacore_init(&cia_initdesc[0], maincpu_alarm_context, maincpu_int_status,
                 maincpu_clk_guard);
}

void cia2_setup_context(machine_context_t *machine_context)
{
    cia_context_t *cia;

    machine_context->cia2 = lib_malloc(sizeof(cia_context_t));
    cia = machine_context->cia2;

    cia->prv = NULL;
    cia->context = NULL;

    cia->rmw_flag = &maincpu_rmw_flag;
    cia->clk_ptr = &maincpu_clk;

    cia->todticks = 100000;

    ciacore_setup_context(cia);

    cia->debugFlag = 0;
    cia->irq_line = IK_NMI;
    cia->myname = lib_msprintf("CIA2");

    cia->undump_ciapa = undump_ciapa;
    cia->undump_ciapb = undump_ciapb;
    cia->store_ciapa = store_ciapa;
    cia->store_ciapb = store_ciapb;
    cia->store_sdr = store_sdr;
    cia->read_ciapa = read_ciapa;
    cia->read_ciapb = read_ciapb;
    cia->read_ciaicr = read_ciaicr;
    cia->read_sdr = read_sdr;
    cia->cia_set_int_clk = cia_set_int_clk;
    cia->cia_restore_int = cia_restore_int;
    cia->do_reset_cia = do_reset_cia;
    cia->pulse_ciapc = pulse_ciapc;
    cia->pre_store = pre_store;
    cia->pre_read = pre_read;
    cia->pre_peek = pre_peek;
}

void printer_interface_userport_set_busy(int b)
{
    if (!b)
        ciacore_set_flag(machine_context.cia2);
}

