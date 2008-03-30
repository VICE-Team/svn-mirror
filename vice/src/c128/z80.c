/*
 * z80.c
 *
 * Written by
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
 *
 */

#include "vice.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "alarm.h"
#include "interrupt.h"
#include "log.h"
#include "mon.h"
#include "z80.h"
#include "z80mem.h"

#define TRACE

static BYTE reg_a = 0;
static BYTE reg_b = 0;
static BYTE reg_c = 0;
static BYTE reg_d = 0;
static BYTE reg_e = 0;
static BYTE reg_f = 0;
static BYTE reg_h = 0;
static BYTE reg_l = 0;
/*static WORD reg_ix = 0;*/
/*static WORD reg_iy = 0;*/
static WORD reg_sp = 0;
static DWORD reg_pc = 0;
static BYTE reg_iff = 0;

static int dma_request = 0;

#define xit(ex_val)                                                    \
  do {                                                                 \
    log_message(LOG_DEFAULT,                                           \
                "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "      \
                "H%02x L%02x OP %02x %02x %02x.",                      \
                CLK, reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e, \
                reg_h, reg_l, p0, p1, p2);                             \
                exit(ex_val);                                          \
  } while (0)


void z80_trigger_dma(void)
{
    dma_request = 1;
}

void z80_reset(void)
{
    reg_pc = 0;
}

inline static BYTE *z80mem_read_base(int addr)
{
    BYTE *p = _z80mem_read_base_tab_ptr[addr >> 8];

    if (p == 0)
        return p;

    return p - (addr & 0xff00);
}

inline static int z80mem_read_limit(int addr)
{
    return z80mem_read_limit_tab_ptr[addr >> 8];
}

#define JUMP(addr)                            \
   do {                                       \
     reg_pc = (addr);                         \
     bank_base = z80mem_read_base(reg_pc);    \
     bank_limit = z80mem_read_limit(reg_pc);  \
     z80_old_reg_pc = reg_pc;                 \
   } while (0)


#define LOAD(addr) \
    (*_z80mem_read_tab_ptr[(addr) >> 8])((ADDRESS)(addr))

#define STORE(addr, value) \
    (*_z80mem_write_tab_ptr[(addr) >> 8])((ADDRESS)(addr), (BYTE)(value))


#define IN(addr) \
    (io_read_tab[(addr) >> 8])((ADDRESS)(addr))

#define OUT(addr, value) \
    (io_write_tab[(addr) >> 8])((ADDRESS)(addr), (BYTE)(value))

#define opcode_t DWORD
#define FETCH_OPCODE(o) ((o) = (LOAD(reg_pc)			\
                               | (LOAD(reg_pc + 1) << 8)	\
                               | (LOAD(reg_pc + 2) << 16)	\
                               | (LOAD(reg_pc + 3) << 24)))

#define p0 (opcode & 0xff)
#define p1 ((opcode >> 8) & 0xff)
#define p2 ((opcode >> 16) & 0xff)
#define p3 (opcode >> 24)

#define p12 ((opcode >> 8) & 0xffff)
#define p23 ((opcode >> 16) & 0xffff)

#define CLK clk

#define INC_PC(value)   (reg_pc += (value))

/* ------------------------------------------------------------------------- */

/* Word register manipulation.  */

#define BC_WORD() ((reg_b << 8) | reg_c)
#define DE_WORD() ((reg_d << 8) | reg_e)
#define HL_WORD() ((reg_h << 8) | reg_l)

#define DEC_BC_WORD() \
  do {                \
      if (!reg_c)     \
          reg_b--;    \
      reg_c--;        \
  } while (0)

#define DEC_DE_WORD() \
  do {                \
      if (!reg_e)     \
          reg_d--;    \
      reg_e--;        \
  } while (0)

#define DEC_HL_WORD() \
  do {                \
      if (!reg_l)     \
          reg_h--;    \
      reg_l--;        \
  } while (0)

#define INC_BC_WORD() \
  do {                \
      reg_c++;        \
      if (!reg_c)     \
          reg_b++;    \
  } while (0)

#define INC_DE_WORD() \
  do {                \
      reg_e++;        \
      if (!reg_e)     \
          reg_d++;    \
  } while (0)

#define INC_HL_WORD() \
  do {                \
      reg_l++;        \
      if (!reg_l)     \
          reg_h++;    \
  } while (0)

/* ------------------------------------------------------------------------- */

/* Flags.  */

#define C_FLAG 0x01
#define N_FLAG 0x02
#define P_FLAG 0x04
#define H_FLAG 0x10
#define Z_FLAG 0x40
#define S_FLAG 0x80

#define LOCAL_SET_CARRY(val)     ((val) ? (reg_f |= C_FLAG)       \
                                        : (reg_f &= ~C_FLAG))
#define LOCAL_SET_NADDSUB(val)   ((val) ? (reg_f |= N_FLAG)       \
                                        : (reg_f &= ~N_FLAG))
#define LOCAL_SET_PARITY(val)    ((val) ? (reg_f |= P_FLAG)       \
                                        : (reg_f &= ~P_FLAG))
#define LOCAL_SET_HALFCARRY(val) ((val) ? (reg_f |= H_FLAG)       \
                                        : (reg_f &= ~H_FLAG))
#define LOCAL_SET_ZERO(val)      ((val) ? (reg_f |= Z_FLAG)       \
                                        : (reg_f &= ~Z_FLAG))
#define LOCAL_SET_SIGN(val)      ((val) ? (reg_f |= S_FLAG)       \
                                        : (reg_f &= ~S_FLAG))

#define LOCAL_CARRY()     (reg_f & C_FLAG)
#define LOCAL_NADDSUB()   (reg_f & N_FLAG)
#define LOCAL_PARITY()    (reg_f & P_FLAG)
#define LOCAL_HALFCARRY() (reg_f & H_FLAG)
#define LOCAL_ZERO()      (reg_f & Z_FLAG)
#define LOCAL_SIGN()      (reg_f & S_FLAG)

static BYTE SZP[256] = {
    P_FLAG|Z_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
           P_FLAG,             0,             0,        P_FLAG,
           P_FLAG,             0,             0,        P_FLAG,
                0,        P_FLAG,        P_FLAG,             0,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
           S_FLAG, S_FLAG|P_FLAG, S_FLAG|P_FLAG,        S_FLAG,
    S_FLAG|P_FLAG,        S_FLAG,        S_FLAG, S_FLAG|P_FLAG };

/* ------------------------------------------------------------------------- */

/* Interrupt handling.  */

#define DO_INTERRUPT(int_kind)                                           \
  do {                                                                   \
        BYTE ik = (int_kind);                                            \
                                                                         \
        if (ik & (IK_TRAP | IK_RESET)) {                                 \
            if (ik & IK_TRAP) {                                          \
                do_trap(cpu_int_status, (ADDRESS) reg_pc);               \
                if (check_pending_interrupt(cpu_int_status) & IK_RESET)  \
                    ik |= IK_RESET;                                      \
            }                                                            \
            if (ik & IK_RESET) {                                         \
            }                                                            \
        }                                                                \
        if (ik & (IK_MONITOR)) {                                         \
            caller_space = e_comp_space;                                 \
            if (mon_mask[e_comp_space] & (MI_BREAK)) {                   \
               if (check_breakpoints(e_comp_space, (ADDRESS) reg_pc)) {  \
                  mon((ADDRESS) reg_pc);                                 \
               }                                                         \
            }                                                            \
            if (mon_mask[e_comp_space] & (MI_STEP)) {                    \
               mon_check_icount((ADDRESS) reg_pc);                       \
            }                                                            \
            if (mon_mask[e_comp_space] & (MI_WATCH)) {                   \
               mon_check_watchpoints((ADDRESS) reg_pc);                  \
            }                                                            \
        }                                                                \
                                                                         \
  } while (0)

/* ------------------------------------------------------------------------- */

/* Opcodes.  */

#define ADCREG(reg_val, clk_inc, pc_inc)                           \
  do {                                                             \
      BYTE tmp, carry;                                             \
                                                                   \
      carry = reg_f & C_FLAG;                                      \
      tmp = reg_a + reg_val + carry;                               \
      reg_f = SZP[tmp];                                            \
      LOCAL_SET_CARRY((WORD)((WORD)reg_a + (WORD)reg_val           \
                      + (WORD)(carry)) & 0x100);                   \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_val ^ tmp) & H_FLAG);       \
      LOCAL_SET_PARITY(~(reg_a ^ reg_val) & (reg_a ^ tmp) & 0x80); \
      reg_a = tmp;                                                 \
      CLK += clk_inc;                                              \
      INC_PC(pc_inc);                                              \
  } while (0)

#define ADDHLREG(reg_valh, reg_vall)                             \
  do {                                                           \
      DWORD tmp;                                                 \
                                                                 \
      tmp = (DWORD)((reg_h << 8) + reg_l)                        \
            + (DWORD)(((reg_valh << 8) + reg_vall));             \
      reg_h = tmp >> 8;                                          \
      reg_l = tmp & 0xff;                                        \
      LOCAL_SET_NADDSUB(0);                                      \
      LOCAL_SET_CARRY(tmp & 0x10000);                            \
      LOCAL_SET_HALFCARRY((reg_h << 8) ^ (reg_valh << 8) ^ tmp); \
      CLK += 11;                                                 \
      INC_PC(1);                                                 \
  } while (0)

#define ADDHLSP()                                                \
  do {                                                           \
      DWORD tmp;                                                 \
                                                                 \
      tmp = (DWORD)((reg_h << 8) + reg_l) + (DWORD)(reg_sp);     \
      reg_h = tmp >> 8;                                          \
      reg_l = tmp & 0xff;                                        \
      LOCAL_SET_NADDSUB(0);                                      \
      LOCAL_SET_CARRY(tmp & 0x10000);                            \
      LOCAL_SET_HALFCARRY((reg_h << 8) ^ reg_sp ^ tmp);          \
      CLK += 11;                                                 \
      INC_PC(1);                                                 \
  } while (0)

