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

#include "interrupt.h"
#include "log.h"
#include "z80.h"
#include "z80mem.h"

static BYTE reg_a = 0;
static BYTE reg_b = 0;
static BYTE reg_c = 0;
static BYTE reg_d = 0;
static BYTE reg_e = 0;
static BYTE reg_f = 0;
static BYTE reg_h = 0;
static BYTE reg_l = 0;
static WORD reg_ix = 0;
static WORD reg_iy = 0;
static WORD reg_sp = 0;
static DWORD reg_pc = 0;
static BYTE reg_iff = 0;

static int dma_request = 0;

void z80_trigger_dma(void)
{
    dma_request = 1;
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
     old_reg_pc = reg_pc;                     \
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

#define DO_INTERRUPT()                              \
  do {                                              \
     /* Handle IRQs here.  */                       \
  } while (0)

/* ------------------------------------------------------------------------- */

/* Opcodes.  */

#define AND(value, clk_inc1, clk_inc2, pc_inc)      \
  do {                                              \
      CLK += (clk_inc1);                            \
      reg_a &= (value);                             \
      reg_f = H_FLAG | (reg_a & 0x28) | SZP[reg_a]; \
      CLK += (clk_inc2);                            \
      INC_PC(pc_inc);                               \
  } while (0)

#define BRANCH(cond, value)                                           \
  do {                                                                \
      if (cond) {                                                     \
          unsigned int dest_addr = reg_pc + 2 + (signed char)(value); \
          reg_pc = dest_addr & 0xffff;                                \
          CLK += (7);                                                 \
      } else {                                                        \
          CLK += (7);                                                 \
          INC_PC(2);                                                  \
      }                                                               \
  } while (0)

#define CPL()                                   \
  do {                                          \
      CLK += 4;                                 \
      reg_a = ~reg_a;                           \
      LOCAL_SET_NADDSUB(1);                     \
      LOCAL_SET_HALFCARRY(1);                   \
      INC_PC(1);                                \
  } while (0)

#define DECREG(reg_val)                                             \
  do {                                                              \
      CLK += 4;                                                     \
      reg_val--;                                                    \
      reg_f = N_FLAG | (SZP[reg_val] & ~P_FLAG) | (reg_f & C_FLAG); \
      LOCAL_SET_PARITY((reg_val == 0x7f));                          \
      LOCAL_SET_HALFCARRY((reg_val == 0x0f));                       \
      INC_PC(1);                                                    \
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

#define DECSP()                                 \
  do {                                          \
      CLK += 6;                                 \
      reg_sp--;                                 \
      INC_PC(1);                                \
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

#define INBC(value, clk_inc, pc_inc)            \
  do {                                          \
      ADDRESS tmp;                              \
      tmp = BC_WORD();                          \
      CLK += (clk_inc);                         \
      value = IN(tmp);                          \
      INC_PC(pc_inc);                           \
  } while (0)

#define INCBC(clk_inc, pc_inc)                  \
  do {                                          \
      CLK += (clk_inc);                         \
      INC_BC_WORD();                            \
      INC_PC(pc_inc);                           \
  } while (0)

#define INCDE(clk_inc, pc_inc)                  \
  do {                                          \
      CLK += (clk_inc);                         \
      INC_DE_WORD();                            \
      INC_PC(pc_inc);                           \
  } while (0)

#define INCHL(clk_inc, pc_inc)                  \
  do {                                          \
      CLK += (clk_inc);                         \
      INC_HL_WORD();                            \
      INC_PC(pc_inc);                           \
  } while (0)

#define INCSP(clk_inc, pc_inc)                  \
  do {                                          \
      CLK += (clk_inc);                         \
      reg_sp++;                                 \
      INC_PC(pc_inc);                           \
  } while (0)

#define INCREG(reg_val)                                             \
  do {                                                              \
      CLK += 4;                                                     \
      reg_val++;                                                    \
      reg_f = (SZP[reg_val] & ~P_FLAG) | (reg_f & C_FLAG);          \
      LOCAL_SET_PARITY((reg_val == 0x80));                          \
      LOCAL_SET_HALFCARRY((reg_val == 0x10));                       \
      INC_PC(1);                                                    \
  } while (0)

#define JMP(addr, clk_inc1, clk_inc2)           \
  do {                                          \
      CLK += (clk_inc1);                        \
      JUMP(addr);                               \
      CLK += (clk_inc2);                        \
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
          CLK += 21;                            \
      } while (BC_WORD() != 0);                 \
      LOCAL_SET_NADDSUB(0);                     \
      LOCAL_SET_PARITY(0);                      \
      LOCAL_SET_HALFCARRY(0);                   \
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
          CLK += 21;                            \
      } while (BC_WORD() != 0);                 \
      LOCAL_SET_NADDSUB(0);                     \
      LOCAL_SET_PARITY(0);                      \
      LOCAL_SET_HALFCARRY(0);                   \
      INC_PC(2);                                \
  } while (0)

#define LDSP(value, clk_inc1, clk_inc2, pc_inc) \
  do {                                          \
      CLK += (clk_inc1);                        \
      reg_sp = (value);                         \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)

#define LDREG(reg_value, value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                                       \
      CLK += (clk_inc1);                                     \
      reg_value = (value);                                   \
      CLK += (clk_inc2);                                     \
      INC_PC(pc_inc);                                        \
  } while (0)

