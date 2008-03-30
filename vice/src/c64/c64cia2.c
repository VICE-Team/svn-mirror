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
#include "clkguard.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "log.h"
#include "printer.h"
#include "types.h"
#include "vicii.h"

#ifdef HAVE_RS232
#include "rsuser.h"
#endif


/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init cia2_init
#define mycia_reset cia2_reset
#define mycia_store ciacore2_store
#define mycia_read ciacore2_read
#define mycia_peek ciacore2_peek
#define mycia_set_flag cia2_set_flag
#define mycia_set_sdr cia2_set_sdr
#define mycia_snapshot_write_module cia2_snapshot_write_module
#define mycia_snapshot_read_module cia2_snapshot_read_module

#define mycia_debugFlag cia2_debugFlag
#define myciat_logfl cia2t_logfl

#include "interrupt.h"
#include "maincpu.h"

#define mycpu_clk_guard  maincpu_clk_guard
#define mycpu_int_status maincpu_int_status

void REGPARM3 mycia_store(cia_context_t *cia_context, WORD addr, BYTE data);
BYTE REGPARM2 mycia_read(cia_context_t *cia_context, WORD addr);
BYTE REGPARM2 mycia_peek(cia_context_t *cia_context, WORD addr);

void REGPARM2 cia2_store(WORD addr, BYTE data)
{
    mycia_store(&(machine_context.cia2), addr, data);
}

BYTE REGPARM1 cia2_read(WORD addr)
{
    return mycia_read(&(machine_context.cia2), addr);
}

BYTE REGPARM1 cia2_peek(WORD addr)
{
    return mycia_peek(&(machine_context.cia2), addr);
}

static void cia_set_int_clk(cia_context_t *cia_context, int value, CLOCK clk)
{
    interrupt_set_nmi(maincpu_int_status, cia_context->int_num, value, clk);
}

static void cia_restore_int(cia_context_t *cia_context, int value)
{
    interrupt_set_nmi_noclk(maincpu_int_status, cia_context->int_num, value);
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


static inline void do_reset_cia(cia_context_t *cia_context)
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


#define PRE_STORE_CIA \
    vicii_handle_pending_alarms_external(maincpu_num_write_cycles());

#define PRE_READ_CIA \
    vicii_handle_pending_alarms_external(0);

#define PRE_PEEK_CIA \
    vicii_handle_pending_alarms_external(0);

static inline void store_ciapa(cia_context_t *cia_context, CLOCK rclk,
                               BYTE byte)
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

static inline void undump_ciapa(cia_context_t *cia_context, CLOCK rclk,
                                BYTE byte)
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


static inline void store_ciapb(cia_context_t *cia_context, CLOCK rclk,
                               BYTE byte)
{
    if (drive[0].parallel_cable_enabled || drive[1].parallel_cable_enabled)
        parallel_cable_cpu_write((BYTE)byte);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)byte);
#endif
}

static inline void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{
    if (drive[0].parallel_cable_enabled || drive[1].parallel_cable_enabled)
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
static inline BYTE read_ciapa(cia_context_t *cia_context)
{
    BYTE byte;
    if (!drive[0].enable && !drive[1].enable)
        return ((cia_context->c_cia[CIA_PRA] | ~(cia_context->c_cia[CIA_DDRA]))
            & 0x3f) |
            (cia2_iec_info->iec_fast_1541 & 0x30) << 2;
    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    byte = ((cia_context->c_cia[CIA_PRA] | ~(cia_context->c_cia[CIA_DDRA]))
           & 0x3f) | cia2_iec_info->cpu_port;
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapb(cia_context_t *cia_context)
{
    BYTE byte;
#ifdef HAVE_RS232
    byte = ((drive[0].parallel_cable_enabled || drive[1].parallel_cable_enabled)            ? parallel_cable_cpu_read()
            : (rsuser_enabled
                ? rsuser_read_ctrl()
                : 0xff ));
#else
    byte = ((drive[0].parallel_cable_enabled || drive[1].parallel_cable_enabled)            ? parallel_cable_cpu_read()
            : 0xff );
#endif
    byte = (byte & ~(cia_context->c_cia[CIA_DDRB]))
           | (cia_context->c_cia[CIA_PRB] & cia_context->c_cia[CIA_DDRB]);
    return byte;
}

static inline void read_ciaicr(cia_context_t *cia_context)
{
    if (drive[0].parallel_cable_enabled)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].parallel_cable_enabled)
        drive1_cpu_execute(maincpu_clk);
}

