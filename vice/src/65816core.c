/*
 * 65816core.c - 65816/65802 emulation core.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Kajtar Zsolt <soci@c64.rulez.org>
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

/* This file is currently not included by any CPU definition files */

/* any CPU definition file that includes this file needs to do the following:
 *
 * - define reg_c as 16bit (8bit on 6502/65C02).
 * - define reg_x as 16bit (8bit on 6502/65C02).
 * - define reg_y as 16bit (8bit on 6502/65C02).
 * - define reg_pbr (Program Bank Register) as 8bit.
 * - define reg_dbr (Data Bank Register) as 8bit.
 * - define reg_dpr (Direct Page Register) as 16bit.
 * - define reg_emul (65C02 Emulation) as int.
 * - define reg_sp as 16bit (8bit on 6502/65C02).
 * - define reg_p as 8bit.
 * - define reg_pc as 16bit.
 * - define a function to handle the STP opcode (STP_65816(void)).
 * - define a function to handle the WAI opcode (WAI_65816(void)).
 * - define a function to handle the COP opcode (COP_65816(BYTE value)).
 *
 * reg_a and reg_b combined is reg_c.

 * the way to define the a, b and c regs is:
 * union regs {
 *     WORD reg_s;
 *     BYTE reg_c[2];
 * } regs65816;
 *
 * #define reg_c regs65816.reg_s
 * #ifndef WORDS_BIGENDIAN
 * #define reg_a regs65816.reg_c[0]
 * #define reg_b regs65816.reg_c[1]
 * #else
 * #define reg_a regs65816.reg_c[1]
 * #define reg_b regs65816.reg_c[0]
 * #endif
 */

#ifndef CPU_STR
#define CPU_STR "65816/65802 CPU"
#endif

#include "traps.h"

/* To avoid 'magic' numbers, the following defines are used. */

#define CYCLES_0   0
#define CYCLES_1   1
#define CYCLES_2   2
#define CYCLES_3   3
#define CYCLES_4   4
#define CYCLES_5   5
#define CYCLES_6   6
#define CYCLES_7   7

#define SIZE_1   1
#define SIZE_2   2
#define SIZE_3   3
#define SIZE_4   4

#define BITS8    1
#define BITS16   0

#define RESET_CYCLES    6

/* ------------------------------------------------------------------------- */
/* Backup for non-variable cycle CPUs.  */

#ifndef CLK_ADD
#define CLK_ADD(clock, amount) clock += amount
#endif

/* ------------------------------------------------------------------------- */
/* Hook for additional delay.  */

#ifndef CPU_DELAY_CLK
#define CPU_DELAY_CLK
#endif

#ifndef CPU_REFRESH_CLK
#define CPU_REFRESH_CLK
#endif

#ifndef EMULATION_MODE_CHANGED
#define EMULATION_MODE_CHANGED
#endif

#ifndef WAI_65816 
#define WAI_65816() INC_PC(SIZE_1)
#endif

#ifndef STP_65816 
#define STP_65816() INC_PC(SIZE_1)
#endif

#ifndef COP_65816 
#define COP_65816(value) INC_PC(SIZE_1)
#endif


#ifndef FETCH_PARAM_DUMMY
#define FETCH_PARAM_DUMMY(addr) FETCH_PARAM(addr)
#endif

#ifndef LOAD_LONG_DUMMY
#define LOAD_LONG_DUMMY(addr) LOAD_LONG(addr)
#endif

#ifndef CYCLE_EXACT_ALARM
#define PROCESS_ALARMS                                             \
    while (CLK >= alarm_context_next_pending_clk(ALARM_CONTEXT)) { \
        alarm_context_dispatch(ALARM_CONTEXT, CLK);                \
        CPU_DELAY_CLK                                              \
    }
#else
#define PROCESS_ALARMS
#endif
/* ------------------------------------------------------------------------- */
/* Hook for interrupt address manipulation.  */

#ifndef LOAD_INT_ADDR
#define LOAD_INT_ADDR(addr)                 \
    do {                                    \
        reg_pc = LOAD_LONG(addr);           \
        reg_pc |= LOAD_LONG(addr + 1) << 8; \
    } while (0)
#endif

/* ------------------------------------------------------------------------- */

#define LOCAL_SET_NZ(val, bits8)           \
    do {                                   \
        if (!bits8) {                      \
            flag_n = (val >> 8);           \
            flag_z = (val) | flag_n;       \
        } else {                           \
            flag_z = flag_n = (val);       \
        }                                  \
    } while (0)

#define LOCAL_SET_OVERFLOW(val)   \
    do {                          \
        if (val)                  \
            reg_p |= P_OVERFLOW;  \
        else                      \
            reg_p &= ~P_OVERFLOW; \
    } while (0)

#define LOCAL_SET_BREAK(val)   \
    do {                       \
        if (val)               \
            reg_p |= P_BREAK;  \
        else                   \
            reg_p &= ~P_BREAK; \
    } while (0)

#define LOCAL_SET_DECIMAL(val)   \
    do {                         \
        if (val)                 \
            reg_p |= P_DECIMAL;  \
        else                     \
            reg_p &= ~P_DECIMAL; \
    } while (0)

#define LOCAL_SET_INTERRUPT(val)   \
    do {                           \
        if (val)                   \
            reg_p |= P_INTERRUPT;  \
        else                       \
            reg_p &= ~P_INTERRUPT; \
    } while (0)

#define LOCAL_SET_CARRY(val)   \
    do {                       \
        if (val)               \
            reg_p |= P_CARRY;  \
        else                   \
            reg_p &= ~P_CARRY; \
    } while (0)

#define LOCAL_SET_65816_M(val)   \
    do {                         \
        if (val)                 \
            reg_p |= P_65816_M;  \
        else                     \
            reg_p &= ~P_65816_M; \
    } while (0)

#define LOCAL_SET_65816_X(val)   \
    do {                         \
        if (val)                 \
            reg_p |= P_65816_X;  \
        else                     \
            reg_p &= ~P_65816_X; \
    } while (0)

#define LOCAL_SET_SIGN(val)      (flag_n = (val) ? 0x80 : 0)
#define LOCAL_SET_ZERO(val)      (flag_z = !(val))
#define LOCAL_SET_STATUS(val)    (reg_p = ((val) & ~(P_ZERO | P_SIGN)), \
                                  LOCAL_SET_ZERO((val) & P_ZERO),       \
                                  flag_n = (val))

#define LOCAL_OVERFLOW()         (reg_p & P_OVERFLOW)
#define LOCAL_BREAK()            (reg_p & P_BREAK)
#define LOCAL_DECIMAL()          (reg_p & P_DECIMAL)
#define LOCAL_INTERRUPT()        (reg_p & P_INTERRUPT)
#define LOCAL_CARRY()            (reg_p & P_CARRY)
#define LOCAL_SIGN()             (flag_n & 0x80)
#define LOCAL_ZERO()             (!flag_z)
#define LOCAL_STATUS()           (reg_p | (flag_n & 0x80) | P_UNUSED    \
                                  | (LOCAL_ZERO() ? P_ZERO : 0))

#define LOCAL_65816_M()          (reg_emul || (reg_p & P_65816_M))
#define LOCAL_65816_X()          (reg_emul || (reg_p & P_65816_X))

#define LOCAL_65816_STATUS()     (reg_p | (flag_n & 0x80) | (LOCAL_ZERO() ? P_ZERO : 0))

#ifdef LAST_OPCODE_INFO

/* If requested, gather some info about the last executed opcode for timing
   purposes.  */

/* Remember the number of the last opcode.  By default, the opcode does not
   delay interrupt and does not change the I flag.  */
#define SET_LAST_OPCODE(x) \
    OPINFO_SET(LAST_OPCODE_INFO, (x), 0, 0, 0)

/* Remember that the last opcode changed the I flag from 0 to 1, so we have
   to dispatch an IRQ even if the I flag is 0 when we check it.  */
#define OPCODE_DISABLES_IRQ() \
    OPINFO_SET_DISABLES_IRQ(LAST_OPCODE_INFO, 1)

/* Remember that the last opcode changed the I flag from 1 to 0, so we must
   not dispatch an IRQ even if the I flag is 1 when we check it.  */
#define OPCODE_ENABLES_IRQ() \
    OPINFO_SET_ENABLES_IRQ(LAST_OPCODE_INFO, 1)

#else

/* Info about the last opcode is not needed.  */
#define SET_LAST_OPCODE(x)
#define OPCODE_DISABLES_IRQ()
#define OPCODE_ENABLES_IRQ()

#endif

#ifdef LAST_OPCODE_ADDR
#define SET_LAST_ADDR(x) LAST_OPCODE_ADDR = (x)
#else
#error "please define LAST_OPCODE_ADDR"
#endif

/* Export the local version of the registers.  */
#define EXPORT_REGISTERS()         \
  do {                             \
      GLOBAL_REGS.pc = reg_pc;     \
      GLOBAL_REGS.a = reg_a;       \
      GLOBAL_REGS.b = reg_b;       \
      GLOBAL_REGS.x = reg_x;       \
      GLOBAL_REGS.y = reg_y;       \
      GLOBAL_REGS.emul = reg_emul; \
      GLOBAL_REGS.dpr = reg_dpr;   \
      GLOBAL_REGS.pbr = reg_pbr;   \
      GLOBAL_REGS.dbr = reg_dbr;   \
      GLOBAL_REGS.sp = reg_sp;     \
      GLOBAL_REGS.p = reg_p;       \
      GLOBAL_REGS.n = flag_n;      \
      GLOBAL_REGS.z = flag_z;      \
  } while (0)

/* Import the public version of the registers.  */
#define IMPORT_REGISTERS()                               \
  do {                                                   \
      reg_a = GLOBAL_REGS.a;                             \
      reg_b = GLOBAL_REGS.b;                             \
      reg_x = GLOBAL_REGS.x;                             \
      reg_y = GLOBAL_REGS.y;                             \
      reg_emul = GLOBAL_REGS.emul;                       \
      reg_dpr = GLOBAL_REGS.dpr;                         \
      reg_pbr = GLOBAL_REGS.pbr;                         \
      reg_dbr = GLOBAL_REGS.dbr;                         \
      reg_sp = GLOBAL_REGS.sp;                           \
      reg_p = GLOBAL_REGS.p;                             \
      flag_n = GLOBAL_REGS.n;                            \
      flag_z = GLOBAL_REGS.z;                            \
      if (reg_emul) { /* fixup emulation mode */         \
          reg_x &= 0xff;                                 \
          reg_y &= 0xff;                                 \
          reg_sp = 0x100 | (reg_sp & 0xff);              \
      }                                                  \
      bank_start = bank_limit = 0; /* prevent caching */ \
      EMULATION_MODE_CHANGED;                            \
      JUMP(GLOBAL_REGS.pc);                              \
  } while (0)

/* Stack operations. */

#ifndef PUSH
#define PUSH(val)                                             \
  do {                                                        \
      STORE_LONG(reg_sp, val);                                \
      if (reg_emul) {                                         \
          reg_sp = 0x100 | ((reg_sp - 1) & 0xff);             \
      } else {                                                \
          reg_sp--;                                           \
      }                                                       \
  } while (0)
#endif

#ifndef PULL
#define PULL() ((reg_sp = (reg_emul) ? 0x100 | ((reg_sp + 1) & 0xff) : reg_sp + 1), LOAD_LONG(reg_sp))
#endif

#ifdef DEBUG
#define TRACE_NMI() \
    do { if (TRACEFLG) debug_nmi(CPU_INT_STATUS, CLK); } while (0)
#define TRACE_IRQ() \
    do { if (TRACEFLG) debug_irq(CPU_INT_STATUS, CLK); } while (0)
#define TRACE_BRK() do { if (TRACEFLG) debug_text("*** BRK"); } while (0)
#define TRACE_COP() do { if (TRACEFLG) debug_text("*** COP"); } while (0)
#else
#define TRACE_NMI()
#define TRACE_IRQ()
#define TRACE_BRK()
#define TRACE_COP()
#endif

/* Perform the interrupts in `int_kind'.  If we have both NMI and IRQ,
   execute NMI.  */
