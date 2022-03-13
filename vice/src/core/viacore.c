/*
 * viacore.c - Core functions for VIA emulation.
 *
 * Written by
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
 *  Olaf Seibert <rhialto@falu.nl>
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
#include <string.h>

#include "alarm.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "monitor.h"
#include "snapshot.h"
#include "types.h"
#include "via.h"


/*
 * 24jan97 a.fachat
 * new interrupt handling, hopefully according to the specs now.
 * All interrupts (note: not timer events (i.e. alarms) are put
 * into one interrupt flag.
 * if an interrupt condition changes, the function (i.e. cpp macro)
 * update_myviairq() id called, that checks the IRQ line state.
 * This is now possible, as ettore has decoupled A_* alarm events
 * from interrupts for performance reasons.
 *
 * A new function for signaling rising/falling edges on the
 * control lines is introduced:
 *      myvia_signal(VIA_SIG_[CA1|CA2|CB1|CB2], VIA_SIG_[RISE|FALL])
 * which signals the corresponding edge to the VIA. The constants
 * are defined in via.h.
 *
 * Except for shift register affecting CB1 (and associated IFR bits),
 * T2 counting PB6 pulses, and input latching everything should be ok now.
 * T1 affecting PB7 doesn't seem to work (properly), though.
 */

/* Timer debugging */
/*#define MYVIA_TIMER_DEBUG */
/* when PB7 is really used, set this
   to enable pulse output from the timer.
   Otherwise PB7 state is computed only
   when port B is read -
   not yet implemented */
#define MYVIA_NEED_PB7
/* When you really need latching, define this.
   It implies additional READ_PR* when
   writing the snapshot. When latching is
   enabled: it reads the port when enabling,
   and when an active C*1 transition occurs.
   It does not read the port when reading the
   port register. Side-effects beware! */
/* FIXME: this doesnt even work anymore */
/* #define MYVIA_NEED_LATCHING */

/*
 * Add consistency checks to verify that CPU access for clock N
 * occurs before the alarm for clock N.
 */
#define CHECK_CPU_VS_ALARM_CLOCKS       0
#define VIADEBUG                        0

#if VIADEBUG > 0
# define VIALOG(...)     fprintf(stderr, __VA_ARGS__)
#else
# define VIALOG(...)
#endif
#if VIADEBUG > 1
# define VIALOG2(...)    fprintf(stderr, __VA_ARGS__)
#else
# define VIALOG2(...)
#endif
#if VIADEBUG > 8
# define VIALOG9(...)    fprintf(stderr, __VA_ARGS__)
#else
# define VIALOG9(...)
#endif

/*
 * local functions
 */

#define IS_CA2_OUTPUT()          ((via_context->via[VIA_PCR] & 0x0c) == 0x0c)
#define IS_CA2_INDINPUT()        ((via_context->via[VIA_PCR] & 0x0a) == 0x02)
#define IS_CA2_HANDSHAKE()       ((via_context->via[VIA_PCR] & 0x0c) == 0x08)
#define IS_CA2_PULSE_MODE()      ((via_context->via[VIA_PCR] & 0x0e) == 0x09)
#define IS_CA2_TOGGLE_MODE()     ((via_context->via[VIA_PCR] & 0x0e) == 0x08)

#define IS_CB2_OUTPUT()          ((via_context->via[VIA_PCR] & 0xc0) == 0xc0)
#define IS_CB2_INDINPUT()        ((via_context->via[VIA_PCR] & 0xa0) == 0x20)
#define IS_CB2_HANDSHAKE()       ((via_context->via[VIA_PCR] & 0xc0) == 0x80)
#define IS_CB2_PULSE_MODE()      ((via_context->via[VIA_PCR] & 0xe0) == 0x90)
#define IS_CB2_TOGGLE_MODE()     ((via_context->via[VIA_PCR] & 0xe0) == 0x80)

#define IS_PA_INPUT_LATCH()      (via_context->via[VIA_ACR] & 0x01)
#define IS_PB_INPUT_LATCH()      (via_context->via[VIA_ACR] & 0x02)

#define IS_SR_SHIFTING_OUT()     (((via_context->via[VIA_ACR]) & 0x10))
#define IS_SR_SHIFT_OUT_BY_T2()  (((via_context->via[VIA_ACR]) & 0x1c) == 0x14)
#define IS_SR_FREE_RUNNING()     (((via_context->via[VIA_ACR]) & 0x1c) == 0x10)

#define IS_SR_SHIFT_IN_BY_EXT(byte)  (((byte) & 0x1c) == 0x0C)
#define IS_SR_T2_CONTROLLED(byte)  ((((byte) & 0x0c) == 0x04) || (((byte) & 0x1c) == 0x10)) /* SR modes 0x04, 0x14, 0x10 */
#define IS_T2_PULSE_COUNTING(byte) (((byte) & VIA_ACR_T2_CONTROL) == VIA_ACR_T2_COUNTPB6)
#define IS_T2_TIMER(byte)          (((byte) & VIA_ACR_T2_CONTROL) == VIA_ACR_T2_TIMER)

/*
 * 01apr98 a.fachat
 *
 * One-shot Timing (partly from 6522-VIA.txt):

                       +---- memory access for clock N
                       |/--- alarm call    for clock N
                       vv
                     +-+ +-+ +-+ +-+ +-+ +-+   +-+ +-+ +-+ +-+ +-+ +-+
                02 --+ +-+ +-+ +-+ +-+ +-+ +-#-+ +-+ +-+ +-+ +-+ +-+ +-
                       |   |                           |
                       +---+                           |
       WRITE T1C-H ----+   +-----------------#-------------------------
        ___                |                           |
        IRQ OUTPUT --------------------------#---------+
                           |                           +---------------
                           |                           |
        PB7 OUTPUT --------+                           +---------------
                           +-----------------#---------+
         T1                | N |N-1|N-2|N-3|     | 0 | -1|N  |N-1|N-2|
         T2                | N |N-1|N-2|N-3|     | 0 | -1| -2| -3| -4|
                           |                           |
                           |<---- N + 1.5 CYCLES ----->|<--- N + 2 cycles --->
                                                         +---+
 myviat*u* clk ------------------------------------------+   +--------
                                                         ^
                                                         |
                                                      call of
                                                    viacore_t2_zero_alarm
                                                       here

   real myviatau value = myviatau* + TAUOFFSET
   myviatbu = myviatbu* + 0

 *
 * IRQ and PB7 are set/toggled at the low-high transition of Phi2,
 * but int_* is called a half-cycle before that. Does that matter?
 *
 * PB7 output is still to be implemented
 */

/* timer values do not depend on a certain value here, but PB7 does... */
#define TAUOFFSET       (-1)

/*
 * Timing as can be observed by the CPU.
 * Alarms for rclock N run after CPU accesses for rclock N.
 
rclk       1     2     3     4     5     6     7     8     9    10    11    12
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
T2        1207  1206  1205  1204  1203  1202  1201  1200  11FF  1107  1106  1105
SR                80...                                                 01...
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
CB1                1...                                                  0...
CB2                0...                                                  1...
IFR          0..+SR?                                      +T2         +SR?
        +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
                                                       ^     ^     ^
                                      t2_zero_alarm ---+     |     |
                                      t2_underflow_alarm ----+     |
                                      t2_shift_alarm --------------+
 */

static void viacore_intt1(CLOCK offset, void *data);
static void viacore_t2_zero_alarm(CLOCK offset, void *data);
static void viacore_t2_underflow_alarm(CLOCK offset, void *data);
static void do_shiftregister(CLOCK offset, via_context_t *via_context);
inline static void schedule_t2_zero_alarm(via_context_t *via_context, CLOCK rclk);


static void via_restore_int(via_context_t *via_context, int value)
{
    (via_context->restore_int)(via_context, via_context->int_num, value);
}

inline static void update_myviairq_rclk(via_context_t *via_context, CLOCK rclk)
{
    (via_context->set_int)(via_context, via_context->int_num,
                           (via_context->ifr & via_context->ier & 0x7f)
                           ? via_context->irq_line : 0, rclk);
}

inline static void update_myviairq(via_context_t *via_context)
{
    update_myviairq_rclk(via_context, *(via_context->clk_ptr));
}

/* the next two are used in viacore_read() */

inline static CLOCK myviata(via_context_t *via_context)
{
    if (*(via_context->clk_ptr) < via_context->tau - TAUOFFSET) {
        return via_context->tau - TAUOFFSET - *(via_context->clk_ptr) - 2;
    } else {
        return (via_context->tal - (*(via_context->clk_ptr) - via_context->tau
                                    + TAUOFFSET) % (via_context->tal + 2));
    }
}

#define TBI_OFFSET              1
#define TBU_NEXT_OFFSET         2

#define SR_PHI2_FIRST_OFFSET    3
#define SR_PHI2_NEXT_OFFSET     1

/*
 * Get the current T2 value.
 *
 * When counting PB6 pulses, the value is actually stored in t2ch/t2cl.
 * Otherwise it counts down at clock speed and that is optimized.
 *
 * If tbi != 0, then t2zero is the next time that T2 will be xx00.
 *              so t2ch is needed to fill in xx.
 *              The t2_zero_alarm is set.
 * If tbi == 0, t2zero indicates when T2 will be / was last 0000,
 *              given the current rclk we calculate the current full T2 value.
 *              The t2_zero_alarm is not set.
 *
 * If the shift register mode sets the timer in 8-bit mode, tbi must be 0.
 *
 * In 16-bit mode, tbi *may* be 0. This is typically after the T2 IRQ has
 * triggered. But it may also be non-0. In that case, it will keep setting
 * t2_zero_alarms, until xx00 counts down to 0000. Then tbi will become 0
 * and t2_zero_alarms will be turned off.
 *
 * To be run from cpu access only, not from an alarm?
 */