#define ADDREG(reg_val, clk_inc, pc_inc)                            \
  do {                                                              \
      BYTE tmp;                                                     \
                                                                    \
      tmp = reg_a + reg_val;                                        \
      reg_f = SZP[tmp];                                             \
      LOCAL_SET_CARRY((WORD)((WORD)reg_a + (WORD)reg_val) & 0x100); \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_val ^ tmp) & H_FLAG);        \
      LOCAL_SET_PARITY(~(reg_a ^ reg_val) & (reg_a ^ tmp) & 0x80);  \
      reg_a = tmp;                                                  \
      CLK += clk_inc;                                               \
      INC_PC(pc_inc);                                               \
  } while (0)

#define AND(value, clk_inc1, clk_inc2, pc_inc)      \
  do {                                              \
      CLK += (clk_inc1);                            \
      reg_a &= (value);                             \
      reg_f = H_FLAG | (reg_a & 0x28) | SZP[reg_a]; \
      CLK += (clk_inc2);                            \
      INC_PC(pc_inc);                               \
  } while (0)

#define ANDREG(reg_val)                             \
  do {                                              \
      reg_a &= (reg_val);                           \
      reg_f = H_FLAG | SZP[reg_a];                  \
      CLK += 4;                                     \
      INC_PC(1);                                    \
  } while (0)

#define BIT(reg_val, value, clk_inc)                \
  do {                                              \
      LOCAL_SET_NADDSUB(0);                         \
      LOCAL_SET_HALFCARRY(1);                       \
      LOCAL_SET_ZERO(!(reg_val & (1 << value)));    \
      CLK += (clk_inc);                             \
      INC_PC(2);                                    \
  } while (0)

#define BRANCH(cond, value)                                  \
  do {                                                       \
      if (cond) {                                            \
          unsigned int dest_addr;                            \
                                                             \
          dest_addr = reg_pc + 2 + (signed char)(value);     \
          reg_pc = dest_addr & 0xffff;                       \
          CLK += 7;                                          \
      } else {                                               \
          CLK += 7;                                          \
          INC_PC(2);                                         \
      }                                                      \
  } while (0)

#define CALL(reg_val, clk_inc, pc_inc)                  \
  do {                                                  \
      INC_PC(pc_inc);                                   \
      --reg_sp;                                         \
      STORE((reg_sp), ((BYTE)(reg_pc >> 8)));           \
      --reg_sp;                                         \
      STORE((reg_sp), ((BYTE)(reg_pc & 0xff)));         \
      JUMP(reg_val);                                    \
      CLK += clk_inc;                                   \
  } while (0)

#define CALL_COND(reg_value, cond, clk_inc1, clk_inc2, pc_inc)    \
  do {                                                            \
      if (cond) {                                                 \
          CALL(reg_value, clk_inc1, pc_inc);                      \
      } else {                                                    \
          CLK += clk_inc2;                                        \
          INC_PC(3);                                              \
      }                                                           \
  } while (0)

#define CCF()                                 \
  do {                                        \
      reg_f ^= C_FLAG;                        \
      LOCAL_SET_HALFCARRY(!(LOCAL_CARRY()));  \
      LOCAL_SET_NADDSUB(0);                   \
      CLK += 4;                               \
      INC_PC(1);                              \
  } while (0)

#define CPBYTE(reg_val)                                           \
  do {                                                            \
      BYTE tmp;                                                   \
                                                                  \
      tmp = reg_a - reg_val;                                      \
      reg_f = N_FLAG | SZP[tmp];                                  \
      LOCAL_SET_CARRY(reg_val > reg_a);                           \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_val ^ tmp) & H_FLAG);      \
      LOCAL_SET_PARITY((reg_a ^ reg_val) & (reg_a ^ tmp) & 0x80); \
      CLK += 7;                                                   \
      INC_PC(2);                                                  \
  } while (0)

#define CPI()                                                     \
  do {                                                            \
      BYTE val, tmp;                                              \
                                                                  \
      val = LOAD(HL_WORD());                                      \
      tmp = reg_a - val;                                          \
      INC_HL_WORD();                                              \
      DEC_BC_WORD();                                              \
      reg_f = N_FLAG | SZP[tmp] | (reg_f & C_FLAG);               \
      LOCAL_SET_HALFCARRY((reg_a ^ val ^ tmp) & H_FLAG);          \
      LOCAL_SET_PARITY(reg_b | reg_c);                            \
      CLK += 4; /* TEMPORARY */                                   \
      INC_PC(2);                                                  \
  } while (0)

#define CPIR()                                                    \
  do {                                                            \
      BYTE val, tmp;                                              \
                                                                  \
      do {                                                        \
          val = LOAD(HL_WORD());                                  \
          tmp = reg_a - val;                                      \
          INC_HL_WORD();                                          \
          DEC_BC_WORD();                                          \
      } while (BC_WORD() != 0 && tmp);                            \
      reg_f = N_FLAG | SZP[tmp] | (reg_f & C_FLAG);               \
      LOCAL_SET_HALFCARRY((reg_a ^ val ^ tmp) & H_FLAG);          \
      LOCAL_SET_PARITY(reg_b | reg_c);                            \
      CLK += 4; /* TEMPORARY */                                   \
      INC_PC(2);                                                  \
  } while (0)

#define CPREG(reg_val, clk_inc)                                   \
  do {                                                            \
      BYTE tmp;                                                   \
                                                                  \
      tmp = reg_a - reg_val;                                      \
      reg_f = N_FLAG | SZP[tmp];                                  \
      LOCAL_SET_CARRY(reg_val > reg_a);                           \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_val ^ tmp) & H_FLAG);      \
      LOCAL_SET_PARITY((reg_a ^ reg_val) & (reg_a ^ tmp) & 0x80); \
      CLK += (clk_inc);                                           \
      INC_PC(1);                                                  \
  } while (0)

#define CPL()                                   \
  do {                                          \
      CLK += 4;                                 \
      reg_a = ~reg_a;                           \
      LOCAL_SET_NADDSUB(1);                     \
      LOCAL_SET_HALFCARRY(1);                   \
      INC_PC(1);                                \
  } while (0)

#define DECBC()                                 \
  do {                                          \
      CLK += 6;                                 \
      DEC_BC_WORD();                            \
      INC_PC(1);                                \
  } while (0)

#define DECDE()                                 \
  do {                                          \
      CLK += 6;                                 \
      DEC_DE_WORD();                            \
      INC_PC(1);                                \
  } while (0)

#define DECHL()                                 \
  do {                                          \
      CLK += 6;                                 \
      DEC_HL_WORD();                            \
      INC_PC(1);                                \
  } while (0)

#define DECHLIND()                                          \
  do {                                                      \
      BYTE tmp;                                             \
                                                            \
      tmp = LOAD(HL_WORD());                                \
      tmp--;                                                \
      STORE(HL_WORD(), tmp);                                \
      reg_f = N_FLAG | SZP[tmp] | (reg_f & C_FLAG);         \
      LOCAL_SET_PARITY((tmp == 0x7f));                      \
      LOCAL_SET_HALFCARRY(((tmp & 0x0f) == 0x0f));          \
      CLK += 11;                                            \
      INC_PC(1);                                            \
  } while (0)

#define DECREG(reg_val)                                     \
  do {                                                      \
      CLK += 4;                                             \
      reg_val--;                                            \
      reg_f = N_FLAG | SZP[reg_val] | (reg_f & C_FLAG);     \
      LOCAL_SET_PARITY((reg_val == 0x7f));                  \
      LOCAL_SET_HALFCARRY(((reg_val & 0x0f) == 0x0f));      \
      INC_PC(1);                                            \
  } while (0)

#define DECSP()                                 \
  do {                                          \
      CLK += 6;                                 \
      reg_sp--;                                 \
      INC_PC(1);                                \
  } while (0)

#define DJNZ(value)                             \
  do {                                          \
      reg_b--;                                  \
      BRANCH(reg_b, value);                     \
  } while (0)

#define DI()                                    \
  do {                                          \
      CLK += 4;                                 \
      reg_iff &= 0xfe;                          \
      INC_PC(1);                                \
  } while (0)

#define EI()                                    \
  do {                                          \
      CLK += 4;                                 \
      reg_iff |= 0x01;                          \
      INC_PC(1);                                \
  } while (0)

#define EXDEHL()                                \
  do {                                          \
      BYTE tmpl, tmph;                          \
                                                \
      tmph = reg_d;                             \
      tmpl = reg_e;                             \
      reg_d = reg_h;                            \
      reg_e = reg_l;                            \
      reg_h = tmph;                             \
      reg_l = tmpl;                             \
      CLK += 4;                                 \
      INC_PC(1);                                \
  } while (0)

#define EXHLSP()                                \
  do {                                          \
      BYTE tmpl, tmph;                          \
                                                \
      tmph = reg_h;                             \
      tmpl = reg_l;                             \
      reg_h = LOAD(reg_sp + 1);                 \
      reg_l = LOAD(reg_sp);                     \
      STORE((reg_sp + 1), tmph);                \
      STORE(reg_sp, tmpl);                      \
      CLK += 19;                                \
      INC_PC(1);                                \
  } while (0)

#define IM(value)                               \
  do {                                          \
    reg_f = value | (reg_f & 0xf9);             \
    CLK += 8;                                   \
    INC_PC(2);                                  \
  } while (0)

#define INBC(value, clk_inc, pc_inc)                \
  do {                                              \
      CLK += (clk_inc);                             \
      value = IN(BC_WORD());                        \
      reg_f = SZP[value & 0xff] | (reg_f & C_FLAG); \
      INC_PC(pc_inc);                               \
  } while (0)

#define INCBC(clk_inc, pc_inc)                  \
  do {                                          \
      CLK += (clk_inc);                         \
      INC_BC_WORD();                            \
      INC_PC(pc_inc);                           \
  } while (0)

#define INCDE(clk_inc, pc_inc)          \
  do {                                  \
      CLK += (clk_inc);                 \
      INC_DE_WORD();                    \
      INC_PC(pc_inc);                   \
  } while (0)

#define INCHL(clk_inc, pc_inc)          \
  do {                                  \
      CLK += (clk_inc);                 \
      INC_HL_WORD();                    \
      INC_PC(pc_inc);                   \
  } while (0)

#define INCHLIND()                                     \
  do {                                                 \
      BYTE tmp;                                        \
                                                       \
      tmp = LOAD(HL_WORD());                           \
      tmp++;                                           \
      STORE(HL_WORD(), tmp);                           \
      reg_f = SZP[tmp] | (reg_f & C_FLAG);             \
      LOCAL_SET_PARITY((tmp == 0x80));                 \
      LOCAL_SET_HALFCARRY(!(tmp & 0x0f));              \
      CLK += 11;                                       \
      INC_PC(1);                                       \
  } while (0)