#define DO_INTERRUPT(int_kind)                                                 \
    do {                                                                       \
        BYTE ik = (int_kind);                                                  \
                                                                               \
        if (ik & (IK_IRQ | IK_IRQPEND | IK_NMI)) {                             \
            if ((ik & IK_NMI)                                                  \
                && interrupt_check_nmi_delay(CPU_INT_STATUS, CLK)) {           \
                TRACE_NMI();                                                   \
                if (monitor_mask[CALLER] & (MI_STEP)) {                        \
                    monitor_check_icount_interrupt();                          \
                }                                                              \
                interrupt_ack_nmi(CPU_INT_STATUS);                             \
                FETCH_PARAM(reg_pc);                                           \
                FETCH_PARAM_DUMMY(reg_pc);                                     \
                if (reg_emul) {                                                \
                    LOCAL_SET_BREAK(0);                                        \
                    PUSH(reg_pc >> 8);                                         \
                    PUSH(reg_pc);                                              \
                    PUSH(LOCAL_STATUS());                                      \
                    LOAD_INT_ADDR(0xfffa);                                     \
                } else {                                                       \
                    PUSH(reg_pbr);                                             \
                    PUSH(reg_pc >> 8);                                         \
                    PUSH(reg_pc);                                              \
                    PUSH(LOCAL_65816_STATUS());                                \
                    LOAD_INT_ADDR(0xffea);                                     \
                }                                                              \
                LOCAL_SET_DECIMAL(0);                                          \
                LOCAL_SET_INTERRUPT(1);                                        \
                reg_pbr = 0;                                                   \
                JUMP(reg_pc);                                                  \
                SET_LAST_OPCODE(0);                                            \
            } else if ((ik & (IK_IRQ | IK_IRQPEND))                            \
                       && (!LOCAL_INTERRUPT()                                  \
                       || OPINFO_DISABLES_IRQ(LAST_OPCODE_INFO))               \
                       && interrupt_check_irq_delay(CPU_INT_STATUS,            \
                                                    CLK)) {                    \
                TRACE_IRQ();                                                   \
                if (monitor_mask[CALLER] & (MI_STEP)) {                        \
                    monitor_check_icount_interrupt();                          \
                }                                                              \
                interrupt_ack_irq(CPU_INT_STATUS);                             \
                FETCH_PARAM(reg_pc);                                           \
                FETCH_PARAM_DUMMY(reg_pc);                                     \
                if (reg_emul) {                                                \
                    LOCAL_SET_BREAK(0);                                        \
                    PUSH(reg_pc >> 8);                                         \
                    PUSH(reg_pc);                                              \
                    PUSH(LOCAL_STATUS());                                      \
                    LOAD_INT_ADDR(0xfffe);                                     \
                } else {                                                       \
                    PUSH(reg_pbr);                                             \
                    PUSH(reg_pc >> 8);                                         \
                    PUSH(reg_pc);                                              \
                    PUSH(LOCAL_65816_STATUS());                                \
                    LOAD_INT_ADDR(0xffee);                                     \
                }                                                              \
                LOCAL_SET_INTERRUPT(1);                                        \
                LOCAL_SET_DECIMAL(0);                                          \
                reg_pbr = 0;                                                   \
                JUMP(reg_pc);                                                  \
                SET_LAST_OPCODE(0);                                            \
            }                                                                  \
        }                                                                      \
        if (ik & (IK_TRAP | IK_RESET)) {                                       \
            if (ik & IK_TRAP) {                                                \
                EXPORT_REGISTERS();                                            \
                interrupt_do_trap(CPU_INT_STATUS, (WORD)reg_pc);               \
                IMPORT_REGISTERS();                                            \
                if (CPU_INT_STATUS->global_pending_int & IK_RESET)             \
                    ik |= IK_RESET;                                            \
            }                                                                  \
            if (ik & IK_RESET) {                                               \
                interrupt_ack_reset(CPU_INT_STATUS);                           \
                cpu_reset();                                                   \
                bank_start = bank_limit = 0; /* prevent caching */             \
                reg_emul = 1;                                                  \
                EMULATION_MODE_CHANGED;                                        \
                LOCAL_SET_BREAK(0);                                            \
                LOCAL_SET_INTERRUPT(1);                                        \
                LOCAL_SET_DECIMAL(0);                                          \
                reg_x &= 0xff;                                                 \
                reg_y &= 0xff;                                                 \
                reg_sp = 0x100 | (reg_sp & 0xff);                              \
                reg_dpr = 0;                                                   \
                reg_dbr = 0;                                                   \
                reg_pbr = 0;                                                   \
                LOAD_INT_ADDR(0xfffc);                                         \
                JUMP(reg_pc);                                                  \
                DMA_ON_RESET;                                                  \
            }                                                                  \
        }                                                                      \
        if (ik & (IK_MONITOR | IK_DMA)) {                                      \
            if (ik & IK_MONITOR) {                                             \
                if (monitor_force_import(CALLER))                              \
                    IMPORT_REGISTERS();                                        \
                if (monitor_mask[CALLER])                                      \
                    EXPORT_REGISTERS();                                        \
                if (monitor_mask[CALLER] & (MI_BREAK)) {                       \
                    if (monitor_check_breakpoints(CALLER,                      \
                        (WORD)reg_pc)) {                                       \
                        monitor_startup(CALLER);                               \
                        IMPORT_REGISTERS();                                    \
                    }                                                          \
                }                                                              \
                if (monitor_mask[CALLER] & (MI_STEP)) {                        \
                    monitor_check_icount((WORD)reg_pc);                        \
                    IMPORT_REGISTERS();                                        \
                }                                                              \
                if (monitor_mask[CALLER] & (MI_WATCH)) {                       \
                    monitor_check_watchpoints(LAST_OPCODE_ADDR, (WORD)reg_pc); \
                    IMPORT_REGISTERS();                                        \
                }                                                              \
            }                                                                  \
            if (ik & IK_DMA) {                                                 \
                EXPORT_REGISTERS();                                            \
                DMA_FUNC;                                                      \
                interrupt_ack_dma(CPU_INT_STATUS);                             \
                IMPORT_REGISTERS();                                            \
                JUMP(reg_pc);                                                  \
            }                                                                  \
        }                                                                      \
    } while (0)

/* ------------------------------------------------------------------------- */

/* Addressing modes.  For convenience, page boundary crossing cycles and
   ``idle'' memory reads are handled here as well. */

#define LOAD_PBR(addr) \
    LOAD_LONG((addr) + (reg_pbr << 16))

#define LOAD_BANK0(addr) \
    LOAD_LONG((addr) & 0xffff)

#define DPR_DELAY \
      if (reg_dpr & 0xff) {                            \
          FETCH_PARAM_DUMMY(reg_pc);                   \
      }                                                \

#define FETCH_PARAM(addr) ((((int)(addr)) < bank_limit) ? (CLK_ADD(CLK, CYCLES_1), bank_base[addr]) : LOAD_PBR(addr))

/* s */
#define LOAD_STACK(var, bits8) \
    INC_PC(SIZE_1);            \
    FETCH_PARAM_DUMMY(reg_pc); \
    var = PULL();              \
    if (!bits8) {              \
        var |= PULL() << 8;    \
    }

/* a */
#define LOAD_ACCU_RRW(var, bits8)                      \
    if (bits8) {                                       \
        var = reg_a;                                   \
    } else {                                           \
        var = reg_c;                                   \
    }                                                  \
    INC_PC(SIZE_1);

/* # */
#define LOAD_IMMEDIATE_FUNC(var, bits8)                \
    if (bits8) {                                       \
        var = p1;                                      \
        INC_PC(SIZE_2);                                \
    } else {                                           \
        var = p2;                                      \
        INC_PC(SIZE_3);                                \
    }

/* $ff wrapping */
#define LOAD_DIRECT_PAGE_FUNC(var, bits8)              \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      DPR_DELAY                                        \
      ea = p1 + reg_dpr;                               \
      INC_PC(SIZE_1);                                  \
      var = LOAD_BANK0(ea);                            \
      if (!bits8) {                                    \
          var |= LOAD_BANK0(ea + 1) << 8;              \
      }                                                \
  } while (0)

/* $ff wrapping */
#define LOAD_DIRECT_PAGE_FUNC_RRW(var, bits8)          \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      DPR_DELAY                                        \
      ea = (p1 + reg_dpr) & 0xffff;                    \
      INC_PC(SIZE_1);                                  \
      var = LOAD_LONG(ea);                             \
      if (reg_emul) {                                  \
          STORE_LONG(ea, var);                         \
      } else {                                         \
          if (!bits8) {                                \
              ea = (ea + 1) & 0xffff;                  \
              var |= LOAD_LONG(ea) << 8;               \
          }                                            \
          LOAD_LONG_DUMMY(ea);                         \
      }

/* $ff,r wrapping */
#define LOAD_DIRECT_PAGE_R_FUNC(var, bits8, reg_r)     \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      DPR_DELAY                                        \
      FETCH_PARAM_DUMMY(reg_pc);                       \
      INC_PC(SIZE_1);                                  \
      if (reg_emul) {                                  \
          if (reg_dpr & 0xff) {                        \
              ea = p1 + reg_r + reg_dpr;               \
          } else {                                     \
              ea = ((p1 + reg_r) & 0xff) + reg_dpr;    \
          }                                            \
          var = LOAD_BANK0(ea);                        \
      } else {                                         \
          ea = p1 + reg_dpr + reg_r;                   \
          var = LOAD_BANK0(ea);                        \
          if (!bits8) {                                \
              var |= LOAD_BANK0(ea + 1) << 8;          \
          }                                            \
      }                                                \
  } while (0)

/* $ff,x */
#define LOAD_DIRECT_PAGE_X_FUNC(var, bits8) \
    LOAD_DIRECT_PAGE_R_FUNC(var, bits8, reg_x)

/* $ff,y */
#define LOAD_DIRECT_PAGE_Y_FUNC(var, bits8) \
    LOAD_DIRECT_PAGE_R_FUNC(var, bits8, reg_y)

/* $ff,x wrapping */
#define LOAD_DIRECT_PAGE_X_FUNC_RRW(var, bits8)        \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      DPR_DELAY                                        \
      FETCH_PARAM_DUMMY(reg_pc);                       \
      INC_PC(SIZE_1);                                  \
      if (reg_emul) {                                  \
          if (reg_dpr & 0xff) {                        \
              ea = (p1 + reg_x + reg_dpr) & 0xffff;    \
          } else {                                     \
              ea = ((p1 + reg_x) & 0xff) + reg_dpr;    \
          }                                            \
          var = LOAD_LONG(ea);                         \
          STORE_LONG(ea, var);                         \
      } else {                                         \
          ea = (p1 + reg_dpr + reg_x) & 0xffff;        \
          var = LOAD_LONG(ea);                         \
          if (!bits8) {                                \
              ea = (ea + 1) & 0xffff;                  \
              var |= LOAD_LONG(ea) << 8;               \
          }                                            \
          LOAD_LONG_DUMMY(ea);                         \
      }

/* ($ff) no wrapping */
#define LOAD_INDIRECT_FUNC(var, bits8)                     \
  do {                                                     \
      unsigned int ea, ea2;                                \
                                                           \
      INC_PC(SIZE_1);                                      \
      DPR_DELAY                                            \
      INC_PC(SIZE_1);                                      \
      ea2 = p1 + reg_dpr;                                  \
      ea = LOAD_BANK0(ea2);                                \
      if (!reg_emul || (reg_dpr & 0xff) || (p1 != 0xff)) { \
          ea |= LOAD_BANK0(ea2 + 1) << 8;                  \
      } else {                                             \
          ea |= LOAD_LONG(reg_dpr) << 8;                   \
      }                                                    \
      ea |= reg_dbr << 16;                                 \
      var = LOAD_LONG(ea);                                 \
      if (!bits8) {                                        \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;      \
      }                                                    \
  } while (0)
      
/* ($ff,x) no wrapping */
#define LOAD_INDIRECT_X_FUNC(var, bits8)             \
  do {                                               \
      unsigned int ea, ea2;                          \
                                                     \
      INC_PC(SIZE_1);                                \
      DPR_DELAY                                      \
      FETCH_PARAM_DUMMY(reg_pc);                     \
      INC_PC(SIZE_1);                                \
      if (!reg_emul || (reg_dpr & 0xff)) {           \
          ea2 = p1 + reg_x + reg_dpr;                \
          ea = LOAD_BANK0(ea2);                      \
          ea |= LOAD_BANK0(ea2 + 1) << 8;            \
      } else {                                       \
          ea2 = ((p1 + reg_x) & 0xff) + reg_dpr;     \
          ea = LOAD_LONG(ea2);                       \
          ea2 = ((p1 + reg_x + 1) & 0xff) + reg_dpr; \
          ea |= LOAD_LONG(ea2) << 8;                 \
      }                                              \
      ea |= reg_dbr << 16;                           \
      var = LOAD_LONG(ea);                           \
      if (!bits8) {                                  \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;\
      }                                              \
  } while (0)

/* ($ff),y no wrapping */
#define LOAD_INDIRECT_Y_FUNC(var, bits8)                                            \
  do {                                                                              \
      unsigned int ea, ea2;                                                         \
                                                                                    \
      INC_PC(SIZE_1);                                                               \
      DPR_DELAY                                                                     \
      INC_PC(SIZE_1);                                                               \
      ea2 = p1 + reg_dpr;                                                           \
      ea = LOAD_BANK0(ea2);                                                         \
      if (!reg_emul || (reg_dpr & 0xff) || (p1 != 0xff)) {                          \
          ea |= LOAD_BANK0(ea2 + 1) << 8;                                           \
      } else {                                                                      \
          ea |= LOAD_LONG(reg_dpr) << 8;                                            \
      }                                                                             \
      if (((reg_y + ea) ^ ea) & ~0xff) {                                            \
          LOAD_LONG_DUMMY(((ea + reg_y) & 0xff) + (ea & 0xff00) + (reg_dbr << 16)); \
      }                                                                             \
      ea = (ea + reg_y + (reg_dbr << 16)) & 0xffffff;                               \
      var = LOAD_LONG(ea);                                                          \
      if (!bits8) {                                                                 \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;                               \
      }                                                                             \
  } while (0)

/* [$ff] no wrapping */
#define LOAD_INDIRECT_LONG_FUNC(var, bits8)                \
  do {                                                     \
      unsigned int ea, ea2;                                \
                                                           \
      INC_PC(SIZE_1);                                      \
      DPR_DELAY                                            \
      ea2 = p1 + reg_dpr;                                  \
      INC_PC(SIZE_1);                                      \
      ea = LOAD_BANK0(ea2);                                \
      ea |= LOAD_BANK0(ea2 + 1) << 8;                      \
      ea |= LOAD_BANK0(ea2 + 2) << 16;                     \
      var = LOAD_LONG(ea);                                 \
      if (!bits8) {                                        \
         var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;       \
      }                                                    \
  } while (0)

/* [$ff],y no wrapping */
#define LOAD_INDIRECT_LONG_Y_FUNC(var, bits8)               \
  do {                                                      \
      unsigned int ea, ea2;                                 \
                                                            \
      INC_PC(SIZE_1);                                       \
      DPR_DELAY                                             \
      ea2 = p1 + reg_dpr;                                   \
      INC_PC(SIZE_1);                                       \
      ea = LOAD_BANK0(ea2);                                 \
      ea |= LOAD_BANK0(ea2 + 1) << 8;                       \
      ea |= LOAD_BANK0(ea2 + 2) << 16;                      \
      ea = (ea + reg_y) & 0xffffff;                         \
      var = LOAD_LONG(ea);                                  \
      if (!bits8) {                                         \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;       \
      }                                                     \
  } while (0)

/* $ffff no wrapping */
#define LOAD_ABS_FUNC(var, bits8)                      \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      ea = p2 + (reg_dbr << 16);                       \
      INC_PC(SIZE_3);                                  \
      var = LOAD_LONG(ea);                             \
      if (!bits8) {                                    \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;  \
      }                                                \
  } while (0)

/* $ffff no wrapping */
#define LOAD_ABS_FUNC_RRW(var, bits8)                  \
      unsigned int ea;                                 \
                                                       \
      ea = p2 + (reg_dbr << 16);                       \
      INC_PC(SIZE_3);                                  \
      var = LOAD_LONG(ea);                             \
      if (reg_emul) {                                  \
          STORE_LONG(ea, var);                         \
      } else {                                         \
          if (!bits8) {                                \
              ea = (ea + 1) & 0xffffff;                \
              var |= LOAD_LONG(ea) << 8;               \
          }                                            \
          LOAD_LONG_DUMMY(ea);                         \
      }

/* $ffff wrapping */
#define LOAD_ABS2_FUNC(var, bits8)                                      \
  do {                                                                  \
      unsigned int ea;                                                  \
                                                                        \
      ea = p2 + (reg_dbr << 16);                                        \
      INC_PC(SIZE_3);                                                   \
      var = LOAD_LONG(ea);                                              \
      if (!bits8) {                                                     \
          var |= LOAD_LONG(((ea + 1) & 0xffff) + (reg_dbr << 16)) << 8; \
      }                                                                 \
  } while (0)

