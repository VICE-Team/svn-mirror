/*
 * cia.h - Definitions for MOS6526 (CIA) chip emulation.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#ifndef _CIA_H
#define _CIA_H

#include "ciatimer.h"
#include "types.h"

#define byte2bcd(byte)  (((((byte) / 10) << 4) + ((byte) % 10)) & 0xff)
#define bcd2byte(bcd)   (((10*(((bcd) & 0xf0) >> 4)) + ((bcd) & 0xf)) & 0xff)

/* MOS 6526 registers */
#define CIA_PRA         0  /* Port A */
#define CIA_PRB         1  /* Port B */
#define CIA_DDRA        2  /* Data direction register for port A */
#define CIA_DDRB        3  /* Data direction register for port B */

#define CIA_TAL         4  /* Timer A low */
#define CIA_TAH         5  /* Timer A high */
#define CIA_TBL         6  /* Timer B low */
#define CIA_TBH         7  /* Timer B high */

#define CIA_TOD_TEN     8  /* Time Of Day clock 1/10 s */
#define CIA_TOD_SEC     9  /* Time Of Day clock sec */
#define CIA_TOD_MIN     10 /* Time Of Day clock min */
#define CIA_TOD_HR      11 /* Time Of Day clock hour */

#define CIA_SDR         12 /* Serial port shift register */
#define CIA_ICR         13 /* Interrupt control register */
#define CIA_CRA         14 /* Control register A */
#define CIA_CRB         15 /* Control register B */


struct alarm_context_s;
struct cia_context_s;
struct clk_guard_s;
struct interrupt_cpu_status_s;
struct snapshot_s;

/* Interrupt Masks */
#define CIA_IM_SET      0x80    /* Control Bit */
#define CIA_IM_TA       1       /* Timer A underflow */
#define CIA_IM_TB       2       /* Timer B underflow */
#define CIA_IM_TOD      4       /* TOD Clock Alarm */
#define CIA_IM_SDR      8       /* Shift Register completion */
#define CIA_IM_FLG      16      /* Handshake */


typedef struct cia_context_s {
    BYTE c_cia[16];
    struct alarm_s *ta_alarm;
    struct alarm_s *tb_alarm;
    struct alarm_s *tod_alarm;
    int irqflags;
    BYTE irq_enabled;
    CLOCK rdi;
    unsigned int tat;
    unsigned int tbt;
    CLOCK todclk;
    unsigned int sr_bits;
    int sdr_valid;
    BYTE shifter;
    BYTE old_pa;
    BYTE old_pb;
    char todstopped;
    char todlatched;
    BYTE todalarm[4];
    BYTE todlatch[4];
    int todticks;                 /* init to 100000 */
    signed int log;               /* init to LOG_ERR */

    ciat_t ta;
    ciat_t tb;
    CLOCK read_clk;               /* init to 0 */
    int read_offset;              /* init to 0 */
    BYTE last_read;               /* init to 0 */
    int debugFlag;                /* init to 0 */

    int irq_line;                 /* IK_IRQ */
    unsigned int int_num;

    char myname[12];

    CLOCK *clk_ptr;
    int *rmw_flag;

    void *prv;
    void *context;

    void (*undump_ciapa)(struct cia_context_s *, CLOCK, BYTE);
    void (*undump_ciapb)(struct cia_context_s *, CLOCK, BYTE);
    void (*store_ciapa)(struct cia_context_s *, CLOCK, BYTE);
    void (*store_ciapb)(struct cia_context_s *, CLOCK, BYTE);
    void (*store_sdr)(struct cia_context_s *, BYTE);
    BYTE (*read_ciapa)(struct cia_context_s *);
    BYTE (*read_ciapb)(struct cia_context_s *);
    void (*read_ciaicr)(struct cia_context_s *);
    void (*read_sdr)(struct cia_context_s *);
    void (*cia_set_int_clk)(struct cia_context_s *, int, CLOCK);
    void (*cia_restore_int)(struct cia_context_s *, int);
    void (*do_reset_cia)(struct cia_context_s *);
    void (*pulse_ciapc)(struct cia_context_s *, CLOCK);
    void (*pre_store)(void);
    void (*pre_read)(void);
    void (*pre_peek)(void);
} cia_context_t;

typedef struct cia_initdesc_s {
    struct cia_context_s *cia_ptr;
    void (*int_ta)(CLOCK);
    void (*int_tb)(CLOCK);
    void (*int_tod)(CLOCK);
} cia_initdesc_t;

extern void ciacore_setup_context(struct cia_context_s *cia_context);
extern void ciacore_init(const cia_initdesc_t *cd,
                         struct alarm_context_s *alarm_context,
                         struct interrupt_cpu_status_s *int_status,
                         struct clk_guard_s *clk_guard);
extern void ciacore_shutdown(cia_context_t *cia_context);
extern void ciacore_reset(struct cia_context_s *cia_context);
extern void REGPARM2 ciacore_store(struct cia_context_s *cia_context,
                                   WORD addr, BYTE data);
extern BYTE REGPARM1 ciacore_read(struct cia_context_s *cia_context,
                                  WORD addr);
extern BYTE REGPARM1 ciacore_peek(struct cia_context_s *cia_context,
                                  WORD addr);

extern void ciacore_intta(struct cia_context_s *cia_context, CLOCK offset);
extern void ciacore_inttb(struct cia_context_s *cia_context, CLOCK offset);
extern void ciacore_inttod(struct cia_context_s *cia_context, CLOCK offset);

extern void ciacore_set_flag(struct cia_context_s *cia_context);
extern void ciacore_set_sdr(struct cia_context_s *cia_context, BYTE data);

extern int ciacore_snapshot_write_module(struct cia_context_s *cia_context,
                                         struct snapshot_s *s);
extern int ciacore_snapshot_read_module(struct cia_context_s *cia_context,
                                        struct snapshot_s *s);

#endif