inline static uint16_t myviatb(via_context_t *via_context, CLOCK rclk)
{
    uint16_t t2;

    if (via_context->via[VIA_ACR] & VIA_ACR_T2_COUNTPB6) {
        t2 = (via_context->t2ch << 8) | via_context->t2cl;
        VIALOG2("myviatb: countpb6: %04x\n", t2);
    } else {
        t2 = via_context->t2zero - rclk;
        VIALOG2("myviatb: timer: %04x\n", t2);

        if (via_context->tbi) {
            uint8_t t2hi = via_context->t2ch;
            VIALOG2("       : timer: %04x tbi:t2ch: %02x  rclk %04lu tbi %04lu t2_zero %lu\n", t2, t2hi, rclk, via_context->tbi, via_context->t2zero);

            t2 = (t2hi << 8) | (t2 & 0xff);
        }
    }

    return t2;
}

inline static void update_myviatal(via_context_t *via_context, CLOCK rclk)
{
    via_context->pb7x = 0;
    via_context->pb7xx = 0;

    if (rclk > via_context->tau) {
        CLOCK nuf = (via_context->tal + 1 + rclk - via_context->tau)
                  / (via_context->tal + 2);

        if (!(via_context->via[VIA_ACR] & VIA_ACR_T1_FREE_RUN)) {
            /* one shot mode */
            if (((nuf - via_context->pb7sx) > 1) || (!(via_context->pb7))) {
                via_context->pb7o = 1;
                via_context->pb7sx = 0;
            }
        }
        via_context->pb7 ^= (nuf & 1);

        via_context->tau = TAUOFFSET + via_context->tal + 2
                   + (rclk - (rclk - via_context->tau + TAUOFFSET)
                   % (via_context->tal + 2));
        if (rclk == via_context->tau - via_context->tal - 1) {
            via_context->pb7xx = 1;
        }
    }

    if (via_context->tau == rclk) {
        via_context->pb7x = 1;
    }

    via_context->tal = via_context->via[VIA_T1LL]
                       + (via_context->via[VIA_T1LH] << 8);
}

/* ------------------------------------------------------------------------- */
void viacore_disable(via_context_t *via_context)
{
    alarm_unset(via_context->t1_alarm);
    alarm_unset(via_context->t2_zero_alarm);
    alarm_unset(via_context->t2_underflow_alarm);
    alarm_unset(via_context->t2_shift_alarm);
    alarm_unset(via_context->phi2_sr_alarm);
    via_context->enabled = 0;
}

/*
 * according to Rockwell, all internal registers are cleared, except
 * for the Timer (1 and 2, counter and latches) and the shift register.
 */
void viacore_reset(via_context_t *via_context)
{
    int i;

    /* port data/ddr */
    for (i = 0; i < 4; i++) {
        via_context->via[i] = 0;
    }
    /* timer 1/2 counter/latches */
#if 0
    for (i = 4; i < 10; i++) {
        via_context->via[i] = 0xff;
    }
#endif
    /* omit shift register (10) */
    for (i = 11; i < 16; i++) {
        via_context->via[i] = 0;
    }

    via_context->tal = 0xffff;
    via_context->t2cl = 0xff;
    via_context->t2ch = 0xff;
    via_context->tau = *(via_context->clk_ptr);
    via_context->t2zero = *(via_context->clk_ptr);

    via_context->read_clk = 0;

    via_context->ier = 0;
    via_context->ifr = 0;

    via_context->pb7 = 0;
    via_context->pb7x = 0;
    via_context->pb7o = 0;
    via_context->pb7xx = 0;
    via_context->pb7sx = 0;

    via_context->shift_state = FINISHED_SHIFTING;      /* not shifting */
    via_context->t2_irq_allowed = 0;

    /* disable vice interrupts */
    via_context->tai = 0;
    via_context->tbi = 0;
    alarm_unset(via_context->t1_alarm);
    alarm_unset(via_context->t2_zero_alarm);
    alarm_unset(via_context->t2_underflow_alarm);
    alarm_unset(via_context->t2_shift_alarm);
    alarm_unset(via_context->phi2_sr_alarm);
    update_myviairq(via_context);

    via_context->oldpa = 0;
    via_context->oldpb = 0;

    via_context->ca2_state = 1;
    via_context->cb2_state = 1;
    (via_context->set_ca2)(via_context, via_context->ca2_state);      /* input = high */
    (via_context->set_cb2)(via_context, via_context->cb2_state);      /* input = high */

    if (via_context->reset) {
        (via_context->reset)(via_context);
    }

    via_context->enabled = 1;
}

void viacore_signal(via_context_t *via_context, int line, int edge)
{
    switch (line) {
        case VIA_SIG_CA1:
            if ((edge ? 1 : 0) == (via_context->via[VIA_PCR] & 0x01)) {
                if (IS_CA2_TOGGLE_MODE() && !(via_context->ca2_state)) {
                    via_context->ca2_state = 1;
                    (via_context->set_ca2)(via_context, via_context->ca2_state);
                }
                via_context->ifr |= VIA_IM_CA1;
                update_myviairq(via_context);
#ifdef MYVIA_NEED_LATCHING
                if (IS_PA_INPUT_LATCH()) {
                    via_context->ila = (via_context->read_pra)(via_context, VIA_PRA);
                }
#endif
            }
            break;
        case VIA_SIG_CA2:
            if (!(via_context->via[VIA_PCR] & 0x08)) {
                via_context->ifr |= (((edge << 2)
                                    ^ via_context->via[VIA_PCR]) & 0x04) ?
                                    0 : VIA_IM_CA2;
                update_myviairq(via_context);
            }
            break;
        case VIA_SIG_CB1:
            if ((edge ? 0x10 : 0) == (via_context->via[VIA_PCR] & 0x10)) {
                if (IS_CB2_TOGGLE_MODE() && !(via_context->cb2_state)) {
                    via_context->cb2_state = 1;
                    (via_context->set_cb2)(via_context, via_context->cb2_state);
                }
                via_context->ifr |= VIA_IM_CB1;
                update_myviairq(via_context);
#ifdef MYVIA_NEED_LATCHING
                if (IS_PB_INPUT_LATCH()) {
                    via_context->ilb = (via_context->read_prb)(via_context);
                }
#endif
            }
            break;
        case VIA_SIG_CB2:
            if (!(via_context->via[VIA_PCR] & 0x80)) {
                via_context->ifr |= (((edge << 6)
                                    ^ via_context->via[VIA_PCR]) & 0x40) ?
                                    0 : VIA_IM_CB2;
                update_myviairq(via_context);
            }
            break;
    }
}

/*
 * Return the clock when this alarm is due.
 * Alarms for clock N run after CPU accesses for that clock.
 * If the alarm is not set, returns 0.
 */
inline static CLOCK alarm_clk(alarm_t *alarm)
{
    alarm_context_t *context;
    int idx;

    context = alarm->context;
    idx = alarm->pending_idx;

    if (idx >= 0) {
        return context->pending_alarms[idx].clk;
    } else {
        return 0;
    }
}

#if CHECK_CPU_VS_ALARM_CLOCKS
static CLOCK last_alarm_clock;
static CLOCK last_cpu_clock;
#endif

/*
 * To be called only during CPU access to the registers.
 *
 * Run any pending alarms that should have run before the current clock cycle.
 * Alarms scheduled for cycle N run AFTER CPU accesses of cycle N.
 * Therefore we only run alarms < clk, i.e. alarms that should have run
 * up to and including the previous cycle.
 *
 * The normal execution run uses "clk >= alarm...", but since we're running
 * this during CPU access, here we use "clk > ...".
 */
inline static void run_pending_alarms(CLOCK clk, alarm_context_t *alarm_context)
{
    while (clk > alarm_context_next_pending_clk(alarm_context)) {
        VIALOG2("run_pending_alarms: %lu\n", clk);
#if CHECK_CPU_VS_ALARM_CLOCKS
        /* catch-up alarms don't count for checking which runs before which. */
        last_cpu_clock = 0;
#endif
        alarm_context_dispatch(alarm_context, clk);
#if CHECK_CPU_VS_ALARM_CLOCKS
        last_alarm_clock = 0;
#endif
    }
}

inline static int alarm_is_pending(alarm_t *alarm)
{
    return alarm->pending_idx >= 0;
}

inline static void alarm_set_if_not_pending(alarm_t *alarm, CLOCK cpu_clk)
{
    if (!alarm_is_pending(alarm)) {
        alarm_set(alarm, cpu_clk);
    }
}

/*
 * Before calling this, make sure that t2cl and t2ch are the correct T2
 * values for time rclk.
 *
 * Schedules an alarm for when T2 sets the interrupt flag (t2_zero_alarm).
 * tbi is set to indicate that T2 is in 8-bit mode (but the exact value
 * is not important any more) OR the next 0000 value should cause an IRQ.
 *
 * See also myviatb() and comments.
 */
inline static void schedule_t2_zero_alarm(via_context_t *via_context, CLOCK rclk)
{
    via_context->t2zero = rclk + via_context->t2cl;
    via_context->tbi    = rclk + via_context->t2cl + TBI_OFFSET;
    alarm_set(via_context->t2_zero_alarm, via_context->t2zero);
}