/* $ffff wrapping */
#define LOAD_ABS2_FUNC_RRW(var, bits8)                   \
      unsigned int ea;                                   \
                                                         \
      ea = p2 + (reg_dbr << 16);                         \
      INC_PC(SIZE_3);                                    \
      var = LOAD_LONG(ea);                               \
      if (reg_emul) {                                    \
          STORE_LONG(ea, var);                           \
      } else {                                           \
          if (!bits8) {                                  \
              ea = ((ea + 1) & 0xffff) + (reg_dbr << 16);\
              var |= LOAD_LONG(ea) << 8;                 \
          }                                              \
          LOAD_LONG_DUMMY(ea);                           \
      }

/* $ffff,r no wrapping */
#define LOAD_ABS_R_FUNC(var, bits8, reg_r)                                          \
  do {                                                                              \
      unsigned int ea;                                                              \
                                                                                    \
      ea = p2;                                                                      \
      INC_PC(SIZE_3);                                                               \
      if (!LOCAL_65816_X() || ((p1 + reg_r) > 0xff)) {                              \
          LOAD_LONG_DUMMY(((ea + reg_r) & 0xff) + (ea & 0xff00) + (reg_dbr << 16)); \
      }                                                                             \
      ea = (ea + reg_r + (reg_dbr << 16)) & 0xffffff;                               \
      var = LOAD_LONG(ea);                                                          \
      if (!bits8) {                                                                 \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;                               \
      }                                                                             \
  } while (0)

/* $ffff,x */
#define LOAD_ABS_X_FUNC(var, bits8) \
    LOAD_ABS_R_FUNC(var, bits8, reg_x)        

/* $ffff,x */
#define LOAD_ABS_Y_FUNC(var, bits8) \
    LOAD_ABS_R_FUNC(var, bits8, reg_y)        

/* $ffff,x no wrapping */
#define LOAD_ABS_X_FUNC_RRW(var, bits8)                                             \
      unsigned int ea;                                                              \
                                                                                    \
      ea = p2;                                                                      \
      INC_PC(SIZE_3);                                                               \
      if (!LOCAL_65816_X() || ((p1 + reg_x) > 0xff)) {                              \
          LOAD_LONG_DUMMY(((ea + reg_x) & 0xff) + (ea & 0xff00) + (reg_dbr << 16)); \
      } else {                                                                      \
          LOAD_LONG_DUMMY((ea + reg_x) + (reg_dbr << 16));                          \
      }                                                                             \
      ea = (ea + reg_x + (reg_dbr << 16)) & 0xffffff;                               \
      var = LOAD_LONG(ea);                                                          \
      if (reg_emul) {                                                               \
          STORE_LONG(ea, var);                                                      \
      } else {                                                                      \
          if (!bits8) {                                                             \
              ea = (ea + 1) & 0xffffff;                                             \
              var |= LOAD_LONG(ea) << 8;                                            \
          }                                                                         \
          LOAD_LONG_DUMMY(ea);                                                      \
      }

/* $ffff,r wrapping */
#define LOAD_ABS2_R_FUNC(var, bits8, reg_r)                                         \
  do {                                                                              \
      unsigned int ea;                                                              \
                                                                                    \
      ea = p2;                                                                      \
      INC_PC(SIZE_3);                                                               \
      if (!LOCAL_65816_X() || ((p1 + reg_r) > 0xff)) {                              \
          LOAD_LONG_DUMMY(((ea + reg_r) & 0xff) + (ea & 0xff00) + (reg_dbr << 16)); \
      }                                                                             \
      ea = ((ea + reg_r) & 0xffff) + (reg_dbr << 16);                               \
      var = LOAD_LONG(ea);                                                          \
      if (!bits8) {                                                                 \
          var |= LOAD_LONG(((ea + 1) & 0xffff) + (reg_dbr << 16)) << 8;             \
      }                                                                             \
  } while (0)

/* $ffff,x */
#define LOAD_ABS2_X_FUNC(var, bits8) \
    LOAD_ABS2_R_FUNC(var, bits8, reg_x)        

/* $ffff,y */
#define LOAD_ABS2_Y_FUNC(var, bits8) \
    LOAD_ABS2_R_FUNC(var, bits8, reg_y)        

/* $ffffff no wrapping */
#define LOAD_ABS_LONG_FUNC(var, bits8)                \
  do {                                                \
      unsigned int ea;                                \
                                                      \
      ea = p3;                                        \
      INC_PC(SIZE_4);                                 \
      var = LOAD_LONG(ea);                            \
      if (!bits8) {                                   \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8; \
      }                                               \
  } while (0)

/* $ffffff,x no wrapping */
#define LOAD_ABS_LONG_X_FUNC(var, bits8)              \
  do {                                                \
      unsigned int ea;                                \
                                                      \
      ea = (p3 + reg_x) & 0xffffff;                   \
      INC_PC(SIZE_4);                                 \
      var = LOAD_LONG(ea);                            \
      if (!bits8) {                                   \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8; \
      }                                               \
  } while (0)

/* $ff,s no wrapping */
#define LOAD_STACK_REL_FUNC(var, bits8)                \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      FETCH_PARAM_DUMMY(reg_pc);                       \
      INC_PC(SIZE_1);                                  \
      ea = p1 + reg_sp;                                \
      var = LOAD_BANK0(ea);                            \
      if (!bits8) {                                    \
          var |= LOAD_BANK0(ea + 1) << 8;              \
      }                                                \
  } while (0)

/* ($ff,s),y no wrapping */
#define LOAD_STACK_REL_Y_FUNC(var, bits8)                            \
  do {                                                               \
      unsigned int ea, ea2;                                          \
                                                                     \
      INC_PC(SIZE_1);                                                \
      FETCH_PARAM_DUMMY(reg_pc);                                     \
      INC_PC(SIZE_1);                                                \
      ea2 = p1 + reg_sp;                                             \
      ea = LOAD_BANK0(ea2);                                          \
      ea |= LOAD_BANK0(ea2 + 1) << 8;                                \
      LOAD_LONG_DUMMY((ea2 + 1) & 0xffff);                           \
      ea = (ea + reg_y + (reg_dbr << 16)) & 0xffffff;                \
      var = LOAD_LONG(ea);                                           \
      if (!bits8) {                                                  \
          var |= LOAD_LONG((ea + 1) & 0xffffff) << 8;                \
      }                                                              \
  } while (0)

#define STORE_BANK0(addr, value) \
    STORE_LONG((addr) & 0xffff, value); 

#define STORE_LONG_WORD(addr, value, bits8)                \
  do {                                                     \
      STORE_LONG(addr, value);                             \
      if (!bits8) {                                        \
          STORE_LONG(((addr) + 1) & 0xffffff, value >> 8); \
      }                                                    \
  } while (0)

/* s */
#define STORE_STACK(value, bits8)             \
  do {                                        \
      if (bits8) {                            \
          INC_PC(SIZE_1);                     \
          PUSH(value);                        \
      } else {                                \
          INC_PC(SIZE_1);                     \
          PUSH(value >> 8);                   \
          PUSH(value);                        \
      }                                       \
  } while (0)

/* a */
#define STORE_ACCU_RRW(value, bits8)          \
  do {                                        \
      if (bits8) {                            \
          reg_a = value;                      \
      } else {                                \
          reg_c = value;                      \
      }                                       \
  } while (0)

/* $ff wrapping */
#define STORE_DIRECT_PAGE(value, bits8)       \
  do {                                        \
      unsigned int ea;                        \
                                              \
      INC_PC(SIZE_1);                         \
      DPR_DELAY                               \
      ea = p1 + reg_dpr;                      \
      INC_PC(SIZE_1);                         \
      STORE_BANK0(ea, value);                 \
      if (!bits8) {                           \
          STORE_BANK0(ea + 1, value >> 8);    \
      }                                       \
  } while (0)

/* $ff wrapping */
#define STORE_DIRECT_PAGE_RRW(value, bits8)    \
  do {                                         \
      if (bits8) {                             \
          STORE_LONG(ea, value);               \
      } else {                                 \
          STORE_LONG(ea, value >> 8);          \
          STORE_BANK0(ea - 1, value);          \
      }                                        \
  } while (0)

/* $ff,r wrapping */
#define STORE_DIRECT_PAGE_R(value, bits8, reg_r)       \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      DPR_DELAY                                        \
      FETCH_PARAM_DUMMY(reg_pc);                       \
      INC_PC(SIZE_1);                                  \
      if (reg_emul) {                                  \
          if (reg_dpr & 0xff) {                        \
              ea = p1 + reg_r + reg_dpr;               \
          } else {                                     \
              ea = ((p1 + reg_r) & 0xff) + reg_dpr;    \
          }                                            \
      } else {                                         \
          ea = p1 + reg_dpr + reg_r;                   \
      }                                                \
      STORE_BANK0(ea, value);                          \
      if (!bits8) {                                    \
          STORE_BANK0(ea + 1, value >> 8);             \
      }                                                \
  } while (0)

/* $ff,x */
#define STORE_DIRECT_PAGE_X(value, bits8) STORE_DIRECT_PAGE_R(value, bits8, reg_x)

/* $ff,y */
#define STORE_DIRECT_PAGE_Y(value, bits8) STORE_DIRECT_PAGE_R(value, bits8, reg_y)

/* $ff,x wrapping */
#define STORE_DIRECT_PAGE_X_RRW(value, bits8) \
    STORE_DIRECT_PAGE_RRW(value, bits8) 

/* ($ff) no wrapping */
#define STORE_INDIRECT(value, bits8)                       \
  do {                                                     \
      unsigned int ea, ea2;                                \
                                                           \
      INC_PC(SIZE_1);                                      \
      DPR_DELAY                                            \
      INC_PC(SIZE_1);                                      \
      ea2 = p1 + reg_dpr;                                  \
      ea = LOAD_BANK0(ea2);                                \
      if (!reg_emul || (reg_dpr & 0xff) || (p1 != 0xff)) { \
          ea |= LOAD_BANK0(ea2 + 1) << 8;                  \
      } else {                                             \
          ea |= LOAD_LONG(ea2 ^ 0xff) << 8;                \
      }                                                    \
      ea |= reg_dbr << 16;                                 \
      STORE_LONG_WORD(ea, value, bits8);                   \
  } while (0)

/* ($ff,x) no wrapping */
#define STORE_INDIRECT_X(value, bits8)              \
  do {                                              \
      unsigned int ea, ea2;                         \
                                                    \
      INC_PC(SIZE_1);                               \
      DPR_DELAY                                     \
      FETCH_PARAM_DUMMY(reg_pc);                    \
      INC_PC(SIZE_1);                               \
      if (!reg_emul || (reg_dpr & 0xff)) {          \
          ea2 = p1 + reg_x + reg_dpr;               \
          ea = LOAD_BANK0(ea2);                     \
          ea |= LOAD_BANK0(ea2 + 1) << 8;           \
      } else {                                      \
          ea2 = ((p1 + reg_x) & 0xff) + reg_dpr;    \
          ea = LOAD_LONG(ea2);                      \
          ea2 = ((p1 + reg_x + 1) & 0xff) + reg_dpr;\
          ea |= LOAD_LONG(ea2) << 8;                \
      }                                             \
      ea |= reg_dbr << 16;                          \
      STORE_LONG_WORD(ea, value, bits8);            \
  } while (0)

/* ($ff),y no wrapping */
#define STORE_INDIRECT_Y(value, bits8)                                          \
  do {                                                                          \
      unsigned int ea, ea2;                                                     \
                                                                                \
      INC_PC(SIZE_1);                                                           \
      DPR_DELAY                                                                 \
      INC_PC(SIZE_1);                                                           \
      ea2 = p1 + reg_dpr;                                                       \
      ea = LOAD_BANK0(ea2);                                                     \
      if (!reg_emul || (reg_dpr & 0xff) || (p1 != 0xff)) {                      \
          ea |= LOAD_BANK0(ea2 + 1) << 8;                                       \
      } else {                                                                  \
          ea |= LOAD_LONG(ea2 ^ 0xff) << 8;                                     \
      }                                                                         \
      LOAD_LONG_DUMMY(((ea + reg_y) & 0xff) + (ea & 0xff00) + (reg_dbr << 16)); \
      ea = (ea + reg_y + (reg_dbr << 16)) & 0xffffff;                           \
      STORE_LONG_WORD(ea, value, bits8);                                        \
  } while (0);

/* [$ff] no wrapping */
#define STORE_INDIRECT_LONG(value, bits8)                    \
  do {                                                       \
      unsigned int ea, ea2;                                  \
                                                             \
      INC_PC(SIZE_1);                                        \
      DPR_DELAY                                              \
      ea2 = p1 + reg_dpr;                                    \
      INC_PC(SIZE_1);                                        \
      ea = LOAD_BANK0(ea2);                                  \
      ea |= LOAD_BANK0(ea2 + 1) << 8;                        \
      ea |= LOAD_BANK0(ea2 + 2) << 16;                       \
      STORE_LONG_WORD(ea, value, bits8);                     \
  } while (0)

/* [$ff],y no wrapping */
#define STORE_INDIRECT_LONG_Y(value, bits8)                  \
  do {                                                       \
      unsigned int ea, ea2;                                  \
                                                             \
      INC_PC(SIZE_1);                                        \
      DPR_DELAY                                              \
      ea2 = p1 + reg_dpr;                                    \
      INC_PC(SIZE_1);                                        \
      ea = LOAD_BANK0(ea2);                                  \
      ea |= LOAD_BANK0(ea2 + 1) << 8;                        \
      ea |= LOAD_BANK0(ea2 + 2) << 16;                       \
      ea = (ea + reg_y) & 0xffffff;                          \
      STORE_LONG_WORD(ea, value, bits8);                     \
  } while (0)

/* $ffff no wrapping */
#define STORE_ABS(value, bits8)                         \
  do {                                                  \
      unsigned int ea;                                  \
                                                        \
      ea = p2 + (reg_dbr << 16);                        \
      INC_PC(SIZE_3);                                   \
      STORE_LONG_WORD(ea, value, bits8);               \
  } while (0)

/* $ffff wrapping */
#define STORE_ABS2(value, bits8)                                         \
  do {                                                                   \
      unsigned int ea;                                                   \
                                                                         \
      ea = p2 + (reg_dbr << 16);                                         \
      INC_PC(SIZE_3);                                                    \
      STORE_LONG(ea, value);                                             \
      if (!bits8) {                                                      \
          STORE_LONG(((ea + 1) & 0xffff) + (reg_dbr << 16), value >> 8); \
      }                                                                  \
  } while (0)

/* $ffff no wrapping */
#define STORE_ABS_RRW(value, bits8)                   \
  do {                                                \
      if (bits8) {                                    \
          STORE_LONG(ea, value);                      \
      } else {                                        \
          STORE_LONG(ea, value >> 8);                 \
          STORE_LONG((ea - 1) & 0xffffff, value);     \
      }                                               \
  } while (0)

