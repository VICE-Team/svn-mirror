/*
 * 6510core.c - MOS6510 emulation core.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifdef __1541__
#define CPU_STR "1541 CPU"
#else
#define CPU_STR "Main CPU"
#endif

#if 0
#define UNDOC_WARNING() fprintf(logfile, CPU_STR " Warning: undocumented $%02X\n", p0);
#else
#define UNDOC_WARNING()
#endif

#define IRQ_CYCLES      7
#define NMI_CYCLES      7
#define RESET_CYCLES    6

 /* ------------------------------------------------------------------------- */

/* backup for non-6509 CPUs */

#ifndef LOAD_IND
#define        LOAD_IND(a)     LOAD(a)
#endif
#ifndef STORE_IND
#define        STORE_IND(a,b)  STORE(a,b)
#endif

/* ------------------------------------------------------------------------- */

#define LOCAL_SET_NZ(val)        (flag_z = flag_n = (val))

#if defined __1541__
#define LOCAL_SET_OVERFLOW(val)			\
    do {					\
        if (!(val)) _drive_set_byte_ready(0);	\
        ((val) ? (reg_p |= P_OVERFLOW)		\
         : (reg_p &= ~P_OVERFLOW));		\
    } while (0)
#else
#define LOCAL_SET_OVERFLOW(val)  ((val) ? (reg_p |= P_OVERFLOW)    \
                                        : (reg_p &= ~P_OVERFLOW))
#endif

#define LOCAL_SET_BREAK(val)     ((val) ? (reg_p |= P_BREAK)       \
                                        : (reg_p &= ~P_BREAK))
#define LOCAL_SET_DECIMAL(val)   ((val) ? (reg_p |= P_DECIMAL)     \
                                        : (reg_p &= ~P_DECIMAL))
#define LOCAL_SET_INTERRUPT(val) ((val) ? (reg_p |= P_INTERRUPT)   \
                                        : (reg_p &= ~P_INTERRUPT))
#define LOCAL_SET_CARRY(val)     ((val) ? (reg_p |= P_CARRY)       \
                                        : (reg_p &= ~P_CARRY))
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

/* Additional debugging.  */
#if 1 /* def __1541__ */
#define DO_DEBUG(ab)    while(0)
#else
#define DO_DEBUG(ab)                                                    \
    do {                                                                \
        if (app_resources.debugFlag)                                    \
            fprintf(logfile, "CPU " ab " at clk=%d, PC=%04x\n", clk, reg_pc);     \
    } while(0)
#endif

#ifdef LAST_OPCODE_INFO

/* If requested, gather some info about the last executed opcode for timing
   purposes.  */

/* Remember the number of the last opcode.  By default, the opcode does not
   delay interrupt and does not change the I flag.  */
#define SET_LAST_OPCODE(x) \
    OPINFO_SET(LAST_OPCODE_INFO, (x), 0, 0, 0)

/* Remember that the last opcode delayed a pending IRQ or NMI by one cycle.  */
#define OPCODE_DELAYS_INTERRUPT() \
    OPINFO_SET_DELAYS_INTERRUPT(LAST_OPCODE_INFO, 1)

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
#define OPCODE_DELAYS_INTERRUPT()
#define OPCODE_DISABLES_IRQ()
#define OPCODE_ENABLES_IRQ()

#endif

#ifndef __1541__
/* Export the local version of the registers.  */
#define EXPORT_REGISTERS()                      \
  do {                                          \
      GLOBAL_REGS.reg_pc = reg_pc;              \
      GLOBAL_REGS.reg_a = reg_a;                \
      GLOBAL_REGS.reg_x = reg_x;                \
      GLOBAL_REGS.reg_y = reg_y;                \
      GLOBAL_REGS.reg_sp = reg_sp;              \
      GLOBAL_REGS.reg_p = reg_p;                \
      GLOBAL_REGS.flag_n = flag_n;              \
      GLOBAL_REGS.flag_z = flag_z;              \
  } while (0)

/* Import the public version of the registers.  */
#define IMPORT_REGISTERS()                      \
  do {                                          \
      reg_a = GLOBAL_REGS.reg_a;                \
      reg_x = GLOBAL_REGS.reg_x;                \
      reg_y = GLOBAL_REGS.reg_y;                \
      reg_sp = GLOBAL_REGS.reg_sp;              \
      reg_p = GLOBAL_REGS.reg_p;                \
      flag_n = GLOBAL_REGS.flag_n;              \
      flag_z = GLOBAL_REGS.flag_z;              \
      JUMP(GLOBAL_REGS.reg_pc);                 \
  } while (0)
#else  /* __1541__ */
#define IMPORT_REGISTERS()
#define EXPORT_REGISTERS()
#endif /* !__1541__ */

/* Stack operations. */

#define PUSH(val) ((PAGE_ONE)[(reg_sp--)] = (val))
#define PULL()    ((PAGE_ONE)[(++reg_sp)])

#ifdef TRACE
#define TRACE_NMI() do { if (TRACEFLG) puts("*** NMI"); } while (0)
#define TRACE_IRQ() do { if (TRACEFLG) puts("*** IRQ"); } while (0)
#define TRACE_BRK() do { if (TRACEFLG) puts("*** BRK"); } while (0)
#else
#define TRACE_NMI()
#define TRACE_IRQ()
#define TRACE_BRK()
#endif

/* Perform the interrupts in `int_kind'.  If we have both NMI and IRQ,
   execute NMI.  */
#define DO_INTERRUPT(int_kind)                                           \
    do {                                                                 \
        BYTE ik = (int_kind);                                            \
                                                                         \
        if (ik & (IK_IRQ | IK_NMI)) {                                    \
            if ((ik & IK_NMI)                                            \
		&& check_nmi_delay(&CPU_INT_STATUS, CLK)) {              \
                TRACE_NMI();                                             \
                ack_nmi(&CPU_INT_STATUS);                                \
                LOCAL_SET_BREAK(0);                                      \
                PUSH(reg_pc >> 8);                                       \
                PUSH(reg_pc & 0xff);                                     \
                PUSH(LOCAL_STATUS());                                    \
                LOCAL_SET_INTERRUPT(1);                                  \
                JUMP(LOAD_ADDR(0xfffa));                                 \
                SET_LAST_OPCODE(0);                                      \
                CLK += NMI_CYCLES;                                       \
            } else if ((ik & IK_IRQ)                                     \
                       && (!LOCAL_INTERRUPT()                            \
                           || OPINFO_DISABLES_IRQ(LAST_OPCODE_INFO))     \
                       && check_irq_delay(&CPU_INT_STATUS, CLK)) {       \
                TRACE_IRQ();                                             \
                LOCAL_SET_BREAK(0);                                      \
                PUSH(reg_pc >> 8);                                       \
                PUSH(reg_pc & 0xff);                                     \
                PUSH(LOCAL_STATUS());                                    \
                LOCAL_SET_INTERRUPT(1);                                  \
                JUMP(LOAD_ADDR(0xfffe));                                 \
                SET_LAST_OPCODE(0);                                      \
                CLK += IRQ_CYCLES;                                       \
            }                                                            \
        }                                                                \
        if (ik & (IK_TRAP | IK_RESET)) {                                 \
            if (ik & IK_TRAP) {                                          \
                EXPORT_REGISTERS();                                      \
                do_trap(&CPU_INT_STATUS, (ADDRESS) reg_pc);              \
                IMPORT_REGISTERS();                                      \
                if (check_pending_interrupt(&CPU_INT_STATUS) & IK_RESET) \
                    ik |= IK_RESET;                                      \
            }                                                            \
            if (ik & IK_RESET) {                                         \
                ack_reset(&CPU_INT_STATUS);                              \
                reset();                                                 \
                JUMP(LOAD_ADDR(0xfffc));                                 \
            }                                                            \
        }                                                                \
        if (ik & (IK_MONITOR)) {					 \
           caller_space = CALLER;					 \
           if (mon_force_import(CALLER))				 \
              IMPORT_REGISTERS();					 \
           if (mon_mask[CALLER])					 \
              EXPORT_REGISTERS();					 \
           if (mon_mask[CALLER] & (MI_BREAK)) {				 \
              if (check_breakpoints(CALLER, (ADDRESS) reg_pc)) {	 \
                 mon((ADDRESS) reg_pc);					 \
                 IMPORT_REGISTERS();					 \
              }								 \
           }								 \
           if (mon_mask[CALLER] & (MI_STEP)) {				 \
              mon_check_icount((ADDRESS) reg_pc);		         \
              IMPORT_REGISTERS();					 \
           }								 \
           if (mon_mask[CALLER] & (MI_WATCH)) {				 \
              mon_check_watchpoints((ADDRESS) reg_pc);   		 \
              IMPORT_REGISTERS();					 \
           }								 \
        }								 \
    } while (0)

/* ------------------------------------------------------------------------- */

/* Addressing modes.  For convenience, page boundary crossing cycles and
   ``idle'' memory reads are handled here as well. */

#define LOAD_ABS(addr) \
   LOAD(addr)

#define LOAD_ABS_X(addr)                                        \
   ((((addr) & 0xff) + reg_x) > 0xff                            \
    ? (LOAD(((addr) & 0xff00) | (((addr) + reg_x) & 0xff)),     \
       CLK++,                                                   \
       LOAD((addr) + reg_x))                                    \
    : LOAD((addr) + reg_x))

