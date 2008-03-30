/*
 * cbm2cia1.c - Definitions for the MOS6526 (CIA) chip in the CBM-II
 *
 * Written by
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
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

#include "cbm2.h"
#include "cbm2cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "interrupt.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "maincpu.h"
#include "parallel.h"
#include "printer.h"
#include "tpi.h"
#include "types.h"


void REGPARM2 cia1_store(WORD addr, BYTE data)
{
    ciacore_store(&(machine_context.cia1), addr, data);
}

BYTE REGPARM1 cia1_read(WORD addr)
{
    return ciacore_read(&(machine_context.cia1), addr);
}

BYTE REGPARM1 cia1_peek(WORD addr)
{
    return ciacore_peek(&(machine_context.cia1), addr);
}

static void cia_set_int_clk(cia_context_t *cia_context, int value, CLOCK clk)
{
    tpicore_set_int(&(machine_context.tpi1), 2, value);
}

static void cia_restore_int(cia_context_t *cia_context, int value)
{
    tpicore_restore_int(&(machine_context.tpi1), 2, value);
}

/*************************************************************************
 * I/O
 */

#define cycles_per_sec               machine_get_cycles_per_second()


static int cia1_ieee_is_output;

void cia1_set_ieee_dir(cia_context_t *cia_context, int isout)
{
    cia1_ieee_is_output = isout;
    if (isout) {
        parallel_cpu_set_bus(cia_context->old_pa);
    } else {
        parallel_cpu_set_bus(0xff);
    }
}


static void do_reset_cia(cia_context_t *cia_context)
{
    printer_interface_userport_write_strobe(1);
    printer_interface_userport_write_data(0xff);
}

static void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{

}

static void store_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    /* FIXME: PA0 and PA1 are used as selector for the
       Paddle 1/2 selection for the A/D converter. */
    parallel_cpu_set_bus((BYTE)(cia1_ieee_is_output ? byte : 0xff));
}

static void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(cia1_ieee_is_output ? byte : 0xff));
}

static void undump_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
}

static void store_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    printer_interface_userport_write_data(byte);
    printer_interface_userport_write_strobe(0);
    printer_interface_userport_write_strobe(1);
}

/* read_* functions must return 0xff if nothing to read!!! */
static BYTE read_ciapa(cia_context_t *cia_context)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    /* this reads the 8 bit IEEE488 data bus, but joystick 1 and 2 buttons
       can pull down inputs pa6 and pa7 resp. */
    byte = parallel_bus;
    if (parallel_debug) {
        log_message(LOG_DEFAULT,
                    "read: parallel_bus=%02x, pra=%02x, ddra=%02x -> %02x\n",
                    parallel_bus, cia_context->c_cia[CIA_PRA],
                    cia_context->c_cia[CIA_DDRA], byte);
    }
    byte = ((byte & ~(cia_context->c_cia[CIA_DDRA]))
           | (cia_context->c_cia[CIA_PRA] & cia_context->c_cia[CIA_DDRA]))
           & ~( ((joystick_value[1] & 0x10) ? 0x40 : 0)
           | ((joystick_value[2] & 0x10) ? 0x80 : 0) );
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static BYTE read_ciapb(cia_context_t *cia_context)
{
    BYTE byte;

    byte = ((0xff & ~(cia_context->c_cia[CIA_DDRB]))
           | (cia_context->c_cia[CIA_PRB] & cia_context->c_cia[CIA_DDRB]))
           & ~( (joystick_value[1] & 0x0f)
           | ((joystick_value[2] & 0x0f) << 4));
    return byte;
}

static void read_ciaicr(cia_context_t *cia_context)
{
}

static void read_sdr(cia_context_t *cia_context)
{
}

static void store_sdr(cia_context_t *cia_context, BYTE byte)
{
}

static void int_cia1ta(CLOCK offset)
{
    ciacore_intta(&(machine_context.cia1), offset);
}

static void int_cia1tb(CLOCK offset)
{
    ciacore_inttb(&(machine_context.cia1), offset);
}

static void int_cia1tod(CLOCK offset)
{
    ciacore_inttod(&(machine_context.cia1), offset);
}

static const cia_initdesc_t cia_initdesc[2] = {
    { &(machine_context.cia1), int_cia1ta, int_cia1tb, int_cia1tod },
};

void cia1_init(cia_context_t *cia_context)
{
    ciacore_init(&cia_initdesc[0], maincpu_alarm_context, maincpu_int_status,
                 maincpu_clk_guard);
}

void cia1_setup_context(machine_context_t *machine_context)
{
    cia_context_t *cia;

    cia = &(machine_context->cia1);

    cia->context = NULL;

    cia->rmw_flag = &maincpu_rmw_flag;
    cia->clk_ptr = &maincpu_clk;

    cia->todticks = 100000;

    ciacore_setup_context(cia);

    cia->debugFlag = 0;
    cia->irq_line = IK_IRQ;
    cia->myname = lib_msprintf("CIA1");

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
    cia->pre_store = NULL;
    cia->pre_read = NULL;
    cia->pre_peek = NULL;
}

void printer_interface_userport_set_busy(int b)
{
    if (!b)
        ciacore_set_flag(&(machine_context.cia1));
}