/* $ffff wrapping */
#define STORE_ABS2_RRW(value, bits8)                                \
  do {                                                              \
      if (bits8) {                                                  \
          STORE_LONG(ea, value);                                    \
      } else {                                                      \
          STORE_LONG(ea, value >> 8);                               \
          STORE_LONG(((ea - 1) & 0xffff) + (reg_dbr << 16), value); \
      }                                                             \
  } while (0)

/* $ffff,x no wrapping */
#define STORE_ABS_X_RRW(value, bits8) \
    STORE_ABS_RRW(value, bits8)

/* $ffff,r no wrapping */
#define STORE_ABS_R(value, bits8, reg_r)                                        \
  do {                                                                          \
      unsigned int ea;                                                          \
                                                                                \
      ea = p2;                                                                  \
      INC_PC(SIZE_3);                                                           \
      LOAD_LONG_DUMMY(((ea + reg_r) & 0xff) + (ea & 0xff00) + (reg_dbr << 16)); \
      ea = (ea + reg_r + (reg_dbr << 16)) & 0xffffff;                           \
      STORE_LONG_WORD(ea, value, bits8);                                        \
  } while (0)

/* $ffff,x */
#define STORE_ABS_X(value, bits8) STORE_ABS_R(value, bits8, reg_x)

/* $ffff,y */
#define STORE_ABS_Y(value, bits8) STORE_ABS_R(value, bits8, reg_y)

/* $ffffff no wrapping */
#define STORE_ABS_LONG(value, bits8)                                       \
  do {                                                                     \
      unsigned int ea;                                                     \
                                                                           \
      ea = p3;                                                             \
      INC_PC(SIZE_4);                                                      \
      STORE_LONG_WORD(ea, value, bits8);                                   \
  } while (0)

/* $ffffff,x no wrapping */
#define STORE_ABS_LONG_X(value, bits8)                                     \
  do {                                                                     \
      unsigned int ea;                                                     \
                                                                           \
      ea = (p3 + reg_x) & 0xffffff;                                        \
      INC_PC(SIZE_4);                                                      \
      STORE_LONG_WORD(ea, value, bits8);                                   \
  } while (0)

/* $ff,s no wrapping */
#define STORE_STACK_REL(value, bits8)                  \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      INC_PC(SIZE_1);                                  \
      FETCH_PARAM_DUMMY(reg_pc);                       \
      INC_PC(SIZE_1);                                  \
      ea = (p1 + reg_sp) & 0xffff;                     \
      STORE_LONG_WORD(ea, value, bits8);               \
  } while (0)

/* ($ff,s),y no wrapping*/
#define STORE_STACK_REL_Y(value, bits8)                     \
  do {                                                      \
      unsigned int ea, ea2;                                 \
                                                            \
      INC_PC(SIZE_1);                                       \
      FETCH_PARAM_DUMMY(reg_pc);                            \
      INC_PC(SIZE_1);                                       \
      ea2 = p1 + reg_sp;                                    \
      ea = LOAD_BANK0(ea2);                                 \
      ea |= LOAD_BANK0(ea2 + 1) << 8;                       \
      LOAD_LONG_DUMMY((ea2 + 1) & 0xffff);                  \
      ea = (ea + reg_y + (reg_dbr << 16)) & 0xffffff;       \
      STORE_LONG_WORD(ea, value, bits8);                    \
  } while (0)

#define INC_PC(value)   (reg_pc = (reg_pc + (value)) & 0xffff)

/* ------------------------------------------------------------------------- */

/* Opcodes.  */

/*
   A couple of caveats about PC:

   - the VIC-II emulation requires PC to be incremented before the first
     write access (this is not (very) important when writing to the zero
     page);

   - `p0', `p1' and `p2' can only be used *before* incrementing PC: some
     machines (eg. the C128) might depend on this.
*/

#define ADC(load_func)                                                                               \
  do {                                                                                               \
      unsigned int tmp_value;                                                                        \
      unsigned int tmp, tmp2;                                                                        \
                                                                                                     \
      tmp = LOCAL_CARRY();                                                                           \
      if (LOCAL_65816_M()) {                                                                         \
          load_func(tmp_value, 1);                                                                   \
          if (LOCAL_DECIMAL()) {                                                                     \
              tmp2  = (reg_a & 0x0f) + (tmp_value & 0x0f) + tmp;                                     \
              tmp = (reg_a & 0xf0) + (tmp_value & 0xf0) + tmp2;                                      \
              if (tmp2 > 0x9) {                                                                      \
                  tmp += 0x6;                                                                        \
              }                                                                                      \
              if (tmp > 0x99) {                                                                      \
                  tmp += 0x60;                                                                       \
              }                                                                                      \
          } else {                                                                                   \
              tmp += tmp_value + reg_a;                                                              \
          }                                                                                          \
          LOCAL_SET_CARRY(tmp & 0x100);                                                              \
          LOCAL_SET_OVERFLOW(~(reg_a ^ tmp_value) & (reg_a ^ tmp) & 0x80);                           \
          LOCAL_SET_NZ(tmp, 1);                                                                      \
          reg_a = tmp;                                                                               \
      } else {                                                                                       \
          load_func(tmp_value, 0);                                                                   \
          if (LOCAL_DECIMAL()) {                                                                     \
              tmp2 = (reg_c & 0x000f) + (tmp_value & 0x000f) + tmp;                                  \
              tmp = (reg_c & 0x00f0) + (tmp_value & 0x00f0) + tmp2;                                  \
              if (tmp2 > 0x9) {                                                                      \
                  tmp += 0x6;                                                                        \
              }                                                                                      \
              tmp2 = (reg_c & 0x0f00) + (tmp_value & 0x0f00) + tmp;                                  \
              if (tmp > 0x99) {                                                                      \
                  tmp2 += 0x60;                                                                      \
              }                                                                                      \
              tmp = (reg_c & 0xf000) + (tmp_value & 0xf000) + tmp2;                                  \
              if (tmp2 > 0x999) {                                                                    \
                  tmp += 0x600;                                                                      \
              }                                                                                      \
              if (tmp > 0x9999) {                                                                    \
                  tmp += 0x6000;                                                                     \
              }                                                                                      \
          } else {                                                                                   \
              tmp += tmp_value + reg_c;                                                              \
          }                                                                                          \
          LOCAL_SET_CARRY(tmp & 0x10000);                                                            \
          LOCAL_SET_OVERFLOW(~(reg_c ^ tmp_value) & (reg_c ^ tmp) & 0x8000);                         \
          LOCAL_SET_NZ(tmp, 0);                                                                      \
          reg_c = tmp;                                                                               \
      }                                                                                              \
  } while (0)

#define LOGICAL(load_func, logic)       \
  do {                                  \
      unsigned int tmp;                 \
                                        \
      if (LOCAL_65816_M()) {            \
          load_func(tmp, 1);            \
          reg_a logic (BYTE)tmp;        \
          LOCAL_SET_NZ(reg_a, 1);       \
      } else {                          \
          load_func(tmp, 0);            \
          reg_c logic (WORD)tmp;        \
          LOCAL_SET_NZ(reg_c, 0);       \
      }                                 \
  } while (0)

#define AND(load_func) LOGICAL(load_func, &=)

#define ASLROL(load_func, store_func, carry) \
  do {                                       \
      unsigned int tmp;                      \
                                             \
      if (LOCAL_65816_M()) {                 \
          load_func(tmp, 1);                 \
          tmp = (tmp << 1) | carry;          \
          LOCAL_SET_CARRY(tmp & 0x100);      \
          LOCAL_SET_NZ(tmp, 1);              \
          store_func(tmp, 1);                \
      } else {                               \
          load_func(tmp, 0);                 \
          tmp = (tmp << 1) | carry;          \
          LOCAL_SET_CARRY(tmp & 0x10000);    \
          LOCAL_SET_NZ(tmp, 0);              \
          store_func(tmp, 0);                \
      }                                      \
  } while (0)

#define ASL(load_func, store_func) ASLROL(load_func, store_func, 0)

#define BIT_IMM(load_func)                      \
  do {                                          \
      unsigned int tmp_value;                   \
                                                \
      if (LOCAL_65816_M()) {                    \
          load_func(tmp_value, 1);              \
          LOCAL_SET_ZERO(!(tmp_value & reg_a)); \
      } else {                                  \
          load_func(tmp_value, 0);              \
          LOCAL_SET_ZERO(!(tmp_value & reg_c)); \
      }                                         \
  } while (0)

#define BIT(load_func)                        \
  do {                                        \
      unsigned int tmp;                       \
                                              \
      if (LOCAL_65816_M()) {                  \
          load_func(tmp, 1);                  \
          LOCAL_SET_SIGN(tmp & 0x80);         \
          LOCAL_SET_OVERFLOW(tmp & 0x40);     \
          LOCAL_SET_ZERO(!(tmp & reg_a));     \
      } else {                                \
          load_func(tmp, 0);                  \
          LOCAL_SET_SIGN(tmp & 0x8000);       \
          LOCAL_SET_OVERFLOW(tmp & 0x4000);   \
          LOCAL_SET_ZERO(!(tmp & reg_c));     \
      }                                       \
  } while (0)

#define BRANCH(cond)                                         \
  do {                                                       \
      unsigned int dest_addr;                                \
      INC_PC(SIZE_2);                                        \
                                                             \
      if (cond) {                                            \
          dest_addr = (reg_pc + (signed char)(p1)) & 0xffff; \
                                                             \
          FETCH_PARAM_DUMMY(reg_pc);                         \
          if (((reg_pc ^ dest_addr) & ~0xff) && reg_emul) {  \
              FETCH_PARAM_DUMMY(dest_addr);                  \
          }                                                  \
          reg_pc = dest_addr;                                \
          JUMP(reg_pc);                                      \
      }                                                      \
  } while (0)

#define BRANCH_LONG()                                        \
  do {                                                       \
      INC_PC(SIZE_2);                                        \
      FETCH_PARAM_DUMMY(reg_pc);                             \
      INC_PC(p2 + 1);                                        \
      JUMP(reg_pc);                                          \
  } while (0)

#define BRK()                          \
  do {                                 \
      EXPORT_REGISTERS();              \
      TRACE_BRK();                     \
      INC_PC(SIZE_2);                  \
      if (reg_emul) {                  \
          LOCAL_SET_BREAK(1);          \
          PUSH(reg_pc >> 8);           \
          PUSH(reg_pc);                \
          PUSH(LOCAL_STATUS());        \
          LOAD_INT_ADDR(0xfffe);       \
      } else {                         \
          PUSH(reg_pbr);               \
          PUSH(reg_pc >> 8);           \
          PUSH(reg_pc);                \
          PUSH(LOCAL_65816_STATUS());  \
          LOAD_INT_ADDR(0xffe6);       \
      }                                \
      LOCAL_SET_DECIMAL(0);            \
      LOCAL_SET_INTERRUPT(1);          \
      reg_pbr = 0;                     \
      JUMP(reg_pc);                    \
  } while (0)

#define CLC()             \
  do {                    \
      INC_PC(SIZE_1);     \
      LOCAL_SET_CARRY(0); \
  } while (0)

#define CLD()               \
  do {                      \
      INC_PC(SIZE_1);       \
      LOCAL_SET_DECIMAL(0); \
  } while (0)

#define CLI()                   \
  do {                          \
      INC_PC(SIZE_1);           \
      if (LOCAL_INTERRUPT()) {  \
          OPCODE_ENABLES_IRQ(); \
      }                         \
      LOCAL_SET_INTERRUPT(0);   \
  } while (0)

#define CLV()                \
  do {                       \
      INC_PC(SIZE_1);        \
      LOCAL_SET_OVERFLOW(0); \
  } while (0)

#define CMP(load_func)                         \
  do {                                         \
      unsigned int tmp;                        \
      unsigned int value;                      \
                                               \
      if (LOCAL_65816_M()) {                   \
          load_func(value, 1);                 \
          tmp = reg_a - value;                 \
          LOCAL_SET_CARRY(tmp < 0x100);        \
          LOCAL_SET_NZ(tmp, 1);                \
      } else {                                 \
          load_func(value, 0);                 \
          tmp = reg_c - value;                 \
          LOCAL_SET_CARRY(tmp < 0x10000);      \
          LOCAL_SET_NZ(tmp, 0);                \
      }                                        \
  } while (0)

#define CMPI(load_func, reg_r)                 \
  do {                                         \
      unsigned int tmp;                        \
      unsigned int value;                      \
                                               \
      if (LOCAL_65816_X()) {                   \
          load_func(value, 1);                 \
          tmp = reg_r - value;                 \
          LOCAL_SET_CARRY(tmp < 0x100);        \
          LOCAL_SET_NZ(tmp, 1);                \
      } else {                                 \
          load_func(value, 0);                 \
          tmp = reg_r - value;                 \
          LOCAL_SET_CARRY(tmp < 0x10000);      \
          LOCAL_SET_NZ(tmp, 0);                \
      }                                        \
  } while (0)

#define COP()                          \
  do {                                 \
      EXPORT_REGISTERS();              \
      TRACE_COP();                     \
      INC_PC(SIZE_2);                  \
      if (reg_emul) {                  \
          LOCAL_SET_BREAK(1);          \
          PUSH(reg_pc >> 8);           \
          PUSH(reg_pc);                \
          PUSH(LOCAL_STATUS());        \
          LOAD_INT_ADDR(0xfff4);       \
      } else {                         \
          PUSH(reg_pbr);               \
          PUSH(reg_pc >> 8);           \
          PUSH(reg_pc);                \
          PUSH(LOCAL_65816_STATUS());  \
          LOAD_INT_ADDR(0xffe4);       \
      }                                \
      LOCAL_SET_DECIMAL(0);            \
      LOCAL_SET_INTERRUPT(1);          \
      reg_pbr = 0;                     \
      JUMP(reg_pc);                    \
  } while (0)

#define CPX(load_func) CMPI(load_func, reg_x)

#define CPY(load_func) CMPI(load_func, reg_y)

#define INCDEC(load_func, store_func, logic)   \
  do {                                         \
      unsigned int tmp;                        \
                                               \
      if (LOCAL_65816_M()) {                   \
          load_func(tmp, 1);                   \
          tmp logic;                           \
          LOCAL_SET_NZ(tmp, 1);                \
          store_func(tmp, 1);                  \
      } else {                                 \
          load_func(tmp, 0);                   \
          tmp logic;                           \
          LOCAL_SET_NZ(tmp, 0);                \
          store_func(tmp, 0);                  \
      }                                        \
  } while (0)

#define DEC(load_func, store_func) INCDEC(load_func, store_func, --)

#define INCDECI(reg_r, logic)               \
  do {                                      \
      INC_PC(SIZE_1);                       \
      reg_r logic;                          \
      if (LOCAL_65816_X()) {                \
          reg_r &= 0xff;                    \
          LOCAL_SET_NZ(reg_r, 1);           \
      } else {                              \
          LOCAL_SET_NZ(reg_r, 0);           \
      }                                     \
  } while (0)