#define LOAD_ABS_X_RMW(addr)                                    \
   (LOAD(((addr) & 0xff00) | (((addr) + reg_x) & 0xff)),        \
    CLK++,                                                      \
    LOAD((addr) + reg_x))

#define LOAD_ABS_Y(addr)                                        \
   ((((addr) & 0xff) + reg_y) > 0xff                            \
    ? (LOAD(((addr) & 0xff00) | (((addr) + reg_y) & 0xff)),     \
       CLK++,                                                   \
       LOAD((addr) + reg_y))                                    \
    : LOAD((addr) + reg_y))

#define LOAD_ABS_Y_RMW(addr)                                    \
   (LOAD(((addr) & 0xff00) | (((addr) + reg_y) & 0xff)),        \
    CLK++,                                                      \
    LOAD((addr) + reg_y))

#define LOAD_IND_X(addr)                        \
   (LOAD(LOAD_ZERO_ADDR((addr) + reg_x)))

#define LOAD_IND_Y(addr)                                        \
   (((LOAD_ZERO_ADDR((addr)) & 0xff) + reg_y) > 0xff            \
    ? (LOAD((LOAD_ZERO_ADDR((addr)) & 0xff00)                   \
            | ((LOAD_ZERO_ADDR((addr)) + reg_y) & 0xff)),       \
       CLK++,                                                   \
       LOAD(LOAD_ZERO_ADDR((addr)) + reg_y))                    \
    : LOAD(LOAD_ZERO_ADDR((addr)) + reg_y))

#define LOAD_ZERO_X(addr)                       \
   (LOAD_ZERO((addr) + reg_x))

#define LOAD_ZERO_Y(addr)                       \
   (LOAD_ZERO((addr) + reg_y))

#define LOAD_IND_Y_BANK(addr)                                   \
   (((LOAD_ZERO_ADDR((addr)) & 0xff) + reg_y) > 0xff            \
    ? (LOAD_IND((LOAD_ZERO_ADDR((addr)) & 0xff00)               \
            | ((LOAD_ZERO_ADDR((addr)) + reg_y) & 0xff)),       \
       CLK++,                                                   \
       LOAD_IND(LOAD_ZERO_ADDR((addr)) + reg_y))                \
    : LOAD_IND(LOAD_ZERO_ADDR((addr)) + reg_y))

#define STORE_ABS(addr, value, inc)             \
  do {                                          \
      CLK += (inc);                             \
      STORE((addr), (value));                   \
  } while (0)

#define STORE_ABS_X(addr, value, inc)                           \
  do {                                                          \
      CLK += (inc) - 2;                                         \
      LOAD((((addr) + reg_x) & 0xff) | ((addr) & 0xff00));      \
      CLK += 2;                                                 \
      STORE((addr) + reg_x, (value));                           \
  } while (0)

#define STORE_ABS_X_RMW(addr, value, inc)	\
  do {						\
      CLK += (inc);				\
      STORE((addr) + reg_x, (value));		\
  } while (0)					\

#define STORE_ABS_Y(addr, value, inc)                           \
  do {                                                          \
      CLK += (inc) - 2;                                         \
      LOAD((((addr) + reg_y) & 0xff) | ((addr) & 0xff00));      \
      CLK += 2;                                                 \
      STORE((addr) + reg_y, (value));                           \
  } while (0)

#define STORE_ABS_Y_RMW(addr, value, inc)	\
  do {						\
      CLK += (inc);				\
      STORE((addr) + reg_y, (value));		\
  } while (0)

#define INC_PC(value)   (reg_pc += (value))

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

#define ADC(value, clk_inc1, clk_inc2, pc_inc)                                \
  do {                                                                        \
      unsigned int tmp_value;                                                 \
      unsigned int tmp;                                                       \
                                                                              \
      CLK += (clk_inc1);                                                      \
      tmp_value = (value);                                                    \
      CLK += (clk_inc2);                                                      \
                                                                              \
      if (LOCAL_DECIMAL()) {                                                  \
          tmp = (reg_a & 0xf) + (tmp_value & 0xf) + (reg_p & 0x1);            \
          if (tmp > 0x9)                                                      \
              tmp += 0x6;                                                     \
          if (tmp <= 0x0f)                                                    \
              tmp = (tmp & 0xf) + (reg_a & 0xf0) + (tmp_value & 0xf0);        \
          else                                                                \
              tmp = (tmp & 0xf) + (reg_a & 0xf0) + (tmp_value & 0xf0) + 0x10; \
          LOCAL_SET_ZERO(!((reg_a + tmp_value + (reg_p & 0x1)) & 0xff));      \
          LOCAL_SET_SIGN(tmp & 0x80);                                         \
          LOCAL_SET_OVERFLOW(((reg_a ^ tmp) & 0x80)                           \
                              && !((reg_a ^ tmp_value) & 0x80));              \
          if ((tmp & 0x1f0) > 0x90)                                           \
              tmp += 0x60;                                                    \
          LOCAL_SET_CARRY((tmp & 0xff0) > 0xf0);                              \
      } else {                                                                \
          tmp = tmp_value + reg_a + (reg_p & P_CARRY);                        \
          LOCAL_SET_NZ(tmp & 0xff);                                           \
          LOCAL_SET_OVERFLOW(!((reg_a ^ tmp_value) & 0x80)                    \
                              && ((reg_a ^ tmp) & 0x80));                     \
          LOCAL_SET_CARRY(tmp > 0xff);                                        \
      }                                                                       \
      reg_a = tmp;                                                            \
      INC_PC(pc_inc);                                                         \
  } while (0)

#define ANC(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      UNDOC_WARNING();                          \
      CLK += (clk_inc1);                        \
      reg_a &= (value);                         \
      LOCAL_SET_NZ(reg_a);                      \
      CLK += (clk_inc2);                        \
      LOCAL_SET_CARRY(LOCAL_SIGN());            \
      INC_PC(pc_inc);                           \
  } while (0)

#define AND(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_a &= (value);                         \
      LOCAL_SET_NZ(reg_a);                      \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define ANE(value, clk_inc1, clk_inc2, pc_inc)          \
  do {                                                  \
      UNDOC_WARNING();                                  \
      CLK += (clk_inc1);                                \
      reg_a = ((reg_a | 0xee) & reg_x & (value));       \
      LOCAL_SET_NZ(reg_a);                              \
      CLK += (clk_inc2);                                \
      INC_PC(pc_inc);                                   \
  } while (0)

/* The fanciest opcode ever... ARR! */
#define ARR(value, clk_inc1, clk_inc2, pc_inc)                          \
  do {                                                                  \
      unsigned int tmp;                                                 \
                                                                        \
      UNDOC_WARNING();                                                  \
      CLK += (clk_inc1);                                                \
      tmp = reg_a & (value);                                            \
      CLK += (clk_inc2);                                                \
      if (LOCAL_DECIMAL()) {                                            \
          int tmp_2 = tmp;                                              \
          tmp_2 |= (reg_p & P_CARRY) << 8;                              \
          tmp_2 >>= 1;                                                  \
          LOCAL_SET_SIGN(LOCAL_CARRY());                                \
          LOCAL_SET_ZERO(!tmp_2);                                       \
          LOCAL_SET_OVERFLOW((tmp_2 ^ tmp) & 0x40);                     \
          if (((tmp & 0xf) + (tmp & 0x1)) > 0x5)                        \
              tmp_2 = (tmp_2 & 0xf0) | ((tmp_2 + 0x6) & 0xf);           \
          if (((tmp & 0xf0) + (tmp & 0x10)) > 0x50) {                   \
              tmp_2 = (tmp_2 & 0x0f) | ((tmp_2 + 0x60) & 0xf0);         \
              LOCAL_SET_CARRY(1);                                       \
          } else                                                        \
              LOCAL_SET_CARRY(0);                                       \
          reg_a = tmp_2;                                                \
      } else {                                                          \
          tmp |= (reg_p & P_CARRY) << 8;                                \
          tmp >>= 1;                                                    \
          LOCAL_SET_NZ(tmp);                                            \
          LOCAL_SET_CARRY(tmp & 0x40);                                  \
          LOCAL_SET_OVERFLOW((tmp & 0x40) ^ ((tmp & 0x20) << 1));       \
          reg_a = tmp;                                                  \
      }                                                                 \
      INC_PC(pc_inc);                                                   \
  } while (0)

#define ASL(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp_value, tmp_addr;                                 \
                                                                        \
      tmp_addr = (addr);                                                \
      CLK += (clk_inc1);                                                \
      tmp_value = load_func(tmp_addr);                                  \
      LOCAL_SET_CARRY(tmp_value & 0x80);                                \
      tmp_value = (tmp_value << 1) & 0xff;                              \
      LOCAL_SET_NZ(tmp_value);                                          \
      RMW_FLAG = 1;                                                     \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp_value, clk_inc2);                        \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define ASL_A()                                 \
  do {                                          \
      CLK += 2;                                 \
      LOCAL_SET_CARRY(reg_a & 0x80);            \
      reg_a <<= 1;                              \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define ASR(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      UNDOC_WARNING();                          \
      CLK += (clk_inc1);                        \
      tmp = reg_a & (value);                    \
      CLK += (clk_inc2);                        \
      LOCAL_SET_CARRY(tmp & 0x01);              \
      reg_a = tmp >> 1;                         \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(pc_inc);                           \
  } while (0)

#define BIT(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = (value);                            \
      CLK += (clk_inc2);                        \
      LOCAL_SET_SIGN(tmp & 0x80);               \
      LOCAL_SET_OVERFLOW(tmp & 0x40);           \
      LOCAL_SET_ZERO(!(tmp & reg_a));           \
      INC_PC(pc_inc);                           \
  } while (0)