static inline void read_sdr(cia_context_t *cia_context)
{
}

static inline void store_sdr(cia_context_t *cia_context, BYTE byte)
{
}

/* Temporary!  */
void cia2_set_flagx(void)
{
    cia2_set_flag(&(machine_context.cia2));
}

void cia2_set_sdrx(BYTE received_byte)
{
    cia2_set_sdr(&(machine_context.cia2), received_byte);
}


static void int_ciata(cia_context_t *cia_context, CLOCK offset);
static void int_ciatb(cia_context_t *cia_context, CLOCK offset);
static void int_ciatod(cia_context_t *cia_context, CLOCK offset);

static void clk_overflow_callback(cia_context_t *cia_context, CLOCK sub,
                                  void *data);

static void clk_overflow_callback_cia2(CLOCK sub, void *data)
{
    clk_overflow_callback(&(machine_context.cia2), sub, data);
}

static void int_cia2ta(CLOCK offset)
{
    int_ciata(&(machine_context.cia2), offset);
}

static void int_cia2tb(CLOCK offset)
{
    int_ciatb(&(machine_context.cia2), offset);
}

static void int_cia2tod(CLOCK offset)
{
    int_ciatod(&(machine_context.cia2), offset);
}

void cia2_init(cia_context_t *cia_context)
{
    char buffer[16];

    cia_context->log = log_open(cia_context->myname);

    cia_context->int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                        cia_context->myname);

    sprintf(buffer, "%s_TA", cia_context->myname);
    cia_context->ta_alarm = alarm_new(maincpu_alarm_context, buffer,
                                      int_cia2ta);
    sprintf(buffer, "%s_TB", cia_context->myname);
    cia_context->tb_alarm = alarm_new(maincpu_alarm_context, buffer,
                                      int_cia2tb);
    sprintf(buffer, "%s_TOD", cia_context->myname);
    cia_context->tod_alarm = alarm_new(maincpu_alarm_context, buffer,
                                       int_cia2tod);

    clk_guard_add_callback(mycpu_clk_guard, clk_overflow_callback_cia2, NULL);

    sprintf(buffer, "%s_TA", cia_context->myname);
    ciat_init(&(cia_context->ta), buffer, *(cia_context->clk_ptr),
              cia_context->ta_alarm);
    sprintf(buffer, "%s_TB", cia_context->myname);
    ciat_init(&(cia_context->tb), buffer, *(cia_context->clk_ptr),
              cia_context->tb_alarm);
}

void cia2_setup_context(machine_context_t *machine_context)
{
    machine_context->cia2.context = NULL;

    machine_context->cia2.rmw_flag = &maincpu_rmw_flag;
    machine_context->cia2.clk_ptr = &maincpu_clk;

    machine_context->cia2.todticks = 100000;
    machine_context->cia2.log = LOG_ERR;
    machine_context->cia2.read_clk = 0;
    machine_context->cia2.read_offset = 0;
    machine_context->cia2.last_read = 0;
    machine_context->cia2.debugFlag = 0;
    machine_context->cia2.irq_line = IK_NMI;
    sprintf(machine_context->cia2.myname, "CIA2");
}

#include "ciacore.c"

void printer_interface_userport_set_busy(int b)
{
    if (!b)
        cia2_set_flag(&(machine_context.cia2));
}