#define DEX() INCDECI(reg_x, --)

#define DEY() INCDECI(reg_y, --)

#define EOR(load_func) LOGICAL(load_func, ^=)

#define INC(load_func, store_func) INCDEC(load_func, store_func, ++)

#define INX() INCDECI(reg_x, ++)

#define INY() INCDECI(reg_y, ++)

/* The 0x02 COP opcode is also used to patch the ROM.  The function trap_handler()
   returns nonzero if this is not a patch, but a `real' NOP instruction. */

#define COP_02()                                                \
  do {                                                          \
      DWORD trap_result;                                        \
      EXPORT_REGISTERS();                                       \
      if (!ROM_TRAP_ALLOWED()                                   \
          || (trap_result = ROM_TRAP_HANDLER()) == (DWORD)-1) { \
          COP_65816(p1);                                        \
      } else {                                                  \
          if (trap_result) {                                    \
             SET_OPCODE(trap_result);                           \
             IMPORT_REGISTERS();                                \
             goto trap_skipped;                                 \
          } else {                                              \
             IMPORT_REGISTERS();                                \
          }                                                     \
      }                                                         \
  } while (0)
  
#define JMP()     \
  do {            \
      JUMP(p2);   \
  } while (0)

#define JMP_IND()                                      \
  do {                                                 \
      unsigned int ea;                                 \
                                                       \
      ea = p2;                                         \
      reg_pc = LOAD_LONG(ea);                          \
      reg_pc |= LOAD_BANK0(ea + 1) << 8;               \
      JUMP(reg_pc);                                    \
  } while (0)

#define JMP_IND_LONG()                                   \
  do {                                                   \
      unsigned int ea;                                   \
                                                         \
      ea = p2;                                           \
      reg_pc = LOAD_LONG(ea);                            \
      reg_pc |= LOAD_BANK0(ea + 1) << 8;                 \
      reg_pbr = LOAD_BANK0(ea + 2);                      \
      JUMP(reg_pc);                                      \
  } while (0)

#define JMP_IND_X()                                         \
  do {                                                      \
      unsigned int ea;                                      \
                                                            \
      ea = (p2 + reg_x) & 0xffff;                           \
      INC_PC(SIZE_2);                                       \
      FETCH_PARAM_DUMMY(reg_pc);                            \
      reg_pc = FETCH_PARAM(ea);                             \
      reg_pc |= FETCH_PARAM((ea + 1) & 0xffff) << 8;        \
      JUMP(reg_pc);                                         \
  } while (0)

#define JMP_LONG()          \
  do {                      \
      reg_pc = p2;          \
      reg_pbr = p3 >> 16;   \
      JUMP(reg_pc);         \
  } while (0)

#define JSR()                                \
  do {                                       \
      INC_PC(SIZE_2);                        \
      FETCH_PARAM_DUMMY(reg_pc);             \
      PUSH(reg_pc >> 8);                     \
      PUSH(reg_pc);                          \
      reg_pc = p2;                           \
      JUMP(reg_pc);                          \
  } while (0)

#define JSR_IND_X()                                           \
  do {                                                        \
      unsigned int ea;                                        \
                                                              \
      INC_PC(SIZE_2);                                         \
      STORE_LONG(reg_sp, reg_pc >> 8);                        \
      reg_sp--;                                               \
      PUSH(reg_pc);                                           \
      ea = (p1 + (FETCH_PARAM(reg_pc) << 8) + reg_x) & 0xffff;\
      FETCH_PARAM_DUMMY(reg_pc);                              \
      reg_pc = FETCH_PARAM(ea);                               \
      reg_pc |= FETCH_PARAM((ea + 1) & 0xffff) << 8;          \
      JUMP(reg_pc);                                           \
  } while (0)

#define JSR_LONG()                               \
  do {                                           \
      INC_PC(SIZE_3);                            \
      STORE_LONG(reg_sp, reg_pbr);               \
      LOAD_LONG_DUMMY(reg_sp);                   \
      reg_sp--;                                  \
      reg_pbr = FETCH_PARAM(reg_pc);             \
      STORE_LONG(reg_sp, reg_pc >> 8);           \
      reg_sp--;                                  \
      PUSH(reg_pc);                              \
      reg_pc = p2;                               \
      JUMP(reg_pc);                              \
  } while (0)

#define LDA(load_func) LOGICAL(load_func, =)

#define LDI(load_func, reg_r)            \
  do {                                   \
      unsigned int value;                \
                                         \
      if (LOCAL_65816_X()) {             \
          load_func(value, 1);           \
          reg_r = value;                 \
          LOCAL_SET_NZ(reg_r, 1);        \
      } else {                           \
          load_func(value, 0);           \
          reg_r = value;                 \
          LOCAL_SET_NZ(reg_r, 0);        \
      }                                  \
  } while (0)

#define LDX(load_func) LDI(load_func, reg_x)

#define LDY(load_func) LDI(load_func, reg_y)

#define LSRROR(load_func, store_func, carry) \
  do {                                       \
      unsigned int tmp;                      \
                                             \
      if (LOCAL_65816_M()) {                 \
          load_func(tmp, 1);                 \
          tmp |= carry ? 0x100 : 0;          \
          LOCAL_SET_CARRY(tmp & 1);          \
          tmp >>= 1;                         \
          LOCAL_SET_NZ(tmp, 1);              \
          store_func(tmp, 1);                \
      } else {                               \
          load_func(tmp, 0);                 \
          tmp |= carry ? 0x10000 : 0;        \
          LOCAL_SET_CARRY(tmp & 1);          \
          tmp >>= 1;                         \
          LOCAL_SET_NZ(tmp, 0);              \
          store_func(tmp, 0);                \
      }                                      \
  } while (0)

#define LSR(load_func, store_func) LSRROR(load_func, store_func, 0)

#define MOVE(logic)                             \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      if (reg_c == 0) {                         \
          INC_PC(SIZE_3);                       \
      }                                         \
      reg_dbr = p2 >> 8;                        \
      tmp = LOAD_LONG(reg_x + (reg_dbr << 16)); \
      reg_dbr = p1;                             \
      STORE_LONG(reg_y + (reg_dbr << 16), tmp); \
      LOAD_LONG_DUMMY(reg_y + (reg_dbr << 16)); \
      LOAD_LONG_DUMMY(reg_y + (reg_dbr << 16)); \
      reg_x logic;                              \
      reg_y logic;                              \
      if (LOCAL_65816_X()) {                    \
          reg_x &= 0xff;                        \
          reg_y &= 0xff;                        \
      }                                         \
      reg_c--;                                  \
  } while (0)

#define MVN() MOVE(++)

#define MVP() MOVE(--)

#define WDM()  INC_PC(SIZE_2)

#define NOP()  INC_PC(SIZE_1)

#define ORA(load_func) LOGICAL(load_func, |=)

#define PEA(load_func)                \
  do {                                \
      unsigned int value;             \
                                      \
      load_func(value, 0);            \
      STORE_LONG(reg_sp, value >> 8); \
      reg_sp--;                       \
      PUSH(value);                    \
  } while (0)

#define PEI()                            \
  do {                                   \
      unsigned int value, ea;            \
                                         \
      INC_PC(SIZE_1);                    \
      DPR_DELAY                          \
      INC_PC(SIZE_1);                    \
      ea = p1 + reg_dpr;                 \
      value = LOAD_BANK0(ea);            \
      value |= LOAD_BANK0(ea + 1) << 8;  \
      STORE_LONG(reg_sp, value >> 8);    \
      reg_sp--;                          \
      PUSH(value);                       \
  } while (0)

#define PER()                                     \
  do {                                            \
      unsigned int dest_addr;                     \
      INC_PC(SIZE_2);                             \
      FETCH_PARAM_DUMMY(reg_pc);                  \
      INC_PC(SIZE_1);                             \
                                                  \
      dest_addr = reg_pc + (signed short)(p2);    \
      STORE_LONG(reg_sp, dest_addr >> 8);         \
      reg_sp--;                                   \
      PUSH(dest_addr);                            \
  } while (0)

#define PHA(store_func)           \
  do {                            \
      if (LOCAL_65816_M()) {      \
          store_func(reg_a, 1);   \
      } else {                    \
          store_func(reg_c, 0);   \
      }                           \
  } while (0)

#define PHB(store_func) store_func(reg_dbr, 1);

#define PHD()                         \
  do {                                \
      INC_PC(SIZE_1);                 \
      STORE_LONG(reg_sp, reg_dpr >> 8);\
      reg_sp--;                       \
      PUSH(reg_dpr);                  \
  } while (0)

#define PHK(store_func) store_func(reg_pbr, 1);

#define PHP(store_func)                            \
  do {                                             \
      if (reg_emul) {                              \
          store_func(LOCAL_STATUS() | P_BREAK, 1); \
      } else {                                     \
          store_func(LOCAL_65816_STATUS(), 1);     \
      }                                            \
  } while (0)

#define PHX(store_func) store_func(reg_x, LOCAL_65816_X())

#define PHY(store_func) store_func(reg_y, LOCAL_65816_X())

#define PLA(load_func)                      \
  do {                                      \
      if (LOCAL_65816_M()) {                \
          load_func(reg_a, 1);              \
          LOCAL_SET_NZ(reg_a, 1);           \
      } else {                              \
          load_func(reg_c, 0);              \
          LOCAL_SET_NZ(reg_c, 0);           \
      }                                     \
  } while (0)

#define PLB(load_func)          \
  do {                          \
      load_func(reg_dbr, 1);    \
      LOCAL_SET_NZ(reg_dbr, 1); \
  } while (0)

#define PLD()                              \
  do {                                     \
      INC_PC(SIZE_1);                      \
      FETCH_PARAM_DUMMY(reg_pc);           \
      reg_sp++;                            \
      reg_dpr = LOAD_LONG(reg_sp);         \
      reg_sp++;                            \
      reg_dpr |= LOAD_LONG(reg_sp) << 8;   \
      if (reg_emul) {                      \
          reg_sp = (reg_sp & 0xff) | 0x100;\
      }                                    \
      LOCAL_SET_NZ(reg_dpr, 0);            \
  } while (0)

#define PLP(load_func)                                      \
  do {                                                      \
      unsigned int s;                                       \
                                                            \
      load_func(s, 1);                                      \
                                                            \
      if (!(s & P_INTERRUPT) && LOCAL_INTERRUPT()) {        \
          OPCODE_ENABLES_IRQ();                             \
      } else if ((s & P_INTERRUPT) && !LOCAL_INTERRUPT()) { \
          OPCODE_DISABLES_IRQ();                            \
      }                                                     \
      LOCAL_SET_STATUS(s);                                  \
      if (LOCAL_65816_X()) {                                \
          reg_x &= 0xff;                                    \
          reg_y &= 0xff;                                    \
      }                                                     \
  } while (0)

#define PLI(load_func, reg_r)               \
  do {                                      \
      if (LOCAL_65816_X()) {                \
          load_func(reg_r, 1);              \
          LOCAL_SET_NZ(reg_r, 1);           \
      } else {                              \
          load_func(reg_r, 0);              \
          LOCAL_SET_NZ(reg_r, 0);           \
      }                                     \
  } while (0)

#define PLX(load_func) PLI(load_func, reg_x)

#define PLY(load_func) PLI(load_func, reg_y)

#define REPSEP(load_func, v)            \
  do {                                  \
      unsigned int value;               \
                                        \
      load_func(value, 1);              \
      if (value & 0x80) {               \
          LOCAL_SET_SIGN(v);            \
      }                                 \
      if (value & 0x40) {               \
          LOCAL_SET_OVERFLOW(v);        \
      }                                 \
      if ((value & 0x20) && !reg_emul) {\
          LOCAL_SET_65816_M(v);         \
      }                                 \
      if ((value & 0x10) && !reg_emul) {\
          LOCAL_SET_65816_X(v);         \
          if (v) {                      \
              reg_x &= 0xff;            \
              reg_y &= 0xff;            \
          }                             \
      }                                 \
      if (value & 0x08) {               \
          LOCAL_SET_DECIMAL(v);         \
      }                                 \
      if (value & 0x04) {               \
          LOCAL_SET_INTERRUPT(v);       \
      }                                 \
      if (value & 0x02) {               \
          LOCAL_SET_ZERO(v);            \
      }                                 \
      if (value & 0x01) {               \
          LOCAL_SET_CARRY(v);           \
      }                                 \
      FETCH_PARAM_DUMMY(reg_pc);        \
  } while (0)

#define REP(load_func) REPSEP(load_func, 0)

#define ROL(load_func, store_func) ASLROL(load_func, store_func, LOCAL_CARRY())

#define ROR(load_func, store_func) LSRROR(load_func, store_func, LOCAL_CARRY())

/* RTI does must not use `OPCODE_ENABLES_IRQ()' even if the I flag changes
   from 1 to 0 because the value of I is set 3 cycles before the end of the
   opcode, and thus the 6510 has enough time to call the interrupt routine as
   soon as the opcode ends, if necessary.  */
#define RTI(load_func)             \
  do {                             \
      unsigned int tmp;            \
                                   \
      load_func(tmp, 1);           \
      LOCAL_SET_STATUS(tmp);       \
      if (LOCAL_65816_X()) {       \
          reg_x &= 0xff;           \
          reg_y &= 0xff;           \
      }                            \
      reg_pc = PULL();             \
      reg_pc |= PULL() << 8;       \
      if (!reg_emul) {             \
          reg_pbr = PULL();        \
      }                            \
      JUMP(reg_pc);                \
  } while (0)

#define RTL()                              \
  do {                                     \
      INC_PC(SIZE_1);                      \
      FETCH_PARAM_DUMMY(reg_pc);           \
      reg_sp++;                            \
      reg_pc = LOAD_LONG(reg_sp);          \
      reg_sp++;                            \
      reg_pc |= LOAD_LONG(reg_sp) << 8;    \
      reg_sp++;                            \
      reg_pbr = LOAD_LONG(reg_sp);         \
      if (reg_emul) {                      \
          reg_sp = (reg_sp & 0xff) | 0x100;\
      }                                    \
      INC_PC(SIZE_1);                      \
      JUMP(reg_pc);                        \
  } while (0)

#define RTS(load_func)           \
  do {                           \
      load_func(reg_pc, 0);      \
      LOAD_LONG_DUMMY(reg_sp);   \
      INC_PC(SIZE_1);            \
      JUMP(reg_pc);              \
  } while (0)