/*
 * Potentially enable the shifting of SR, by setting the shift_state,
 * in reaction to a read or write of the shift register.
 * Depending on the mode (phi2), will set an alarm to do the shifting.
 * For other modes, the alarm is already managed elsewhere.
 */
static inline void setup_shifting(via_context_t *via_context, CLOCK rclk)
{
    uint8_t acr = via_context->via[VIA_ACR];

    switch (acr & VIA_ACR_SR_CONTROL) {
    case VIA_ACR_SR_DISABLED:
        /*
         * Writing to SR while disabled apparently doesn't "start" the
         * shift counter. viasr18.prg fails if you do that.
         * But probably this doesn't *stop* it either.
         * viasr18.prg works either way (disabling or doing nothing).
         */
        VIALOG2("setup_shifting: disabled (do not change state %d)\n", via_context->shift_state);
        /* via_context->shift_state = FINISHED_SHIFTING; */
        break;
    case VIA_ACR_SR_IN_T2:
    case VIA_ACR_SR_OUT_T2:
            /*
             * Rockwell says that this happens, Commodore/MOS doesn't mention it
             * (in VIA_ACR_SR_IN/OUT_T2 modes):
             *
             * "The shifting operation is triggered by the read or write of
             * the SR if the SR flag is set in the IFR. Otherwise the first
             * shift will occur at the next time-out of T2 after a read or
             * write of the SR."
             *
             * If we believe the timing diagram, this happens on the 3rd
             * next positive flank of phi2.
             *
             * We just wait for T2 to underflow.
             */
    case VIA_ACR_SR_IN_CB1:
    case VIA_ACR_SR_OUT_CB1:
        if (via_context->shift_state == FINISHED_SHIFTING) {
	    VIALOG2("setup_shifting: enable (change state 16 to 0)\n");
            via_context->shift_state = START_SHIFTING;
        }
        break;
    case VIA_ACR_SR_IN_PHI2:
    case VIA_ACR_SR_OUT_PHI2:
        if (via_context->shift_state == FINISHED_SHIFTING) {
	    VIALOG2("setup_shifting: enable (change state 16 to 0)\n");
            via_context->shift_state = START_SHIFTING;
            /*
             * The phi2_sr_alarm triggers every cycle. If we just start shifting,
             * don't do the first shift in the same cycle.
             */
            alarm_set(via_context->phi2_sr_alarm, rclk + 1);
        }
        break;

    case VIA_ACR_SR_OUT_FREE_T2:
        VIALOG2("setup_shifting: enable free running (change state %d to %d)\n", via_context->shift_state, via_context->shift_state & 0x0F);
        /* Make sure it's < 16 and preserve even/oddness */
        via_context->shift_state &= 0x0F;
        break;
    }
}