#define BRANCH(cond, value)                                             \
  do {                                                                  \
      if (cond) {                                                       \
          unsigned int dest_addr = reg_pc + 2 + (signed char)(value);   \
                                                                        \
          if (((reg_pc + 2) ^ dest_addr) & 0xff00) {                    \
              CLK += 4;                                                 \
          } else {                                                      \
              CLK += 3;                                                 \
              OPCODE_DELAYS_INTERRUPT();                                \
          }                                                             \
          reg_pc = dest_addr & 0xffff;                                  \
      } else {                                                          \
          CLK += 2;                                                     \
          INC_PC(2);                                                    \
      }                                                                 \
  } while (0)

/* The BRK opcode is also used to patch the ROM.  The function trap_handler()
   returns nonzero if this is not a patch, but a `real' BRK instruction. */

#define BRK()                                                   \
  do {                                                          \
      CLK += 7;                                                 \
      EXPORT_REGISTERS();                                       \
      if (!ROM_TRAP_ALLOWED() || ROM_TRAP_HANDLER()) {          \
	  TRACE_BRK();						\
          INC_PC(2);                                            \
          LOCAL_SET_BREAK(1);                                   \
          PUSH(reg_pc >> 8);                                    \
          PUSH(reg_pc & 0xff);                                  \
          PUSH(LOCAL_STATUS());                                 \
          LOCAL_SET_INTERRUPT(1);                               \
          JUMP(LOAD_ADDR(0xfffe));                              \
      } else {                                                  \
          IMPORT_REGISTERS();                                   \
      }                                                         \
  } while (0)

#define CLC()                                   \
  do {                                          \
      CLK += 2;                                 \
      INC_PC(1);                                \
      LOCAL_SET_CARRY(0);                       \
  } while (0)

#define CLD()                                   \
  do {                                          \
      CLK += 2;                                 \
      INC_PC(1);                                \
      LOCAL_SET_DECIMAL(0);                     \
  } while (0)

#define CLI()                                   \
  do {                                          \
      CLK += 2;                                 \
      INC_PC(1);                                \
      if (LOCAL_INTERRUPT())                    \
          OPCODE_ENABLES_IRQ();                 \
      LOCAL_SET_INTERRUPT(0);                   \
  } while (0)

#define CLV()                                   \
  do {                                          \
      CLK += 2;                                 \
      INC_PC(1);                                \
      LOCAL_SET_OVERFLOW(0);                    \
  } while (0)

#define CMP(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = reg_a - (value);                    \
      LOCAL_SET_CARRY(tmp < 0x100);             \
      LOCAL_SET_NZ(tmp & 0xff);                 \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define CPX(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = reg_x - (value);                    \
      LOCAL_SET_CARRY(tmp < 0x100);             \
      LOCAL_SET_NZ(tmp & 0xff);                 \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define CPY(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = reg_y - (value);                    \
      LOCAL_SET_CARRY(tmp < 0x100);             \
      LOCAL_SET_NZ(tmp & 0xff);                 \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define DCP(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      UNDOC_WARNING();                                                  \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      tmp = load_func(tmp_addr);                                        \
      tmp = (tmp - 1) & 0xff;                                           \
      LOCAL_SET_CARRY(reg_a >= tmp);                                    \
      LOCAL_SET_NZ((reg_a - tmp));                                      \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp, (clk_inc2));                            \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define DCP_IND_Y(addr)                                         \
  do {                                                          \
      unsigned int tmp;                                         \
      unsigned int tmp_addr = LOAD_ZERO_ADDR(addr);             \
                                                                \
      UNDOC_WARNING();                                          \
      RMW_FLAG = 1;                                             \
      CLK += 5;                                                 \
      LOAD((tmp_addr & 0xff00) | ((tmp_addr + reg_y) & 0xff));  \
      CLK++;                                                    \
      tmp_addr += reg_y;                                        \
      tmp = LOAD(tmp_addr);                                     \
      tmp = (tmp - 1) & 0xff;                                   \
      LOCAL_SET_CARRY(reg_a >= tmp);                            \
      LOCAL_SET_NZ((reg_a - tmp));                              \
      INC_PC(2);                                                \
      STORE_ABS(tmp_addr, tmp, 2);                              \
      RMW_FLAG = 0;                                             \
  } while (0)

#define DEC(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      CLK += (clk_inc1);                                                \
      tmp = load_func(tmp_addr);                                        \
      tmp = (tmp - 1) & 0xff;                                           \
      LOCAL_SET_NZ(tmp);                                                \
      RMW_FLAG = 1;                                                     \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp, (clk_inc2));                            \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define DEX()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_x--;                                  \
      LOCAL_SET_NZ(reg_x);                      \
      INC_PC(1);                                \
  } while (0)

#define DEY()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_y--;                                  \
      LOCAL_SET_NZ(reg_y);                      \
      INC_PC(1);                                \
  } while (0)

#define EOR(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_a ^= (value);                         \
      LOCAL_SET_NZ(reg_a);                      \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define INC(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      CLK += (clk_inc1);                                                \
      tmp = (load_func(tmp_addr) + 1) & 0xff;                           \
      LOCAL_SET_NZ(tmp);                                                \
      RMW_FLAG = 1;                                                     \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp, (clk_inc2));                            \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define INX()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_x++;                                  \
      LOCAL_SET_NZ(reg_x);                      \
      INC_PC(1);                                \
  } while (0)

#define INY()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_y++;                                  \
      LOCAL_SET_NZ(reg_y);                      \
      INC_PC(1);                                \
  } while (0)

#define ISB(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      BYTE my_src;                                                      \
      int my_addr = (addr);                                             \
                                                                        \
      UNDOC_WARNING();                                                  \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      my_src = load_func(my_addr);                                      \
      my_src = (my_src + 1) & 0xff;                                     \
      SBC(my_src, 0, 0, 0);                                             \
      INC_PC(pc_inc);                                                   \
      store_func(my_addr, my_src, clk_inc2);                            \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define ISB_IND_Y(addr)                                         \
  do {                                                          \
      BYTE my_src;                                              \
      int my_addr = LOAD_ZERO_ADDR(addr);                       \
                                                                \
      UNDOC_WARNING();                                          \
      RMW_FLAG = 1;                                             \
      CLK += 5;                                                 \
      LOAD((my_addr & 0xff00) | ((my_addr + reg_y) & 0xff));    \
      CLK++;                                                    \
      my_addr += reg_y;                                         \
      my_src = LOAD(my_addr);                                   \
      my_src = (my_src + 1) & 0xff;                             \
      SBC(my_src, 0, 0, 0);                                     \
      INC_PC(2);                                                \
      STORE_ABS(my_addr, my_src, 2);                            \
      RMW_FLAG = 0;                                             \
  } while (0)

#define JMP(addr, clk_inc1, clk_inc2)           \
  do {                                          \
      CLK += (clk_inc1);                        \
      JUMP(addr);                               \
      CLK += (clk_inc2);                        \
  } while (0)

#define JSR(addr, clk_inc1, clk_inc2, pc_inc)           \
  do {                                                  \
      unsigned int tmp_addr;                            \
                                                        \
      CLK += (clk_inc1);                                \
      tmp_addr = (addr);                                \
      CLK += (clk_inc2);                                \
      PUSH(((reg_pc + (pc_inc) - 1) >> 8) & 0xff);      \
      PUSH((reg_pc + (pc_inc) - 1) & 0xff);             \
      JUMP(tmp_addr);                                   \
  } while (0)

#define LAS(value, clk_inc1, clk_inc2, pc_inc)          \
  do {                                                  \
      UNDOC_WARNING();                                  \
      CLK += (clk_inc1);                                \
      reg_a = reg_x = reg_sp = reg_sp & (value);        \
      LOCAL_SET_NZ(reg_a);                              \
      CLK += (clk_inc2);                                \
      INC_PC(pc_inc);                                   \
  } while (0)