#define SBC(load_func)                                                                               \
  do {                                                                                               \
      unsigned int tmp_value;                                                                        \
      unsigned int tmp, tmp2;                                                                        \
                                                                                                     \
      tmp = LOCAL_CARRY();                                                                           \
      if (LOCAL_65816_M()) {                                                                         \
          load_func(tmp_value, 1);                                                                   \
          if (LOCAL_DECIMAL()) {                                                                     \
              tmp2  = (reg_a & 0x0f) - (tmp_value & 0x0f) + !tmp;                                    \
              tmp = (reg_a & 0xf0) - (tmp_value & 0xf0) + tmp2;                                      \
              if (tmp2 > 0xff) {                                                                     \
                  tmp -= 0x6;                                                                        \
              }                                                                                      \
              if (tmp > 0xff) {                                                                      \
                  tmp -= 0x60;                                                                       \
              }                                                                                      \
              LOCAL_SET_CARRY(!(tmp & 0x100));                                                       \
          } else {                                                                                   \
              tmp += (tmp_value ^ 0xff) + reg_a;                                                     \
              LOCAL_SET_CARRY(tmp & 0x100);                                                          \
          }                                                                                          \
          LOCAL_SET_OVERFLOW((reg_a ^ tmp_value) & (reg_a ^ tmp) & 0x80);                            \
          LOCAL_SET_NZ(tmp, 1);                                                                      \
          reg_a = tmp;                                                                               \
      } else {                                                                                       \
          load_func(tmp_value, 0);                                                                   \
          if (LOCAL_DECIMAL()) {                                                                     \
              tmp2 = (reg_c & 0x000f) - (tmp_value & 0x000f) + !tmp;                                 \
              tmp = (reg_c & 0x00f0) - (tmp_value & 0x00f0) + tmp2;                                  \
              if (tmp2 > 0xffff) {                                                                   \
                  tmp -= 0x6;                                                                        \
              }                                                                                      \
              tmp2 = (reg_c & 0x0f00) - (tmp_value & 0x0f00) + tmp;                                  \
              if (tmp > 0xffff) {                                                                    \
                  tmp2 -= 0x60;                                                                      \
              }                                                                                      \
              tmp = (reg_c & 0xf000) - (tmp_value & 0xf000) + tmp2;                                  \
              if (tmp2 > 0xffff) {                                                                   \
                  tmp -= 0x600;                                                                      \
              }                                                                                      \
              if (tmp > 0xffff) {                                                                    \
                  tmp -= 0x6000;                                                                     \
              }                                                                                      \
              LOCAL_SET_CARRY(!(tmp & 0x10000));                                                     \
          } else {                                                                                   \
              tmp += (tmp_value ^ 0xffff) + reg_c;                                                   \
              LOCAL_SET_CARRY(tmp & 0x10000);                                                        \
          }                                                                                          \
          LOCAL_SET_OVERFLOW((reg_c ^ tmp_value) & (reg_c ^ tmp) & 0x8000);                          \
          LOCAL_SET_NZ(tmp, 0);                                                                      \
          reg_c = tmp;                                                                               \
      }                                                                                              \
  } while (0)

#undef SEC    /* defined in time.h on SunOS. */
#define SEC()             \
  do {                    \
      INC_PC(SIZE_1);     \
      LOCAL_SET_CARRY(1); \
  } while (0)

#define SED()               \
  do {                      \
      INC_PC(SIZE_1);       \
      LOCAL_SET_DECIMAL(1); \
  } while (0)

#define SEI()                    \
  do {                           \
      INC_PC(SIZE_1);            \
      if (!LOCAL_INTERRUPT()) {  \
          OPCODE_DISABLES_IRQ(); \
      }                          \
      LOCAL_SET_INTERRUPT(1);    \
  } while (0)

#define SEP(load_func) REPSEP(load_func, 1)

#define STA(store_func) \
      store_func(reg_c, LOCAL_65816_M());

#define STX(store_func) \
      store_func(reg_x, LOCAL_65816_X())

#define STY(store_func) \
      store_func(reg_y, LOCAL_65816_X())

#define STZ(store_func) \
      store_func(0, LOCAL_65816_M())

#define TRANSI(reg_s, reg_r)                \
  do {                                      \
      INC_PC(SIZE_1);                       \
      reg_r = reg_s;                        \
      if (LOCAL_65816_X()) {                \
          reg_r &= 0xff;                    \
          LOCAL_SET_NZ(reg_r, 1);           \
      } else {                              \
          LOCAL_SET_NZ(reg_r, 0);           \
      }                                     \
  } while (0)

#define TAX() TRANSI(reg_c, reg_x)
#define TAY() TRANSI(reg_c, reg_y)

#define TCD()                   \
  do {                          \
      INC_PC(SIZE_1);           \
      reg_dpr = reg_c;          \
      LOCAL_SET_NZ(reg_dpr, 0); \
  } while (0)

#define TCS()                                          \
  do {                                                 \
      INC_PC(SIZE_1);                                  \
      if (reg_emul) {                                  \
          reg_sp = 0x100 | reg_a;                      \
      } else {                                         \
          reg_sp = reg_c;                              \
      }                                                \
  } while (0)

#define TDC()                 \
  do {                        \
      INC_PC(SIZE_1);         \
      reg_c = reg_dpr;        \
      LOCAL_SET_NZ(reg_c, 0); \
  } while (0)

#define TRBTSB(load_func, store_func, logic)            \
  do {                                                  \
      unsigned int tmp_value;                           \
                                                        \
      if (LOCAL_65816_M()) {                            \
          load_func(tmp_value, 1);                      \
          LOCAL_SET_ZERO(!(tmp_value & reg_a));         \
          tmp_value logic reg_a;                        \
          store_func(tmp_value, 1);                     \
      } else {                                          \
          load_func(tmp_value, 0);                      \
          LOCAL_SET_ZERO(!(tmp_value & reg_c));         \
          tmp_value logic reg_c;                        \
          store_func(tmp_value, 0);                     \
      }                                                 \
  } while (0)

#define TRB(load_func, store_func) TRBTSB(load_func, store_func, &=~)

#define TSB(load_func, store_func) TRBTSB(load_func, store_func, |=)

#define TSC()                 \
  do {                        \
      INC_PC(SIZE_1);         \
      reg_c = reg_sp;         \
      LOCAL_SET_NZ(reg_c, 0); \
  } while (0)


#define TSX() TRANSI(reg_sp, reg_x)

#define TRANSA(reg_r)                       \
  do {                                      \
      INC_PC(SIZE_1);                       \
      if (LOCAL_65816_M()) {                \
          reg_a = reg_r;                    \
          LOCAL_SET_NZ(reg_a, 1);           \
      } else {                              \
          reg_c = reg_r;                    \
          LOCAL_SET_NZ(reg_c, 0);           \
      }                                     \
  } while (0)

#define TXA() TRANSA(reg_x)

#define TXS()                                 \
  do {                                        \
      INC_PC(SIZE_1);                         \
      if (LOCAL_65816_X()) {                  \
          reg_sp = 0x100 | reg_x;             \
      } else {                                \
          reg_sp = reg_x;                     \
      }                                       \
  } while (0)

#define TXY()                               \
  do {                                      \
      INC_PC(SIZE_1);                       \
      reg_y = reg_x;                        \
      LOCAL_SET_NZ(reg_y, LOCAL_65816_X()); \
  } while (0)

#define TYA() TRANSA(reg_y)

#define TYX()                               \
  do {                                      \
      reg_x = reg_y;                        \
      LOCAL_SET_NZ(reg_y, LOCAL_65816_X()); \
      INC_PC(SIZE_1);                       \
  } while (0)

#define WAI()                                                                              \
  do {                                                                                     \
      unsigned int waiting = 1;                                                            \
      enum cpu_int pending_interrupt;                                                      \
      INC_PC(SIZE_1);                                                                      \
      FETCH_PARAM_DUMMY(reg_pc);                                                           \
      do {                                                                                 \
          CPU_INT_STATUS->num_dma_per_opcode = 0;                                          \
          SET_LAST_ADDR((reg_pc - 1) & 0xffff);                                            \
          SET_LAST_OPCODE(p0);                                                             \
                                                                                           \
          if (!(CPU_INT_STATUS->global_pending_int & IK_IRQ)                               \
                  && (CPU_INT_STATUS->global_pending_int & IK_IRQPEND)                     \
                  && CPU_INT_STATUS->irq_pending_clk <= CLK) {                             \
              interrupt_ack_irq(CPU_INT_STATUS);                                           \
          }                                                                                \
          pending_interrupt = CPU_INT_STATUS->global_pending_int;                          \
          CLK = alarm_context_next_pending_clk(ALARM_CONTEXT);                             \
                                                                                           \
          if (pending_interrupt & IK_NMI) {                                                \
              if (CLK > CPU_INT_STATUS->nmi_clk + INTERRUPT_DELAY + 1) {                   \
                  CLK = CPU_INT_STATUS->nmi_clk + INTERRUPT_DELAY + 1;                     \
              }                                                                            \
          }                                                                                \
          if (pending_interrupt & (IK_IRQ | IK_IRQPEND)) {                                 \
              if (CLK > CPU_INT_STATUS->irq_clk + INTERRUPT_DELAY + 1) {                   \
                  CLK = CPU_INT_STATUS->irq_clk + INTERRUPT_DELAY + 1;                     \
              }                                                                            \
          }                                                                                \
                                                                                           \
          while (CLK >= alarm_context_next_pending_clk(ALARM_CONTEXT)) {                   \
              alarm_context_dispatch(ALARM_CONTEXT, CLK);                                  \
              CPU_DELAY_CLK                                                                \
          }                                                                                \
                                                                                           \
          pending_interrupt = CPU_INT_STATUS->global_pending_int;                          \
                                                                                           \
          if (pending_interrupt & IK_RESET) {                                              \
              waiting = 0;                                                                 \
          }                                                                                \
          if ((pending_interrupt & IK_NMI)                                                 \
                  && interrupt_check_nmi_delay(CPU_INT_STATUS, CLK)) {                     \
              waiting = 0;                                                                 \
          }                                                                                \
          if ((pending_interrupt & (IK_IRQ | IK_IRQPEND))                                  \
                  && interrupt_check_irq_delay(CPU_INT_STATUS, CLK)) {                     \
              waiting = 0;                                                                 \
          }                                                                                \
          DO_INTERRUPT(pending_interrupt);                                                 \
          if (!(CPU_INT_STATUS->global_pending_int & IK_IRQ)) {                            \
              CPU_INT_STATUS->global_pending_int &= ~IK_IRQPEND;                           \
          }                                                                                \
      } while (waiting);                                                                   \
  } while (0)


#define XBA()                    \
  do {                           \
      BYTE tmp;                  \
                                 \
      INC_PC(SIZE_1);            \
      FETCH_PARAM_DUMMY(reg_pc); \
      tmp = reg_a;               \
      reg_a = reg_b;             \
      reg_b = tmp;               \
  } while (0)

#define XCE()                                   \
  do {                                          \
      INC_PC(SIZE_1);                           \
      if (LOCAL_CARRY() != reg_emul) {          \
          if (LOCAL_CARRY()) {                  \
              reg_emul = 1;                     \
              LOCAL_SET_CARRY(0);               \
              LOCAL_SET_BREAK(0);               \
              reg_x &= 0xff;                    \
              reg_y &= 0xff;                    \
              reg_sp = 0x100 | (reg_sp & 0xff); \
          } else {                              \
              reg_emul = 0;                     \
              LOCAL_SET_CARRY(1);               \
              LOCAL_SET_65816_M(1);             \
              LOCAL_SET_65816_X(1);             \
          }                                     \
          EMULATION_MODE_CHANGED;               \
      }                                         \
  } while (0)

 static const BYTE fetch_tab[] = {
            /* 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F */
    /* $00 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $00 */
    /* $10 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $10 */
    /* $20 */  0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $20 */
    /* $30 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $30 */
    /* $40 */  0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x04, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $40 */
    /* $50 */  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x01, 0x10, 0x10, 0x02, 0x01, 0x01, 0x02, /* $50 */
    /* $60 */  0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $60 */
    /* $70 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x10, 0x10, 0x01, 0x01, 0x01, 0x02, /* $70 */
    /* $80 */  0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x10, 0x01, 0x01, 0x01, 0x02, /* $80 */
    /* $90 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x10, 0x01, 0x01, 0x01, 0x02, /* $90 */
    /* $A0 */  0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x10, 0x01, 0x01, 0x01, 0x02, /* $A0 */
    /* $B0 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x10, 0x01, 0x01, 0x01, 0x02, /* $B0 */
    /* $C0 */  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, /* $C0 */
    /* $D0 */  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x10, 0x00, 0x01, 0x01, 0x01, 0x02, /* $D0 */
    /* $E0 */  0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x04, 0x00, 0x10, 0x01, 0x01, 0x01, 0x02, /* $E0 */
    /* $F0 */  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x01, 0x10, 0x10, 0x00, 0x01, 0x01, 0x02  /* $F0 */
};

#define FETCH_M16(opcode) (!LOCAL_65816_M() && (fetch_tab[opcode & 0xff] & 4))
#define FETCH_X16(opcode) (!LOCAL_65816_X() && (fetch_tab[opcode & 0xff] & 8))
#define FETCH_16(opcode) (FETCH_M16(opcode) || FETCH_X16(opcode))

#if !defined WORDS_BIGENDIAN && defined ALLOW_UNALIGNED_ACCESS

#define opcode_t DWORD

#define FETCH_OPCODE(o)                                                     \
    do {                                                                    \
        if (((int)reg_pc) < bank_limit) {                                   \
            o = (*((DWORD *)(bank_base + reg_pc)));                         \
            CLK_ADD(CLK, 2 + (fetch_tab[o & 0xff] & 3) + FETCH_16(opcode)); \
        } else {                                                            \
            o = LOAD_PBR(reg_pc);                                           \
            if ((fetch_tab[o & 0xff] & 0x10)) {                             \
                LOAD_LONG_DUMMY(((reg_pc + 1) & 0xffff) + (reg_pbr << 16)); \
            } else {                                                        \
                o |= LOAD_PBR((reg_pc + 1) & 0xffff) << 8;                  \
                if ((fetch_tab[o & 0xff] & 3) || FETCH_16(opcode)) {        \
                    o |= LOAD_PBR((reg_pc + 2) & 0xffff) << 16;             \
                }                                                           \
                if (fetch_tab[o & 0xff] & 2) {                              \
                    o |= LOAD_PBR((reg_pc + 3) & 0xffff) << 24;             \
                }                                                           \
            }                                                               \
        }                                                                   \
    } while (0)

#define p0 (opcode & 0xff)
#define p1 ((opcode >> 8) & 0xff)
#define p2 ((opcode >> 8) & 0xffff)
#define p3 (opcode >> 8)

#else /* WORDS_BIGENDIAN || !ALLOW_UNALIGNED_ACCESS */

#define opcode_t         \
    struct {             \
        BYTE ins;        \
        union {          \
            BYTE op8[2]; \
            WORD op16;   \
        } op;            \
        BYTE extra_op;   \
    }