void viacore_store(via_context_t *via_context, uint16_t addr, uint8_t byte)
{
    CLOCK rclk;

    if (*(via_context->rmw_flag)) {
        (*(via_context->clk_ptr))--;
        *(via_context->rmw_flag) = 0;
        viacore_store(via_context, addr, via_context->last_read);
        (*(via_context->clk_ptr))++;
    }

    /* stores have a one-cycle offset if CLK++ happens before store */
    rclk = *(via_context->clk_ptr) - via_context->write_offset;
    VIALOG2("viacore_store: rclk=%lu\n", rclk);
#if CHECK_CPU_VS_ALARM_CLOCKS
    last_cpu_clock = rclk;
    if (last_cpu_clock == last_alarm_clock) {
        VIALOG("cpu access after alarm %lu UNEXPECTED\n", rclk);
    }
#endif

    addr &= 0xf;

    if (addr >= VIA_T1CL && addr <= VIA_IER) {
	run_pending_alarms(rclk, via_context->alarm_context);
    }

    switch (addr) {
        /* these are done with saving the value */
        case VIA_PRA:           /* port A */
            via_context->ifr &= ~VIA_IM_CA1;
            if (!IS_CA2_INDINPUT()) {
                via_context->ifr &= ~VIA_IM_CA2;
            }
            if (IS_CA2_HANDSHAKE()) {
                via_context->ca2_state = 0;
                (via_context->set_ca2)(via_context, via_context->ca2_state);
                if (IS_CA2_PULSE_MODE()) {
                    via_context->ca2_state = 1;
                    (via_context->set_ca2)(via_context, via_context->ca2_state);
                }
            }
            if (via_context->ier & (VIA_IM_CA1 | VIA_IM_CA2)) {
                update_myviairq_rclk(via_context, rclk);
            }
            /* fall through */

        case VIA_PRA_NHS: /* port A, no handshake */
            via_context->via[VIA_PRA_NHS] = byte;
            addr = VIA_PRA;
            /* fall through */

        case VIA_DDRA:
            via_context->via[addr] = byte;
            byte = via_context->via[VIA_PRA] | ~(via_context->via[VIA_DDRA]);
            (via_context->store_pra)(via_context, byte, via_context->oldpa, addr);
            via_context->oldpa = byte;
            break;

        case VIA_PRB:           /* port B */
            via_context->ifr &= ~VIA_IM_CB1;
            if ((via_context->via[VIA_PCR] & 0xa0) != 0x20) {
                via_context->ifr &= ~VIA_IM_CB2;
            }
            if (IS_CB2_HANDSHAKE()) {
                via_context->cb2_state = 0;
                (via_context->set_cb2)(via_context, via_context->cb2_state);
                if (IS_CB2_PULSE_MODE()) {
                    via_context->cb2_state = 1;
                    (via_context->set_cb2)(via_context, via_context->cb2_state);
                }
            }
            if (via_context->ier & (VIA_IM_CB1 | VIA_IM_CB2)) {
                update_myviairq_rclk(via_context, rclk);
            }
            /* fall through */

        case VIA_DDRB:
            via_context->via[addr] = byte;
            byte = via_context->via[VIA_PRB] | ~(via_context->via[VIA_DDRB]);
            (via_context->store_prb)(via_context, byte, via_context->oldpb, addr);
            via_context->oldpb = byte;
            break;

        case VIA_SR:            /* Serial Port output buffer */
            via_context->via[addr] = byte;
            /* shift state can only be reset once 8 bits are complete */
            VIALOG2("writ VIA_SR  :          rclk %lu SR %02x shift_state %d\n", rclk, via_context->via[addr], via_context->shift_state);
            setup_shifting(via_context, rclk);
            if (via_context->ifr & VIA_IM_SR) {
                via_context->ifr &= ~VIA_IM_SR;
                update_myviairq_rclk(via_context, rclk);
            }

            (via_context->store_sr)(via_context, byte);
            break;

        /* Timers */

        case VIA_T1CL:
        case VIA_T1LL:
            via_context->via[VIA_T1LL] = byte;
            update_myviatal(via_context, rclk);
            break;

        case VIA_T1CH:  /* Write timer A high */
            via_context->via[VIA_T1LH] = byte;
            update_myviatal(via_context, rclk);
            /* load counter with latch value */
            via_context->tau = rclk + via_context->tal + 3 + TAUOFFSET;
            via_context->tai = rclk + via_context->tal + 2;
            alarm_set(via_context->t1_alarm, via_context->tai);

            /* set pb7 state */
            via_context->pb7 = 0;
            via_context->pb7o = 0;

            /* Clear T1 interrupt */
            via_context->ifr &= ~VIA_IM_T1;
            update_myviairq_rclk(via_context, rclk);
            break;

        case VIA_T1LH:          /* Write timer A high order latch */
            via_context->via[addr] = byte;
            update_myviatal(via_context, rclk);

            /* CAUTION: according to the synertek notes, writing to T1LH does
               NOT change the interrupt flags. however, not doing so breaks eg
               the VIC20 game "bandits". also in a seperare test program it was
               verified that indeed writing to the high order latch clears the
               interrupt flag, also on synertek VIAs. (see via_t1irqack) */

            /* Clear T1 interrupt */
            via_context->ifr &= ~VIA_IM_T1;
            update_myviairq_rclk(via_context, rclk);
            break;

        case VIA_T2LL:          /* Write timer 2 low latch */
            VIALOG2("write VIA_T2LL: %02x\n", byte);
            via_context->via[VIA_T2LL] = byte;
            /*
             * Rules for Shift out under control of T2:
             * - writing in the latch here will only take effect on the counter and
             *   the time of the t2_underflow_alarm at the next T2 underflow;
             * - only writing the SR can start the shifting, and the first bit
             *   is shifted out at the next T2L underflow. Until then T2L is counting
             *   down as usual, and it can be higher than this new latch value.
             */
            (via_context->store_t2l)(via_context, byte);
            break;

        case VIA_T2CH:            /* Write timer 2 high counter/latch */
            /* update counter and latch values */
            VIALOG2("write VIA_T2CH: %02x\n", byte);
            via_context->via[VIA_T2LH] = byte;
            via_context->t2cl = via_context->via[VIA_T2LL];
            via_context->t2ch = byte;

            /* start T2 only in timer mode, leave unchanged in pulse counting mode */
            if (!(via_context->via[VIA_ACR] & VIA_ACR_T2_COUNTPB6)) {
                /*
                 * Set the next alarm to the low latch value as timer cascading
                 * mode change matters at each underflow of the T2 low counter.
                 */
                /* Since starting the countdown takes a cycle, pretend
                 * it is 1 clock later than it really is. In the next cycle we
                 * should observe the starting value.
                 */
                schedule_t2_zero_alarm(via_context, rclk + 1);
                VIALOG2(" now T2: %04x\n", myviatb(via_context, rclk+1));
            } else {
                VIALOG2(" don't start t2_zero_alarm because ACR=%02x\n", via_context->via[VIA_ACR]);
            }

            /* Clear T2 interrupt */
            via_context->ifr &= ~VIA_IM_T2;
            update_myviairq_rclk(via_context, rclk);
            /* Each write to T2H allows one interrupt to occur */
            via_context->t2_irq_allowed = 1;
            break;

        /* Interrupts */

        case VIA_IFR:           /* 6522 Interrupt Flag Register */
            via_context->ifr &= ~byte;
            update_myviairq_rclk(via_context, rclk);

            /* FIXME:
             * clearing any timer interrupt should set the relevant timer alarm.
             * (It is unclear what was meant with that...
             */
            break;

        case VIA_IER:           /* Interrupt Enable Register */
            if (byte & VIA_IM_IRQ) {
                /* set interrupts */
                via_context->ier |= byte & 0x7f;
            } else {
                /* clear interrupts */
                via_context->ier &= ~byte;
            }
            update_myviairq_rclk(via_context, rclk);
            break;

        /* Control */

        case VIA_ACR:
            /* bit 7 timer 1 output to PB7 */
            VIALOG2("write VIA_ACR: %02x\n", byte);
            update_myviatal(via_context, rclk);
            if ((via_context->via[VIA_ACR] ^ byte) & VIA_ACR_T1_PB7_USED) {
                if (byte & VIA_ACR_T1_PB7_USED) {
                    via_context->pb7 = 1 ^ via_context->pb7x;
                }
            }
            if ((via_context->via[VIA_ACR] ^ byte) & VIA_ACR_T1_FREE_RUN) {
                via_context->pb7 ^= via_context->pb7sx;
                if ((byte & VIA_ACR_T1_FREE_RUN)) {
                    if (via_context->pb7x || via_context->pb7xx) {
                        if (via_context->tal) {
                            via_context->pb7o = 1;
                        } else {
                            via_context->pb7o = 0;
                            if ((via_context->via[VIA_ACR] & VIA_ACR_T1_PB7_USED)
                                && via_context->pb7x
                                && (!(via_context->pb7xx))) {
                                via_context->pb7 ^= 1;
                            }
                        }
                    }
                }
            }
            via_context->pb7sx = via_context->pb7x;

            /* bit 1, 0  latch enable port B and A */
#ifdef MYVIA_NEED_LATCHING
            /* switch on port A latching - FIXME: is this ok? */
            if ((!(via_context->via[addr] & VIA_ACR_PA_LATCH)) &&
                                    (byte & VIA_ACR_PA_LATCH)) {
                via_context->ila = (via_context->read_pra)(via_context, addr);
            }
            /* switch on port B latching - FIXME: is this ok? */
            if ((!(via_context->via[addr] & VIA_ACR_PB_LATCH)) &&
                                    (byte & VIA_ACR_PB_LATCH)) {
                via_context->ilb = (via_context->read_prb)(via_context);
            }
#endif

            int t2_startup_delay = 0;
            int restart_t2_alarms = 0;

            /* switch between timer and pulse counting mode if bit 5 changes */
            if ((via_context->via[VIA_ACR] ^ byte) & VIA_ACR_T2_CONTROL) {
                if (byte & VIA_ACR_T2_COUNTPB6) {
                    /* Pulse counting mode: set t2 to the current T2 value; 
                    PB6 should always update t2 and update irq on underflow */
                    /* Just like starting T2 takes a cycle, so does stopping:
                     * it decrements one further tick.
                     */
                    CLOCK stop = myviatb(via_context, rclk) - 1;
                    via_context->t2cl = (uint8_t)(stop & 0xff);
                    via_context->t2ch = (uint8_t)((stop >> 8) & 0xff);

                    /* stop alarm to prevent t2 and T2 updates */
                    alarm_unset(via_context->t2_zero_alarm);
                    via_context->tbi = 0;

                    /*
                     * FIXME re tbi above: is 8-bit mode handled correctly
                     * when in pulse counting mode? Probably not. Currently
                     * this is not relevant since we never detect pulses so
                     * the timer is stopped.
                     */
                } else {
                    /*
                     * Timer mode; set the next alarm to the low latch value as
                     * timer cascading mode change matters at each underflow of
                     * the T2 low counter.
                     *
                     * Since the timer takes a clock cycle before it starts
                     * decrementing, we should wait until then to start it.
                     * Or, since we can't have a bus cycle reading the timer in
                     * this same clock tick, we set the timer 1 too high right
                     * now.
                     */
                    restart_t2_alarms++;
                    t2_startup_delay++;
                }
            }

            /* bit 4, 3, 2 shift register control */
            switch (byte & VIA_ACR_SR_CONTROL) {
            case VIA_ACR_SR_DISABLED:
                alarm_unset(via_context->phi2_sr_alarm);
                /* "In this mode the SR Interrupt Flag is disabled
                 * (held to a logic 0)."
                 */
                if (via_context->ifr & VIA_IM_SR) {
                    via_context->ifr &= ~VIA_IM_SR;
                    update_myviairq_rclk(via_context, rclk);
                }
                break;
            case VIA_ACR_SR_IN_T2:
            case VIA_ACR_SR_OUT_T2:
            case VIA_ACR_SR_OUT_FREE_T2:
                alarm_unset(via_context->phi2_sr_alarm);
                /*
                 * Timer now in 8-bit mode. This requires the t2_zero_alarm.
                 * If OLD SR state was NOT T2-controlled, we may need to re-arm
                 * the t2_zero_alarm again: if the timer reached 0000 it would
                 * be unset by now.
                 * But only if NEW state is timer mode (not pulse counting).
                 */
                restart_t2_alarms =
                    restart_t2_alarms ||
                        (!IS_SR_T2_CONTROLLED(via_context->via[VIA_ACR]) &&
                          IS_T2_TIMER(byte));
                break;
            case VIA_ACR_SR_IN_PHI2:
            case VIA_ACR_SR_OUT_PHI2:
		/* Only schedule the alarm if it isn't scheduled already */
                alarm_set_if_not_pending(via_context->phi2_sr_alarm, rclk + SR_PHI2_FIRST_OFFSET);
                break;
            case VIA_ACR_SR_IN_CB1:
            case VIA_ACR_SR_OUT_CB1:
                /* TODO: Not emulated */
                alarm_unset(via_context->phi2_sr_alarm);
                break;
            }

            if (restart_t2_alarms &&
                    !alarm_is_pending(via_context->t2_zero_alarm) &&
                    !alarm_is_pending(via_context->t2_underflow_alarm)) {
                CLOCK current = myviatb(via_context, rclk);
                VIALOG2("set ACR: clock corr  : %04lx offset: %d\n", current,  + t2_startup_delay);
                /* Timer mode; set the next alarm to the low latch value as
                 * timer cascading mode change matters at each underflow of
                 * the T2 low counter.
                 */
                via_context->t2cl = current & 0xFF;
                via_context->t2ch = current >> 8;
                schedule_t2_zero_alarm(via_context, rclk + t2_startup_delay);
            }

            via_context->via[addr] = byte;
            (via_context->store_acr)(via_context, byte);

            break;

        case VIA_PCR:

            /* bit 7, 6, 5  CB2 handshake/interrupt control */
            /* bit 4  CB1 interrupt control */

            /* bit 3, 2, 1  CA2 handshake/interrupt control */
            /* bit 0  CA1 interrupt control */

            if ((byte & 0x0e) == 0x0c) { /* set output low */
                via_context->ca2_state = 0;
            } else
            if ((byte & 0x0e) == 0x0e) { /* set output high */
                via_context->ca2_state = 1;
            } else {                    /* set to toggle/pulse/input */
                /* FIXME: is this correct if handshake is already active? */
                via_context->ca2_state = 1;
            }
            (via_context->set_ca2)(via_context, via_context->ca2_state);

            if ((byte & 0xe0) == 0xc0) { /* set output low */
                via_context->cb2_state = 0;
            } else
            if ((byte & 0xe0) == 0xe0) { /* set output high */
                via_context->cb2_state = 1;
            } else {                    /* set to toggle/pulse/input */
                /* FIXME: is this correct if handshake is already active? */
                via_context->cb2_state = 1;
            }
            (via_context->set_cb2)(via_context, via_context->cb2_state);

            (via_context->store_pcr)(via_context, byte, addr);

            via_context->via[addr] = byte;

            break;

        default:
            via_context->via[addr] = byte;
    }
}


/* ------------------------------------------------------------------------- */

uint8_t viacore_read(via_context_t *via_context, uint16_t addr)
{
#ifdef MYVIA_TIMER_DEBUG
    uint8_t viacore_read_(via_context_t *via_context, uint16_t);
    uint8_t retv = myvia_read_(via_context, addr);
    addr &= 0x0f;
    if ((addr > 3 && addr < 10) || app_resources.debugFlag) {
        log_message(via_context->log,
                    "viacore_read(%x) -> %02x, clk=%ld", addr, retv,
                    *(via_context->clk_ptr));
    }
    return retv;
}

