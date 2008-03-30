/*
 * petvia.c - VIA emulation in the PET.
 *
 * Written by
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
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
 *
 */

#if 0
#define mycpu maincpu
#define myclk maincpu_clk
#define myvia via
#define myvia_init via_init

#define MYVIA_INT VIA_INT
#define MYVIA_NAME "Via"

#define mycpu_rmw_flag maincpu_rmw_flag
#define mycpu_int_status maincpu_int_status
#define mycpu_alarm_context maincpu_alarm_context
#define mycpu_clk_guard maincpu_clk_guard

#define myvia_reset via_reset
#define myvia_store via_store
#define myvia_read via_read
#define myvia_peek via_peek

#define myvia_log via_log
#define myvia_signal via_signal
#define myvia_prevent_clk_overflow via_prevent_clk_overflow
#define myvia_snapshot_read_module via_snapshot_read_module
#define myvia_snapshot_write_module via_snapshot_write_module
#endif

#include "vice.h"

#include "alarm.h"
#include "clkguard.h"
#include "crtc.h"
#include "datasette.h"
#include "drive.h"
#include "drivecpu.h"
#include "interrupt.h"
#include "keyboard.h"
#include "log.h"
#include "maincpu.h"
#include "parallel.h"
#include "pet.h"
#include "petsound.h"
#include "petvia.h"
#include "printer.h"
#include "types.h"


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

#define myvia_int_num   (via_context->int_num)
#define MYVIA_INT       (via_context->irq_line)

#define mycpu_rmw_flag  (*(via_context->rmw_flag))

#define myvia_reset     via_reset

#define myvia_store     viax_store
#define myvia_read      viax_read
#define myvia_peek      viax_peek

void REGPARM2 myvia_store(via_context_t *via_context, WORD addr, BYTE data);
BYTE REGPARM1 myvia_read(via_context_t *via_context, WORD addr);
BYTE REGPARM1 myvia_peek(via_context_t *via_context, WORD addr);

void REGPARM2 via_store(WORD addr, BYTE data)
{
    myvia_store(&(machine_context.via), addr, data);
}

BYTE REGPARM1 via_read(WORD addr)
{
    return myvia_read(&(machine_context.via), addr);
}

BYTE REGPARM1 via_peek(WORD addr)
{
    return myvia_peek(&(machine_context.via), addr);
}

#define myvia_log       (via_context->log)
#define myvia_signal    via_signal
#define myvia_snapshot_read_module via_snapshot_read_module
#define myvia_snapshot_write_module via_snapshot_write_module


/* switching PET charrom with CA2 */
static void via_set_ca2(int state)
{
    crtc_set_chargen_offset(state ? 256 : 0);
}

/* switching userport strobe with CB2 */
static void via_set_cb2(int state)
{
    printer_interface_userport_write_strobe(state);
}

static void via_set_int(via_context_t *via_context, unsigned int int_num,
                        int value)
{
    interrupt_set_irq(maincpu_int_status, int_num, value,
                      *(via_context->clk_ptr));
}

void petvia_setup_context(machine_context_t *machine_context)
{
    machine_context->via.context = NULL;

    machine_context->via.rmw_flag = &maincpu_rmw_flag;
    machine_context->via.clk_ptr = &maincpu_clk;

    sprintf(machine_context->via.myname, "Via");
    sprintf(machine_context->via.my_module_name, "VIA");
    machine_context->via.read_clk = 0;
    machine_context->via.read_offset = 0;
    machine_context->via.last_read = 0;
    machine_context->via.irq_line = IK_IRQ;
    machine_context->via.log = LOG_ERR;
}

static void undump_pra(via_context_t *via_context, BYTE byte)
{
    printer_interface_userport_write_data(byte);
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE myoldpa, WORD addr)
{
        printer_interface_userport_write_data(byte);
}

static void undump_prb(via_context_t *via_context, BYTE byte)
{
    parallel_cpu_set_nrfd((BYTE)(!(byte & 0x02)));
    parallel_cpu_restore_atn((BYTE)(!(byte & 0x04)));
}

inline static void store_prb(via_context_t *via_context, BYTE byte,
                             BYTE myoldpb, WORD addr)
{
    if ((addr == VIA_DDRB) && (myvia[addr] & 0x20)) {
        log_warning(myvia_log, "PET: Killer POKE! might kill a real PET!\n");
    }
    parallel_cpu_set_nrfd((BYTE)(!(byte & 0x02)));
    parallel_cpu_set_atn((BYTE)(!(byte & 0x04)));
    if ((byte ^ myoldpb) & 0x8)
        datasette_toggle_write_bit((~myvia[VIA_DDRB] | byte) & 0x8);
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
#if 0
    register BYTE tmp = byte;
    /* first set bit 1 and 5 to the real output values */
    if ((tmp & 0x0c) != 0x0c)
        tmp |= 0x02;
    if ((tmp & 0xc0) != 0xc0)
        tmp |= 0x20;
    crtc_set_char(byte & 2); /* switching PET charrom with CA2 */
                             /* switching userport strobe with CB2 */
#endif
}