#define FETCH_OPCODE(o)                                                          \
    do {                                                                         \
        if (((int)reg_pc) < bank_limit) {                                        \
            (o).ins = *(bank_base + reg_pc);                                     \
            (o).op.op16 = (*(bank_base + reg_pc + 1)                             \
                          | (*(bank_base + reg_pc + 2) << 8));                   \
            (o).extra_op = *(bank_base + reg_pc + 3);                            \
            CLK_ADD(CLK, 2 + (fetch_tab[(o).ins & 0xff] & 3) + FETCH_16(opcode));\
        } else {                                                                 \
            (o).ins = LOAD_PBR(reg_pc);                                          \
            if ((fetch_tab[o & 0xff] & 0x10)) {                                  \
                LOAD_LONG_DUMMY(((reg_pc + 1) & 0xffff) + (reg_pbr << 16));      \
            } else {                                                             \
                (o).op.op16 = LOAD_PBR((reg_pc + 1) & 0xffff);                   \
                if ((fetch_tab[(o).ins] & 3) || (FETCH_16((o).ins))) {           \
                    (o).op.op16 |= LOAD_PBR((reg_pc + 2) & 0xffff) << 8;         \
                }                                                                \
                if (fetch_tab[(o).ins] & 2) {                                    \
                    (o).extra_op = LOAD_PBR((reg_pc + 3) & 0xffff);              \
                }                                                                \
            }                                                                    \
        }                                                                        \
    } while (0)

#define p0 (opcode.ins)
#define p2 (opcode.op.op16)

#ifdef WORDS_BIGENDIAN
#  define p1 (opcode.op.op8[1])
#else
#  define p1 (opcode.op.op8[0])
#endif

#define p3 (opcode.op.op16) | (opcode.extra_op << 24)

#endif /* !WORDS_BIGENDIAN */

/*  SET_OPCODE for traps */
#if !defined WORDS_BIGENDIAN && defined ALLOW_UNALIGNED_ACCESS
#define SET_OPCODE(o) (opcode) = o;
#else
#if !defined WORDS_BIGENDIAN
#define SET_OPCODE(o)                          \
    do {                                       \
        opcode.ins = (o) & 0xff;               \
        opcode.op.op8[0] = ((o) >> 8) & 0xff;  \
        opcode.op.op8[1] = ((o) >> 16) & 0xff; \
    } while (0)
#else
#define SET_OPCODE(o)                          \
    do {                                       \
        opcode.ins = (o) & 0xff;               \
        opcode.op.op8[1] = ((o) >> 8) & 0xff;  \
        opcode.op.op8[0] = ((o) >> 16) & 0xff; \
    } while (0)
#endif
#endif

/* ------------------------------------------------------------------------ */

/* Here, the CPU is emulated. */