#define LAX(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      UNDOC_WARNING();                          \
      CLK += (clk_inc1);                        \
      reg_a = reg_x = (value);                  \
      LOCAL_SET_NZ(reg_a);                      \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDA(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_a = (value);                          \
      CLK += (clk_inc2);                        \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDX(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_x = (value);                          \
      LOCAL_SET_NZ(reg_x);                      \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDY(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_y = (value);                          \
      LOCAL_SET_NZ(reg_y);                      \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LSR(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      tmp = load_func(tmp_addr);                                        \
      INC_PC(pc_inc);                                                   \
      LOCAL_SET_CARRY(tmp & 0x01);                                      \
      tmp >>= 1;                                                        \
      LOCAL_SET_NZ(tmp);                                                \
      store_func(tmp_addr, tmp, clk_inc2);                              \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define LSR_A()                                 \
  do {                                          \
      CLK += 2;                                 \
      LOCAL_SET_CARRY(reg_a & 0x01);            \
      reg_a >>= 1;                              \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

/* Note: this is not always exact, as this opcode can be quite unstable!
   Moreover, the behavior is different from the one described in 64doc. */
#define LXA(value, clk_inc1, clk_inc2, pc_inc)                  \
  do {                                                          \
      UNDOC_WARNING();                                          \
      CLK += (clk_inc1);                                        \
      reg_a = reg_x = ((reg_a | 0xee) & (value));               \
      LOCAL_SET_NZ(reg_a);                                      \
      CLK += (clk_inc2);                                        \
      INC_PC(pc_inc);                                           \
  } while (0)

#define ORA(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_a |= (value);                         \
      LOCAL_SET_NZ(reg_a);                      \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define NOOP(clk_inc, pc_inc)                   \
  (CLK += (clk_inc), INC_PC(pc_inc))

#define NOOP_ABS()                              \
  do {                                          \
      CLK += 3;                                 \
      LOAD(p2);                                 \
      CLK++;                                    \
      INC_PC(3);                                \
  } while (0)

#define NOOP_ABS_X()                            \
  do {                                          \
      CLK += 3;                                 \
      LOAD_ABS_X(p2);                           \
      CLK++;                                    \
      INC_PC(3);                                \
  } while (0)

#define NOP()  NOOP(2, 1)

#define PHA()                                   \
  do {                                          \
      CLK += 3;                                 \
      PUSH(reg_a);                              \
      INC_PC(1);                                \
  } while (0)

#define PHP()                                   \
  do {                                          \
      CLK += 3;                                 \
      PUSH(LOCAL_STATUS() | P_BREAK);           \
      INC_PC(1);                                \
  } while (0)

#define PLA()                                   \
  do {                                          \
      CLK += 4;                                 \
      reg_a = PULL();                           \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define PLP()                                           \
  do {                                                  \
      BYTE s = PULL();                                  \
                                                        \
      if (!(s & P_INTERRUPT) && LOCAL_INTERRUPT())      \
          OPCODE_ENABLES_IRQ();                         \
      else if ((s & P_INTERRUPT) && !LOCAL_INTERRUPT()) \
          OPCODE_DISABLES_IRQ();                        \
      CLK += 4;                                         \
      LOCAL_SET_STATUS(s);                              \
      INC_PC(1);                                        \
  } while (0)

#define RLA(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      UNDOC_WARNING();                                                  \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      tmp = ((load_func(tmp_addr) << 1) | (reg_p & P_CARRY));           \
      LOCAL_SET_CARRY(tmp & 0x100);                                     \
      reg_a &= tmp;                                                     \
      LOCAL_SET_NZ(reg_a);                                              \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp, clk_inc2);                              \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define RLA_IND_Y(addr)                                         \
  do {                                                          \
      unsigned int tmp;                                         \
      unsigned int tmp_addr = LOAD_ZERO_ADDR(addr);             \
                                                                \
      UNDOC_WARNING();                                          \
      RMW_FLAG = 1;                                             \
      CLK += 5;                                                 \
      LOAD((tmp_addr & 0xff00) | ((tmp_addr + reg_y) & 0xff));  \
      CLK++;                                                    \
      tmp_addr += reg_y;                                        \
      tmp = ((LOAD(tmp_addr) << 1) | (reg_p & P_CARRY));        \
      LOCAL_SET_CARRY(tmp & 0x100);                             \
      reg_a &= tmp;                                             \
      LOCAL_SET_NZ(reg_a);                                      \
      INC_PC(2);                                                \
      STORE_ABS(tmp_addr, tmp, 2);                              \
      RMW_FLAG = 1;                                             \
  } while (0)

#define ROL(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int tmp, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      CLK += (clk_inc1);                                                \
      tmp = load_func(tmp_addr);                                        \
      tmp = (tmp << 1) | (reg_p & P_CARRY);                             \
      LOCAL_SET_CARRY(tmp & 0x100);                                     \
      LOCAL_SET_NZ(tmp & 0xff);                                         \
      RMW_FLAG = 1;                                                     \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp, clk_inc2);                              \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define ROL_A()                                 \
  do {                                          \
      unsigned int tmp = reg_a << 1;            \
                                                \
      CLK += 2;                                 \
      reg_a = tmp | (reg_p & P_CARRY);          \
      LOCAL_SET_CARRY(tmp & 0x100);             \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define ROR(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      unsigned int src, tmp_addr;                                       \
                                                                        \
      tmp_addr = (addr);                                                \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      src = load_func(tmp_addr);                                        \
      if (reg_p & P_CARRY)                                              \
          src |= 0x100;                                                 \
      LOCAL_SET_CARRY(src & 0x01);                                      \
      src >>= 1;                                                        \
      LOCAL_SET_NZ(src);                                                \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, src, (clk_inc2));                            \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define ROR_A()                                 \
  do {                                          \
      BYTE tmp = reg_a;                         \
                                                \
      CLK += 2;                                 \
      reg_a = (reg_a >> 1) | (reg_p << 7);      \
      LOCAL_SET_CARRY(tmp & 0x01);              \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define RRA(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      BYTE src;                                                         \
      unsigned int my_temp, tmp_addr;                                   \
                                                                        \
      UNDOC_WARNING();                                                  \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      tmp_addr = (addr);                                                \
      src = load_func(tmp_addr);                                        \
      INC_PC(pc_inc);                                                   \
      my_temp = src >> 1;                                               \
      if (reg_p & P_CARRY)                                              \
          my_temp |= 0x80;                                              \
      LOCAL_SET_CARRY(src & 0x1);                                       \
      ADC(my_temp, 0, 0, 0);                                            \
      store_func(tmp_addr, my_temp, clk_inc2);                          \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define RRA_IND_Y(addr)                                                 \
  do {                                                                  \
      BYTE src;                                                         \
      unsigned int my_tmp_addr;                                         \
      unsigned int my_temp;                                             \
                                                                        \
      UNDOC_WARNING();                                                  \
      RMW_FLAG = 1;                                                     \
      CLK += 5;                                                         \
      my_tmp_addr = LOAD_ZERO_ADDR(addr);                               \
      LOAD((my_tmp_addr & 0xff00) | ((my_tmp_addr + reg_y) & 0xff));    \
      CLK++;                                                            \
      my_tmp_addr += reg_y;                                             \
      src = LOAD(my_tmp_addr);                                          \
      INC_PC(2);                                                        \
      my_temp = src >> 1;                                               \
      if (reg_p & P_CARRY)                                              \
          my_temp |= 0x80;                                              \
      LOCAL_SET_CARRY(src & 0x1);                                       \
      ADC(my_temp, 0, 0, 0);                                            \
      STORE_ABS(my_tmp_addr, my_temp, 2);                               \
      RMW_FLAG = 0;                                                     \
  } while (0)

/* RTI does must not use `OPCODE_ENABLES_IRQ()' even if the I flag changes
   from 1 to 0 because the value of I is set 3 cycles before the end of the
   opcode, and thus the 6510 has enough time to call the interrupt routine as
   soon as the opcode ends, if necessary.  */
#define RTI()                                   \
  do {                                          \
      WORD tmp;                                 \
                                                \
      CLK += 6;                                 \
      tmp = (WORD) PULL();                      \
      LOCAL_SET_STATUS((BYTE) tmp);             \
      tmp = (WORD) PULL();                      \
      tmp |= (WORD) PULL() << 8;                \
      JUMP(tmp);                                \
  } while (0)

#define RTS()                                   \
  do {                                          \
      WORD tmp;                                 \
                                                \
      CLK += 6;                                 \
      tmp = PULL();                             \
      tmp = (tmp | (PULL() << 8)) + 1;          \
      JUMP(tmp);                                \
  } while (0)


#define SAX(addr, clk_inc1, clk_inc2, pc_inc)   \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      UNDOC_WARNING();                          \
      CLK += (clk_inc1);                        \
      tmp = (addr);                             \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
      STORE(tmp, reg_a & reg_x);                \
  } while (0)

#define SAX_ZERO(addr, clk_inc, pc_inc)         \
  do {                                          \
      UNDOC_WARNING();                          \
      CLK += (clk_inc);                         \
      INC_PC(pc_inc);                           \
      STORE_ZERO((addr), reg_a & reg_x);        \
  } while (0)

#define SBC(value, clk_inc1, clk_inc2, pc_inc)                               \
  do {                                                                       \
      WORD src, tmp;                                                         \
                                                                             \
      CLK += (clk_inc1);                                                     \
      src = (value);                                                         \
      CLK += (clk_inc2);                                                     \
      tmp = reg_a - src - ((reg_p & P_CARRY) ? 0 : 1);                       \
      if (reg_p & P_DECIMAL) {                                               \
          unsigned int tmp_a;                                                \
          tmp_a = (reg_a & 0xf) - (src & 0xf) - ((reg_p & P_CARRY) ? 0 : 1); \
          if (tmp_a & 0x10)                                                  \
              tmp_a = ((tmp_a - 6) & 0xf)                                    \
                       | ((reg_a & 0xf0) - (src & 0xf0) - 0x10);             \
          else                                                               \
              tmp_a = (tmp_a & 0xf) | ((reg_a & 0xf0) - (src & 0xf0));       \
          if (tmp_a & 0x100)                                                 \
              tmp_a -= 0x60;                                                 \
          LOCAL_SET_CARRY(tmp < 0x100);                                      \
          LOCAL_SET_NZ(tmp & 0xff);                                          \
          LOCAL_SET_OVERFLOW(((reg_a ^ tmp) & 0x80)                          \
                             && ((reg_a ^ src) & 0x80));                     \
          reg_a = (BYTE) tmp_a;                                              \
      } else {                                                               \
          LOCAL_SET_NZ(tmp & 0xff);                                          \
          LOCAL_SET_CARRY(tmp < 0x100);                                      \
          LOCAL_SET_OVERFLOW(((reg_a ^ tmp) & 0x80)                          \
                             && ((reg_a ^ src) & 0x80));                     \
          reg_a = (BYTE) tmp;                                                \
      }                                                                      \
      INC_PC(pc_inc);                                                        \
    }                                                                        \
  while (0)

#define SBX(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      UNDOC_WARNING();                          \
      CLK += (clk_inc1);                        \
      tmp = (value);                            \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
      tmp = (reg_a & reg_x) - tmp;              \
      LOCAL_SET_CARRY(tmp < 0x100);             \
      reg_x = tmp & 0xff;                       \
      LOCAL_SET_NZ(reg_x);                      \
  } while (0)

#undef SEC                      /* defined in time.h on SunOS. */
#define SEC()                                   \
  do {                                          \
      CLK += 2;                                 \
      LOCAL_SET_CARRY(1);                       \
      INC_PC(1);                                \
  } while (0)

#define SED()                                   \
  do {                                          \
      CLK += 2;                                 \
      LOCAL_SET_DECIMAL(1);                     \
      INC_PC(1);                                \
  } while (0)

#define SEI()                                   \
  do {                                          \
      CLK += 2;                                 \
      if (!LOCAL_INTERRUPT())                   \
          OPCODE_DISABLES_IRQ();                \
      LOCAL_SET_INTERRUPT(1);                   \
      INC_PC(1);                                \
  } while (0)

#define SHA_ABS_Y(addr)                                                 \
  do {                                                                  \
      unsigned int tmp;                                                 \
                                                                        \
      UNDOC_WARNING();                                                  \
      tmp = (addr);                                                     \
      INC_PC(3);                                                        \
      STORE_ABS_Y(tmp, reg_a & reg_x & (((tmp + reg_y) >> 8) + 1), 5);  \
  } while (0)

#define SHA_IND_Y(addr)                                 \
  do {                                                  \
      unsigned int tmp;                                 \
                                                        \
      UNDOC_WARNING();                                  \
      CLK += 5;                                         \
      tmp = LOAD_ZERO_ADDR(addr);                       \
      LOAD((tmp & 0xff00) | ((tmp + reg_y) & 0xff));    \
      CLK++;                                            \
      tmp += reg_y;                                     \
      INC_PC(2);                                        \
      STORE(tmp, reg_a & reg_x & ((tmp >> 8) + 1));     \
  } while (0)

#define SHX_ABS_Y(addr)                                         \
  do {                                                          \
      unsigned int tmp;                                         \
                                                                \
      UNDOC_WARNING();                                          \
      tmp = (addr);                                             \
      INC_PC(3);                                                \
      STORE_ABS_Y(tmp, reg_x & (((tmp + reg_y) >> 8) + 1), 5);  \
  } while (0)

#define SHY_ABS_X(addr)                                         \
  do {                                                          \
      unsigned int tmp;                                         \
                                                                \
      UNDOC_WARNING();                                          \
      tmp = (addr);                                             \
      INC_PC(3);                                                \
      STORE_ABS_X(tmp, reg_y & (((tmp + reg_x) >> 8) + 1), 5);  \
  } while (0)

#define SHS_ABS_Y(addr)                                                 \
  do {                                                                  \
      int tmp = (addr);                                                 \
                                                                        \
      UNDOC_WARNING();                                                  \
      INC_PC(3);                                                        \
      STORE_ABS_Y(tmp, reg_a & reg_x & (((tmp + reg_y) >> 8) + 1), 5);  \
      reg_sp = reg_a & reg_x;                                           \
  } while (0)

#define SLO(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      BYTE tmp_value;                                                   \
      int tmp_addr;                                                     \
                                                                        \
      UNDOC_WARNING();                                                  \
      CLK += (clk_inc1);                                                \
      tmp_addr = (addr);                                                \
      tmp_value = load_func(tmp_addr);                                  \
      RMW_FLAG = 1;                                                     \
      LOCAL_SET_CARRY(tmp_value & 0x80);                                \
      tmp_value <<= 1;                                                  \
      reg_a |= tmp_value;                                               \
      LOCAL_SET_NZ(reg_a);                                              \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp_value, clk_inc2);                        \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define SLO_IND_Y(addr)                                         \
  do {                                                          \
      BYTE tmp_value;                                           \
      unsigned int tmp_addr;                                    \
                                                                \
      UNDOC_WARNING();                                          \
      CLK += 5;                                                 \
      tmp_addr = LOAD_ZERO_ADDR(addr);                          \
      LOAD((tmp_addr & 0xff00) | ((tmp_addr + reg_y) & 0xff));  \
      CLK++;                                                    \
      tmp_addr += reg_y;                                        \
      tmp_value = LOAD(tmp_addr);                               \
      RMW_FLAG = 1;                                             \
      LOCAL_SET_CARRY(tmp_value & 0x80);                        \
      tmp_value <<= 1;                                          \
      reg_a |= tmp_value;                                       \
      LOCAL_SET_NZ(reg_a);                                      \
      INC_PC(2);                                                \
      STORE_ABS(tmp_addr, tmp_value, 2);                        \
      RMW_FLAG = 0;                                             \
  } while (0)

#define SRE(addr, clk_inc1, clk_inc2, pc_inc, load_func, store_func)    \
  do {                                                                  \
      BYTE tmp;                                                         \
      unsigned int tmp_addr;                                            \
                                                                        \
      UNDOC_WARNING();                                                  \
      RMW_FLAG = 1;                                                     \
      CLK += (clk_inc1);                                                \
      tmp_addr = (addr);                                                \
      tmp = load_func(tmp_addr);                                        \
      LOCAL_SET_CARRY(tmp & 0x01);                                      \
      tmp >>= 1;                                                        \
      reg_a ^= tmp;                                                     \
      LOCAL_SET_NZ(reg_a);                                              \
      INC_PC(pc_inc);                                                   \
      store_func(tmp_addr, tmp, clk_inc2);                              \
      RMW_FLAG = 0;                                                     \
  } while (0)

#define SRE_IND_Y(addr)                                         \
  do {                                                          \
      BYTE tmp;                                                 \
      unsigned int tmp_addr = LOAD_ZERO_ADDR(addr);             \
                                                                \
      UNDOC_WARNING();                                          \
      RMW_FLAG = 1;                                             \
      CLK += 5;                                                 \
      LOAD((tmp_addr & 0xff00) | ((tmp_addr + reg_y) & 0xff));  \
      CLK++;                                                    \
      tmp_addr += reg_y;                                        \
      tmp = LOAD(tmp_addr);                                     \
      LOCAL_SET_CARRY(tmp & 0x01);                              \
      tmp >>= 1;                                                \
      reg_a ^= tmp;                                             \
      LOCAL_SET_NZ(reg_a);                                      \
      INC_PC(2);                                                \
      STORE_ABS(tmp_addr, tmp, 2);                              \
      RMW_FLAG = 0;                                             \
  } while (0)

#define STA(addr, clk_inc1, clk_inc2, pc_inc, store_func)       \
  do {                                                          \
      unsigned int tmp;                                         \
                                                                \
      CLK += (clk_inc1);                                        \
      tmp = (addr);                                             \
      INC_PC(pc_inc);                                           \
      store_func(tmp, reg_a, clk_inc2);                         \
  } while (0)

#define STA_ZERO(addr, clk_inc, pc_inc)         \
  do {                                          \
      CLK += (clk_inc);                         \
      STORE_ZERO((addr), reg_a);                \
      INC_PC(pc_inc);                           \
  } while (0)

#define STA_IND_Y(addr)                                 \
  do {                                                  \
      unsigned int tmp;                                 \
                                                        \
      CLK += 5;                                         \
      tmp = LOAD_ZERO_ADDR(addr);                       \
      LOAD_IND((tmp & 0xff00) | ((tmp + reg_y) & 0xff));\
      CLK++;                                            \
      INC_PC(2);                                        \
      STORE_IND(tmp + reg_y, reg_a);                    \
  } while (0)

#define STX(addr, clk_inc1, clk_inc2, pc_inc)   \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = (addr);                             \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
      STORE(tmp, reg_x);                        \
  } while (0)

#define STX_ZERO(addr, clk_inc, pc_inc)         \
  do {                                          \
      CLK += (clk_inc);                         \
      STORE_ZERO((addr), reg_x);                \
      INC_PC(pc_inc);                           \
  } while (0)

#define STY(addr, clk_inc1, clk_inc2, pc_inc)   \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = (addr);                             \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
      STORE(tmp, reg_y);                        \
  } while (0)

#define STY_ZERO(addr, clk_inc, pc_inc)         \
  do {                                          \
      CLK += (clk_inc);                         \
      STORE_ZERO((addr), reg_y);                \
      INC_PC(pc_inc);                           \
  } while (0)

#define TAX()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_x = reg_a;                            \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define TAY()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_y = reg_a;                            \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define TSX()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_x = reg_sp;                           \
      LOCAL_SET_NZ(reg_sp);                     \
      INC_PC(1);                                \
  } while (0)

#define TXA()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_a = reg_x;                            \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)

#define TXS()                                   \
  do {                                          \
      reg_sp = reg_x;                           \
      CLK += 2;                                 \
      INC_PC(1);                                \
  } while (0)

#define TYA()                                   \
  do {                                          \
      CLK += 2;                                 \
      reg_a = reg_y;                            \
      LOCAL_SET_NZ(reg_a);                      \
      INC_PC(1);                                \
  } while (0)


/* ------------------------------------------------------------------------- */

#if !defined FETCH_OPCODE

#  if !defined WORDS_BIGENDIAN && defined ALLOW_UNALIGNED_ACCESS

#    define opcode_t DWORD
#    define FETCH_OPCODE(o) ((o) = (bank_base                              \
                                    ? (*((DWORD *)(bank_base + reg_pc))    \
                                       & 0xffffff)                         \
                                    : (LOAD(reg_pc)                        \
                                       | (LOAD(reg_pc + 1) << 8)           \
                                       | (LOAD(reg_pc + 2) << 16))))