uint8_t viacore_read_(via_context_t *via_context, uint16_t addr)
{
#endif
    uint8_t byte = 0xff;
    CLOCK rclk;

    addr &= 0xf;

    via_context->read_clk = *(via_context->clk_ptr);
    via_context->read_offset = 0;
    rclk = *(via_context->clk_ptr);
    VIALOG2("viacore_read_: rclk=%lu\n", rclk);
#if CHECK_CPU_VS_ALARM_CLOCKS
    last_cpu_clock = rclk;
    if (last_cpu_clock == last_alarm_clock) {
        VIALOG("cpu read access after alarm %lu UNEXPECTED!!\n", rclk);
    }
#endif

    if (addr >= VIA_T1CL && addr <= VIA_IER) {
	run_pending_alarms(rclk, via_context->alarm_context);
    }

    switch (addr) {
        case VIA_PRA:           /* port A */
            via_context->ifr &= ~VIA_IM_CA1;
            if ((via_context->via[VIA_PCR] & 0x0a) != 0x02) {
                via_context->ifr &= ~VIA_IM_CA2;
            }
            if (IS_CA2_HANDSHAKE()) {
                via_context->ca2_state = 0;
                (via_context->set_ca2)(via_context, via_context->ca2_state);
                if (IS_CA2_PULSE_MODE()) {
                    via_context->ca2_state = 1;
                    (via_context->set_ca2)(via_context, via_context->ca2_state);
                }
            }
            if (via_context->ier & (VIA_IM_CA1 | VIA_IM_CA2)) {
                update_myviairq_rclk(via_context, rclk);
            }
            /* falls through */

        case VIA_PRA_NHS: /* port A, no handshake */
            /* WARNING: this pin reads the voltage of the output pins, not
               the ORA value as the other port. Value read might be different
               from what is expected due to excessive load. */
#ifdef MYVIA_NEED_LATCHING
            if (IS_PA_INPUT_LATCH()) {
                byte = via_context->ila;
            } else {
                byte = (via_context->read_pra)(via_context, addr);
            }
#else
            byte = (via_context->read_pra)(via_context, addr);
#endif
            via_context->ila = byte;
            via_context->last_read = byte;
            return byte;

        case VIA_PRB:           /* port B */
            via_context->ifr &= ~VIA_IM_CB1;
            if ((via_context->via[VIA_PCR] & 0xa0) != 0x20) {
                via_context->ifr &= ~VIA_IM_CB2;
            }
            if (via_context->ier & (VIA_IM_CB1 | VIA_IM_CB2)) {
                update_myviairq_rclk(via_context, rclk);
            }

            /* WARNING: this pin reads the ORA for output pins, not
               the voltage on the pins as the other port. */
#ifdef MYVIA_NEED_LATCHING
            if (IS_PB_INPUT_LATCH()) {
                byte = via_context->ilb;
            } else {
                byte = (via_context->read_prb)(via_context);
            }
#else
            byte = (via_context->read_prb)(via_context);
#endif
            via_context->ilb = byte;
            byte = (byte & ~(via_context->via[VIA_DDRB]))
                   | (via_context->via[VIA_PRB] & via_context->via[VIA_DDRB]);

            if (via_context->via[VIA_ACR] & 0x80) {
                update_myviatal(via_context, rclk);
                byte = (byte & 0x7f)
                       | (((via_context->pb7 ^ via_context->pb7x)
                           | via_context->pb7o) ? 0x80 : 0);
            }
            via_context->last_read = byte;
            return byte;

        /* Timers */

        case VIA_T1CL /*TIMER_AL */:    /* timer A low counter */
            via_context->ifr &= ~VIA_IM_T1;
            update_myviairq_rclk(via_context, rclk);
            via_context->last_read = (uint8_t)(myviata(via_context) & 0xff);
            return via_context->last_read;

        case VIA_T1CH /*TIMER_AH */:    /* timer A high counter */
            via_context->last_read = (uint8_t)((myviata(via_context) >> 8) & 0xff);
            return via_context->last_read;

        case VIA_T2CL /*TIMER_BL */:    /* timer B low counter */
            via_context->ifr &= ~VIA_IM_T2;
            update_myviairq_rclk(via_context, rclk);
            via_context->last_read = (uint8_t)(myviatb(via_context, rclk) & 0xff);
            VIALOG2("read  VIA_T2CL: %02x\n", via_context->last_read);
            return via_context->last_read;

        case VIA_T2CH /*TIMER_BH */:    /* timer B high counter */
            via_context->last_read = (uint8_t)((myviatb(via_context, rclk) >> 8) & 0xff);
            return via_context->last_read;

        case VIA_SR:            /* Serial Port Shift Register */
            /* shift state can only be reset once 8 bits are complete */
            VIALOG2("read VIA_SR  :          rclk %lu SR %02x shift_state %d\n", rclk, via_context->via[addr], via_context->shift_state);
            setup_shifting(via_context, rclk);
            if (via_context->ifr & VIA_IM_SR) {
                via_context->ifr &= ~VIA_IM_SR;
                update_myviairq_rclk(via_context, rclk);
            }
            via_context->last_read = via_context->via[addr];
            return via_context->last_read;

        /* Interrupts */

        case VIA_IFR:           /* Interrupt Flag Register */
            {
                uint8_t t = via_context->ifr;
                if (via_context->ifr & via_context->ier /*[VIA_IER] */) {
                    t |= 0x80;
                }
                via_context->last_read = t;
                return (t);
            }

        case VIA_IER:           /* 6522 Interrupt Enable Register */
            /* CBM datasheet says that bit 7 reads as logic 0, Rockwell says 1 */
            via_context->last_read = (via_context->ier /*[VIA_IER] */ | 0x80);
            return via_context->last_read;
    }

    via_context->last_read = via_context->via[addr];

    return via_context->via[addr];
}

/* return value of a register without side effects */
/* FIXME: this is buggy/incomplete */
uint8_t viacore_peek(via_context_t *via_context, uint16_t addr)
{

    addr &= 0xf;

    switch (addr) {
        case VIA_PRA:
        case VIA_PRA_NHS: /* port A, no handshake */
            {
                uint8_t byte;
                /* WARNING: this pin reads the voltage of the output pins, not
                the ORA value as the other port. Value read might be different
                from what is expected due to excessive load. */
#ifdef MYVIA_NEED_LATCHING
                if (IS_PA_INPUT_LATCH()) {
                    byte = via_context->ila;
                } else {
                    /* FIXME: side effects ? */
                    byte = (via_context->read_pra)(via_context, addr);
                }
#else
                /* FIXME: side effects ? */
                byte = (via_context->read_pra)(via_context, addr);
#endif
                return byte;
            }

        case VIA_PRB:           /* port B */
            {
                uint8_t byte;
#ifdef MYVIA_NEED_LATCHING
                if (IS_PB_INPUT_LATCH()) {
                    byte = via_context->ilb;
                } else {
                    /* FIXME: side effects ? */
                    byte = (via_context->read_prb)(via_context);
                }
#else
                /* FIXME: side effects ? */
                byte = (via_context->read_prb)(via_context);
#endif
                byte = (byte & ~(via_context->via[VIA_DDRB]))
                       | (via_context->via[VIA_PRB] & via_context->via[VIA_DDRB]);
                if (via_context->via[VIA_ACR] & 0x80) {
                    /* update_myviatal(via_context, rclk); */
                    byte = (byte & 0x7f) | (((via_context->pb7 ^ via_context->pb7x)
                                             | via_context->pb7o) ? 0x80 : 0);
                }
                return byte;
            }
        case VIA_DDRA:
        case VIA_DDRB:
            break;

        /* Timers */

        case VIA_T1CL /*TIMER_AL */:    /* timer A low */
            return (uint8_t)(myviata(via_context) & 0xff);

        case VIA_T1CH /*TIMER_AH */:    /* timer A high */
            return (uint8_t)((myviata(via_context) >> 8) & 0xff);

        case VIA_T1LL: /* timer A low order latch */
        case VIA_T1LH: /* timer A high order latch */
            break;

        case VIA_T2CL /*TIMER_BL */:    /* timer B low */
            return (uint8_t)(myviatb(via_context, *(via_context->clk_ptr)) & 0xff);

        case VIA_T2CH /*TIMER_BH */:    /* timer B high */
            return (uint8_t)((myviatb(via_context, *(via_context->clk_ptr)) >> 8) & 0xff);

        case VIA_IFR:           /* Interrupt Flag Register */
            return via_context->ifr;

        case VIA_IER:           /* 6522 Interrupt Enable Register */
            return via_context->ier | 0x80;

        case VIA_PCR:
        case VIA_ACR:
        case VIA_SR:
            break;
    }

    return via_context->via[addr];
}

/* ------------------------------------------------------------------------- */

static void viacore_intt1(CLOCK offset, void *data)
{
    CLOCK rclk;
    via_context_t *via_context = (via_context_t *)data;

    rclk = *(via_context->clk_ptr) - offset;


#ifdef MYVIA_TIMER_DEBUG
    if (app_resources.debugFlag) {
        log_message(via_context->log, "myvia timer A interrupt");
    }
#endif

    if (!(via_context->via[VIA_ACR] & 0x40)) {     /* one-shot mode */
#ifdef MYVIA_TIMER_DEBUG
        log_message(via_context->log,
                    "MYVIA Timer A interrupt -- one-shot mode: next int won't happen");
#endif
        alarm_unset(via_context->t1_alarm);
        via_context->tai = 0;
    } else {                    /* continuous mode */
        /* load counter with latch value */
        via_context->tai += via_context->tal + 2;
        alarm_set(via_context->t1_alarm, via_context->tai);

        /* Let tau also keep up with the cpu clock
           this should avoid "% (via_context->tal + 2)" case */
        via_context->tau += via_context->tal + 2;
    }
    via_context->ifr |= VIA_IM_T1;
    update_myviairq_rclk(via_context, rclk);

    /* TODO: toggle PB7? */
    /*(viaier & VIA_IM_T1) ? 1:0; */
}

/* ------------------------------------------------------------------------- */