{
    CPU_DELAY_CLK

    PROCESS_ALARMS

    {
        enum cpu_int pending_interrupt;

        if (!(CPU_INT_STATUS->global_pending_int & IK_IRQ)
            && (CPU_INT_STATUS->global_pending_int & IK_IRQPEND)
            && CPU_INT_STATUS->irq_pending_clk <= CLK) {
            interrupt_ack_irq(CPU_INT_STATUS);
        }

        pending_interrupt = CPU_INT_STATUS->global_pending_int;
        if (pending_interrupt != IK_NONE) {
            DO_INTERRUPT(pending_interrupt);
            if (!(CPU_INT_STATUS->global_pending_int & IK_IRQ)) {
                    CPU_INT_STATUS->global_pending_int &= ~IK_IRQPEND;
            }
            CPU_DELAY_CLK

            PROCESS_ALARMS
        }
    }

    {
        opcode_t opcode;
#ifdef DEBUG
        CLOCK debug_clk;
        debug_clk = maincpu_clk;
#endif

        SET_LAST_ADDR(reg_pc);
        FETCH_OPCODE(opcode);

#ifdef DEBUG
        if (TRACEFLG) {
            BYTE op = (BYTE)(p0);
            BYTE lo = (BYTE)(p1);
            BYTE hi = (BYTE)(p2 >> 8);
            BYTE bk = (BYTE)(p3 >> 16);

            if (op == 0xfc) {
                hi = LOAD(((reg_pc + 2) & 0xffff) + (reg_pbr << 16));
            }
            if (op == 0x22) {
                bk = LOAD(((reg_pc + 3) & 0xffff) + (reg_pbr << 16));
            }

            debug_main65816cpu((DWORD)(reg_pc), debug_clk,
                          mon_disassemble_to_string(e_comp_space,
                                                    reg_pc, op,
                                                    lo, hi, bk, 1, "65816"),
                          reg_c, reg_x, reg_y, reg_sp, reg_pbr);
        }
        if (debug.perform_break_into_monitor)
        {
            monitor_startup_trap();
            debug.perform_break_into_monitor = 0;
        }
#endif

trap_skipped:
        SET_LAST_OPCODE(p0);

        switch (p0) {

          case 0x42:            /* WDM */
            WDM();
            break;

          case 0x00:            /* BRK */
            BRK();
            break;

          case 0x01:            /* ORA ($nn,X) */
            ORA(LOAD_INDIRECT_X_FUNC);
            break;

          case 0x02:            /* NOP #$nn - also used for traps */
            STATIC_ASSERT(TRAP_OPCODE == 0x02);
            COP_02();
            break;

          case 0x03:            /* ORA $nn,S */
            ORA(LOAD_STACK_REL_FUNC);
            break;

          case 0x04:            /* TSB $nn */
            TSB(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0x05:            /* ORA $nn */
            ORA(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0x06:            /* ASL $nn */
            ASL(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0x07:            /* ORA [$nn] */
            ORA(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0x08:            /* PHP */
            PHP(STORE_STACK);
            break;

          case 0x09:            /* ORA #$nn */
            ORA(LOAD_IMMEDIATE_FUNC);
            break;

          case 0x0a:            /* ASL A */
            ASL(LOAD_ACCU_RRW, STORE_ACCU_RRW);
            break;

          case 0x0b:            /* PHD */
            PHD();
            break;

          case 0x0c:            /* TSB $nnnn */
            TSB(LOAD_ABS2_FUNC_RRW, STORE_ABS2_RRW);
            break;

          case 0x0d:            /* ORA $nnnn */
            ORA(LOAD_ABS_FUNC);
            break;

          case 0x0e:            /* ASL $nnnn */
            ASL(LOAD_ABS_FUNC_RRW, STORE_ABS_RRW);
            break;

          case 0x0f:            /* ORA $nnnnnn */
            ORA(LOAD_ABS_LONG_FUNC);
            break;

          case 0x10:            /* BPL $nnnn */
            BRANCH(!LOCAL_SIGN());
            break;

          case 0x11:            /* ORA ($nn),Y */
            ORA(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0x12:            /* ORA ($nn) */
            ORA(LOAD_INDIRECT_FUNC);
            break;

          case 0x13:            /* ORA ($nn,S),Y */
            ORA(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0x14:            /* TRB $nn */
            TRB(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0x15:            /* ORA $nn,X */
            ORA(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0x16:            /* ASL $nn,X */
            ASL(LOAD_DIRECT_PAGE_X_FUNC_RRW, STORE_DIRECT_PAGE_X_RRW);
            break;

          case 0x17:            /* ORA [$nn],Y */
            ORA(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0x18:            /* CLC */
            CLC();
            break;

          case 0x19:            /* ORA $nnnn,Y */
            ORA(LOAD_ABS_Y_FUNC);
            break;

          case 0x1a:            /* INA */
            INC(LOAD_ACCU_RRW, STORE_ACCU_RRW);
            break;

          case 0x1b:            /* TCS */
            TCS();
            break;

          case 0x1c:            /* TRB $nnnn */
            TRB(LOAD_ABS2_FUNC_RRW, STORE_ABS2_RRW);
            break;

          case 0x1d:            /* ORA $nnnn,X */
            ORA(LOAD_ABS_X_FUNC);
            break;

          case 0x1e:            /* ASL $nnnn,X */
            ASL(LOAD_ABS_X_FUNC_RRW, STORE_ABS_X_RRW);
            break;

          case 0x1f:            /* ORA $nnnnnn,X */
            ORA(LOAD_ABS_LONG_X_FUNC);
            break;

          case 0x20:            /* JSR $nnnn */
            JSR();
            break;

          case 0x21:            /* AND ($nn,X) */
            AND(LOAD_INDIRECT_X_FUNC);
            break;

          case 0x22:            /* JSR $nnnnnn */
            JSR_LONG();
            break;

          case 0x23:            /* AND $nn,S */
            AND(LOAD_STACK_REL_FUNC);
            break;

          case 0x24:            /* BIT $nn */
            BIT(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0x25:            /* AND $nn */
            AND(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0x26:            /* ROL $nn */
            ROL(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0x27:            /* AND [$nn] */
            AND(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0x28:            /* PLP */
            PLP(LOAD_STACK);
            break;

          case 0x29:            /* AND #$nn */
            AND(LOAD_IMMEDIATE_FUNC);
            break;

          case 0x2a:            /* ROL A */
            ROL(LOAD_ACCU_RRW, STORE_ACCU_RRW);
            break;

          case 0x2b:            /* PLD */
            PLD();
            break;

          case 0x2c:            /* BIT $nnnn */
            BIT(LOAD_ABS_FUNC);
            break;

          case 0x2d:            /* AND $nnnn */
            AND(LOAD_ABS_FUNC);
            break;

          case 0x2e:            /* ROL $nnnn */
            ROL(LOAD_ABS_FUNC_RRW, STORE_ABS_RRW);
            break;

          case 0x2f:            /* AND $nnnnnn */
            AND(LOAD_ABS_LONG_FUNC);
            break;

          case 0x30:            /* BMI $nnnn */
            BRANCH(LOCAL_SIGN());
            break;

          case 0x31:            /* AND ($nn),Y */
            AND(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0x32:            /* AND ($nn) */
            AND(LOAD_INDIRECT_FUNC);
            break;

          case 0x33:            /* AND ($nn,S),Y */
            AND(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0x34:            /* BIT $nn,X */
            BIT(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0x35:            /* AND $nn,X */
            AND(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0x36:            /* ROL $nn,X */
            ROL(LOAD_DIRECT_PAGE_X_FUNC_RRW, STORE_DIRECT_PAGE_X_RRW);
            break;

          case 0x37:            /* AND [$nn],Y */
            AND(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0x38:            /* SEC */
            SEC();
            break;

          case 0x39:            /* AND $nnnn,Y */
            AND(LOAD_ABS_Y_FUNC);
            break;

          case 0x3a:            /* DEA */
            DEC(LOAD_ACCU_RRW, STORE_ACCU_RRW);
            break;

          case 0x3b:            /* TSC */
            TSC();
            break;

          case 0x3c:            /* BIT $nnnn,X */
            BIT(LOAD_ABS_X_FUNC);
            break;

          case 0x3d:            /* AND $nnnn,X */
            AND(LOAD_ABS_X_FUNC);
            break;

          case 0x3e:            /* ROL $nnnn,X */
            ROL(LOAD_ABS_X_FUNC_RRW, STORE_ABS_X_RRW);
            break;

          case 0x3f:            /* AND $nnnnnn,X */
            AND(LOAD_ABS_LONG_X_FUNC);
            break;

          case 0x40:            /* RTI */
            RTI(LOAD_STACK);
            break;

          case 0x41:            /* EOR ($nn,X) */
            EOR(LOAD_INDIRECT_X_FUNC);
            break;

          case 0x43:            /* EOR $nn,S */
            EOR(LOAD_STACK_REL_FUNC);
            break;

          case 0x44:            /* MVP $nn,$nn */
            MVP();
            break;

          case 0x45:            /* EOR $nn */
            EOR(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0x46:            /* LSR $nn */
            LSR(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0x47:            /* EOR [$nn] */
            EOR(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0x48:            /* PHA */
            PHA(STORE_STACK);
            break;

          case 0x49:            /* EOR #$nn */
            EOR(LOAD_IMMEDIATE_FUNC);
            break;

          case 0x4a:            /* LSR A */
            LSR(LOAD_ACCU_RRW, STORE_ACCU_RRW);
            break;

          case 0x4b:            /* PHK */
            PHK(STORE_STACK);
            break;

          case 0x4c:            /* JMP $nnnn */
            JMP();
            break;

          case 0x4d:            /* EOR $nnnn */
            EOR(LOAD_ABS_FUNC);
            break;

          case 0x4e:            /* LSR $nnnn */
            LSR(LOAD_ABS_FUNC_RRW, STORE_ABS_RRW);
            break;

          case 0x4f:            /* EOR $nnnnnn */
            EOR(LOAD_ABS_LONG_FUNC);
            break;

          case 0x50:            /* BVC $nnnn */
            BRANCH(!LOCAL_OVERFLOW());
            break;

          case 0x51:            /* EOR ($nn),Y */
            EOR(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0x52:            /* EOR ($nn) */
            EOR(LOAD_INDIRECT_FUNC);
            break;

          case 0x53:            /* EOR ($nn,S),Y */
            EOR(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0x54:            /* MVN $nn,$nn */
            MVN();
            break;

          case 0x55:            /* EOR $nn,X */
            EOR(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0x56:            /* LSR $nn,X */
            LSR(LOAD_DIRECT_PAGE_X_FUNC_RRW, STORE_DIRECT_PAGE_X_RRW);
            break;

          case 0x57:            /* EOR [$nn],Y */
            EOR(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0x58:            /* CLI */
            CLI();
            break;

          case 0x59:            /* EOR $nnnn,Y */
            EOR(LOAD_ABS_Y_FUNC);
            break;

          case 0x5a:            /* PHY */
            PHY(STORE_STACK);
            break;

          case 0x5b:            /* TCD */
            TCD();
            break;

          case 0x5c:            /* JMP $nnnnnn */
            JMP_LONG();
            break;

          case 0x5d:            /* EOR $nnnn,X */
            EOR(LOAD_ABS_X_FUNC);
            break;

          case 0x5e:            /* LSR $nnnn,X */
            LSR(LOAD_ABS_X_FUNC_RRW, STORE_ABS_X_RRW);
            break;

          case 0x5f:            /* EOR $nnnnnn,X */
            EOR(LOAD_ABS_LONG_X_FUNC);
            break;

          case 0x60:            /* RTS */
            RTS(LOAD_STACK);
            break;

          case 0x61:            /* ADC ($nn,X) */
            ADC(LOAD_INDIRECT_X_FUNC);
            break;

          case 0x62:            /* PER $nnnn */
            PER();
            break;

          case 0x63:            /* ADC $nn,S */
            ADC(LOAD_STACK_REL_FUNC);
            break;

          case 0x64:            /* STZ $nn */
            STZ(STORE_DIRECT_PAGE);
            break;

          case 0x65:            /* ADC $nn */
            ADC(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0x66:            /* ROR $nn */
            ROR(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0x67:            /* ADC [$nn] */
            ADC(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0x68:            /* PLA */
            PLA(LOAD_STACK);
            break;

          case 0x69:            /* ADC #$nn */
            ADC(LOAD_IMMEDIATE_FUNC);
            break;

          case 0x6a:            /* ROR A */
            ROR(LOAD_ACCU_RRW, STORE_ACCU_RRW);
            break;

          case 0x6b:            /* RTL */
            RTL();
            break;

          case 0x6c:            /* JMP ($nnnn) */
            JMP_IND();
            break;

          case 0x6d:            /* ADC $nnnn */
            ADC(LOAD_ABS_FUNC);
            break;

          case 0x6e:            /* ROR $nnnn */
            ROR(LOAD_ABS_FUNC_RRW, STORE_ABS_RRW);
            break;

          case 0x6f:            /* ADC $nnnnnn */
            ADC(LOAD_ABS_LONG_FUNC);
            break;

          case 0x70:            /* BVS $nnnn */
            BRANCH(LOCAL_OVERFLOW());
            break;

          case 0x71:            /* ADC ($nn),Y */
            ADC(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0x72:            /* ADC ($nn) */
            ADC(LOAD_INDIRECT_FUNC);
            break;

          case 0x73:            /* ADC ($nn,S),Y */
            ADC(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0x74:            /* STZ $nn,X */
            STZ(STORE_DIRECT_PAGE_X);
            break;

          case 0x75:            /* ADC $nn,X */
            ADC(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0x76:            /* ROR $nn,X */
            ROR(LOAD_DIRECT_PAGE_X_FUNC_RRW, STORE_DIRECT_PAGE_X_RRW);
            break;

          case 0x77:            /* ADC [$nn],Y */
            ADC(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0x78:            /* SEI */
            SEI();
            break;

          case 0x79:            /* ADC $nnnn,Y */
            ADC(LOAD_ABS_Y_FUNC);
            break;

          case 0x7a:            /* PLY */
            PLY(LOAD_STACK);
            break;

          case 0x7b:            /* TDC */
            TDC();
            break;

          case 0x7c:            /* JMP ($nnnn,X) */
            JMP_IND_X();
            break;

          case 0x7d:            /* ADC $nnnn,X */
            ADC(LOAD_ABS_X_FUNC);
            break;

          case 0x7e:            /* ROR $nnnn,X */
            ROR(LOAD_ABS_X_FUNC_RRW, STORE_ABS_X_RRW);
            break;

          case 0x7f:            /* ADC $nnnnnn,X */
            ADC(LOAD_ABS_LONG_X_FUNC);
            break;

          case 0x80:            /* BRA $nnnn */
            BRANCH(1);
            break;

          case 0x81:            /* STA ($nn,X) */
            STA(STORE_INDIRECT_X);
            break;

          case 0x82:            /* BRL $nnnn */
            BRANCH_LONG();
            break;

          case 0x83:            /* STA $nn,S */
            STA(STORE_STACK_REL);
            break;

          case 0x84:            /* STY $nn */
            STY(STORE_DIRECT_PAGE);
            break;

          case 0x85:            /* STA $nn */
            STA(STORE_DIRECT_PAGE);
            break;

          case 0x86:            /* STX $nn */
            STX(STORE_DIRECT_PAGE);
            break;

          case 0x87:            /* STA [$nn] */
            STA(STORE_INDIRECT_LONG);
            break;

          case 0x88:            /* DEY */
            DEY();
            break;

          case 0x89:            /* BIT #$nn */
            BIT_IMM(LOAD_IMMEDIATE_FUNC);
            break;

          case 0x8a:            /* TXA */
            TXA();
            break;

          case 0x8b:            /* PHB */
            PHB(STORE_STACK);
            break;

          case 0x8c:            /* STY $nnnn */
            STY(STORE_ABS2);
            break;

          case 0x8d:            /* STA $nnnn */
            STA(STORE_ABS);
            break;

          case 0x8e:            /* STX $nnnn */
            STX(STORE_ABS2);
            break;

          case 0x8f:            /* STA $nnnnnn */
            STA(STORE_ABS_LONG);
            break;

          case 0x90:            /* BCC $nnnn */
            BRANCH(!LOCAL_CARRY());
            break;

          case 0x91:            /* STA ($nn),Y */
            STA(STORE_INDIRECT_Y);
            break;

          case 0x92:            /* STA ($nn) */
            STA(STORE_INDIRECT);
            break;

          case 0x93:            /* STA ($nn,S),Y */
            STA(STORE_STACK_REL_Y);
            break;

          case 0x94:            /* STY $nn,X */
            STY(STORE_DIRECT_PAGE_X);
            break;

          case 0x95:            /* STA $nn,X */
            STA(STORE_DIRECT_PAGE_X);
            break;

          case 0x96:            /* STX $nn,Y */
            STX(STORE_DIRECT_PAGE_Y);
            break;

          case 0x97:            /* STA [$nn],Y */
            STA(STORE_INDIRECT_LONG_Y);
            break;

          case 0x98:            /* TYA */
            TYA();
            break;

          case 0x99:            /* STA $nnnn,Y */
            STA(STORE_ABS_Y);
            break;

          case 0x9a:            /* TXS */
            TXS();
            break;

          case 0x9b:            /* TXY */
            TXY();
            break;

          case 0x9c:            /* STZ $nnnn */
            STZ(STORE_ABS);
            break;

          case 0x9d:            /* STA $nnnn,X */
            STA(STORE_ABS_X);
            break;

          case 0x9e:            /* STZ $nnnn,X */
            STZ(STORE_ABS_X);
            break;

          case 0x9f:            /* STA $nnnnnn,X */
            STA(STORE_ABS_LONG_X);
            break;

          case 0xa0:            /* LDY #$nn */
            LDY(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xa1:            /* LDA ($nn,X) */
            LDA(LOAD_INDIRECT_X_FUNC);
            break;

          case 0xa2:            /* LDX #$nn */
            LDX(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xa3:            /* LDA $nn,S */
            LDA(LOAD_STACK_REL_FUNC);
            break;

          case 0xa4:            /* LDY $nn */
            LDY(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xa5:            /* LDA $nn */
            LDA(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xa6:            /* LDX $nn */
            LDX(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xa7:            /* LDA [$nn] */
            LDA(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0xa8:            /* TAY */
            TAY();
            break;

          case 0xa9:            /* LDA #$nn */
            LDA(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xaa:            /* TAX */
            TAX();
            break;

          case 0xab:            /* PLB */
            PLB(LOAD_STACK);
            break;

          case 0xac:            /* LDY $nnnn */
            LDY(LOAD_ABS2_FUNC);
            break;

          case 0xad:            /* LDA $nnnn */
            LDA(LOAD_ABS_FUNC);
            break;

          case 0xae:            /* LDX $nnnn */
            LDX(LOAD_ABS2_FUNC);
            break;

          case 0xaf:            /* LDA $nnnnnn */
            LDA(LOAD_ABS_LONG_FUNC);
            break;

          case 0xb0:            /* BCS $nnnn */
            BRANCH(LOCAL_CARRY());
            break;

          case 0xb1:            /* LDA ($nn),Y */
            LDA(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0xb2:            /* LDA ($nn) */
            LDA(LOAD_INDIRECT_FUNC);
            break;

          case 0xb3:            /* LDA ($nn,S),Y */
            LDA(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0xb4:            /* LDY $nn,X */
            LDY(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0xb5:            /* LDA $nn,X */
            LDA(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0xb6:            /* LDX $nn,Y */
            LDX(LOAD_DIRECT_PAGE_Y_FUNC);
            break;

          case 0xb7:            /* LDA [$nn],Y */
            LDA(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0xb8:            /* CLV */
            CLV();
            break;

          case 0xb9:            /* LDA $nnnn,Y */
            LDA(LOAD_ABS_Y_FUNC);
            break;

          case 0xba:            /* TSX */
            TSX();
            break;

          case 0xbb:            /* TYX */
            TYX();
            break;

          case 0xbc:            /* LDY $nnnn,X */
            LDY(LOAD_ABS2_X_FUNC);
            break;

          case 0xbd:            /* LDA $nnnn,X */
            LDA(LOAD_ABS_X_FUNC);
            break;

          case 0xbe:            /* LDX $nnnn,Y */
            LDX(LOAD_ABS2_Y_FUNC);
            break;

          case 0xbf:            /* LDA $nnnnnn,X */
            LDA(LOAD_ABS_LONG_X_FUNC);
            break;

          case 0xc0:            /* CPY #$nn */
            CPY(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xc1:            /* CMP ($nn,X) */
            CMP(LOAD_INDIRECT_X_FUNC);
            break;

          case 0xc2:            /* REP #$nn */
            REP(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xc3:            /* CMP $nn,S */
            CMP(LOAD_STACK_REL_FUNC);
            break;

          case 0xc4:            /* CPY $nn */
            CPY(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xc5:            /* CMP $nn */
            CMP(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xc6:            /* DEC $nn */
            DEC(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0xc7:            /* CMP [$nn] */
            CMP(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0xc8:            /* INY */
            INY();
            break;

          case 0xc9:            /* CMP #$nn */
            CMP(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xca:            /* DEX */
            DEX();
            break;

          case 0xcb:            /* WAI */
            WAI_65816();
            break;

          case 0xcc:            /* CPY $nnnn */
            CPY(LOAD_ABS_FUNC);
            break;

          case 0xcd:            /* CMP $nnnn */
            CMP(LOAD_ABS_FUNC);
            break;

          case 0xce:            /* DEC $nnnn */
            DEC(LOAD_ABS_FUNC_RRW, STORE_ABS_RRW);
            break;

          case 0xcf:            /* CMP $nnnnnn */
            CMP(LOAD_ABS_LONG_FUNC);
            break;

          case 0xd0:            /* BNE $nnnn */
            BRANCH(!LOCAL_ZERO());
            break;

          case 0xd1:            /* CMP ($nn),Y */
            CMP(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0xd2:            /* CMP ($nn) */
            CMP(LOAD_INDIRECT_FUNC);
            break;

          case 0xd3:            /* CMP ($nn,S),Y */
            CMP(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0xd4:            /* PEI ($nn) */
            PEI();
            break;

          case 0xd5:            /* CMP $nn,X */
            CMP(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0xd6:            /* DEC $nn,X */
            DEC(LOAD_DIRECT_PAGE_X_FUNC_RRW, STORE_DIRECT_PAGE_X_RRW);
            break;

          case 0xd7:            /* CMP [$nn],Y */
            CMP(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0xd8:            /* CLD */
            CLD();
            break;

          case 0xd9:            /* CMP $nnnn,Y */
            CMP(LOAD_ABS_Y_FUNC);
            break;

          case 0xda:            /* PHX */
            PHX(STORE_STACK);
            break;

          case 0xdb:            /* STP (WDC65C02) */
            STP_65816();
            break;

          case 0xdc:            /* JMP [$nnnn] */
            JMP_IND_LONG();
            break;

          case 0xdd:            /* CMP $nnnn,X */
            CMP(LOAD_ABS_X_FUNC);
            break;

          case 0xde:            /* DEC $nnnn,X */
            DEC(LOAD_ABS_X_FUNC_RRW, STORE_ABS_X_RRW);
            break;

          case 0xdf:            /* CMP $nnnnnn,X */
            CMP(LOAD_ABS_LONG_X_FUNC);
            break;

          case 0xe0:            /* CPX #$nn */
            CPX(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xe1:            /* SBC ($nn,X) */
            SBC(LOAD_INDIRECT_X_FUNC);
            break;

          case 0xe2:            /* SEP #$nn */
            SEP(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xe3:            /* SBC $nn,S */
            SBC(LOAD_STACK_REL_FUNC);
            break;

          case 0xe4:            /* CPX $nn */
            CPX(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xe5:            /* SBC $nn */
            SBC(LOAD_DIRECT_PAGE_FUNC);
            break;

          case 0xe6:            /* INC $nn */
            INC(LOAD_DIRECT_PAGE_FUNC_RRW, STORE_DIRECT_PAGE_RRW);
            break;

          case 0xe7:            /* SBC [$nn] */
            SBC(LOAD_INDIRECT_LONG_FUNC);
            break;

          case 0xe8:            /* INX */
            INX();
            break;

          case 0xe9:            /* SBC #$nn */
            SBC(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xea:            /* NOP */
            NOP();
            break;

          case 0xeb:            /* XBA */
            XBA();
            break;

          case 0xec:            /* CPX $nnnn */
            CPX(LOAD_ABS_FUNC);
            break;

          case 0xed:            /* SBC $nnnn */
            SBC(LOAD_ABS_FUNC);
            break;

          case 0xee:            /* INC $nnnn */
            INC(LOAD_ABS_FUNC_RRW, STORE_ABS_RRW);
            break;

          case 0xef:            /* SBC $nnnnnn */
            SBC(LOAD_ABS_LONG_FUNC);
            break;

          case 0xf0:            /* BEQ $nnnn */
            BRANCH(LOCAL_ZERO());
            break;

          case 0xf1:            /* SBC ($nn),Y */
            SBC(LOAD_INDIRECT_Y_FUNC);
            break;

          case 0xf2:            /* SBC ($nn) */
            SBC(LOAD_INDIRECT_FUNC);
            break;

          case 0xf3:            /* SBC ($nn,S),Y */
            SBC(LOAD_STACK_REL_Y_FUNC);
            break;

          case 0xf4:            /* PEA $nnnn */
            PEA(LOAD_IMMEDIATE_FUNC);
            break;

          case 0xf5:            /* SBC $nn,X */
            SBC(LOAD_DIRECT_PAGE_X_FUNC);
            break;

          case 0xf6:            /* INC $nn,X */
            INC(LOAD_DIRECT_PAGE_X_FUNC_RRW, STORE_DIRECT_PAGE_X_RRW);
            break;

          case 0xf7:            /* SBC [$nn],Y */
            SBC(LOAD_INDIRECT_LONG_Y_FUNC);
            break;

          case 0xf8:            /* SED */
            SED();
            break;

          case 0xf9:            /* SBC $nnnn,Y */
            SBC(LOAD_ABS_Y_FUNC);
            break;

          case 0xfa:            /* PLX */
            PLX(LOAD_STACK);
            break;

          case 0xfb:            /* XCE */
            XCE();
            break;

          case 0xfc:            /* JSR ($nnnn,X) */
            JSR_IND_X();
            break;

          case 0xfd:            /* SBC $nnnn,X */
            SBC(LOAD_ABS_X_FUNC);
            break;

          case 0xfe:            /* INC $nnnn,X */
            INC(LOAD_ABS_X_FUNC_RRW, STORE_ABS_X_RRW);
            break;

          case 0xff:            /* SBC $nnnnnn,X */
            SBC(LOAD_ABS_LONG_X_FUNC);
            break;
        }
    }
}