#    define p0 (opcode & 0xff)
#    define p1 ((opcode >> 8) & 0xff)
#    define p2 (opcode >> 8)

#  else /* WORDS_BIGENDIAN || ALLOW_UNALIGNED_ACCESS */

#    define opcode_t                              \
        struct {                                  \
            BYTE ins;                             \
            union {                               \
                BYTE op8[2];                      \
                WORD op16;                        \
            } op;                                 \
        }

#    define FETCH_OPCODE(o)                                                  \
          (bank_base ? ((o).ins = *(bank_base + reg_pc),                     \
                        (o).op.op16 = (*(bank_base + reg_pc + 1)             \
                                       | (*(bank_base + reg_pc + 2) << 8)))  \
                     : ((o).ins = LOAD(reg_pc),                              \
                        (o).op.op16 = (LOAD(reg_pc + 1)                      \
                                       | (LOAD(reg_pc + 2) << 8))))

#    define p0 (opcode.ins)
#    define p2 (opcode.op.op16)

#    ifdef WORDS_BIGENDIAN
#      define p1 (opcode.op.op8[1])
#    else
#      define p1 (opcode.op.op8[0])
#    endif

#  endif /* !WORDS_BIGENDIAN */

#endif /* !FETCH_OPCODE */

/* ------------------------------------------------------------------------ */