inline static void store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
#if 0
    if (byte != myvia[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if((tmp & 0xc0) != 0xc0)
            tmp |= 0x20;
        crtc_set_char(byte & 2); /* switching PET charrom with CA2 */
                                 /* switching userport strobe with CB2 */
        printer_interface_userport_write_strobe(byte & 0x20);
    }
#endif
}

static void undump_acr(via_context_t *via_context, BYTE byte)
{
    store_petsnd_onoff(myvia[VIA_T2LL] ? (((byte & 0x1c) == 0x10) ? 1 : 0) : 0);
}

inline void static store_acr(via_context_t *via_context, BYTE byte)
{
    store_petsnd_onoff(myvia[VIA_T2LL] ? (((byte & 0x1c) == 0x10) ? 1 : 0) : 0);
}

inline void static store_sr(via_context_t *via_context, BYTE byte)
{
    store_petsnd_sample(byte);
}

inline void static store_t2l(via_context_t *via_context, BYTE byte)
{
    store_petsnd_rate(2 * byte + 4);
    if (!byte) {
        store_petsnd_onoff(0);
    } else {
        store_petsnd_onoff(((myvia[VIA_ACR] & 0x1c) == 0x10) ? 1 : 0);
    }
}

static void res_via(via_context_t *via_context)
{
    /* set IEC output lines */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_nrfd(0);

    printer_interface_userport_write_data(0xff);
    printer_interface_userport_write_strobe(1);
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    BYTE byte;
    byte = 255;
    /* VIA PA is connected to the userport pins C-L */
    byte &= (joystick_value[1] & 1) ? ~0x80 : 0xff;
    byte &= (joystick_value[1] & 2) ? ~0x40 : 0xff;
    byte &= (joystick_value[1] & 4) ? ~0x20 : 0xff;
    byte &= (joystick_value[1] & 8) ? ~0x10 : 0xff;
    byte &= (joystick_value[1] & 16)? ~0xc0 : 0xff;
    byte &= (joystick_value[2] & 1) ? ~0x08 : 0xff;
    byte &= (joystick_value[2] & 2) ? ~0x04 : 0xff;
    byte &= (joystick_value[2] & 4) ? ~0x02 : 0xff;
    byte &= (joystick_value[2] & 8) ? ~0x01 : 0xff;
    byte &= (joystick_value[2] & 16)? ~0x0c : 0xff;

    /* joystick always pulls low, even if high output, so no
       masking with DDRA */
    /*return ((j & ~myvia[VIA_DDRA]) | (myvia[VIA_PRA] & myvia[VIA_DDRA]));*/
    return byte;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    /* read parallel IEC interface line states */
    byte = 255
           - (parallel_nrfd ? 64 : 0)
           - (parallel_ndac ? 1 : 0)
           - (parallel_dav ? 128 : 0);
    /* vertical retrace */
    byte -= crtc_offscreen() ? 32 : 0;

    /* none of the load changes output register value -> std. masking */
    byte = ((byte & ~myvia[VIA_DDRB]) | (myvia[VIA_PRB] & myvia[VIA_DDRB]));
    return byte;
}

void printer_interface_userport_set_busy(int b)
{
    via_signal(&(machine_context.via),
               VIA_SIG_CA1, b ? VIA_SIG_RISE : VIA_SIG_FALL);
}

static void clk_overflow_callback(via_context_t *, CLOCK, void *);
static void int_myviat1(via_context_t *, CLOCK);
static void int_myviat2(via_context_t *, CLOCK);

static void clk_overflow_callback_via(CLOCK sub, void *data)
{
    clk_overflow_callback(&(machine_context.via), sub, data);
}

static void int_viat1(CLOCK c)
{
    int_myviat1(&(machine_context.via), c);
}

static void int_viat2(CLOCK c)
{
    int_myviat2(&(machine_context.via), c);
}

void via_init(via_context_t *via_context)
{
    char buffer[16];

    via_context->log = log_open(via_context->my_module_name);

    sprintf(buffer, "%sT1", via_context->myname);
    via_context->t1_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_viat1);
    sprintf(buffer, "%sT2", via_context->myname);
    via_context->t2_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_viat2);

    via_context->int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                        via_context->myname);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback_via, NULL);
}

#define VIA_SHARED_CODE

#include "viacore.c"