#define INCSP(clk_inc, pc_inc)          \
  do {                                  \
      CLK += (clk_inc);                 \
      reg_sp++;                         \
      INC_PC(pc_inc);                   \
  } while (0)

#define INCREG(reg_val)                                 \
  do {                                                  \
      CLK += 4;                                         \
      reg_val++;                                        \
      reg_f = SZP[reg_val] | (reg_f & C_FLAG);          \
      LOCAL_SET_PARITY((reg_val == 0x80));              \
      LOCAL_SET_HALFCARRY(!(reg_val & 0x0f));           \
      INC_PC(1);                                        \
  } while (0)

#define JMP(addr, clk_inc)                      \
  do {                                          \
      CLK += (clk_inc);                         \
      JUMP(addr);                               \
  } while (0)

#define JMP_COND(addr, cond, clk_inc1, clk_inc2) \
  do {                                           \
      if (cond) {                                \
          JMP(addr, clk_inc1);                   \
      } else {                                   \
          CLK += (clk_inc2);                     \
          INC_PC(3);                             \
      }                                          \
  } while (0)

#define LDBC(value, clk_inc1, clk_inc2, pc_inc) \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_c = ((value) & 0xff);                 \
      reg_b = ((value) >> 8);                   \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDDE(value, clk_inc1, clk_inc2, pc_inc) \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_e = ((value) & 0xff);                 \
      reg_d = ((value) >> 8);                   \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDDR()                                  \
  do {                                          \
      do {                                      \
          STORE(DE_WORD(), LOAD(HL_WORD()));    \
          DEC_BC_WORD();                        \
          DEC_DE_WORD();                        \
          DEC_HL_WORD();                        \
      } while (BC_WORD() != 0);                 \
      LOCAL_SET_NADDSUB(0);                     \
      LOCAL_SET_PARITY(0);                      \
      LOCAL_SET_HALFCARRY(0);                   \
      CLK += 4; /* TEMPORARY */                 \
      INC_PC(2);                                \
  } while (0)

#define LDHL(value, clk_inc1, clk_inc2, pc_inc) \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_l = ((value) & 0xff);                 \
      reg_h = ((value) >> 8);                   \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDIR()                                  \
  do {                                          \
      do {                                      \
          STORE(DE_WORD(), LOAD(HL_WORD()));    \
          DEC_BC_WORD();                        \
          INC_DE_WORD();                        \
          INC_HL_WORD();                        \
      } while (BC_WORD() != 0);                 \
      LOCAL_SET_NADDSUB(0);                     \
      LOCAL_SET_PARITY(0);                      \
      LOCAL_SET_HALFCARRY(0);                   \
      CLK += 4; /* TEMPORARY */                 \
      INC_PC(2);                                \
  } while (0)

#define LDIND(value, reg_valh, reg_vall, clk_inc1, clk_inc2, pc_inc) \
  do {                                                               \
      CLK += clk_inc1;                                               \
      reg_vall = LOAD(value);                                        \
      reg_valh = LOAD(value + 1);                                    \
      CLK += clk_inc2;                                               \
      INC_PC(pc_inc);                                                \
  } while (0)

#define LDSP(value, clk_inc1, clk_inc2, pc_inc) \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_sp = (value);                         \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDSPIND(value, clk_inc1, clk_inc2, pc_inc)                   \
  do {                                                               \
      CLK += clk_inc1;                                               \
      reg_sp = LOAD(value) | (LOAD(value + 1) >> 8);                 \
      CLK += clk_inc2;                                               \
      INC_PC(pc_inc);                                                \
  } while (0)

#define LDREG(reg_value, value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                                       \
      CLK += (clk_inc1);                                     \
      reg_value = (value);                                   \
      CLK += (clk_inc2);                                     \
      INC_PC(pc_inc);                                        \
  } while (0)

#define MOVEREG(reg_dest, reg_src)              \
  do {                                          \
      reg_dest = reg_src;                       \
      CLK += 4;                                 \
      INC_PC(1);                                \
  } while (0)

#define NOP(clk_inc, pc_inc)                    \
  (CLK += (clk_inc), INC_PC(pc_inc))

#define ORREG(reg_val, clk_inc1, clk_inc2, pc_inc)      \
  do {                                                  \
      CLK += (clk_inc1);                                \
      reg_a |= reg_val;                                 \
      reg_f = SZP[reg_a];                               \
      CLK += (clk_inc2);                                \
      INC_PC(pc_inc);                                   \
  } while (0)

#define OUTBC(value, clk_inc, pc_inc)           \
  do {                                          \
      CLK += (clk_inc);                         \
      OUT(BC_WORD(), value);                    \
      INC_PC(pc_inc);                           \
  } while (0)

#define POP(reg_valh, reg_vall)                         \
  do {                                                  \
      CLK += 7;                                         \
      reg_vall = LOAD(reg_sp);                          \
      ++reg_sp;                                         \
      reg_valh = LOAD(reg_sp);                          \
      ++reg_sp;                                         \
      CLK += 3;                                         \
      INC_PC(1);                                        \
  } while (0)

#define PUSH(reg_valh, reg_vall)                        \
  do {                                                  \
      CLK += 8;                                         \
      --reg_sp;                                         \
      STORE((reg_sp), (reg_valh));                      \
      --reg_sp;                                         \
      STORE((reg_sp), (reg_vall));                      \
      CLK += 3;                                         \
      INC_PC(1);                                        \
  } while (0)

#define RES(reg_val, value)                             \
  do {                                                  \
      reg_val &= (~(1 << value));                       \
      CLK += 8;                                         \
      INC_PC(2);                                        \
  } while (0)

#define RESHL(value)                                    \
  do {                                                  \
      BYTE tmp;                                         \
                                                        \
      tmp = LOAD(HL_WORD());                            \
      tmp &= (~(1 << value));                           \
      STORE(HL_WORD(), tmp);                            \
      CLK += 15;                                        \
      INC_PC(2);                                        \
  } while (0)

#define RET()                                           \
  do {                                                  \
      unsigned int tmp;                                 \
                                                        \
      CLK += 7;                                         \
      tmp = LOAD(reg_sp) | (LOAD((reg_sp + 1)) << 8);   \
      reg_sp += 2;                                      \
      JUMP(tmp);                                        \
      CLK += 3;                                         \
  } while (0)

#define RET_COND(cond)                                      \
  do {                                                      \
      if (cond) {                                           \
          unsigned int tmp;                                 \
                                                            \
          CLK += 7;                                         \
          tmp = LOAD(reg_sp) | (LOAD((reg_sp + 1)) << 8);   \
          reg_sp += 2;                                      \
          JUMP(tmp);                                        \
          CLK += 3;                                         \
      } else {                                              \
          CLK += 5;                                         \
          INC_PC(1);                                        \
      }                                                     \
  } while (0)

#define RLA()                                                   \
  do {                                                          \
      BYTE rot;                                                 \
                                                                \
      rot = (reg_a & 0x80) ? C_FLAG : 0;                        \
      reg_a = (reg_a << 1) | (reg_f & C_FLAG);                  \
      LOCAL_SET_CARRY(rot);                                     \
      LOCAL_SET_NADDSUB(0);                                     \
      LOCAL_SET_HALFCARRY(0);                                   \
      CLK += 4;                                                 \
      INC_PC(1);                                                \
  } while (0)

#define RLCA()                                                  \
  do {                                                          \
      BYTE rot;                                                 \
                                                                \
      rot = (reg_a & 0x80) ? C_FLAG : 0;                        \
      reg_a = (reg_a << 1) | rot;                               \
      LOCAL_SET_CARRY(rot);                                     \
      LOCAL_SET_NADDSUB(0);                                     \
      LOCAL_SET_HALFCARRY(0);                                   \
      CLK += 4;                                                 \
      INC_PC(1);                                                \
  } while (0)

#define RRA()                                                   \
  do {                                                          \
      BYTE rot;                                                 \
                                                                \
      rot = reg_a & C_FLAG;                                     \
      reg_a = (reg_a >> 1) | ((reg_f & C_FLAG) ? 0x80 : 0);     \
      LOCAL_SET_CARRY(rot);                                     \
      LOCAL_SET_NADDSUB(0);                                     \
      LOCAL_SET_HALFCARRY(0);                                   \
      CLK += 4;                                                 \
      INC_PC(1);                                                \
  } while (0)

#define RRCA()                                                  \
  do {                                                          \
      BYTE rot;                                                 \
                                                                \
      rot = reg_a & C_FLAG;                                     \
      reg_a = (reg_a >> 1) | ((rot) ? 0x80 : 0);                \
      LOCAL_SET_CARRY(rot);                                     \
      LOCAL_SET_NADDSUB(0);                                     \
      LOCAL_SET_HALFCARRY(0);                                   \
      CLK += 4;                                                 \
      INC_PC(1);                                                \
  } while (0)

#define SBCHLREG(reg_valh, reg_vall)                                        \
  do {                                                                      \
      DWORD tmp;                                                            \
      BYTE carry;                                                           \
                                                                            \
      carry = reg_f & C_FLAG;                                               \
      tmp = (DWORD)(HL_WORD()) - (DWORD)((reg_valh << 8) + reg_vall)        \
            - (DWORD)(carry);                                               \
      reg_f = N_FLAG;                                                       \
      LOCAL_SET_CARRY((DWORD)((DWORD)(reg_valh << 8) + (DWORD)(reg_vall)    \
                      + (DWORD)(carry)) > (DWORD)(HL_WORD()));              \
      LOCAL_SET_HALFCARRY((reg_h ^ reg_valh ^ (tmp >> 8)) & 0x10);          \
      LOCAL_SET_PARITY(((reg_h ^ (tmp >> 8)) & (reg_h ^ reg_valh)) & 0x80); \
      LOCAL_SET_ZERO(!(tmp & 0xffff));                                      \
      LOCAL_SET_SIGN(tmp & 0x8000);                                         \
      reg_h = tmp >> 8;                                                     \
      reg_l = tmp & 0xff;                                                   \
      CLK += 15;                                                            \
      INC_PC(2);                                                            \
  } while (0)