/* WARNING: this is a hack, used to interface with c64fastiec.c, c128fastiec.c */
void viacore_set_sr(via_context_t *via_context, uint8_t data)
{
    if (!(via_context->via[VIA_ACR] & VIA_ACR_SR_OUT) &&
         (via_context->via[VIA_ACR] & 0x0c)) {
        via_context->via[VIA_SR] = data;
        via_context->ifr |= VIA_IM_SR;
        update_myviairq(via_context);
        via_context->shift_state = FINISHED_SHIFTING;

        VIALOG("burstmodehack engaged... data %02x\n", data);
    }
}

/* ------------------------------------------------------------------------- */

/* T2 can be switched between 8 and 16 bit modes ad-hoc, any time, by setting
   the shifter to be controlled by T2 via selecting the relevant ACR shift
   register operating mode.
   This change affects how the next T2 low underflow is handled.

   This alarm should be enabled only when the timer is in 8-bit mode, or still
   has to generate its first IRQ (reach its first 16-bit underflow).
   But not if T2 is in pulse counting mode.

   Alarm functions for cycle N are called after the CPU has run the memory
   access for cycle N. Furthermore they are typically run after the whole
   instruction so offset tells you how many cycles to go back.

   Called due to t2_zero_alarm.
 */
static void viacore_t2_zero_alarm(CLOCK offset, void *data)
{
    CLOCK rclk;
    via_context_t *via_context = (via_context_t *)data;

    rclk = *(via_context->clk_ptr) - offset;
    VIALOG2("viacore_t2_zero_alarm %2x %2x tbi %lu offset %lu rclk %lu\n", via_context->t2ch, via_context->t2cl, via_context->tbi, offset, rclk);

    /* T2 low count underflow always decreases T2 high count */
    via_context->t2ch--;
    VIALOG2("viacore_t2_zero_alarm %2x %2x (decremented) offset %lu rclk %lu\n", via_context->t2ch, via_context->t2cl, offset, rclk);

    /* 16 bit timer underflow generates an interrupt */
    /* FIXME: does 16 bit underflow generate an IRQ in 8 bit mode? 8 bit underflow does not. Probable answer: YES. */
    /* FIXME: no IRQ when shift register is in free running mode? */
    if (via_context->t2ch == 0xff) {
        /*
         * At this point, maybe we should check t2_irq_allowed here;
         * each write to T2H is supposed to trigger no more than one IRQ.
         * However we have no test program to cover this case.
         */
        via_context->ifr |= VIA_IM_T2;
        update_myviairq_rclk(via_context, rclk);
        via_context->t2_irq_allowed = 0;
        VIALOG2("viacore_t2_zero_alarm set VIA_IM_T2\n");
    }

    alarm_unset(via_context->t2_zero_alarm);
    alarm_set(via_context->t2_underflow_alarm, rclk + 1);
}

/*
 * Called after T2 underflows (i.e. it has shown xxFF), and it has
 * to decide if the latch gets reloaded into the counter, or not.
 * If so, there may be a shift action another cycle later.
 */
static void viacore_t2_underflow_alarm(CLOCK offset, void *data)
{
    CLOCK rclk;
    int next_alarm;
    via_context_t *via_context = (via_context_t *)data;

    rclk = *(via_context->clk_ptr) - offset;

    /* If the shifter is under T2 control, the T2 timer works differently, and have a period of T2 low.
       T2 high is still cascaded though and decreases at each T2 low underflow */
    if ((via_context->via[VIA_ACR] & 0x0c) == 0x04) {
        /* 8 bit timer mode; reload T2 low from latch */
        via_context->t2cl = via_context->via[VIA_T2LL];

        /* set next alarm to T2 low period */
        next_alarm = via_context->via[VIA_T2LL] + TBU_NEXT_OFFSET;

        /* T2 acts as a pulse generator for CB1
           every second underflow is a pulse updating the shift register, 
           until all 8 bits are complete */
        alarm_set(via_context->t2_shift_alarm, rclk + 1);
    } else if (IS_SR_FREE_RUNNING()) {
        /* Free-running output */
        /* Should we copy the latch to T2 low, as above? */
        via_context->t2cl = via_context->via[VIA_T2LL];

        /* set next alarm to T2 low period */
        next_alarm = via_context->via[VIA_T2LL] + TBU_NEXT_OFFSET;

        /* same as above, except bits will we clocked out CB2 repeatedly without
         * stopping after 8 bits */
        alarm_set(via_context->t2_shift_alarm, rclk + 1);
    } else {
        /* 16 bit timer mode; it is guaranteed that T2 low is in underflow */
        via_context->t2cl = 0xff;

        /* Set next alarm to 256 cycles later, until t2 high underflow.
         * Each write to T2H should generate only one irq at the first underflow.
         * If we don't need another IRQ, don't re-arm the alarm.
         */
        next_alarm = (via_context->t2ch != 0xff) ? 256 : 0;
    }

    /* set the next T2 low zero alarm, or turn off the alarm */
    if (next_alarm) {
        via_context->t2zero += next_alarm;
        /* tbi may be 0, so calculate it as its offset from t2zero */
        via_context->tbi = via_context->t2zero + TBI_OFFSET;
        alarm_set(via_context->t2_zero_alarm, via_context->t2zero);
    } else {
        alarm_unset(via_context->t2_zero_alarm);
        /*
         * Switch to 16-bit counter calculation.
         * This is ok since the current clock is 1 after t2_zero,
         * and T2 reads 0xFFFF aka -1.
         */
        via_context->tbi = 0;
    }

    alarm_unset(via_context->t2_underflow_alarm);
}

/*
 * This function is there to delay the actual shifting of the shift
 * register. It is supposed to shift AFTER Timer 2 has been reloaded from
 * the latch.
 *
 * Whether there is a shift at all, depends on the ACR.
 * It is so far unknown if
 * - the decision is made (based on the ACR), and if yes, then there is always
 *   a shift later
 * or
 * - the decision to shift (or not) is made after the delay and then in happens
 *   immediately.
 *
 * There might be a difference if the ACR is written to at an unfortuate moment.
 *
 * This code implements the first version.
 * (Simulation of the VIA from https://github.com/hoglet67/BeebFpga/blob/master/src/common/m6522.vhd
 * suggests that reloading from the latch and the shift
 * either both happen, or both don't happen)
 */

/*
 * Alarm callback for the case when the shift register is delayed by 1 clock.
 *
 * Called due to t2_shift_alarm.
 */
static void viacore_t2_shift_alarm(CLOCK offset, void *data)
{
    via_context_t *via_context = (via_context_t *)data;

#if CHECK_CPU_VS_ALARM_CLOCKS
    CLOCK rclk = *(via_context->clk_ptr) - offset;
    last_alarm_clock = rclk;
    if (last_cpu_clock == last_alarm_clock) {
        VIALOG9("alarm after cpu access %lu (expected)\n", rclk);
    }
#endif

    VIALOG2("viacore_shift_alarm: offset %ld rclk %lu SR %02x shift_state %d\n", (int64_t)offset, rclk, via_context->via[VIA_SR], via_context->shift_state);
    do_shiftregister(offset, via_context);
    alarm_unset(via_context->t2_shift_alarm);
}

static inline void do_shiftregister(CLOCK offset, via_context_t *via_context)
{
    CLOCK rclk;
    rclk = *(via_context->clk_ptr) - offset;

    VIALOG2("do_shiftregister: ->shift_state = %d %02x %lu\n", via_context->shift_state, via_context->via[VIA_SR], rclk);
    if (via_context->shift_state < FINISHED_SHIFTING) {
        /*
         * When shifting out, it shifts first and then waits (holding the value),
         * but when shifting in, it waits first and then shifts.
         */
        int shift_out = via_context->via[VIA_ACR] & VIA_ACR_SR_OUT;

        /* FIXME: CB1 should be toggled, and interrupt flag set according to
         * edge detection in PCR */
        if ((via_context->shift_state & 1) == 0) {
            /* TODO: Even state: set CB1 low, in the right modes. */
            if (shift_out) {
                /* Shift out */
                int cb2 = (via_context->via[VIA_SR] >> 7) & 1;
                via_context->via[VIA_SR] = (via_context->via[VIA_SR] << 1) | cb2;
                /* According to Rockwell figure 26, after CB1 is set low,
                 * 1 clock later CB2 is set (and presumably the shift happens).
                 * Commodore figure 11 shows first CB2 being set and ~half a clock
                 * later CB1 goes low.
                 */
            }
        } else {
            /* TODO: Odd state: set CB1 high, in the right modes. */
            if (!shift_out) {
                /* Shift in */
                /* FIXME: We should read CB2 here instead of 1, but CB2 state
                 * must not be controlled by PCR.
                 * Until the signalling function is correct with shifter active,
                 * just use 1 instead */
                via_context->via[VIA_SR] = (via_context->via[VIA_SR] << 1 ) | 1;
            }
        }

        /*
         * "(shift on CB1 rising edge should occur even in SR disabled mode). "
         * comment from https://github.com/mikestir/fpga-bbc
         * also mentioned by Rockwell but not CBM/MOS.
         */

        via_context->shift_state += 1;
        /* next shifter bit; set SR interrupt if 8 bits are complete */
        if (via_context->shift_state == FINISHED_SHIFTING) {
            /* In free running mode, the counter is "disabled" and no
             * SR interrupt flags are set.
             * We have no test case for this.
             * Nor for a case where a "disabled" counter is switched to
             * a mode with "enabled" counter.
             */
            if (IS_SR_FREE_RUNNING()) {
                via_context->shift_state = 0;
            } else {
                via_context->ifr |= VIA_IM_SR;
                update_myviairq_rclk(via_context, rclk);

                if (via_context->sr_underflow) {
                    via_context->sr_underflow(via_context);
                }
            }
        }
    }
}