/* Here, the CPU is emulated. */

{
    while (CLK >= next_alarm_clk(&CPU_INT_STATUS))
        serve_next_alarm(&CPU_INT_STATUS, CLK);

    {
        enum cpu_int pending_interrupt;

        pending_interrupt = check_pending_interrupt(&CPU_INT_STATUS);
        if (pending_interrupt != IK_NONE) {
            DO_INTERRUPT(pending_interrupt);
            while (CLK >= next_alarm_clk(&CPU_INT_STATUS))
                serve_next_alarm(&CPU_INT_STATUS, CLK);
        }
    }

#ifdef EXECUTE_EXTERN
    EXECUTE_EXTERN();
#endif

    {
/* CPU128 doesn't need this variable */
#ifndef _C128CPU_C
        opcode_t opcode;
#endif

        FETCH_OPCODE(opcode);

#if defined(TRACE)
#ifdef __1541__
        if (TRACEFLG) {
            BYTE op = p0;
            BYTE lo = p1;
            BYTE hi = p2 >> 8;

            fprintf(logfile, "DRIVE: .%04X\t%ld\t%s\n",
                   reg_pc, (long)drive_clk[0],
                   sprint_disassembled(reg_pc, op, lo, hi, 1));
        }
#else
        if (TRACEFLG) {
            BYTE op = p0;
            BYTE lo = p1;
            BYTE hi = p2 >> 8;

            fprintf(logfile, ".%04X %02x %02x %02x\t%ld\t%s\tA=$%02X X=$%02X Y=$%02X\n",
                  reg_pc, op, lo, hi,
                  (long)clk, sprint_opcode(reg_pc, 1),
                  reg_a, reg_x, reg_y);
       }
#endif
#endif

        SET_LAST_OPCODE(p0);

        switch (p0) {

#ifndef ONLY_LDA_BNE
        case 0x00:                      /* BRK */
            BRK();
            break;

        case 0x01:                      /* ORA ($nn,X) */
            ORA(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0x02:                      /* JAM */
        case 0x12:                      /* JAM */
        case 0x22:                      /* JAM */
        case 0x32:                      /* JAM */
        case 0x42:                      /* JAM */
        case 0x52:                      /* JAM */
        case 0x62:                      /* JAM */
        case 0x72:                      /* JAM */
        case 0x92:                      /* JAM */
        case 0xb2:                      /* JAM */
        case 0xd2:                      /* JAM */
        case 0xf2:                      /* JAM */
            JAM();
            break;

        case 0x03:                      /* SLO ($nn,X) */
            SLO(LOAD_ZERO_ADDR(p1 + reg_x), 2, 6, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0x04:                      /* NOOP $nn */
            NOOP(3, 2);
            break;

        case 0x05:                      /* ORA $nn */
            ORA(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0x06:                      /* ASL $nn */
            ASL(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x07:                      /* SLO $nn */
            SLO(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x08:                      /* PHP */
#ifdef __1541__
            if (_drive_byte_ready())
                LOCAL_SET_OVERFLOW(1);
#endif
            PHP();
            break;

        case 0x09:                      /* ORA #$nn */
            ORA(p1, 2, 0, 2);
            break;

        case 0x0a:                      /* ASL A */
            ASL_A();
            break;

        case 0x0b:                      /* ANC #$nn */
            ANC(p1, 2, 0, 2);
            break;

        case 0x0c:                      /* NOOP $nnnn */
            NOOP_ABS();
            break;

        case 0x0d:                      /* ORA $nnnn */
            ORA(LOAD(p2), 3, 1, 3);
            break;

        case 0x0e:                      /* ASL $nnnn */
            ASL(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x0f:                      /* SLO $nnnn */
            SLO(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x10:                      /* BPL $nnnn */
            BRANCH(!LOCAL_SIGN(), p1);
            break;

        case 0x11:                      /* ORA ($nn),Y */
            ORA(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0x13:                      /* SLO ($nn),Y */
            SLO_IND_Y(p1);
            break;

        case 0x14:                      /* NOOP $nn,X */
            NOOP(4, 2);
            break;

        case 0x15:                      /* ORA $nn,X */
            ORA(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0x16:                      /* ASL $nn,X */
            ASL((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x17:                      /* SLO $nn,X */
            SLO((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x18:                      /* CLC */
            CLC();
            break;

        case 0x19:                      /* ORA $nnnn,Y */
            ORA(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0x1a:                      /* NOOP */
            NOOP(2, 1);
            break;

        case 0x1b:                      /* SLO $nnnn,Y */
            SLO(p2, 3, 3, 3, LOAD_ABS_Y_RMW, STORE_ABS_Y_RMW);
            break;

        case 0x1c:                      /* NOOP $nnnn,X */
            NOOP_ABS_X();
            break;

        case 0x1d:                      /* ORA $nnnn,X */
            ORA(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0x1e:                      /* ASL $nnnn,X */
            ASL(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x1f:                      /* SLO $nnnn,X */
            SLO(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x20:                      /* JSR $nnnn */
            JSR(p2, 3, 3, 3);
            break;

        case 0x21:                      /* AND ($nn,X) */
            AND(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0x23:                      /* RLA ($nn,X) */
            RLA(LOAD_ZERO_ADDR(p1 + reg_x), 2, 6, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0x24:                      /* BIT $nn */
            BIT(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0x25:                      /* AND $nn */
            AND(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0x26:                      /* ROL $nn */
            ROL(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x27:                      /* RLA $nn */
            RLA(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x28:                      /* PLP */
            PLP();
            break;

        case 0x29:                      /* AND #$nn */
            AND(p1, 2, 0, 2);
            break;

        case 0x2a:                      /* ROL A */
            ROL_A();
            break;

        case 0x2b:                      /* ANC #$nn */
            ANC(p1, 2, 0, 2);
            break;

        case 0x2c:                      /* BIT $nnnn */
            BIT(LOAD(p2), 3, 1, 3);
            break;

        case 0x2d:                      /* AND $nnnn */
            AND(LOAD(p2), 3, 1, 3);
            break;

        case 0x2e:                      /* ROL $nnnn */
            ROL(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x2f:                      /* RLA $nnnn */
            RLA(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x30:                      /* BMI $nnnn */
            BRANCH(LOCAL_SIGN(), p1);
            break;

        case 0x31:                      /* AND ($nn),Y */
            AND(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0x33:                      /* RLA ($nn),Y */
            RLA_IND_Y(p1);
            break;

        case 0x34:                      /* NOOP */
            NOOP(4, 2);
            break;

        case 0x35:                      /* AND $nn,X */
            AND(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0x36:                      /* ROL $nn,X */
            ROL((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x37:                      /* RLA $nn,X */
            RLA((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x38:                      /* SEC */
            SEC();
            break;

        case 0x39:                      /* AND $nnnn,Y */
            AND(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0x3a:                      /* NOOP */
            NOOP(2, 1);
            break;

        case 0x3b:                      /* RLA $nnnn,Y */
            RLA(p2, 3, 3, 3, LOAD_ABS_Y_RMW, STORE_ABS_Y_RMW);
            break;

        case 0x3c:                      /* NOOP $nnnn,X */
            NOOP_ABS_X();
            break;

        case 0x3d:                      /* AND $nnnn,X */
            AND(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0x3e:                      /* ROL $nnnn,X */
            ROL(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x3f:                      /* RLA $nnnn,X */
            RLA(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x40:                      /* RTI */
            RTI();
            break;

        case 0x41:                      /* EOR ($nn,X) */
            EOR(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0x43:                      /* SRE ($nn,X) */
            SRE(LOAD_ZERO_ADDR(p1 + reg_x), 2, 6, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0x44:                      /* NOOP $nn */
            NOOP(3, 2);
            break;

        case 0x45:                      /* EOR $nn */
            EOR(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0x46:                      /* LSR $nn */
            LSR(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x47:                      /* SRE $nn */
            SRE(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x48:                      /* PHA */
            PHA();
            break;

        case 0x49:                      /* EOR #$nn */
            EOR(p1, 2, 0, 2);
            break;

        case 0x4a:                      /* LSR A */
            LSR_A();
            break;

        case 0x4b:                      /* ASR #$nn */
            ASR(p1, 2, 0, 2);
            break;

        case 0x4c:                      /* JMP $nnnn */
            JMP(p2, 3, 0);
            break;

        case 0x4d:                      /* EOR $nnnn */
            EOR(LOAD(p2), 3, 1, 3);
            break;

        case 0x4e:                      /* LSR $nnnn */
            LSR(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x4f:                      /* SRE $nnnn */
            SRE(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x50:                      /* BVC $nnnn */
#ifndef __1541__
            BRANCH(!LOCAL_OVERFLOW(), p1);
#else
            if (_drive_byte_ready())
                LOCAL_SET_OVERFLOW(1);
            BRANCH(!LOCAL_OVERFLOW(), p1);
#endif
            break;

        case 0x51:                      /* EOR ($nn),Y */
            EOR(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0x53:                      /* SRE ($nn),Y */
            SRE_IND_Y(p1);
            break;

        case 0x54:                      /* NOOP $nn,X */
            NOOP(4, 2);
            break;

        case 0x55:                      /* EOR $nn,X */
            EOR(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0x56:                      /* LSR $nn,X */
            LSR((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x57:                      /* SRE $nn,X */
            SRE((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x58:                      /* CLI */
            CLI();
            break;

        case 0x59:                      /* EOR $nnnn,Y */
            EOR(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0x5a:                      /* NOOP */
            NOOP(2, 1);
            break;

        case 0x5b:                      /* SRE $nnnn,Y */
            SRE(p2, 3, 3, 3, LOAD_ABS_Y_RMW, STORE_ABS_Y_RMW);
            break;

        case 0x5c:                      /* NOOP $nnnn,X */
            NOOP_ABS_X();
            break;

        case 0x5d:                      /* EOR $nnnn,X */
            EOR(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0x5e:                      /* LSR $nnnn,X */
            LSR(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x5f:                      /* SRE $nnnn,X */
            SRE(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x60:                      /* RTS */
            RTS();
            break;

        case 0x61:                      /* ADC ($nn,X) */
            ADC(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0x63:                      /* RRA ($nn,X) */
            RRA(LOAD_ZERO_ADDR(p1 + reg_x), 2, 6, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0x64:                      /* NOOP $nn */
            NOOP(3, 2);
            break;

        case 0x65:                      /* ADC $nn */
            ADC(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0x66:                      /* ROR $nn */
            ROR(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x67:                      /* RRA $nn */
            RRA(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x68:                      /* PLA */
            PLA();
            break;

        case 0x69:                      /* ADC #$nn */
            ADC(p1, 2, 0, 2);
            break;

        case 0x6a:                      /* ROR A */
            ROR_A();
            break;

        case 0x6b:                      /* ARR #$nn */
            ARR(p1, 2, 0, 2);
            break;

        case 0x6c:                      /* JMP ($nnnn) */
            /* FIXME: This is not correct (timing-wise).  */
            JMP(LOAD(p2) | (LOAD((p2 & 0xff00) | ((p2 + 1) & 0xff)) << 8),
                3, 2);
            break;

        case 0x6d:                      /* ADC $nnnn */
            ADC(LOAD(p2), 3, 1, 3);
            break;

        case 0x6e:                      /* ROR $nnnn */
            ROR(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x6f:                      /* RRA $nnnn */
            RRA(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0x70:                      /* BVS $nnnn */
#ifndef __1541__
            BRANCH(LOCAL_OVERFLOW(), p1);
#else
            if (_drive_byte_ready())
                LOCAL_SET_OVERFLOW(1);
            BRANCH(LOCAL_OVERFLOW(), p1);
#endif
            break;

        case 0x71:                      /* ADC ($nn),Y */
            ADC(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0x73:                      /* RRA ($nn),Y */
            RRA_IND_Y(p1);
            break;

        case 0x74:                      /* NOOP $nn,X */
            NOOP(4, 2);
            break;

        case 0x75:                      /* ADC $nn,X */
            ADC(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0x76:                      /* ROR $nn,X */
            ROR((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x77:                      /* RRA $nn,X */
            RRA((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0x78:                      /* SEI */
            SEI();
            break;

        case 0x79:                      /* ADC $nnnn,Y */
            ADC(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0x7a:                      /* NOOP */
            NOOP(2, 1);
            break;

        case 0x7b:                      /* RRA $nnnn,Y */
            RRA(p2, 3, 3, 3, LOAD_ABS_Y_RMW, STORE_ABS_Y_RMW);
            break;

        case 0x7c:                      /* NOOP $nnnn,X */
            NOOP_ABS_X();
            break;

        case 0x7d:                      /* ADC $nnnn,X */
            ADC(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0x7e:                      /* ROR $nnnn,X */
            ROR(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x7f:                      /* RRA $nnnn,X */
            RRA(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0x80:                      /* NOOP #$nn */
            NOOP(2, 2);
            break;

        case 0x81:                      /* STA ($nn,X) */
            STA(LOAD_ZERO_ADDR(p1 + reg_x), 2, 4, 2, STORE_ABS);
            break;

        case 0x82:                      /* NOOP #$nn */
            NOOP(2, 2);
            break;

        case 0x83:                      /* SAX ($nn,X) */
            SAX(LOAD_ZERO_ADDR(p1 + reg_x), 2, 4, 2);
            break;

        case 0x84:                      /* STY $nn */
            STY_ZERO(p1, 3, 2);
            break;

        case 0x85:                      /* STA $nn */
            STA_ZERO(p1, 3, 2);
            break;

        case 0x86:                      /* STX $nn */
            STX_ZERO(p1, 3, 2);
            break;

        case 0x87:                      /* SAX $nn */
            SAX_ZERO(p1, 3, 2);
            break;

        case 0x88:                      /* DEY */
            DEY();
            break;

        case 0x89:                      /* NOOP #$nn */
            NOOP(2, 2);
            break;

        case 0x8a:                      /* TXA */
            TXA();
            break;

        case 0x8b:                      /* ANE #$nn */
            ANE(p1, 2, 0, 2);
            break;

        case 0x8c:                      /* STY $nnnn */
            STY(p2, 3, 1, 3);
            break;

        case 0x8d:                      /* STA $nnnn */
            STA(p2, 3, 1, 3, STORE_ABS);
            break;

        case 0x8e:                      /* STX $nnnn */
            STX(p2, 3, 1, 3);
            break;

        case 0x8f:                      /* SAX $nnnn */
            SAX(p2, 3, 1, 3);
            break;

        case 0x90:                      /* BCC $nnnn */
            BRANCH(!LOCAL_CARRY(), p1);
            break;

        case 0x91:                      /* STA ($nn),Y */
            STA_IND_Y(p1);
            break;

        case 0x93:                      /* SHA ($nn),Y */
            SHA_IND_Y(p1);
            break;

        case 0x94:                      /* STY $nn,X */
            STY_ZERO(p1 + reg_x, 4, 2);
            break;

        case 0x95:                      /* STA $nn,X */
            STA_ZERO(p1 + reg_x, 4, 2);
            break;

        case 0x96:                      /* STX $nn,Y */
            STX_ZERO(p1 + reg_y, 4, 2);
            break;

        case 0x97:                      /* SAX $nn,Y */
            SAX((p1 + reg_y) & 0xff, 2, 2, 2);
            break;

        case 0x98:                      /* TYA */
            TYA();
            break;

        case 0x99:                      /* STA $nnnn,Y */
            STA(p2, 3, 2, 3, STORE_ABS_Y);
            break;

        case 0x9a:                      /* TXS */
            TXS();
            break;

        case 0x9b:                      /* SHS $nnnn,Y */
            SHS_ABS_Y(p2);
            break;

        case 0x9c:                      /* SHY $nnnn,X */
            SHY_ABS_X(p2);
            break;

        case 0x9d:                      /* STA $nnnn,X */
            STA(p2, 3, 2, 3, STORE_ABS_X);
            break;

        case 0x9e:                      /* SHX $nnnn,Y */
            SHX_ABS_Y(p2);
            break;

        case 0x9f:                      /* SHA $nnnn,Y */
            SHA_ABS_Y(p2);
            break;

        case 0xa0:                      /* LDY #$nn */
            LDY(p1, 2, 0, 2);
            break;

        case 0xa1:                      /* LDA ($nn,X) */
            LDA(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0xa2:                      /* LDX #$nn */
            LDX(p1, 2, 0, 2);
            break;

        case 0xa3:                      /* LAX ($nn,X) */
            LAX(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0xa4:                      /* LDY $nn */
            LDY(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xa5:                      /* LDA $nn */
            LDA(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xa6:                      /* LDX $nn */
            LDX(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xa7:                      /* LAX $nn */
            LAX(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xa8:                      /* TAY */
            TAY();
            break;

        case 0xa9:                      /* LDA #$nn */
            LDA(p1, 2, 0, 2);
            break;

        case 0xaa:                      /* TAX */
            TAX();
            break;

        case 0xab:                      /* LXA #$nn */
            LXA(p1, 2, 0, 2);
            break;

        case 0xac:                      /* LDY $nnnn */
            LDY(LOAD(p2), 3, 1, 3);
            break;

#endif /* !ONLY_LDA_BNE */

        case 0xad:                      /* LDA $nnnn */
            LDA(LOAD(p2), 3, 1, 3);
            break;

#ifndef ONLY_LDA_BNE
        case 0xae:                      /* LDX $nnnn */
            LDX(LOAD(p2), 3, 1, 3);
            break;

        case 0xaf:                      /* LAX $nnnn */
            LAX(LOAD(p2), 3, 1, 3);
            break;

        case 0xb0:                      /* BCS $nnnn */
            BRANCH(LOCAL_CARRY(), p1);
            break;

        case 0xb1:                      /* LDA ($nn),Y */
            LDA(LOAD_IND_Y_BANK(p1), 2, 3, 2);
            break;

        case 0xb3:                      /* LAX ($nn),Y */
            LAX(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0xb4:                      /* LDY $nn,X */
            LDY(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0xb5:                      /* LDA $nn,X */
            LDA(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0xb6:                      /* LDX $nn,Y */
            LDX(LOAD_ZERO_Y(p1), 2, 2, 2);
            break;

        case 0xb7:                      /* LAX $nn,Y */
            LAX(LOAD_ZERO_Y(p1), 2, 2, 2);
            break;

        case 0xb8:                      /* CLV */
            CLV();
            break;

        case 0xb9:                      /* LDA $nnnn,Y */
            LDA(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0xba:                      /* TSX */
            TSX();
            break;

        case 0xbb:                      /* LAS $nnnn,Y */
            LAS(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0xbc:                      /* LDY $nnnn,X */
            LDY(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0xbd:                      /* LDA $nnnn,X */
            LDA(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0xbe:                      /* LDX $nnnn,Y */
            LDX(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0xbf:                      /* LAX $nnnn,Y */
            LAX(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0xc0:                      /* CPY #$nn */
            CPY(p1, 2, 0, 2);
            break;

        case 0xc1:                      /* CMP ($nn,X) */
            CMP(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0xc2:                      /* NOOP #$nn */
            NOOP(2, 2);
            break;

        case 0xc3:                      /* DCP ($nn,X) */
            DCP(LOAD_ZERO_ADDR(p1 + reg_x), 2, 6, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0xc4:                      /* CPY $nn */
            CPY(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xc5:                      /* CMP $nn */
            CMP(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xc6:                      /* DEC $nn */
            DEC(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0xc7:                      /* DCP $nn */
            DCP(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0xc8:                      /* INY */
            INY();
            break;

        case 0xc9:                      /* CMP #$nn */
            CMP(p1, 2, 0, 2);
            break;

        case 0xca:                      /* DEX */
            DEX();
            break;

        case 0xcb:                      /* SBX #$nn */
            SBX(p1, 2, 0, 2);
            break;

        case 0xcc:                      /* CPY $nnnn */
            CPY(LOAD(p2), 3, 1, 3);
            break;

        case 0xcd:                      /* CMP $nnnn */
            CMP(LOAD(p2), 3, 1, 3);
            break;

        case 0xce:                      /* DEC $nnnn */
            DEC(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0xcf:                      /* DCP $nnnn */
            DCP(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;
#endif /* !ONLY_LDA_BNE */

        case 0xd0:                      /* BNE $nnnn */
            BRANCH(!LOCAL_ZERO(), p1);
            break;

#ifndef ONLY_LDA_BNE
        case 0xd1:                      /* CMP ($nn),Y */
            CMP(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0xd3:                      /* DCP ($nn),Y */
            DCP_IND_Y(p1);
            break;

        case 0xd4:                      /* NOOP $nn,X */
            NOOP(4, 2);
            break;

        case 0xd5:                      /* CMP $nn,X */
            CMP(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0xd6:                      /* DEC $nn,X */
            DEC((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0xd7:                      /* DCP $nn,X */
            DCP((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0xd8:                      /* CLD */
            CLD();
            break;

        case 0xd9:                      /* CMP $nnnn,Y */
            CMP(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0xda:                      /* NOOP */
            NOOP(2, 1);
            break;

        case 0xdb:                      /* DCP $nnnn,Y */
            DCP(p2, 3, 3, 3, LOAD_ABS_Y_RMW, STORE_ABS_Y_RMW);
            break;

        case 0xdc:                      /* NOOP $nnnn,X */
            NOOP_ABS_X();
            break;

        case 0xdd:                      /* CMP $nnnn,X */
            CMP(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0xde:                      /* DEC $nnnn,X */
            DEC(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0xdf:                      /* DCP $nnnn,X */
            DCP(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0xe0:                      /* CPX #$nn */
            CPX(p1, 2, 0, 2);
            break;

        case 0xe1:                      /* SBC ($nn,X) */
            SBC(LOAD_IND_X(p1), 2, 4, 2);
            break;

        case 0xe2:                      /* NOOP #$nn */
            NOOP(2, 2);
            break;

        case 0xe3:                      /* ISB ($nn,X) */
            ISB(LOAD_ZERO_ADDR(p1 + reg_x), 2, 6, 2, LOAD_ABS, STORE_ABS);
            break;

        case 0xe4:                      /* CPX $nn */
            CPX(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xe5:                      /* SBC $nn */
            SBC(LOAD_ZERO(p1), 2, 1, 2);
            break;

        case 0xe6:                      /* INC $nn */
            INC(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0xe7:                      /* ISB $nn */
            ISB(p1, 2, 3, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0xe8:                      /* INX */
            INX();
            break;

        case 0xe9:                      /* SBC #$nn */
            SBC(p1, 2, 0, 2);
            break;

        case 0xea:                      /* NOP */
            NOP();
            break;

        case 0xeb:                      /* USBC #$nn (same as SBC) */
            SBC(p1, 2, 0, 2);
            break;

        case 0xec:                      /* CPX $nnnn */
            CPX(LOAD(p2), 3, 1, 3);
            break;

        case 0xed:                      /* SBC $nnnn */
            SBC(LOAD(p2), 3, 1, 3);
            break;

        case 0xee:                      /* INC $nnnn */
            INC(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0xef:                      /* ISB $nnnn */
            ISB(p2, 3, 3, 3, LOAD_ABS, STORE_ABS);
            break;

        case 0xf0:                      /* BEQ $nnnn */
            BRANCH(LOCAL_ZERO(), p1);
            break;

        case 0xf1:                      /* SBC ($nn),Y */
            SBC(LOAD_IND_Y(p1), 2, 3, 2);
            break;

        case 0xf3:                      /* ISB ($nn),Y */
            ISB_IND_Y(p1);
            break;

        case 0xf4:                      /* NOOP ($nn,X) */
            NOOP(4, 2);
            break;

        case 0xf5:                      /* SBC $nn,X */
            SBC(LOAD_ZERO_X(p1), 2, 2, 2);
            break;

        case 0xf6:                      /* INC $nn,X */
            INC((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0xf7:                      /* ISB $nn,X */
            ISB((p1 + reg_x) & 0xff, 2, 4, 2, LOAD_ZERO, STORE_ABS);
            break;

        case 0xf8:                      /* SED */
            SED();
            break;

        case 0xf9:                      /* SBC $nnnn,Y */
            SBC(LOAD_ABS_Y(p2), 3, 1, 3);
            break;

        case 0xfa:                      /* NOOP */
            NOOP(2, 1);
            break;

        case 0xfb:                      /* ISB $nnnn,Y */
            ISB(p2, 3, 3, 3, LOAD_ABS_Y_RMW, STORE_ABS_Y_RMW);
            break;

        case 0xfc:                      /* NOOP $nnnn,X */
            NOOP_ABS_X();
            break;

        case 0xfd:                      /* SBC $nnnn,X */
            SBC(LOAD_ABS_X(p2), 3, 1, 3);
            break;

        case 0xfe:                      /* INC $nnnn,X */
            INC(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;

        case 0xff:                      /* ISB $nnnn,X */
            ISB(p2, 3, 3, 3, LOAD_ABS_X_RMW, STORE_ABS_X_RMW);
            break;
#endif /* !ONLY_LDA_BNE */
        }
    }
}