#define SBCHLSP()                                                         \
  do {                                                                    \
      DWORD tmp;                                                          \
      BYTE carry;                                                         \
                                                                          \
      carry = reg_f & C_FLAG;                                             \
      tmp = (DWORD)(HL_WORD()) - (DWORD)reg_sp - (DWORD)(carry);          \
      reg_f = N_FLAG;                                                     \
      LOCAL_SET_CARRY((DWORD)((DWORD)reg_sp + (DWORD)(carry))             \
                      > ((DWORD)HL_WORD()));                              \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_sp ^ (tmp >> 8)) & 0x10);          \
      LOCAL_SET_PARITY(((reg_h ^ (tmp >> 8))                              \
                       & (reg_h ^ (reg_sp >> 8))) & 0x80);                \
      LOCAL_SET_ZERO(!(tmp & 0xffff));                                    \
      LOCAL_SET_SIGN(tmp & 0x8000);                                       \
      reg_h = tmp >> 8;                                                   \
      reg_l = tmp & 0xff;                                                 \
      CLK += 15;                                                          \
      INC_PC(2);                                                          \
  } while (0)

#define SBCREG(reg_val, clk_inc, pc_inc)                          \
  do {                                                            \
      BYTE tmp, carry;                                            \
                                                                  \
      carry = reg_f & C_FLAG;                                     \
      tmp = reg_a - reg_val - carry;                              \
      reg_f = N_FLAG | SZP[tmp];                                  \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_val ^ tmp) & H_FLAG);      \
      LOCAL_SET_PARITY((reg_a ^ reg_val) & (reg_a ^ tmp) & 0x80); \
      LOCAL_SET_CARRY((WORD)((WORD)reg_val                        \
                      + (WORD)(carry)) > reg_a);                  \
      reg_a = tmp;                                                \
      CLK += clk_inc;                                             \
      INC_PC(pc_inc);                                             \
  } while (0)

#define SCF()                             \
  do {                                    \
      LOCAL_SET_CARRY(1);                 \
      LOCAL_SET_HALFCARRY(0);             \
      LOCAL_SET_NADDSUB(0);               \
      CLK += 4;                           \
      INC_PC(1);                          \
  } while (0)

#define SET(reg_val, value)                             \
  do {                                                  \
      reg_val |= (1 << value);                          \
      CLK += 8;                                         \
      INC_PC(2);                                        \
  } while (0)