/* alarm callback for the case when the shift register is under phi2 control */
static void viacore_phi2_sr_alarm(CLOCK offset, void *data)
{
    CLOCK rclk;
    via_context_t *via_context = (via_context_t *)data;
    rclk = *(via_context->clk_ptr) - offset;
#if CHECK_CPU_VS_ALARM_CLOCKS
    last_alarm_clock = rclk;
    if (last_cpu_clock == last_alarm_clock) {
        VIALOG9("alarm after cpu access %lu (expected)\n", rclk);
    }
#endif /* CHECK_CPU_VS_ALARM_CLOCKS */
    VIALOG2("viacore_phi2_sr_alarm: offset %ld rclk %lu SR %02x shift_state %d\n", (int64_t)offset, rclk, via_context->via[VIA_SR], via_context->shift_state);
    do_shiftregister(offset, data);
    /*
     * Possible optimization: don't re-arm the alarm when the shifting is done.
     * Difficulty: When arming it again later, the correct start-up delay must
     * be used.
     */
    alarm_set(via_context->phi2_sr_alarm, rclk + SR_PHI2_NEXT_OFFSET);
}

void viacore_setup_context(via_context_t *via_context)
{
    int i;

    via_context->read_clk = 0;
    via_context->read_offset = 0;
    via_context->last_read = 0;
    via_context->log = LOG_ERR;

    via_context->my_module_name_alt1 = NULL;
    via_context->my_module_name_alt2 = NULL;

    via_context->write_offset = 1;
    /* assume all registers 0 at powerup */
    for (i = 0; i < 16; i++) {
        via_context->via[i] = 0;
    }
    /* timers and timer latches apparently do not contain 0 at powerup */
    via_context->via[4] = via_context->via[6] = 0xff;
    via_context->via[5] = via_context->via[7] = 223;  /* my vic20 gives 223 here (gpz) */
    via_context->via[8] = 0xff;
    via_context->via[9] = 0xff;

    via_context->sr_underflow = NULL;
    via_context->t2_irq_allowed = 0;
}

void viacore_init(via_context_t *via_context, alarm_context_t *alarm_context,
                  interrupt_cpu_status_t *int_status)
{
    char *buffer;

    if (via_context->log == LOG_ERR) {
        via_context->log = log_open(via_context->my_module_name);
    }

    via_context->alarm_context = alarm_context;

    buffer = lib_msprintf("%sT1", via_context->myname);
    via_context->t1_alarm = alarm_new(alarm_context, buffer, viacore_intt1, via_context);
    lib_free(buffer);

    buffer = lib_msprintf("%sT2zero", via_context->myname);
    via_context->t2_zero_alarm = alarm_new(alarm_context, buffer, viacore_t2_zero_alarm, via_context);
    lib_free(buffer);

    buffer = lib_msprintf("%sT2uflow", via_context->myname);
    via_context->t2_underflow_alarm = alarm_new(alarm_context, buffer, viacore_t2_underflow_alarm, via_context);
    lib_free(buffer);

    buffer = lib_msprintf("%sT2SR", via_context->myname);
    via_context->t2_shift_alarm = alarm_new(alarm_context, buffer, viacore_t2_shift_alarm, via_context);
    lib_free(buffer);

    buffer = lib_msprintf("%sSR", via_context->myname);
    via_context->phi2_sr_alarm = alarm_new(alarm_context, buffer, viacore_phi2_sr_alarm, via_context);
    lib_free(buffer);

    via_context->int_num = interrupt_cpu_status_int_new(int_status, via_context->myname);
}

void viacore_shutdown(via_context_t *via_context)
{
    lib_free(via_context->prv);
    lib_free(via_context->myname);
    lib_free(via_context->my_module_name);
    lib_free(via_context->my_module_name_alt1);
    lib_free(via_context->my_module_name_alt2);
    lib_free(via_context);
}

/*------------------------------------------------------------------------*/

/* The name of the modul must be defined before including this file.  */
#define VIA_DUMP_VER_MAJOR      2
#define VIA_DUMP_VER_MINOR      2

/*
 * The dump data:
 *
 * Minor version 1:
 *
 * UBYTE        ORA
 * UBYTE        DDRA
 * UBYTE        ORB
 * UBYTE        DDRB
 * UWORD        T1L             word1   via_context->tal
 * UWORD        T1C             word2   myviata()
 * UBYTE        T2LL
 * UBYTE        T2LH
 * UBYTE        T2CL
 * UBYTE        T2CH
 * UWORD        T2C             word3   myviatb()
 * UBYTE        0x80:tai | 0x40:tbi     byte1
 * UBYTE        SR              
 * UBYTE        ACR
 * UBYTE        PCR
 * UBYTE        IFR              active interrupts
 * UBYTE        IER              interrupt masks
 * UBYTE        PB7              bit 7 = pb7 state
 * UBYTE        SRHBITS          shift register state helper
 * UBYTE        CABSTATE         bit 7 = ca2 state, bi 6 = cb2 state
 * UBYTE        ILA              input latch port A
 * UBYTE        ILB              input latch port B
 *
 * Minor version 2 adds:
 *
 * UBYTE        t2_irq_allowed
 * UBYTE        t2_underflow_alarm 0 if not; 1+time delta if set.
 * UBYTE        t2_shift_alarm   0 if not; 1+time delta if set.
 */

/* FIXME!!!  Error check.  */