#define NOP(clk_inc, pc_inc)                    \
  (CLK += (clk_inc), INC_PC(pc_inc))

#define OUTBC(value, clk_inc, pc_inc)           \
  do {                                          \
      ADDRESS tmp;                              \
      tmp = BC_WORD();                          \
      CLK += (clk_inc);                         \
      OUT(tmp, value);                          \
      INC_PC(pc_inc);                           \
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

#define STHL(addr, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      unsigned int tmp;                         \
						\
      CLK += (clk_inc1);                        \
      tmp = (addr);                             \
      STORE(tmp, reg_l);                        \
      STORE(tmp + 1, reg_h);                    \
      CLK += (clk_inc2);                        \
      INC_PC(pc_inc);                           \
  } while (0)


#define STORE_ABS(addr, value, inc)             \
  do {                                          \
      CLK += (inc);                             \
      STORE((addr), (value));                   \
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

inline void opcode_ed(BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12)
{
    switch (ip1) {
      case 0x40: /* IN B BC */
        INBC(reg_b, 12, 2);
        break;
      case 0x41: /* OUT BC B */
        OUTBC(reg_b, 12, 2);
        break;
      case 0x48: /* IN C BC */
        INBC(reg_c, 12, 2);
        break;
      case 0x49: /* OUT BC C */
        OUTBC(reg_c, 12, 2);
        break;
      case 0x50: /* IN D BC */
        INBC(reg_d, 12, 2);
        break;
      case 0x51: /* OUT BC D */
        OUTBC(reg_d, 12, 2);
        break;
      case 0x58: /* IN E BC */
        INBC(reg_e, 12, 2);
        break;
      case 0x59: /* OUT BC E */
        OUTBC(reg_e, 12, 2);
        break;
      case 0x60: /* IN H BC */
        INBC(reg_h, 12, 2);
        break;
      case 0x61: /* OUT BC H */
        OUTBC(reg_h, 12, 2);
        break;
      case 0x68: /* IN L BC */
        INBC(reg_l, 12, 2);
        break;
      case 0x69: /* OUT BC L */
        OUTBC(reg_l, 12, 2);
        break;
      case 0x70: /* IN F BC */
        INBC(reg_f, 12, 2);
        break;
      case 0x78: /* IN A BC */
        INBC(reg_a, 12, 2);
        break;
      case 0x79: /* OUT BC A */
        OUTBC(reg_a, 12, 2);
        break;
      case 0xb0: /* LDIR */
        LDIR();
        break;
      case 0xb8: /* LDDR */
        LDDR();
        break;
      default:
        exit (-1);
   }
}

/* ------------------------------------------------------------------------- */

/* Z80 mainloop.  */

void z80_mainloop(void)
{
    opcode_t opcode;

    BYTE *bank_base;
    int bank_limit;

    z80mem_set_bank_pointer(&bank_base, &bank_limit);

    dma_request = 0;

    do {

    DO_INTERRUPT();

    FETCH_OPCODE(opcode);

    log_message(LOG_DEFAULT,
                "Z80 PC $%x fetched %x %x %x %x.", reg_pc, p0, p1, p2, p3);

    switch (p0) {

    case 0x00: /* NOP */
    NOP(4, 1);
    break;
    case 0x01: /* LD BC # */
    LDBC(p12, 10, 0, 3);
    break;
    case 0x02: /*  */
    //LD_xBC_A,
    exit (-1);
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
    case 0x07: /*  */
    //RLCA,
    exit (-1);
    break;
    case 0x08: /*  */
    //EX_AF_AF,
    exit (-1);
    break;
    case 0x09: /*  */
    //ADD_HL_BC,
    exit (-1);
    break;
    case 0x0a: /*  */
    //LD_A_xBC,
    exit (-1);
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
    case 0x0f: /*  */
    //RRCA,
    exit (-1);
    break;
    case 0x10: /*  */
    //DJNZ,
    exit (-1);
    break;
    case 0x11: /* LD DE # */
    LDDE(p12, 10, 0, 3);
    break;
    case 0x12: /*  */
    //LD_xDE_A,
    exit (-1);
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
    case 0x17: /*  */
    //RLA,
    exit (-1);
    break;
    case 0x18: /*  */
    //JR,
    exit (-1);
    break;
    case 0x19: /*  */
    //ADD_HL_DE,
    exit (-1);
    break;
    case 0x1a: /*  */
    //LD_A_xDE,
    exit (-1);
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
    case 0x1f: /*  */
    //RRA,
    exit (-1);
    break;
    case 0x20: /* JR NZ */
    BRANCH(!LOCAL_ZERO(), p1);
    break;
    case 0x21: /* LD HL # */
    LDHL(p12, 10, 0, 3);
    break;
    case 0x22: /* LD (WORD) HL */
    STHL(p12, 13, 3, 3);
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
    //DAA,
    exit (-1);
    break;
    case 0x28: /* JR Z */
    BRANCH(LOCAL_ZERO(), p1);
    break;
    case 0x29: /*  */
    //ADD_HL_HL,
    exit (-1);
    break;
    case 0x2a: /*  */
    //LD_HL_xWORD,
    exit (-1);
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
    case 0x34: /*  */
    //INC_xHL,
    exit (-1);
    break;
    case 0x35: /*  */
    //DEC_xHL,
    exit (-1);
    break;
    case 0x36: /*  */
    //LD_xHL_BYTE,
    exit (-1);
    break;
    case 0x37: /*  */
    //SCF,
    exit (-1);
    break;
    case 0x38: /* JR C */
    BRANCH(LOCAL_CARRY(), p1);
    break;
    case 0x39: /*  */
    //ADD_HL_SP,
    exit (-1);
    break;
    case 0x3a: /*  */
    //LD_A_xWORD,
    exit (-1);
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
    case 0x3f: /*  */
    //CCF,
    exit (-1);
    break;
    case 0x40: /*  */
    //LD_B_B,
    exit (-1);
    break;
    case 0x41: /*  */
    //LD_B_C,
    exit (-1);
    break;
    case 0x42: /*  */
    //LD_B_D,
    exit (-1);
    break;
    case 0x43: /*  */
    //LD_B_E,
    exit (-1);
    break;
    case 0x44: /*  */
    //LD_B_H,
    exit (-1);
    break;
    case 0x45: /*  */
    //LD_B_L,
    exit (-1);
    break;
    case 0x46: /*  */
    //LD_B_xHL,
    exit (-1);
    break;
    case 0x47: /*  */
    //LD_B_A,
    exit (-1);
    break;
    case 0x48: /*  */
    //LD_C_B,
    exit (-1);
    break;
    case 0x49: /*  */
    //LD_C_C,
    exit (-1);
    break;
    case 0x4a: /*  */
    //LD_C_D,
    exit (-1);
    break;
    case 0x4b: /*  */
    //LD_C_E,
    exit (-1);
    break;
    case 0x4c: /*  */
    //LD_C_H,
    exit (-1);
    break;
    case 0x4d: /*  */
    //LD_C_L,
    exit (-1);
    break;
    case 0x4e: /*  */
    //LD_C_xHL,
    exit (-1);
    break;
    case 0x4f: /*  */
    //LD_C_A,
    exit (-1);
    break;
    case 0x50: /*  */
    //LD_D_B,
    exit (-1);
    break;
    case 0x51: /*  */
    //LD_D_C,
    exit (-1);
    break;
    case 0x52: /*  */
    //LD_D_D,
    exit (-1);
    break;
    case 0x53: /*  */
    //LD_D_E,
    exit (-1);
    break;
    case 0x54: /*  */
    //LD_D_H,
    exit (-1);
    break;
    case 0x55: /*  */
    //LD_D_L,
    exit (-1);
    break;
    case 0x56: /*  */
    //LD_D_xHL,
    exit (-1);
    break;
    case 0x57: /*  */
    //LD_D_A,
    exit (-1);
    break;
    case 0x58: /*  */
    //LD_E_B,
    exit (-1);
    break;
    case 0x59: /*  */
    //LD_E_C,
    exit (-1);
    break;
    case 0x5a: /*  */
    //LD_E_D,
    exit (-1);
    break;
    case 0x5b: /*  */
    //LD_E_E,
    exit (-1);
    break;
    case 0x5c: /*  */
    //LD_E_H,
    exit (-1);
    break;
    case 0x5d: /*  */
    //LD_E_L,
    exit (-1);
    break;
    case 0x5e: /*  */
    //LD_E_xHL,
    exit (-1);
    break;
    case 0x5f: /*  */
    //LD_E_A,
    exit (-1);
    break;
    case 0x60: /*  */
    //LD_H_B,
    exit (-1);
    break;
    case 0x61: /*  */
    //LD_H_C,
    exit (-1);
    break;
    case 0x62: /*  */
    //LD_H_D,
    exit (-1);
    break;
    case 0x63: /*  */
    //LD_H_E,
    exit (-1);
    break;
    case 0x64: /*  */
    //LD_H_H,
    exit (-1);
    break;
    case 0x65: /*  */
    //LD_H_L,
    exit (-1);
    break;
    case 0x66: /*  */
    //LD_H_xHL,
    exit (-1);
    break;
    case 0x67: /*  */
    //LD_H_A,
    exit (-1);
    break;
    case 0x68: /*  */
    //LD_L_B,
    exit (-1);
    break;
    case 0x69: /*  */
    //LD_L_C,
    exit (-1);
    break;
    case 0x6a: /*  */
    //LD_L_D,
    exit (-1);
    break;
    case 0x6b: /*  */
    //LD_L_E,
    exit (-1);
    break;
    case 0x6c: /*  */
    //LD_L_H,
    exit (-1);
    break;
    case 0x6d: /*  */
    //LD_L_L,
    exit (-1);
    break;
    case 0x6e: /*  */
    //LD_L_xHL,
    exit (-1);
    break;
    case 0x6f: /*  */
    //LD_L_A,
    exit (-1);
    break;
    case 0x70: /*  */
    //LD_xHL_B,
    exit (-1);
    break;
    case 0x71: /*  */
    //LD_xHL_C,
    exit (-1);
    break;
    case 0x72: /*  */
    //LD_xHL_D,
    exit (-1);
    break;
    case 0x73: /*  */
    //LD_xHL_E,
    exit (-1);
    break;
    case 0x74: /*  */
    //LD_xHL_H,
    exit (-1);
    break;
    case 0x75: /*  */
    //LD_xHL_L,
    exit (-1);
    break;
    case 0x76: /*  */
    //HALT,
    exit (-1);
    break;
    case 0x77: /*  */
    //LD_xHL_A,
    exit (-1);
    break;
    case 0x78: /*  */
    //LD_A_B,
    exit (-1);
    break;
    case 0x79: /*  */
    //LD_A_C,
    exit (-1);
    break;
    case 0x7a: /*  */
    //LD_A_D,
    exit (-1);
    break;
    case 0x7b: /*  */
    //LD_A_E,
    exit (-1);
    break;
    case 0x7c: /*  */
    //LD_A_H,
    exit (-1);
    break;
    case 0x7d: /*  */
    //LD_A_L,
    exit (-1);
    break;
    case 0x7e: /* LD A (HL) */
    LDREG(reg_a, LOAD(HL_WORD()), 4, 3, 1);
    break;
    case 0x7f: /*  */
    //LD_A_A,
    exit (-1);
    break;
    case 0x80: /*  */
    //ADD_B,
    exit (-1);
    break;
    case 0x81: /*  */
    //ADD_C,
    exit (-1);
    break;
    case 0x82: /*  */
    //ADD_D,
    exit (-1);
    break;
    case 0x83: /*  */
    //ADD_E,
    exit (-1);
    break;
    case 0x84: /*  */
    //ADD_H,
    exit (-1);
    break;
    case 0x85: /*  */
    //ADD_L,
    exit (-1);
    break;
    case 0x86: /*  */
    //ADD_xHL,
    exit (-1);
    break;
    case 0x87: /*  */
    //ADD_A,
    exit (-1);
    break;
    case 0x88: /*  */
    //ADC_B,
    exit (-1);
    break;
    case 0x89: /*  */
    //ADC_C,
    exit (-1);
    break;
    case 0x8a: /*  */
    //ADC_D,
    exit (-1);
    break;
    case 0x8b: /*  */
    //ADC_E,
    exit (-1);
    break;
    case 0x8c: /*  */
    //ADC_H,
    exit (-1);
    break;
    case 0x8d: /*  */
    //ADC_L,
    exit (-1);
    break;
    case 0x8e: /*  */
    //ADC_xHL,
    exit (-1);
    break;
    case 0x8f: /*  */
    //ADC_A,
    exit (-1);
    break;
    case 0x90: /*  */
    //SUB_B,
    exit (-1);
    break;
    case 0x91: /*  */
    //SUB_C,
    exit (-1);
    break;
    case 0x92: /*  */
    //SUB_D,
    exit (-1);
    break;
    case 0x93: /*  */
    //SUB_E,
    exit (-1);
    break;
    case 0x94: /*  */
    //SUB_H,
    exit (-1);
    break;
    case 0x95: /*  */
    //SUB_L,
    exit (-1);
    break;
    case 0x96: /*  */
    //SUB_xHL,
    exit (-1);
    break;
    case 0x97: /*  */
    //SUB_A,
    exit (-1);
    break;
    case 0x98: /*  */
    //SBC_B,
    exit (-1);
    break;
    case 0x99: /*  */
    //SBC_C,
    exit (-1);
    break;
    case 0x9a: /*  */
    //SBC_D,
    exit (-1);
    break;
    case 0x9b: /*  */
    //SBC_E,
    exit (-1);
    break;
    case 0x9c: /*  */
    //SBC_H,
    exit (-1);
    break;
    case 0x9d: /*  */
    //SBC_L,
    exit (-1);
    break;
    case 0x9e: /*  */
    //SBC_xHL,
    exit (-1);
    break;
    case 0x9f: /*  */
    //SBC_A,
    exit (-1);
    break;
    case 0xa0: /*  */
    //AND_B,
    exit (-1);
    break;
    case 0xa1: /*  */
    //AND_C,
    exit (-1);
    break;
    case 0xa2: /*  */
    //AND_D,
    exit (-1);
    break;
    case 0xa3: /*  */
    //AND_E,
    exit (-1);
    break;
    case 0xa4: /*  */
    //AND_H,
    exit (-1);
    break;
    case 0xa5: /*  */
    //AND_L,
    exit (-1);
    break;
    case 0xa6: /*  */
    //AND_xHL,
    exit (-1);
    break;
    case 0xa7: /*  */
    //AND_A,
    exit (-1);
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
    //XOR_xHL,
    exit (-1);
    break;
    case 0xaf: /* XOR A */
    XORREG(reg_a);
    break;
    case 0xb0: /*  */
    //OR_B,
    exit (-1);
    break;
    case 0xb1: /*  */
    //OR_C,
    exit (-1);
    break;
    case 0xb2: /*  */
    //OR_D,
    exit (-1);
    break;
    case 0xb3: /*  */
    //OR_E,
    exit (-1);
    break;
    case 0xb4: /*  */
    //OR_H,
    exit (-1);
    break;
    case 0xb5: /*  */
    //OR_L,
    exit (-1);
    case 0xb6: /*  */
    break;
    //OR_xHL,
    exit (-1);
    case 0xb7: /*  */
    break;
    //OR_A,
    exit (-1);
    break;
    case 0xb8: /*  */
    //CP_B,
    exit (-1);
    break;
    case 0xb9: /*  */
    //CP_C,
    exit (-1);
    break;
    case 0xba: /*  */
    //CP_D,
    exit (-1);
    break;
    case 0xbb: /*  */
    //CP_E,
    exit (-1);
    break;
    case 0xbc: /*  */
    //CP_H,
    exit (-1);
    break;
    case 0xbd: /*  */
    //CP_L,
    exit (-1);
    break;
    case 0xbe: /*  */
    //CP_xHL,
    exit (-1);
    break;
    case 0xbf: /*  */
    //CP_A,
    exit (-1);
    break;
    case 0xc0: /*  */
    //RET_NZ,
    exit (-1);
    break;
    case 0xc1: /*  */
    //POP_BC,
    exit (-1);
    break;
    case 0xc2: /*  */
    //JP_NZ,
    exit (-1);
    break;
    case 0xc3: /* JP */
    JMP(p12, 10, 0);
    break;
    case 0xc4: /*  */
    //CALL_NZ,
    exit (-1);
    break;
    case 0xc5: /*  */
    //PUSH_BC,
    exit (-1);
    break;
    case 0xc6: /*  */
    //ADD_BYTE,
    exit (-1);
    break;
    case 0xc7: /*  */
    //RST00,
    exit (-1);
    break;
    case 0xc8: /*  */
    //RET_Z,
    exit (-1);
    break;
    case 0xc9: /*  */
    //RET,
    exit (-1);
    break;
    case 0xca: /*  */
    //JP_Z,
    exit (-1);
    break;
    case 0xcb: /*  */
    //PFX_CB,
    exit (-1);
    break;
    case 0xcc: /*  */
    //CALL_Z,
    exit (-1);
    break;
    case 0xcd: /*  */
    //CALL,
    exit (-1);
    break;
    case 0xce: /*  */
    //ADC_BYTE,
    exit (-1);
    break;
    case 0xcf: /*  */
    //RST08,
    exit (-1);
    break;
    case 0xd0: /*  */
    //RET_NC,
    exit (-1);
    break;
    case 0xd1: /*  */
    //POP_DE,
    exit (-1);
    break;
    case 0xd2: /*  */
    //JP_NC,
    exit (-1);
    break;
    case 0xd3: /*  */
    //OUTA,
    exit (-1);
    break;
    case 0xd4: /*  */
    //CALL_NC,
    exit (-1);
    break;
    case 0xd5: /*  */
    //PUSH_DE,
    exit (-1);
    break;
    case 0xd6: /*  */
    //SUB_BYTE,
    exit (-1);
    break;
    case 0xd7: /*  */
    //RST10,
    exit (-1);
    break;
    case 0xd8: /*  */
    //RET_C,
    exit (-1);
    break;
    case 0xd9: /*  */
    //EXX,
    exit (-1);
    break;
    case 0xda: /*  */
    //JP_C,
    exit (-1);
    break;
    case 0xdb: /*  */
    //INA,
    exit (-1);
    break;
    case 0xdc: /*  */
    //CALL_C,
    exit (-1);
    break;
    case 0xdd: /*  */
    //PFX_DD,
    exit (-1);
    break;
    case 0xde: /*  */
    //SBC_BYTE,
    exit (-1);
    break;
    case 0xdf: /*  */
    //RST18,
    exit (-1);
    break;
    case 0xe0: /*  */
    //RET_PO,
    exit (-1);
    break;
    case 0xe1: /*  */
    //POP_HL,
    exit (-1);
    break;
    case 0xe2: /*  */
    //JP_PO,
    exit (-1);
    break;
    case 0xe3: /*  */
    //EX_HL_xSP,
    exit (-1);
    break;
    case 0xe4: /*  */
    //CALL_PO,
    exit (-1);
    break;
    case 0xe5: /*  */
    //PUSH_HL,
    exit (-1);
    break;
    case 0xe6: /* AND # */
    AND(p1, 7, 0, 2);
    break;
    case 0xe7: /*  */
    //RST20,
    exit (-1);
    break;
    case 0xe8: /*  */
    //RET_PE,
    exit (-1);
    break;
    case 0xe9: /*  */
    //LD_PC_HL,
    exit (-1);
    break;
    case 0xea: /*  */
    //JP_PE,
    exit (-1);
    break;
    case 0xeb: /*  */
    //EX_DE_HL,
    exit (-1);
    break;
    case 0xec: /*  */
    //CALL_PE,
    exit (-1);
    break;
    case 0xed: /* OPCODE ED */
    opcode_ed(p1, p2, p3, p12);
    break;
    case 0xee: /*  */
    //XOR_BYTE,
    exit (-1);
    break;
    case 0xef: /*  */
    //RST28,
    exit (-1);
    break;
    case 0xf0: /*  */
    //RET_P,
    exit (-1);
    break;
    case 0xf1: /*  */
    //POP_AF,
    exit (-1);
    break;
    case 0xf2: /*  */
    //JP_P,
    exit (-1);
    break;
    case 0xf3: /* DI */
    DI();
    break;
    case 0xf4: /*  */
    //CALL_P,
    exit (-1);
    break;
    case 0xf5: /*  */
    //PUSH_AF,
    exit (-1);
    break;
    case 0xf6: /*  */
    //OR_BYTE,
    exit (-1);
    break;
    case 0xf7: /*  */
    //RST30,
    exit (-1);
    break;
    case 0xf8: /*  */
    //RET_M,
    exit (-1);
    break;
    case 0xf9: /*  */
    //LD_SP_HL,
    exit (-1);
    break;
    case 0xfa: /*  */
    //JP_M,
    exit (-1);
    break;
    case 0xfb: /* EI */
    EI();
    break;
    case 0xfc: /*  */
    //CALL_M,
    exit (-1);
    break;
    case 0xfd: /*  */
    //PFX_FD,
    exit (-1);
    break;
    case 0xfe: /*  */
    //CP_BYTE,
    exit (-1);
    break;
    case 0xff: /*  */
    //RST38
    exit (-1);
    break;
    }
    } while (!dma_request);
}