#define SETHL(value)                                    \
  do {                                                  \
      BYTE tmp;                                         \
                                                        \
      tmp = LOAD(HL_WORD());                            \
      tmp |= (1 << value);                              \
      STORE(HL_WORD(), tmp);                            \
      CLK += 15;                                        \
      INC_PC(2);                                        \
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

#define STW(addr, reg_valh, reg_vall, clk_inc1, clk_inc2, pc_inc) \
  do {                                                            \
      unsigned int tmp;                                           \
                                                                  \
      CLK += (clk_inc1);                                          \
      tmp = (addr);                                               \
      STORE(tmp, reg_vall);                                       \
      STORE(tmp + 1, reg_valh);                                   \
      CLK += (clk_inc2);                                          \
      INC_PC(pc_inc);                                             \
  } while (0)

#define STSPW(addr, clk_inc1, clk_inc2, pc_inc) \
  do {                                          \
      unsigned int tmp;                         \
                                                \
      CLK += (clk_inc1);                        \
      tmp = (addr);                             \
      STORE(tmp, (reg_sp & 0xff));              \
      STORE(tmp + 1, (reg_sp >> 8));            \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define STORE_ABS(addr, value, inc)             \
  do {                                          \
      CLK += (inc);                             \
      STORE((addr), (value));                   \
  } while (0)

#define STREG(addr, reg_val)                    \
  do {                                          \
      STORE_ABS(addr, reg_val, 7);              \
      INC_PC(1);                                \
  } while (0)

#define SUBREG(reg_val, clk_inc, pc_inc)                          \
  do {                                                            \
      BYTE tmp;                                                   \
                                                                  \
      tmp = reg_a - reg_val;                                      \
      reg_f = N_FLAG | SZP[tmp];                                  \
      LOCAL_SET_HALFCARRY((reg_a ^ reg_val ^ tmp) & H_FLAG);      \
      LOCAL_SET_PARITY((reg_a ^ reg_val) & (reg_a ^ tmp) & 0x80); \
      LOCAL_SET_CARRY(reg_val > reg_a);                           \
      reg_a = tmp;                                                \
      CLK += clk_inc;                                             \
      INC_PC(pc_inc);                                             \
  } while (0)

#define XORREG(reg_val)                         \
  do {                                          \
      CLK += 4;                                 \
      reg_a ^= reg_val;                         \
      reg_f = SZP[reg_a];                       \
      INC_PC(1);                                \
  } while (0)

/* ------------------------------------------------------------------------- */

/* Extented opcodes.  */

inline void opcode_cb(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0x40: /* BIT B 0 */
        BIT(reg_b, 0, 8);
        break;
      case 0x41: /* BIT C 0 */
        BIT(reg_c, 0, 8);
        break;
      case 0x42: /* BIT D 0 */
        BIT(reg_d, 0, 8);
        break;
      case 0x43: /* BIT E 0 */
        BIT(reg_e, 0, 8);
        break;
      case 0x44: /* BIT H 0 */
        BIT(reg_h, 0, 8);
        break;
      case 0x45: /* BIT L 0 */
        BIT(reg_l, 0, 8);
        break;
      case 0x46: /* BIT (HL) 0 */
        BIT(LOAD(HL_WORD()), 0, 12);
        break;
      case 0x47: /* BIT A 0 */
        BIT(reg_a, 1, 8);
        break;
      case 0x48: /* BIT B 1 */
        BIT(reg_b, 1, 8);
        break;
      case 0x49: /* BIT C 1 */
        BIT(reg_c, 1, 8);
        break;
      case 0x4a: /* BIT D 1 */
        BIT(reg_d, 1, 8);
        break;
      case 0x4b: /* BIT E 1 */
        BIT(reg_e, 1, 8);
        break;
      case 0x4c: /* BIT H 1 */
        BIT(reg_h, 1, 8);
        break;
      case 0x4d: /* BIT L 1 */
        BIT(reg_l, 1, 8);
        break;
      case 0x4e: /* BIT (HL) 1 */
        BIT(LOAD(HL_WORD()), 1, 12);
        break;
      case 0x4f: /* BIT A 1 */
        BIT(reg_a, 1, 8);
        break;
      case 0x50: /* BIT B 2 */
        BIT(reg_b, 2, 8);
        break;
      case 0x51: /* BIT C 2 */
        BIT(reg_c, 2, 8);
        break;
      case 0x52: /* BIT D 2 */
        BIT(reg_d, 2, 8);
        break;
      case 0x53: /* BIT E 2 */
        BIT(reg_e, 2, 8);
        break;
      case 0x54: /* BIT H 2 */
        BIT(reg_h, 2, 8);
        break;
      case 0x55: /* BIT L 2 */
        BIT(reg_l, 2, 8);
        break;
      case 0x56: /* BIT (HL) 2 */
        BIT(LOAD(HL_WORD()), 2, 12);
        break;
      case 0x57: /* BIT A 2 */
        BIT(reg_a, 2, 8);
        break;
      case 0x58: /* BIT B 3 */
        BIT(reg_b, 3, 8);
        break;
      case 0x59: /* BIT C 3 */
        BIT(reg_c, 3, 8);
        break;
      case 0x5a: /* BIT D 3 */
        BIT(reg_d, 3, 8);
        break;
      case 0x5b: /* BIT E 3 */
        BIT(reg_e, 3, 8);
        break;
      case 0x5c: /* BIT H 3 */
        BIT(reg_h, 3, 8);
        break;
      case 0x5d: /* BIT L 3 */
        BIT(reg_l, 3, 8);
        break;
      case 0x5e: /* BIT (HL) 3 */
        BIT(LOAD(HL_WORD()), 3, 12);
        break;
      case 0x5f: /* BIT A 3 */
        BIT(reg_a, 3, 8);
        break;
      case 0x60: /* BIT B 4 */
        BIT(reg_b, 4, 8);
        break;
      case 0x61: /* BIT C 4 */
        BIT(reg_c, 4, 8);
        break;
      case 0x62: /* BIT D 4 */
        BIT(reg_d, 4, 8);
        break;
      case 0x63: /* BIT E 4 */
        BIT(reg_e, 4, 8);
        break;
      case 0x64: /* BIT H 4 */
        BIT(reg_h, 4, 8);
        break;
      case 0x65: /* BIT L 4 */
        BIT(reg_l, 4, 8);
        break;
      case 0x66: /* BIT (HL) 4 */
        BIT(LOAD(HL_WORD()), 4, 12);
        break;
      case 0x67: /* BIT A 4 */
        BIT(reg_a, 4, 8);
        break;
      case 0x68: /* BIT B 5 */
        BIT(reg_b, 5, 8);
        break;
      case 0x69: /* BIT C 5 */
        BIT(reg_c, 5, 8);
        break;
      case 0x6a: /* BIT D 5 */
        BIT(reg_d, 5, 8);
        break;
      case 0x6b: /* BIT E 5 */
        BIT(reg_e, 5, 8);
        break;
      case 0x6c: /* BIT H 5 */
        BIT(reg_h, 5, 8);
        break;
      case 0x6d: /* BIT L 5 */
        BIT(reg_l, 5, 8);
        break;
      case 0x6e: /* BIT (HL) 5 */
        BIT(LOAD(HL_WORD()), 5, 12);
        break;
      case 0x6f: /* BIT A 5 */
        BIT(reg_a, 5, 8);
        break;
      case 0x70: /* BIT B 6 */
        BIT(reg_b, 6, 8);
        break;
      case 0x71: /* BIT C 6 */
        BIT(reg_c, 6, 8);
        break;
      case 0x72: /* BIT D 6 */
        BIT(reg_d, 6, 8);
        break;
      case 0x73: /* BIT E 6 */
        BIT(reg_e, 6, 8);
        break;
      case 0x74: /* BIT H 6 */
        BIT(reg_h, 6, 8);
        break;
      case 0x75: /* BIT L 6 */
        BIT(reg_l, 6, 8);
        break;
      case 0x76: /* BIT (HL) 6 */
        BIT(LOAD(HL_WORD()), 6, 12);
        break;
      case 0x77: /* BIT A 6 */
        BIT(reg_a, 6, 8);
        break;
      case 0x78: /* BIT B 7 */
        BIT(reg_b, 7, 8);
        break;
      case 0x79: /* BIT C 7 */
        BIT(reg_c, 7, 8);
        break;
      case 0x7a: /* BIT D 7 */
        BIT(reg_d, 7, 8);
        break;
      case 0x7b: /* BIT E 7 */
        BIT(reg_e, 7, 8);
        break;
      case 0x7c: /* BIT H 7 */
        BIT(reg_h, 7, 8);
        break;
      case 0x7d: /* BIT L 7 */
        BIT(reg_l, 7, 8);
        break;
      case 0x7e: /* BIT (HL) 7 */
        BIT(LOAD(HL_WORD()), 7, 12);
        break;
      case 0x7f: /* BIT A 7 */
        BIT(reg_a, 7, 8);
        break;
      case 0x80: /* RES B 0 */
        RES(reg_b, 0);
        break;
      case 0x81: /* RES C 0 */
        RES(reg_c, 0);
        break;
      case 0x82: /* RES D 0 */
        RES(reg_d, 0);
        break;
      case 0x83: /* RES E 0 */
        RES(reg_e, 0);
        break;
      case 0x84: /* RES H 0 */
        RES(reg_h, 0);
        break;
      case 0x85: /* RES L 0 */
        RES(reg_l, 0);
        break;
      case 0x86: /* RES (HL) 0 */
        RESHL(0);
        break;
      case 0x87: /* RES A 0 */
        RES(reg_a, 1);
        break;
      case 0x88: /* RES B 1 */
        RES(reg_b, 1);
        break;
      case 0x89: /* RES C 1 */
        RES(reg_c, 1);
        break;
      case 0x8a: /* RES D 1 */
        RES(reg_d, 1);
        break;
      case 0x8b: /* RES E 1 */
        RES(reg_e, 1);
        break;
      case 0x8c: /* RES H 1 */
        RES(reg_h, 1);
        break;
      case 0x8d: /* RES L 1 */
        RES(reg_l, 1);
        break;
      case 0x8e: /* RES (HL) 1 */
        RESHL(1);
        break;
      case 0x8f: /* RES A 1 */
        RES(reg_a, 1);
        break;
      case 0x90: /* RES B 2 */
        RES(reg_b, 2);
        break;
      case 0x91: /* RES C 2 */
        RES(reg_c, 2);
        break;
      case 0x92: /* RES D 2 */
        RES(reg_d, 2);
        break;
      case 0x93: /* RES E 2 */
        RES(reg_e, 2);
        break;
      case 0x94: /* RES H 2 */
        RES(reg_h, 2);
        break;
      case 0x95: /* RES L 2 */
        RES(reg_l, 2);
        break;
      case 0x96: /* RES (HL) 2 */
        RESHL(2);
        break;
      case 0x97: /* RES A 2 */
        RES(reg_a, 2);
        break;
      case 0x98: /* RES B 3 */
        RES(reg_b, 3);
        break;
      case 0x99: /* RES C 3 */
        RES(reg_c, 3);
        break;
      case 0x9a: /* RES D 3 */
        RES(reg_d, 3);
        break;
      case 0x9b: /* RES E 3 */
        RES(reg_e, 3);
        break;
      case 0x9c: /* RES H 3 */
        RES(reg_h, 3);
        break;
      case 0x9d: /* RES L 3 */
        RES(reg_l, 3);
        break;
      case 0x9e: /* RES (HL) 3 */
        RESHL(3);
        break;
      case 0x9f: /* RES A 3 */
        RES(reg_a, 3);
        break;
      case 0xa0: /* RES B 4 */
        RES(reg_b, 4);
        break;
      case 0xa1: /* RES C 4 */
        RES(reg_c, 4);
        break;
      case 0xa2: /* RES D 4 */
        RES(reg_d, 4);
        break;
      case 0xa3: /* RES E 4 */
        RES(reg_e, 4);
        break;
      case 0xa4: /* RES H 4 */
        RES(reg_h, 4);
        break;
      case 0xa5: /* RES L 4 */
        RES(reg_l, 4);
        break;
      case 0xa6: /* RES (HL) 4 */
        RESHL(4);
        break;
      case 0xa7: /* RES A 4 */
        RES(reg_a, 4);
        break;
      case 0xa8: /* RES B 5 */
        RES(reg_b, 5);
        break;
      case 0xa9: /* RES C 5 */
        RES(reg_c, 5);
        break;
      case 0xaa: /* RES D 5 */
        RES(reg_d, 5);
        break;
      case 0xab: /* RES E 5 */
        RES(reg_e, 5);
        break;
      case 0xac: /* RES H 5 */
        RES(reg_h, 5);
        break;
      case 0xad: /* RES L 5 */
        RES(reg_l, 5);
        break;
      case 0xae: /* RES (HL) 5 */
        RESHL(5);
        break;
      case 0xaf: /* RES A 5 */
        RES(reg_a, 5);
        break;
      case 0xb0: /* RES B 6 */
        RES(reg_b, 6);
        break;
      case 0xb1: /* RES C 6 */
        RES(reg_c, 6);
        break;
      case 0xb2: /* RES D 6 */
        RES(reg_d, 6);
        break;
      case 0xb3: /* RES E 6 */
        RES(reg_e, 6);
        break;
      case 0xb4: /* RES H 6 */
        RES(reg_h, 6);
        break;
      case 0xb5: /* RES L 6 */
        RES(reg_l, 6);
        break;
      case 0xb6: /* RES (HL) 6 */
        RESHL(6);
        break;
      case 0xb7: /* RES A 6 */
        RES(reg_a, 6);
        break;
      case 0xb8: /* RES B 7 */
        RES(reg_b, 7);
        break;
      case 0xb9: /* RES C 7 */
        RES(reg_c, 7);
        break;
      case 0xba: /* RES D 7 */
        RES(reg_d, 7);
        break;
      case 0xbb: /* RES E 7 */
        RES(reg_e, 7);
        break;
      case 0xbc: /* RES H 7 */
        RES(reg_h, 7);
        break;
      case 0xbd: /* RES L 7 */
        RES(reg_l, 7);
        break;
      case 0xbe: /* RES (HL) 7 */
        RESHL(7);
        break;
      case 0xbf: /* RES A 7 */
        RES(reg_a, 7);
        break;
      case 0xc0: /* SET B 0 */
        SET(reg_b, 0);
        break;
      case 0xc1: /* SET C 0 */
        SET(reg_c, 0);
        break;
      case 0xc2: /* SET D 0 */
        SET(reg_d, 0);
        break;
      case 0xc3: /* SET E 0 */
        SET(reg_e, 0);
        break;
      case 0xc4: /* SET H 0 */
        SET(reg_h, 0);
        break;
      case 0xc5: /* SET L 0 */
        SET(reg_l, 0);
        break;
      case 0xc6: /* SET (HL) 0 */
        SETHL(0);
        break;
      case 0xc7: /* SET A 0 */
        SET(reg_a, 1);
        break;
      case 0xc8: /* SET B 1 */
        SET(reg_b, 1);
        break;
      case 0xc9: /* SET C 1 */
        SET(reg_c, 1);
        break;
      case 0xca: /* SET D 1 */
        SET(reg_d, 1);
        break;
      case 0xcb: /* SET E 1 */
        SET(reg_e, 1);
        break;
      case 0xcc: /* SET H 1 */
        SET(reg_h, 1);
        break;
      case 0xcd: /* SET L 1 */
        SET(reg_l, 1);
        break;
      case 0xce: /* SET (HL) 1 */
        SETHL(1);
        break;
      case 0xcf: /* SET A 1 */
        SET(reg_a, 1);
        break;
      case 0xd0: /* SET B 2 */
        SET(reg_b, 2);
        break;
      case 0xd1: /* SET C 2 */
        SET(reg_c, 2);
        break;
      case 0xd2: /* SET D 2 */
        SET(reg_d, 2);
        break;
      case 0xd3: /* SET E 2 */
        SET(reg_e, 2);
        break;
      case 0xd4: /* SET H 2 */
        SET(reg_h, 2);
        break;
      case 0xd5: /* SET L 2 */
        SET(reg_l, 2);
        break;
      case 0xd6: /* SET (HL) 2 */
        SETHL(2);
        break;
      case 0xd7: /* SET A 2 */
        SET(reg_a, 2);
        break;
      case 0xd8: /* SET B 3 */
        SET(reg_b, 3);
        break;
      case 0xd9: /* SET C 3 */
        SET(reg_c, 3);
        break;
      case 0xda: /* SET D 3 */
        SET(reg_d, 3);
        break;
      case 0xdb: /* SET E 3 */
        SET(reg_e, 3);
        break;
      case 0xdc: /* SET H 3 */
        SET(reg_h, 3);
        break;
      case 0xdd: /* SET L 3 */
        SET(reg_l, 3);
        break;
      case 0xde: /* SET (HL) 3 */
        SETHL(3);
        break;
      case 0xdf: /* SET A 3 */
        SET(reg_a, 3);
        break;
      case 0xe0: /* SET B 4 */
        SET(reg_b, 4);
        break;
      case 0xe1: /* SET C 4 */
        SET(reg_c, 4);
        break;
      case 0xe2: /* SET D 4 */
        SET(reg_d, 4);
        break;
      case 0xe3: /* SET E 4 */
        SET(reg_e, 4);
        break;
      case 0xe4: /* SET H 4 */
        SET(reg_h, 4);
        break;
      case 0xe5: /* SET L 4 */
        SET(reg_l, 4);
        break;
      case 0xe6: /* SET (HL) 4 */
        SETHL(4);
        break;
      case 0xe7: /* SET A 4 */
        SET(reg_a, 4);
        break;
      case 0xe8: /* SET B 5 */
        SET(reg_b, 5);
        break;
      case 0xe9: /* SET C 5 */
        SET(reg_c, 5);
        break;
      case 0xea: /* SET D 5 */
        SET(reg_d, 5);
        break;
      case 0xeb: /* SET E 5 */
        SET(reg_e, 5);
        break;
      case 0xec: /* SET H 5 */
        SET(reg_h, 5);
        break;
      case 0xed: /* SET L 5 */
        SET(reg_l, 5);
        break;
      case 0xee: /* SET (HL) 5 */
        SETHL(5);
        break;
      case 0xef: /* SET A 5 */
        SET(reg_a, 5);
        break;
      case 0xf0: /* SET B 6 */
        SET(reg_b, 6);
        break;
      case 0xf1: /* SET C 6 */
        SET(reg_c, 6);
        break;
      case 0xf2: /* SET D 6 */
        SET(reg_d, 6);
        break;
      case 0xf3: /* SET E 6 */
        SET(reg_e, 6);
        break;
      case 0xf4: /* SET H 6 */
        SET(reg_h, 6);
        break;
      case 0xf5: /* SET L 6 */
        SET(reg_l, 6);
        break;
      case 0xf6: /* SET (HL) 6 */
        SETHL(6);
        break;
      case 0xf7: /* SET A 6 */
        SET(reg_a, 6);
        break;
      case 0xf8: /* SET B 7 */
        SET(reg_b, 7);
        break;
      case 0xf9: /* SET C 7 */
        SET(reg_c, 7);
        break;
      case 0xfa: /* SET D 7 */
        SET(reg_d, 7);
        break;
      case 0xfb: /* SET E 7 */
        SET(reg_e, 7);
        break;
      case 0xfc: /* SET H 7 */
        SET(reg_h, 7);
        break;
      case 0xfd: /* SET L 7 */
        SET(reg_l, 7);
        break;
      case 0xfe: /* SET (HL) 7 */
        SETHL(7);
        break;
      case 0xff: /* SET A 7 */
        SET(reg_a, 7);
        break;
      default:
        log_message(LOG_DEFAULT,
                    "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                    "H%02x L%02x SP%04x OP %02x %02x %02x.",
                    CLK, reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
                    exit(-1);
   }
}

inline void opcode_ed(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0x40: /* IN B BC */
        INBC(reg_b, 12, 2);
        break;
      case 0x41: /* OUT BC B */
        OUTBC(reg_b, 12, 2);
        break;
      case 0x42: /* SBC HL BC */
        SBCHLREG(reg_b, reg_c);
        break;
      case 0x43: /* LD (WORD) BC */
        STW(ip23, reg_b, reg_c, 17, 3, 4);
        break;
      case 0x46: /* IM0 */
        IM(0);
        break;
      case 0x47: /* LD IFF A */
        LDREG(reg_iff, reg_a, 6, 3, 2);
        break;
      case 0x48: /* IN C BC */
        INBC(reg_c, 12, 2);
        break;
      case 0x49: /* OUT BC C */
        OUTBC(reg_c, 12, 2);
        break;
      case 0x4b: /* LD BC (WORD) */
        LDIND(ip23, reg_b, reg_c, 17, 3, 4);
        break;
      case 0x50: /* IN D BC */
        INBC(reg_d, 12, 2);
        break;
      case 0x51: /* OUT BC D */
        OUTBC(reg_d, 12, 2);
        break;
      case 0x52: /* SBC HL DE */
        SBCHLREG(reg_d, reg_e);
        break;
      case 0x53: /* LD (WORD) DE */
        STW(ip23, reg_d, reg_e, 17, 3, 4);
        break;
      case 0x56: /* IM1 */
        IM(2);
        break;
      case 0x57: /* LD A IFF */
        LDREG(reg_a, reg_iff, 6, 3, 2);
        break;
      case 0x58: /* IN E BC */
        INBC(reg_e, 12, 2);
        break;
      case 0x59: /* OUT BC E */
        OUTBC(reg_e, 12, 2);
        break;
      case 0x5b: /* LD DE (WORD) */
        LDIND(ip23, reg_d, reg_e, 17, 3, 4);
        break;
      case 0x5e: /* IM2 */
        IM(4);
        break;
      case 0x60: /* IN H BC */
        INBC(reg_h, 12, 2);
        break;
      case 0x61: /* OUT BC H */
        OUTBC(reg_h, 12, 2);
        break;
      case 0x62: /* SBC HL HL */
        SBCHLREG(reg_h, reg_l);
        break;
      case 0x63: /* LD (WORD) HL */
        STW(ip23, reg_h, reg_l, 17, 3, 4);
        break;
      case 0x68: /* IN L BC */
        INBC(reg_l, 12, 2);
        break;
      case 0x69: /* OUT BC L */
        OUTBC(reg_l, 12, 2);
        break;
      case 0x6b: /* LD HL (WORD) */
        LDIND(ip23, reg_h, reg_l, 17, 3, 4);
        break;
      case 0x70: /* IN F BC */
        INBC(reg_f, 12, 2);
        break;
      case 0x72: /* SBC HL SP */
        SBCHLSP();
        break;
      case 0x73: /* LD (WORD) SP */
        STSPW(ip23, 17, 3, 4);
        break;
      case 0x78: /* IN A BC */
        INBC(reg_a, 12, 2);
        break;
      case 0x79: /* OUT BC A */
        OUTBC(reg_a, 12, 2);
        break;
      case 0x7b: /* LD SP (WORD) */
        LDSPIND(ip23, 17, 3, 4);
        break;
      case 0xa1: /* CPI */
        CPI();
        break;
      case 0xb0: /* LDIR */
        LDIR();
        break;
      case 0xb1: /* CPIR */
        CPIR();
        break;
      case 0xb8: /* LDDR */
        LDDR();
        break;
      default:
        log_message(LOG_DEFAULT,
                    "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                    "H%02x L%02x SP%04x OP %02x %02x %02x.",
                    CLK, reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
                    exit(-1);
   }
}

inline void opcode_fd(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0xb1:
      /* */
      default:
        log_message(LOG_DEFAULT,
                    "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                    "H%02x L%02x SP%04x OP %02x %02x %02x.",
                    CLK, reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
                    exit(-1);
   }
}

/* ------------------------------------------------------------------------- */

/* Z80 mainloop.  */

void z80_mainloop(cpu_int_status_t *cpu_int_status,
                  alarm_context_t *cpu_alarm_context)
{
    opcode_t opcode;

    BYTE *bank_base;
    int bank_limit;

    z80mem_set_bank_pointer(&bank_base, &bank_limit);

    dma_request = 0;

    do {

    while (CLK >= alarm_context_next_pending_clk(cpu_alarm_context))
        alarm_context_dispatch(cpu_alarm_context, CLK);

    {
        enum cpu_int pending_interrupt;

        pending_interrupt = check_pending_interrupt(cpu_int_status);
        if (pending_interrupt != IK_NONE) {
            DO_INTERRUPT(pending_interrupt);
            while (CLK >= alarm_context_next_pending_clk(cpu_alarm_context))
                alarm_context_dispatch(cpu_alarm_context, CLK);
        }
    }

    FETCH_OPCODE(opcode);

#ifdef TRACE
    log_message(LOG_DEFAULT,
                "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                "H%02x L%02x SP%04x OP %02x %02x %02x %02x",
                CLK, reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                reg_h, reg_l, reg_sp, p0, p1, p2, p3);
#endif

    switch (p0) {

    case 0x00: /* NOP */
    NOP(4, 1);
    break;
    case 0x01: /* LD BC # */
    LDBC(p12, 10, 0, 3);
    break;
    case 0x02: /*  */
    /*LD_xBC_A, */
    xit (-1);
    break;
    case 0x03: /* INC BC */
    INCBC(6, 1);
    break;
    case 0x04: /* INC B */
    INCREG(reg_b);
    break;
    case 0x05: /* DEC B */
    DECREG(reg_b);
    break;
    case 0x06: /* LD B # */
    LDREG(reg_b, p1, 4, 3, 2);
    break;
    case 0x07: /* RLCA */
    RLCA();
    break;
    case 0x08: /*  */
    /*EX_AF_AF, */
    xit (-1);
    break;
    case 0x09: /* ADD HL BC */
    ADDHLREG(reg_b, reg_c);
    break;
    case 0x0a: /* LD A (BC) */
    LDREG(reg_a, LOAD(BC_WORD()), 4, 3, 1);
    break;
    case 0x0b: /* DEC BC */
    DECBC();
    break;
    case 0x0c: /* INC C */
    INCREG(reg_c);
    break;
    case 0x0d: /* DEC C */
    DECREG(reg_c);
    break;
    case 0x0e: /* LD C # */
    LDREG(reg_c, p1, 4, 3, 2);
    break;
    case 0x0f: /* RRCA */
    RRCA();
    break;
    case 0x10: /* DJNZ */
    DJNZ(p1);
    break;
    case 0x11: /* LD DE # */
    LDDE(p12, 10, 0, 3);
    break;
    case 0x12: /*  */
    /*LD_xDE_A, */
    xit (-1);
    break;
    case 0x13: /* INC DE */
    INCDE(6, 1);
    break;
    case 0x14: /* INC D */
    INCREG(reg_d);
    break;
    case 0x15: /* DEC D */
    DECREG(reg_d);
    break;
    case 0x16: /* LD D # */
    LDREG(reg_d, p1, 4, 3, 2);
    break;
    case 0x17: /* RLA */
    RLA();
    break;
    case 0x18: /* JR */
    BRANCH(1, p1);
    break;
    case 0x19: /* ADD HL DE */
    ADDHLREG(reg_d, reg_e);
    break;
    case 0x1a: /* LD A DE */
    LDREG(reg_a, LOAD(DE_WORD()), 4, 3, 1);
    break;
    case 0x1b: /* DEC DE */
    DECDE();
    break;
    case 0x1c: /* INC E */
    INCREG(reg_e);
    break;
    case 0x1d: /* DEC E */
    DECREG(reg_e);
    break;
    case 0x1e: /* LD E # */
    LDREG(reg_e, p1, 4, 3, 2);
    break;
    case 0x1f: /* RRA */
    RRA();
    break;
    case 0x20: /* JR NZ */
    BRANCH(!LOCAL_ZERO(), p1);
    break;
    case 0x21: /* LD HL # */
    LDHL(p12, 10, 0, 3);
    break;
    case 0x22: /* LD (WORD) HL */
    STW(p12, reg_h, reg_l, 13, 3, 3);
    break;
    case 0x23: /* INC HL */
    INCHL(6, 1);
    break;
    case 0x24: /* INC H */
    INCREG(reg_h);
    break;
    case 0x25: /* DEC H */
    DECREG(reg_h);
    break;
    case 0x26: /* LD H # */
    LDREG(reg_h, p1, 4, 3, 2);
    break;
    case 0x27: /*  */
    /*DAA, */
    xit (-1);
    break;
    case 0x28: /* JR Z */
    BRANCH(LOCAL_ZERO(), p1);
    break;
    case 0x29: /* ADD HL HL */
    ADDHLREG(reg_h, reg_l);
    break;
    case 0x2a: /* LD HL (WORD) */
    LDIND(p12, reg_h, reg_l, 13, 3, 3);
    break;
    case 0x2b: /* DEC HL */
    DECHL();
    break;
    case 0x2c: /* INC L */
    INCREG(reg_l);
    break;
    case 0x2d: /* DEC L */
    DECREG(reg_l);
    break;
    case 0x2e: /* LD L # */
    LDREG(reg_l, p1, 4, 3, 2);
    break;
    case 0x2f: /* CPL */
    CPL();
    break;
    case 0x30: /* JR NC */
    BRANCH(!LOCAL_CARRY(), p1);
    break;
    case 0x31: /* LD SP # */
    LDSP(p12, 10, 0, 3);
    break;
    case 0x32: /* LD ABS A */
    STA(p12, 10, 3, 3, STORE_ABS);
    break;
    case 0x33: /* INC SP */
    INCSP(6, 1);
    break;
    case 0x34: /* INC (HL) */
    INCHLIND();
    break;
    case 0x35: /* DEC (HL) */
    DECHLIND();
    break;
    case 0x36: /* LD (HL) # */
    STA(HL_WORD(), 8, 2, 2, STORE_ABS);
    break;
    case 0x37: /* SCF */
    SCF();
    break;
    case 0x38: /* JR C */
    BRANCH(LOCAL_CARRY(), p1);
    break;
    case 0x39: /* ADD HL SP */
    ADDHLSP();
    break;
    case 0x3a: /* LD A (WORD) */
    LDREG(reg_a, LOAD(p12), 10, 3, 3);
    break;
    case 0x3b: /* DEC SP */
    DECSP();
    break;
    case 0x3c: /* INC A */
    INCREG(reg_a);
    break;
    case 0x3d: /* DEC A */
    DECREG(reg_a);
    break;
    case 0x3e: /* LD A # */
    LDREG(reg_a, p1, 4, 3, 2);
    break;
    case 0x3f: /* CCF */
    CCF();
    break;
    case 0x40: /* LD B B */
    MOVEREG(reg_b, reg_b);
    break;
    case 0x41: /* LD B C */
    MOVEREG(reg_b, reg_c);
    break;
    case 0x42: /* LD B D */
    MOVEREG(reg_b, reg_d);
    break;
    case 0x43: /* LD B E */
    MOVEREG(reg_b, reg_e);
    break;
    case 0x44: /* LD B H */
    MOVEREG(reg_b, reg_h);
    break;
    case 0x45: /* LD B L */
    MOVEREG(reg_b, reg_l);
    break;
    case 0x46: /* LD B (HL) */
    LDREG(reg_b, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x47: /* LD B A */
    MOVEREG(reg_b, reg_a);
    break;
    case 0x48: /* LD C B */
    MOVEREG(reg_c, reg_b);
    break;
    case 0x49: /* LD C C */
    MOVEREG(reg_c, reg_c);
    break;
    case 0x4a: /* LD C D */
    MOVEREG(reg_c, reg_d);
    break;
    case 0x4b: /* LD C E */
    MOVEREG(reg_c, reg_e);
    break;
    case 0x4c: /* LD C H */
    MOVEREG(reg_c, reg_h);
    break;
    case 0x4d: /* LD C L */
    MOVEREG(reg_c, reg_l);
    break;
    case 0x4e: /* LD C (HL) */
    LDREG(reg_c, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x4f: /* LD C A */
    MOVEREG(reg_c, reg_a);
    break;
    case 0x50: /* LD D B */
    MOVEREG(reg_d, reg_b);
    break;
    case 0x51: /* LD D C */
    MOVEREG(reg_d, reg_c);
    break;
    case 0x52: /* LD D D */
    MOVEREG(reg_d, reg_d);
    break;
    case 0x53: /* LD D E */
    MOVEREG(reg_d, reg_e);
    break;
    case 0x54: /* LD D H */
    MOVEREG(reg_d, reg_h);
    break;
    case 0x55: /* LD D L */
    MOVEREG(reg_d, reg_l);
    break;
    case 0x56: /* LD D (HL) */
    LDREG(reg_d, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x57: /* LD D A */
    MOVEREG(reg_d, reg_a);
    break;
    case 0x58: /* LD E B */
    MOVEREG(reg_e, reg_b);
    break;
    case 0x59: /* LD E C */
    MOVEREG(reg_e, reg_c);
    break;
    case 0x5a: /* LD E D */
    MOVEREG(reg_e, reg_d);
    break;
    case 0x5b: /* LD E E */
    MOVEREG(reg_e, reg_e);
    break;
    case 0x5c: /* LD E H */
    MOVEREG(reg_e, reg_h);
    break;
    case 0x5d: /* LD E L */
    MOVEREG(reg_e, reg_l);
    break;
    case 0x5e: /* LD E (HL) */
    LDREG(reg_e, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x5f: /* LD E A */
    MOVEREG(reg_e, reg_a);
    break;
    case 0x60: /* LD H B */
    MOVEREG(reg_h, reg_b);
    break;
    case 0x61: /* LD H C */
    MOVEREG(reg_h, reg_c);
    break;
    case 0x62: /* LD H D */
    MOVEREG(reg_h, reg_d);
    break;
    case 0x63: /* LD H E */
    MOVEREG(reg_h, reg_e);
    break;
    case 0x64: /* LD H H */
    MOVEREG(reg_h, reg_h);
    break;
    case 0x65: /* LD H L */
    MOVEREG(reg_h, reg_l);
    break;
    case 0x66: /* LD H (HL) */
    LDREG(reg_h, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x67: /* LD H A */
    MOVEREG(reg_h, reg_a);
    break;
    case 0x68: /* LD L B */
    MOVEREG(reg_l, reg_b);
    break;
    case 0x69: /* LD L C */
    MOVEREG(reg_l, reg_c);
    break;
    case 0x6a: /* LD L D */
    MOVEREG(reg_l, reg_d);
    break;
    case 0x6b: /* LD L E */
    MOVEREG(reg_l, reg_e);
    break;
    case 0x6c: /* LD L H */
    MOVEREG(reg_l, reg_h);
    break;
    case 0x6d: /* LD L L */
    MOVEREG(reg_l, reg_l);
    break;
    case 0x6e: /* LD L (HL) */
    LDREG(reg_l, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x6f: /* LD L A */
    MOVEREG(reg_l, reg_a);
    break;
    case 0x70: /* LD (HL) B */
    STREG(HL_WORD(), reg_b);
    break;
    case 0x71: /* LD (HL) C */
    STREG(HL_WORD(), reg_c);
    break;
    case 0x72: /* LD (HL) D */
    STREG(HL_WORD(), reg_d);
    break;
    case 0x73: /* LD (HL) E */
    STREG(HL_WORD(), reg_e);
    break;
    case 0x74: /* LD (HL) H */
    STREG(HL_WORD(), reg_h);
    break;
    case 0x75: /* LD (HL) L */
    STREG(HL_WORD(), reg_l);
    break;
    case 0x76: /*  */
    /*HALT, */
    xit (-1);
    break;
    case 0x77: /* LD (HL) A */
    STREG(HL_WORD(), reg_a);
    break;
    case 0x78: /* LD A B */
    MOVEREG(reg_a, reg_b);
    break;
    case 0x79: /* LD A C */
    MOVEREG(reg_a, reg_c);
    break;
    case 0x7a: /* LD A D */
    MOVEREG(reg_a, reg_d);
    break;
    case 0x7b: /* LD A E */
    MOVEREG(reg_a, reg_e);
    break;
    case 0x7c: /* LD A H */
    MOVEREG(reg_a, reg_h);
    break;
    case 0x7d: /* LD A L */
    MOVEREG(reg_a, reg_l);
    break;
    case 0x7e: /* LD A (HL) */
    LDREG(reg_a, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x7f: /* LD A A */
    MOVEREG(reg_a, reg_a);
    break;
    case 0x80: /* ADD B */
    ADDREG(reg_b, 4, 1);
    break;
    case 0x81: /* ADD C */
    ADDREG(reg_c, 4, 1);
    break;
    case 0x82: /* ADD D */
    ADDREG(reg_d, 4, 1);
    break;
    case 0x83: /* ADD E */
    ADDREG(reg_e, 4, 1);
    break;
    case 0x84: /* ADD H */
    ADDREG(reg_h, 4, 1);
    break;
    case 0x85: /* ADD L */
    ADDREG(reg_l, 4, 1);
    break;
    case 0x86: /* ADD (HL) */
    ADDREG(LOAD(HL_WORD()), 7, 1);
    break;
    case 0x87: /* ADD A */
    ADDREG(reg_a, 4, 1);
    break;
    case 0x88: /* ADC B */
    ADCREG(reg_b, 4, 1);
    break;
    case 0x89: /* ADC C */
    ADCREG(reg_c, 4, 1);
    break;
    case 0x8a: /* ADC D */
    ADCREG(reg_d, 4, 1);
    break;
    case 0x8b: /* ADC E */
    ADCREG(reg_e, 4, 1);
    break;
    case 0x8c: /* ADC H */
    ADCREG(reg_h, 4, 1);
    break;
    case 0x8d: /* ADC L */
    ADCREG(reg_l, 4, 1);
    break;
    case 0x8e: /* ADC (HL) */
    ADCREG(LOAD(HL_WORD()), 7, 1);
    break;
    case 0x8f: /* ADC A */
    ADCREG(reg_a, 4, 1);
    break;
    case 0x90: /* SUB B */
    SUBREG(reg_b, 4, 1);
    break;
    case 0x91: /* SUB C */
    SUBREG(reg_c, 4, 1);
    break;
    case 0x92: /* SUB D */
    SUBREG(reg_d, 4, 1);
    break;
    case 0x93: /* SUB E */
    SUBREG(reg_e, 4, 1);
    break;
    case 0x94: /* SUB H */
    SUBREG(reg_h, 4, 1);
    break;
    case 0x95: /* SUB L */
    SUBREG(reg_l, 4, 1);
    break;
    case 0x96: /* SUB (HL) */
    SUBREG(LOAD(HL_WORD()), 7, 1);
    break;
    case 0x97: /* SUB A */
    SUBREG(reg_a, 4, 1);
    break;
    case 0x98: /* SBC B */
    SBCREG(reg_b, 4, 1);
    break;
    case 0x99: /* SBC C */
    SBCREG(reg_c, 4, 1);
    break;
    case 0x9a: /* SBC D */
    SBCREG(reg_d, 4, 1);
    break;
    case 0x9b: /* SBC E */
    SBCREG(reg_e, 4, 1);
    break;
    case 0x9c: /* SBC H */
    SBCREG(reg_h, 4, 1);
    break;
    case 0x9d: /* SBC L */
    SBCREG(reg_l, 4, 1);
    break;
    case 0x9e: /* SBC (HL) */
    SBCREG(LOAD(HL_WORD()), 7, 1);
    break;
    case 0x9f: /* SBC A */
    SBCREG(reg_a, 4, 1);
    break;
    case 0xa0: /* AND B */
    ANDREG(reg_b);
    break;
    case 0xa1: /* AND C */
    ANDREG(reg_c);
    break;
    case 0xa2: /* AND D */
    ANDREG(reg_d);
    break;
    case 0xa3: /* AND E */
    ANDREG(reg_e);
    break;
    case 0xa4: /* AND H */
    ANDREG(reg_h);
    break;
    case 0xa5: /* AND L */
    ANDREG(reg_l);
    break;
    case 0xa6: /*  */
    /*AND_xHL, */
    xit (-1);
    break;
    case 0xa7: /* AND A */
    ANDREG(reg_a);
    break;
    case 0xa8: /* XOR B */
    XORREG(reg_b);
    break;
    case 0xa9: /* XOR C */
    XORREG(reg_c);
    break;
    case 0xaa: /* XOR D */
    XORREG(reg_d);
    break;
    case 0xab: /* XOR E */
    XORREG(reg_e);
    break;
    case 0xac: /* XOR H */
    XORREG(reg_h);
    break;
    case 0xad: /* XOR L */
    XORREG(reg_l);
    break;
    case 0xae: /*  */
    /*XOR_xHL, */
    xit (-1);
    break;
    case 0xaf: /* XOR A */
    XORREG(reg_a);
    break;
    case 0xb0: /* OR B */
    ORREG(reg_b, 4, 0, 1);
    break;
    case 0xb1: /* OR C */
    ORREG(reg_c, 4, 0, 1);
    break;
    case 0xb2: /* OR D */
    ORREG(reg_d, 4, 0, 1);
    break;
    case 0xb3: /* OR E */
    ORREG(reg_e, 4, 0, 1);
    break;
    case 0xb4: /* OR H */
    ORREG(reg_h, 4, 0, 1);
    break;
    case 0xb5: /* OR L */
    ORREG(reg_l, 4, 0, 1);
    break;
    case 0xb6: /* OR (HL) */
    ORREG(LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0xb7: /* OR A */
    ORREG(reg_a, 4, 0, 1);
    break;
    case 0xb8: /* CP B */
    CPREG(reg_b, 4);
    break;
    case 0xb9: /* CP C */
    CPREG(reg_c, 4);
    break;
    case 0xba: /* CP D */
    CPREG(reg_d, 4);
    break;
    case 0xbb: /* CP E */
    CPREG(reg_e, 4);
    break;
    case 0xbc: /* CP H */
    CPREG(reg_h, 4);
    break;
    case 0xbd: /* CP L */
    CPREG(reg_l, 4);
    break;
    case 0xbe: /* CP (HL) */
    CPREG(LOAD(HL_WORD()), 7);
    break;
    case 0xbf: /* CP A */
    CPREG(reg_a, 4);
    break;
    case 0xc0: /* RET NZ */
    RET_COND(!LOCAL_ZERO());
    break;
    case 0xc1: /* POP BC */
    POP(reg_b, reg_c);
    break;
    case 0xc2: /* JP NZ */
    JMP_COND(p12, !LOCAL_ZERO(), 10, 10);
    break;
    case 0xc3: /* JP */
    JMP(p12, 10);
    break;
    case 0xc4: /* CALL NZ */
    CALL_COND(p12, !LOCAL_ZERO(), 10, 10, 3);
    break;
    case 0xc5: /* PUSH BC */
    PUSH(reg_b, reg_c);
    break;
    case 0xc6: /* ADD # */
    ADDREG(p1, 7, 2);
    break;
    case 0xc7: /* RST 00 */
    CALL(0x00, 11, 1);
    break;
    case 0xc8: /* RET Z */
    RET_COND(LOCAL_ZERO());
    break;
    case 0xc9: /* RET */
    RET();
    break;
    case 0xca: /* JP Z */
    JMP_COND(p12, LOCAL_ZERO(), 10, 10);
    break;
    case 0xcb: /* OPCODE CB */
    opcode_cb(p0, p1, p2, p3, p12, p23);
    break;
    case 0xcc: /* CALL Z */
    CALL_COND(p12, LOCAL_ZERO(), 10, 10, 3);
    break;
    case 0xcd: /* CALL */
    CALL(p12, 7, 3);
    break;
    case 0xce: /* ADC # */
    ADCREG(p1, 4, 2);
    break;
    case 0xcf: /* RST 08 */
    CALL(0x08, 11, 1);
    break;
    case 0xd0: /* RET NC */
    RET_COND(!LOCAL_CARRY());
    break;
    case 0xd1: /* POP DE */
    POP(reg_d, reg_e);
    break;
    case 0xd2: /* JP NC */
    log_message(LOG_DEFAULT, "DEBUG JP NC");
    JMP_COND(p12, !LOCAL_CARRY(), 10, 10);
    break;
    case 0xd3: /*  */
    /*OUTA, */
    xit (-1);
    break;
    case 0xd4: /* CALL NC */
    CALL_COND(p12, !LOCAL_CARRY(), 10, 10, 3);
    break;
    case 0xd5: /* PUSH DE */
    PUSH(reg_d, reg_e);
    break;
    case 0xd6: /* SUB # */
    SUBREG(p1, 7, 2);
    break;
    case 0xd7: /* RST 10 */
    CALL(0x10, 11, 1);
    break;
    case 0xd8: /* RET C */
    RET_COND(LOCAL_CARRY());
    break;
    case 0xd9: /*  */
    /*EXX, */
    xit (-1);
    break;
    case 0xda: /* JP C */
    log_message(LOG_DEFAULT, "DEBUG JP C");
    JMP_COND(p12, LOCAL_CARRY(), 10, 10);
    break;
    case 0xdb: /*  */
    /*INA, */
    xit (-1);
    break;
    case 0xdc: /* CALL C */
    CALL_COND(p12, LOCAL_CARRY(), 10, 10, 3);
    break;
    case 0xdd: /*  */
    /*PFX_DD, */
    xit (-1);
    break;
    case 0xde: /* SBC # */
    SBCREG(p1, 7, 2);
    break;
    case 0xdf: /* RST 18 */
    CALL(0x18, 11, 1);
    break;
    case 0xe0: /* RET PO */
    RET_COND(!LOCAL_PARITY());
    break;
    case 0xe1: /* POP HL */
    POP(reg_h, reg_l);
    break;
    case 0xe2: /* JP PO */
    JMP_COND(p12, !LOCAL_PARITY(), 10, 10);
    break;
    case 0xe3: /* EX HL (SP) */
    EXHLSP();
    break;
    case 0xe4: /* CALL PO */
    CALL_COND(p12, !LOCAL_PARITY(), 10, 10, 3);
    break;
    case 0xe5: /* PUSH HL */
    PUSH(reg_h, reg_l);
    break;
    case 0xe6: /* AND # */
    AND(p1, 7, 0, 2);
    break;
    case 0xe7: /* RST 20 */
    CALL(0x20, 11, 1);
    break;
    case 0xe8: /* RET PE */
    RET_COND(LOCAL_PARITY());
    break;
    case 0xe9: /* LD PC HL */
    JMP((HL_WORD()), 4);
    break;
    case 0xea: /* JP PE */
    JMP_COND(p12, LOCAL_PARITY(), 10, 10);
    break;
    case 0xeb: /* EX DE HL */
    EXDEHL();
    break;
    case 0xec: /* CALL PE */
    CALL_COND(p12, LOCAL_PARITY(), 10, 10, 3);
    break;
    case 0xed: /* OPCODE ED */
    opcode_ed(p0, p1, p2, p3, p12, p23);
    break;
    case 0xee: /*  */
    /*XOR_BYTE, */
    xit (-1);
    break;
    case 0xef: /* RST 28 */
    CALL(0x28, 11, 1);
    break;
    case 0xf0: /* RET P */
    RET_COND(!LOCAL_SIGN());
    break;
    case 0xf1: /* POP AF */
    POP(reg_a, reg_f);
    break;
    case 0xf2: /* JP P */
    JMP_COND(p12, !LOCAL_SIGN(), 10, 10);
    break;
    case 0xf3: /* DI */
    DI();
    break;
    case 0xf4: /* CALL P */
    CALL_COND(p12, !LOCAL_SIGN(), 10, 10, 3);
    break;
    case 0xf5: /* PUSH AF */
    PUSH(reg_a, reg_f);
    break;
    case 0xf6: /* OR # */
    ORREG(p1, 4, 3, 2);
    break;
    case 0xf7: /* RST 30 */
    CALL(0x30, 11, 1);
    break;
    case 0xf8: /* RET M */
    RET_COND(LOCAL_SIGN());
    break;
    case 0xf9: /* LD SP HL */
    LDSP(HL_WORD(), 4, 2, 1);
    break;
    case 0xfa: /* JP M */
    JMP_COND(p12, LOCAL_SIGN(), 10, 10);
    break;
    case 0xfb: /* EI */
    EI();
    break;
    case 0xfc: /* CALL M */
    CALL_COND(p12, LOCAL_SIGN(), 10, 10, 3);
    break;
    case 0xfd: /* OPCODE FD */
    opcode_fd(p0, p1, p2, p3, p12, p23);
    break;
    case 0xfe: /* CP # */
    CPBYTE(p1);
    break;
    case 0xff: /* RST 38 */
    CALL(0x38, 11, 1);
    break;
    }
    } while (!dma_request);
}