int viacore_snapshot_write_module(via_context_t *via_context, snapshot_t *s)
{
    snapshot_module_t *m;
    CLOCK rclk = *(via_context->clk_ptr);

#if 0
    if (via_context->tai && (via_context->tai <= *(via_context->clk_ptr))) {
        viacore_intt1(*(via_context->clk_ptr) - via_context->tai,
                      (void *)via_context);
    }
    if (via_context->tbi && (via_context->tbi <= *(via_context->clk_ptr))) {
        viacore_t2_zero_alarm(*(via_context->clk_ptr) - via_context->tbi,
                      (void *)via_context);
    }
#else
    run_pending_alarms(rclk, via_context->alarm_context);
#endif

    m = snapshot_module_create(s, via_context->my_module_name, VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, via_context->via[VIA_PRA]) < 0
        || SMW_B(m, via_context->via[VIA_DDRA]) < 0
        || SMW_B(m, via_context->via[VIA_PRB]) < 0
        || SMW_B(m, via_context->via[VIA_DDRB]) < 0
        || SMW_W(m, (uint16_t)(via_context->tal)) < 0
        || SMW_W(m, (uint16_t)myviata(via_context)) < 0
        || SMW_B(m, via_context->via[VIA_T2LL]) < 0
        || SMW_B(m, via_context->via[VIA_T2LH]) < 0
        || SMW_B(m, via_context->t2cl) < 0
        || SMW_B(m, via_context->t2ch) < 0
        || SMW_W(m, (uint16_t)myviatb(via_context, *(via_context->clk_ptr))) < 0
        || SMW_B(m, (uint8_t)((via_context->tai ? 0x80 : 0) | (via_context->tbi ? 0x40 : 0))) < 0
        || SMW_B(m, via_context->via[VIA_SR]) < 0
        || SMW_B(m, via_context->via[VIA_ACR]) < 0
        || SMW_B(m, via_context->via[VIA_PCR]) < 0
        || SMW_B(m, (uint8_t)(via_context->ifr)) < 0
        || SMW_B(m, (uint8_t)(via_context->ier)) < 0
        /* FIXME! */
        || SMW_B(m, (uint8_t)((((via_context->pb7 ^ via_context->pb7x) | via_context->pb7o) ? 0x80 : 0))) < 0
        /* SRHBITS */
        || SMW_B(m, (uint8_t)via_context->shift_state) < 0
        || SMW_B(m, (uint8_t)((via_context->ca2_state ? 0x80 : 0) | (via_context->cb2_state ? 0x40 : 0))) < 0
        || SMW_B(m, via_context->ila) < 0
        || SMW_B(m, via_context->ilb) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    /* Add stuff for minor version 2 */
    uint8_t m2_t2_underflow_alarm, m2_t2_shift_alarm;
    CLOCK tmpclock;

    tmpclock = alarm_clk(via_context->t2_underflow_alarm);
    m2_t2_underflow_alarm = tmpclock ? 1 + tmpclock - rclk
                                     : 0;
    tmpclock = alarm_clk(via_context->t2_shift_alarm);
    m2_t2_shift_alarm = tmpclock ? 1 + tmpclock - rclk
                                 : 0;

    if (0
        || SMW_B(m, via_context->t2_irq_allowed) < 0
        || SMW_B(m, m2_t2_underflow_alarm) < 0
        || SMW_B(m, m2_t2_shift_alarm) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    return snapshot_module_close(m);
}

int viacore_snapshot_read_module(via_context_t *via_context, snapshot_t *s)
{
    uint8_t vmajor, vminor;
    uint8_t byte;
    uint8_t byte1, byte2, byte3, byte4, byte5, byte6;
    uint16_t word1, word2, word3;
    uint16_t addr;
    uint8_t m2_t2_irq_allowed, m2_t2_underflow_alarm, m2_t2_shift_alarm;
    CLOCK rclk = *(via_context->clk_ptr);
    snapshot_module_t *m;

    m = snapshot_module_open(s, via_context->my_module_name, &vmajor, &vminor);

    if (m == NULL) {
        if (via_context->my_module_name_alt1 == NULL) {
            return -1;
        }

        m = snapshot_module_open(s, via_context->my_module_name_alt1,
                                 &vmajor, &vminor);
        if (m == NULL) {
            if (via_context->my_module_name_alt2 == NULL) {
                return -1;
            }

            m = snapshot_module_open(s, via_context->my_module_name_alt2,
                                     &vmajor, &vminor);
            if (m == NULL) {
                return -1;
            }
        }
    }

    /* if major version does not match, the snapshot is not compatible */
    if (vmajor != VIA_DUMP_VER_MAJOR) {
        snapshot_set_error(SNAPSHOT_MODULE_INCOMPATIBLE);
        snapshot_module_close(m);
        return -1;
    }
    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(vmajor, vminor, VIA_DUMP_VER_MAJOR, VIA_DUMP_VER_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        snapshot_module_close(m);
        return -1;
    }

    alarm_unset(via_context->t1_alarm);
    alarm_unset(via_context->t2_zero_alarm);
    alarm_unset(via_context->t2_underflow_alarm);
    alarm_unset(via_context->t2_shift_alarm);       /* TODO: load from snapshot */
    alarm_unset(via_context->phi2_sr_alarm);

    via_context->tai = 0;
    via_context->tbi = 0;

    if (0
        || SMR_B(m, &(via_context->via[VIA_PRA])) < 0
        || SMR_B(m, &(via_context->via[VIA_DDRA])) < 0
        || SMR_B(m, &(via_context->via[VIA_PRB])) < 0
        || SMR_B(m, &(via_context->via[VIA_DDRB])) < 0
        || SMR_W(m, &word1) < 0
        || SMR_W(m, &word2) < 0
        || SMR_B(m, &(via_context->via[VIA_T2LL])) < 0
        || SMR_B(m, &(via_context->via[VIA_T2LH])) < 0
        || SMR_B(m, &(via_context->t2cl)) < 0
        || SMR_B(m, &(via_context->t2ch)) < 0
        || SMR_W(m, &word3) < 0
        || SMR_B(m, &byte1) < 0
        || SMR_B(m, &(via_context->via[VIA_SR])) < 0
        || SMR_B(m, &(via_context->via[VIA_ACR])) < 0
        || SMR_B(m, &(via_context->via[VIA_PCR])) < 0
        || SMR_B(m, &byte2) < 0
        || SMR_B(m, &byte3) < 0
        || SMR_B(m, &byte4) < 0
        /* SRHBITS */
        || SMR_B(m, &byte5) < 0
        /* CABSTATE */
        || SMR_B(m, &byte6) < 0
        || SMR_B(m, &(via_context->ila)) < 0
        || SMR_B(m, &(via_context->ilb)) < 0) {
        snapshot_module_close(m);
        return -1;
    }
    /* Read minor version 2 data */
    if (0
        || SMR_B(m, &m2_t2_irq_allowed) < 0
        || SMR_B(m, &m2_t2_underflow_alarm) < 0
        || SMR_B(m, &m2_t2_shift_alarm) < 0) {
        /* Set defaults. This will be some level of imperfect state restoration */
        m2_t2_irq_allowed = 1;
        m2_t2_underflow_alarm = 0;
        m2_t2_shift_alarm = 0;
    }

    addr = VIA_DDRA;
    byte = via_context->via[VIA_PRA] | ~(via_context->via[VIA_DDRA]);
    (via_context->undump_pra)(via_context, byte);
    via_context->oldpa = byte;

    addr = VIA_DDRB;
    byte = via_context->via[VIA_PRB] | ~(via_context->via[VIA_DDRB]);
    (via_context->undump_prb)(via_context, byte);
    via_context->oldpb = byte;

    via_context->tal = word1;
    via_context->via[VIA_T1LL] = via_context->tal & 0xff;
    via_context->via[VIA_T1LH] = (via_context->tal >> 8) & 0xff;

    via_context->tau = rclk + word2 + 2 /* 3 */ + TAUOFFSET;
    via_context->tai = rclk + word2 + 1;

    /* word3 is the effective value of T2 */
    /* I think tbi and t2zero are set wrong; should probably use word3 & 0xFF */
    via_context->t2zero = rclk + (word3 & 0xFF);
    via_context->tbi = via_context->t2zero + TBI_OFFSET;

    if (byte1 & 0x80) {
        alarm_set(via_context->t1_alarm, via_context->tai);
    } else {
        via_context->tai = 0;
    }
    if ((byte1 & 0x40) ||
        ((via_context->via[VIA_ACR] & 0x1c) == 0x04) ||
        ((via_context->via[VIA_ACR] & 0x1c) == 0x10) ||
        ((via_context->via[VIA_ACR] & 0x1c) == 0x14)){
        alarm_set(via_context->t2_zero_alarm, via_context->t2zero);
    } else {
        via_context->t2zero = rclk + word3;
        via_context->tbi = 0;
    }
    /* FIXME: SR alarm */
    if ((via_context->via[VIA_ACR] & 0x0c) == 0x08) {
        alarm_set(via_context->phi2_sr_alarm, rclk + 1);
    }

    via_context->ifr = byte2;
    via_context->ier = byte3;

    via_restore_int(via_context, via_context->ifr & via_context->ier & 0x7f);

    /* FIXME! */
    via_context->pb7 = byte4 ? 1 : 0;
    via_context->pb7x = 0;
    via_context->pb7o = 0;
    via_context->shift_state = byte5;

    via_context->ca2_state = byte6 & 0x80;
    via_context->cb2_state = byte6 & 0x40;

    via_context->t2_irq_allowed = m2_t2_irq_allowed;

    if (m2_t2_underflow_alarm) {
        alarm_set(via_context->t2_underflow_alarm, rclk + m2_t2_underflow_alarm - 1);
    }

    if (m2_t2_shift_alarm) {
        alarm_set(via_context->t2_shift_alarm, rclk + m2_t2_shift_alarm - 1);
    }

    /* undump_pcr also restores the ca2_state/cb2_state effects if necessary;
       i.e. calls set_c*2(c*2_state) if necessary */
    addr = VIA_PCR;
    byte = via_context->via[addr];
    (via_context->undump_pcr)(via_context, byte);

    addr = VIA_SR;
    byte = via_context->via[addr];
    (via_context->store_sr)(via_context, byte);

    addr = VIA_ACR;
    byte = via_context->via[addr];
    (via_context->undump_acr)(via_context, byte);

    return snapshot_module_close(m);
}

int viacore_dump(via_context_t *via_context)
{
    mon_out("Port A: %02x DDR: %02x no HS: %02x\n",
            viacore_peek(via_context, VIA_PRA), viacore_peek(via_context, VIA_DDRA), viacore_peek(via_context, VIA_PRA_NHS));
    mon_out("Port B: %02x DDR: %02x\n", viacore_peek(via_context, VIA_PRB), viacore_peek(via_context, VIA_DDRB));
    mon_out("Timer 1: %04x Latch: %04x\n", viacore_peek(via_context, VIA_T1CL) + (viacore_peek(via_context, VIA_T1CH) * 256U),
            viacore_peek(via_context, VIA_T1LL) + (viacore_peek(via_context, VIA_T1LH) * 256U));
    mon_out("Timer 2: %04x Latch:   %02x t2_zero_alarm: +%llu (idx %d)\n",
            viacore_peek(via_context, VIA_T2CL) + (viacore_peek(via_context, VIA_T2CH) * 256U),
            via_context->via[VIA_T2LL],
            alarm_clk(via_context->t2_zero_alarm) - *(via_context->clk_ptr),
            via_context->t2_zero_alarm->pending_idx);
    mon_out("Aux. control: %02x\n", viacore_peek(via_context, VIA_ACR));
    mon_out("Per. control: %02x\n", viacore_peek(via_context, VIA_PCR));
    mon_out("IRQ flags: %02x\n", viacore_peek(via_context, VIA_IFR));
    mon_out("IRQ enable: %02x\n", viacore_peek(via_context, VIA_IER));
    mon_out("\nShift Register: %02x (%s, shifting %s, count=%d)\n",
            viacore_peek(via_context, VIA_SR), 
            ((via_context->via[VIA_ACR] & 0x1c) == 0) ? "disabled" : "enabled",
            (via_context->via[VIA_ACR] & 0x10) ? "out" : "in",
            via_context->shift_state);
    mon_out("tbi: %llu (clock+%llu),  t2zero: %llu (clock+%llu)\n",
            via_context->tbi,
            via_context->tbi - *(via_context->clk_ptr),
            via_context->t2zero,
            via_context->t2zero - *(via_context->clk_ptr));
    if (alarm_is_pending(via_context->t2_underflow_alarm)) {
        mon_out("t2_underflow_alarm: %llu (clock+%llu)\n",
                alarm_clk(via_context->t2_underflow_alarm),
                alarm_clk(via_context->t2_underflow_alarm) - *(via_context->clk_ptr));
    }
    if (alarm_is_pending(via_context->t2_underflow_alarm)) {
        mon_out("t2_shift_alarm: %llu (clock+%llu)\n",
                alarm_clk(via_context->t2_shift_alarm),
                alarm_clk(via_context->t2_shift_alarm) - *(via_context->clk_ptr));
    }
    if (alarm_is_pending(via_context->phi2_sr_alarm)) {
        mon_out("phi2_sr_alarm: %llu (clock+%llu)\n",
                alarm_clk(via_context->phi2_sr_alarm),
                alarm_clk(via_context->phi2_sr_alarm) - *(via_context->clk_ptr));
    }

    return 0;
}

