/*
 * c64cia2.c - Definitions for the second MOS6526 (CIA) chip in the C64
 * ($DD00).
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "ciacore.h"

#include "c64mem.h"
#include "c64iec.h"
#include "c64cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
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
#define mycia_store cia2_store
#define mycia_read cia2_read
#define mycia_peek cia2_peek
#define mycia_set_flag cia2_set_flag
#define mycia_set_sdr cia2_set_sdr
#define mycia_snapshot_write_module cia2_snapshot_write_module
#define mycia_snapshot_read_module cia2_snapshot_read_module

#define mycia_debugFlag cia2_debugFlag
#define myciat_logfl cia2t_logfl

#define MYCIA_NAME "CIA2"

/*************************************************************************
 * CPU binding
 */

#include "interrupt.h"
#include "maincpu.h"

#define MYCIA_INT       IK_NMI

#define myclk           maincpu_clk
#define mycpu_clk_guard maincpu_clk_guard
#define mycpu_rmw_flag  maincpu_rmw_flag

#define cia_set_int_clk(value,clk) \
                interrupt_set_nmi(&maincpu_int_status,(I_CIA2FL),(value),(clk))

#define cia_restore_int(value) \
                interrupt_set_nmi_noclk(&maincpu_int_status,(I_CIA2FL),(value))

#define mycpu_alarm_context maincpu_alarm_context


#define MYCIA CIA2

/*************************************************************************
 * I/O
 */

/* Pointer to the IEC structure.  */
static iec_info_t *iec_info;

/* Current video bank (0, 1, 2 or 3).  */
static int vbank;

static iec_cpu_write_callback_t iec_cpu_write_callback[4] = {
    iec_cpu_write_conf0, iec_cpu_write_conf1,
    iec_cpu_write_conf2, iec_cpu_write_conf3
};


static inline void do_reset_cia(void)
{
    printer_interface_userport_write_strobe(1);
    printer_interface_userport_write_data((BYTE)0xff);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)0xff);
    rsuser_set_tx_bit(1);
#endif
    iec_info = iec_get_drive_port();

    vbank = 0;
    mem_set_vbank(vbank);
}


#define PRE_STORE_CIA \
    vic_ii_handle_pending_alarms_external(maincpu_num_write_cycles());

#define PRE_READ_CIA \
    vic_ii_handle_pending_alarms_external(0);

#define PRE_PEEK_CIA \
    vic_ii_handle_pending_alarms_external(0);

static inline void store_ciapa(CLOCK rclk, BYTE byte)
{
    if (oldpa != byte) {
        BYTE tmp;
        int new_vbank;

#ifdef HAVE_RS232
        if(rsuser_enabled && ((oldpa^byte)&0x04)) {
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

static inline void undump_ciapa(CLOCK rclk, BYTE byte)
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


static inline void store_ciapb(CLOCK rclk, BYTE byte)
{
    if (drive[0].parallel_cable_enabled || drive[1].parallel_cable_enabled)
        parallel_cable_cpu_write((BYTE)byte);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)byte);
#endif
}

static inline void pulse_ciapc(CLOCK rclk)
{
    if (drive[0].parallel_cable_enabled || drive[1].parallel_cable_enabled)
        parallel_cable_cpu_pulse();
    printer_interface_userport_write_data((BYTE)oldpb);
}

/* FIXME! */
static inline void undump_ciapb(CLOCK rclk, BYTE byte)
{
    parallel_cable_cpu_undump((BYTE)byte);
    printer_interface_userport_write_data((BYTE)byte);
#ifdef HAVE_RS232
    rsuser_write_ctrl((BYTE)byte);
#endif
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapa(void)
{
    BYTE byte;
    if (!drive[0].enable && !drive[1].enable)
        return ((cia[CIA_PRA] | ~cia[CIA_DDRA]) & 0x3f) |
            (iec_info->iec_fast_1541 & 0x30) << 2;
    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    byte = ( (cia[CIA_PRA] | ~cia[CIA_DDRA]) & 0x3f) | iec_info->cpu_port;
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapb(void)
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
    byte = (byte & ~cia[CIA_DDRB]) | (cia[CIA_PRB] & cia[CIA_DDRB]);
    return byte;
}

static inline void read_ciaicr(void)
{
    if (drive[0].parallel_cable_enabled)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].parallel_cable_enabled)
        drive1_cpu_execute(maincpu_clk);
}

static inline void store_sdr(BYTE byte) {}

#include "ciacore.c"

void printer_interface_userport_set_busy(int flank)
{
    if(!flank) {
        cia2_set_flag();
    }
}

