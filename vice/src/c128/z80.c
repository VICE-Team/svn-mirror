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

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "alarm.h"
#include "interrupt.h"
#include "log.h"
#include "maincpu.h"
#include "mon.h"
#include "types.h"
#include "z80.h"
#include "z80mem.h"

/* #define TRACE */

static BYTE reg_a = 0;
static BYTE reg_b = 0;
static BYTE reg_c = 0;
static BYTE reg_d = 0;
static BYTE reg_e = 0;
static BYTE reg_f = 0;
static BYTE reg_h = 0;
static BYTE reg_l = 0;
static WORD reg_ixh = 0;
static WORD reg_ixl = 0;
static WORD reg_iyh = 0;
static WORD reg_iyl = 0;
static WORD reg_sp = 0;
static DWORD z80_reg_pc = 0;
static BYTE reg_i = 0;
static BYTE reg_iff = 0;

static BYTE reg_a2 = 0;
static BYTE reg_b2 = 0;
static BYTE reg_c2 = 0;
static BYTE reg_d2 = 0;
static BYTE reg_e2 = 0;
static BYTE reg_f2 = 0;
static BYTE reg_h2 = 0;
static BYTE reg_l2 = 0;

static int dma_request = 0;

#define xit(ex_val)                                                    \
  do {                                                                 \
    log_message(LOG_DEFAULT,                                           \
                "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "      \
                "H%02x L%02x OP %02x %02x %02x.",                      \
                CLK, z80_reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d,    \
                reg_e, reg_h, reg_l, p0, p1, p2);                      \
  } while (0)


void z80_trigger_dma(void)
{
    dma_request = 1;
}

void z80_reset(void)
{
    z80_reg_pc = 0;
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

#define JUMP(addr)                                \
   do {                                           \
     z80_reg_pc = (addr);                         \
     bank_base = z80mem_read_base(z80_reg_pc);    \
     bank_limit = z80mem_read_limit(z80_reg_pc);  \
     z80_old_reg_pc = z80_reg_pc;                 \
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
#define FETCH_OPCODE(o) ((o) = (LOAD(z80_reg_pc)		\
                               | (LOAD(z80_reg_pc + 1) << 8)	\
                               | (LOAD(z80_reg_pc + 2) << 16)	\
                               | (LOAD(z80_reg_pc + 3) << 24)))

#define p0 (opcode & 0xff)
#define p1 ((opcode >> 8) & 0xff)
#define p2 ((opcode >> 16) & 0xff)
#define p3 (opcode >> 24)

#define p12 ((opcode >> 8) & 0xffff)
#define p23 ((opcode >> 16) & 0xffff)

#define CLK clk

#define INC_PC(value)   (z80_reg_pc += (value))

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

#define C_FLAG  0x01
#define N_FLAG  0x02
#define P_FLAG  0x04
#define U3_FLAG 0x08
#define H_FLAG  0x10
#define U5_FLAG 0x20
#define Z_FLAG  0x40
#define S_FLAG  0x80

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

static BYTE daa_reg_a[2048] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x00,0x01,0x02,0x03,0x04,0x05,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x70,0x71,0x72,0x73,0x74,0x75,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x80,0x81,0x82,0x83,0x84,0x85,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x90,0x91,0x92,0x93,0x94,0x95,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0x00,0x01,0x02,0x03,0x04,0x05,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x30,0x31,0x32,0x33,0x34,0x35,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x40,0x41,0x42,0x43,0x44,0x45,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x50,0x51,0x52,0x53,0x54,0x55,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x60,0x61,0x62,0x63,0x64,0x65,
    0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
    0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
    0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,
    0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,
    0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,
    0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,
    0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,
    0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,
    0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,
    0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,
    0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,
    0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,
    0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,
    0x7E,0x7F,0x80,0x81,0x82,0x83,0x84,0x85,
    0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,
    0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,
    0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,
    0x9E,0x9F,0x00,0x01,0x02,0x03,0x04,0x05,
    0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
    0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
    0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,
    0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,
    0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,
    0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,
    0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,
    0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,
    0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,
    0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,
    0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,
    0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,
    0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,
    0x7E,0x7F,0x80,0x81,0x82,0x83,0x84,0x85,
    0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,
    0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,
    0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,
    0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,
    0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,
    0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,
    0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,
    0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,
    0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,
    0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,
    0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,
    0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,
    0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,
    0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,
    0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,
    0xFE,0xFF,0x00,0x01,0x02,0x03,0x04,0x05,
    0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,
    0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,
    0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,
    0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,
    0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,
    0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,
    0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,
    0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,
    0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,
    0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,
    0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x04,0x05,0x06,0x07,0x08,0x09,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x14,0x15,0x16,0x17,0x18,0x19,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x24,0x25,0x26,0x27,0x28,0x29,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x34,0x35,0x36,0x37,0x38,0x39,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x44,0x45,0x46,0x47,0x48,0x49,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x54,0x55,0x56,0x57,0x58,0x59,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x64,0x65,0x66,0x67,0x68,0x69,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x74,0x75,0x76,0x77,0x78,0x79,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x84,0x85,0x86,0x87,0x88,0x89,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x34,0x35,0x36,0x37,0x38,0x39,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x44,0x45,0x46,0x47,0x48,0x49,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x54,0x55,0x56,0x57,0x58,0x59,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x64,0x65,0x66,0x67,0x68,0x69,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x74,0x75,0x76,0x77,0x78,0x79,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x84,0x85,0x86,0x87,0x88,0x89,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x94,0x95,0x96,0x97,0x98,0x99,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x04,0x05,0x06,0x07,0x08,0x09,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x14,0x15,0x16,0x17,0x18,0x19,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x24,0x25,0x26,0x27,0x28,0x29,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x34,0x35,0x36,0x37,0x38,0x39,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
    0x48,0x49,0x44,0x45,0x46,0x47,0x48,0x49,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
    0x58,0x59,0x54,0x55,0x56,0x57,0x58,0x59,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x64,0x65,0x66,0x67,0x68,0x69,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x74,0x75,0x76,0x77,0x78,0x79,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x84,0x85,0x86,0x87,0x88,0x89,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x94,0x95,0x96,0x97,0x98,0x99,
    0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0x00,0x01,
    0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
    0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,
    0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
    0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
    0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
    0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,
    0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
    0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,
    0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,
    0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
    0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,
    0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
    0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,
    0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
    0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,0x81,
    0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
    0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,
    0x92,0x93,0x34,0x35,0x36,0x37,0x38,0x39,
    0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,
    0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,
    0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
    0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,
    0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
    0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,
    0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
    0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,0x81,
    0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
    0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,
    0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,
    0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,
    0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,
    0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,
    0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,
    0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,
    0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,
    0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,
    0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,
    0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,
    0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,
    0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,
    0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,
    0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,0x00,0x01,
    0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
    0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,
    0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
    0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,
    0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
    0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,
    0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
    0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,
    0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,
    0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
    0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,0x60,0x61,
    0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
    0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,
    0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
    0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,0x80,0x81,
    0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
    0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,
    0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99 };

static BYTE daa_reg_f[2048] = {
    0x44,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
    0x08,0x0C,0x10,0x14,0x14,0x10,0x14,0x10,
    0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
    0x0C,0x08,0x30,0x34,0x34,0x30,0x34,0x30,
    0x20,0x24,0x24,0x20,0x24,0x20,0x20,0x24,
    0x2C,0x28,0x34,0x30,0x30,0x34,0x30,0x34,
    0x24,0x20,0x20,0x24,0x20,0x24,0x24,0x20,
    0x28,0x2C,0x10,0x14,0x14,0x10,0x14,0x10,
    0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,
    0x0C,0x08,0x14,0x10,0x10,0x14,0x10,0x14,
    0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,
    0x08,0x0C,0x34,0x30,0x30,0x34,0x30,0x34,
    0x24,0x20,0x20,0x24,0x20,0x24,0x24,0x20,
    0x28,0x2C,0x30,0x34,0x34,0x30,0x34,0x30,
    0x20,0x24,0x24,0x20,0x24,0x20,0x20,0x24,
    0x2C,0x28,0x90,0x94,0x94,0x90,0x94,0x90,
    0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,
    0x8C,0x88,0x94,0x90,0x90,0x94,0x90,0x94,
    0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,
    0x88,0x8C,0x55,0x11,0x11,0x15,0x11,0x15,
    0x45,0x01,0x01,0x05,0x01,0x05,0x05,0x01,
    0x09,0x0D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x05,0x01,0x05,0x01,0x01,0x05,
    0x0D,0x09,0x31,0x35,0x35,0x31,0x35,0x31,
    0x21,0x25,0x25,0x21,0x25,0x21,0x21,0x25,
    0x2D,0x29,0x35,0x31,0x31,0x35,0x31,0x35,
    0x25,0x21,0x21,0x25,0x21,0x25,0x25,0x21,
    0x29,0x2D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x05,0x01,0x05,0x01,0x01,0x05,
    0x0D,0x09,0x15,0x11,0x11,0x15,0x11,0x15,
    0x05,0x01,0x01,0x05,0x01,0x05,0x05,0x01,
    0x09,0x0D,0x35,0x31,0x31,0x35,0x31,0x35,
    0x25,0x21,0x21,0x25,0x21,0x25,0x25,0x21,
    0x29,0x2D,0x31,0x35,0x35,0x31,0x35,0x31,
    0x21,0x25,0x25,0x21,0x25,0x21,0x21,0x25,
    0x2D,0x29,0x91,0x95,0x95,0x91,0x95,0x91,
    0x81,0x85,0x85,0x81,0x85,0x81,0x81,0x85,
    0x8D,0x89,0x95,0x91,0x91,0x95,0x91,0x95,
    0x85,0x81,0x81,0x85,0x81,0x85,0x85,0x81,
    0x89,0x8D,0xB5,0xB1,0xB1,0xB5,0xB1,0xB5,
    0xA5,0xA1,0xA1,0xA5,0xA1,0xA5,0xA5,0xA1,
    0xA9,0xAD,0xB1,0xB5,0xB5,0xB1,0xB5,0xB1,
    0xA1,0xA5,0xA5,0xA1,0xA5,0xA1,0xA1,0xA5,
    0xAD,0xA9,0x95,0x91,0x91,0x95,0x91,0x95,
    0x85,0x81,0x81,0x85,0x81,0x85,0x85,0x81,
    0x89,0x8D,0x91,0x95,0x95,0x91,0x95,0x91,
    0x81,0x85,0x85,0x81,0x85,0x81,0x81,0x85,
    0x8D,0x89,0xB1,0xB5,0xB5,0xB1,0xB5,0xB1,
    0xA1,0xA5,0xA5,0xA1,0xA5,0xA1,0xA1,0xA5,
    0xAD,0xA9,0xB5,0xB1,0xB1,0xB5,0xB1,0xB5,
    0xA5,0xA1,0xA1,0xA5,0xA1,0xA5,0xA5,0xA1,
    0xA9,0xAD,0x55,0x11,0x11,0x15,0x11,0x15,
    0x45,0x01,0x01,0x05,0x01,0x05,0x05,0x01,
    0x09,0x0D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x05,0x01,0x05,0x01,0x01,0x05,
    0x0D,0x09,0x31,0x35,0x35,0x31,0x35,0x31,
    0x21,0x25,0x25,0x21,0x25,0x21,0x21,0x25,
    0x2D,0x29,0x35,0x31,0x31,0x35,0x31,0x35,
    0x25,0x21,0x21,0x25,0x21,0x25,0x25,0x21,
    0x29,0x2D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x05,0x01,0x05,0x01,0x01,0x05,
    0x0D,0x09,0x15,0x11,0x11,0x15,0x11,0x15,
    0x05,0x01,0x01,0x05,0x01,0x05,0x05,0x01,
    0x09,0x0D,0x35,0x31,0x31,0x35,0x31,0x35,
    0x04,0x00,0x08,0x0C,0x0C,0x08,0x0C,0x08,
    0x08,0x0C,0x10,0x14,0x14,0x10,0x14,0x10,
    0x00,0x04,0x0C,0x08,0x08,0x0C,0x08,0x0C,
    0x0C,0x08,0x30,0x34,0x34,0x30,0x34,0x30,
    0x20,0x24,0x2C,0x28,0x28,0x2C,0x28,0x2C,
    0x2C,0x28,0x34,0x30,0x30,0x34,0x30,0x34,
    0x24,0x20,0x28,0x2C,0x2C,0x28,0x2C,0x28,
    0x28,0x2C,0x10,0x14,0x14,0x10,0x14,0x10,
    0x00,0x04,0x0C,0x08,0x08,0x0C,0x08,0x0C,
    0x0C,0x08,0x14,0x10,0x10,0x14,0x10,0x14,
    0x04,0x00,0x08,0x0C,0x0C,0x08,0x0C,0x08,
    0x08,0x0C,0x34,0x30,0x30,0x34,0x30,0x34,
    0x24,0x20,0x28,0x2C,0x2C,0x28,0x2C,0x28,
    0x28,0x2C,0x30,0x34,0x34,0x30,0x34,0x30,
    0x20,0x24,0x2C,0x28,0x28,0x2C,0x28,0x2C,
    0x2C,0x28,0x90,0x94,0x94,0x90,0x94,0x90,
    0x80,0x84,0x8C,0x88,0x88,0x8C,0x88,0x8C,
    0x8C,0x88,0x94,0x90,0x90,0x94,0x90,0x94,
    0x84,0x80,0x88,0x8C,0x8C,0x88,0x8C,0x88,
    0x88,0x8C,0x55,0x11,0x11,0x15,0x11,0x15,
    0x05,0x01,0x09,0x0D,0x0D,0x09,0x0D,0x09,
    0x09,0x0D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x0D,0x09,0x09,0x0D,0x09,0x0D,
    0x0D,0x09,0x31,0x35,0x35,0x31,0x35,0x31,
    0x21,0x25,0x2D,0x29,0x29,0x2D,0x29,0x2D,
    0x2D,0x29,0x35,0x31,0x31,0x35,0x31,0x35,
    0x25,0x21,0x29,0x2D,0x2D,0x29,0x2D,0x29,
    0x29,0x2D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x0D,0x09,0x09,0x0D,0x09,0x0D,
    0x0D,0x09,0x15,0x11,0x11,0x15,0x11,0x15,
    0x05,0x01,0x09,0x0D,0x0D,0x09,0x0D,0x09,
    0x09,0x0D,0x35,0x31,0x31,0x35,0x31,0x35,
    0x25,0x21,0x29,0x2D,0x2D,0x29,0x2D,0x29,
    0x29,0x2D,0x31,0x35,0x35,0x31,0x35,0x31,
    0x21,0x25,0x2D,0x29,0x29,0x2D,0x29,0x2D,
    0x2D,0x29,0x91,0x95,0x95,0x91,0x95,0x91,
    0x81,0x85,0x8D,0x89,0x89,0x8D,0x89,0x8D,
    0x8D,0x89,0x95,0x91,0x91,0x95,0x91,0x95,
    0x85,0x81,0x89,0x8D,0x8D,0x89,0x8D,0x89,
    0x89,0x8D,0xB5,0xB1,0xB1,0xB5,0xB1,0xB5,
    0xA5,0xA1,0xA9,0xAD,0xAD,0xA9,0xAD,0xA9,
    0xA9,0xAD,0xB1,0xB5,0xB5,0xB1,0xB5,0xB1,
    0xA1,0xA5,0xAD,0xA9,0xA9,0xAD,0xA9,0xAD,
    0xAD,0xA9,0x95,0x91,0x91,0x95,0x91,0x95,
    0x85,0x81,0x89,0x8D,0x8D,0x89,0x8D,0x89,
    0x89,0x8D,0x91,0x95,0x95,0x91,0x95,0x91,
    0x81,0x85,0x8D,0x89,0x89,0x8D,0x89,0x8D,
    0x8D,0x89,0xB1,0xB5,0xB5,0xB1,0xB5,0xB1,
    0xA1,0xA5,0xAD,0xA9,0xA9,0xAD,0xA9,0xAD,
    0xAD,0xA9,0xB5,0xB1,0xB1,0xB5,0xB1,0xB5,
    0xA5,0xA1,0xA9,0xAD,0xAD,0xA9,0xAD,0xA9,
    0xA9,0xAD,0x55,0x11,0x11,0x15,0x11,0x15,
    0x05,0x01,0x09,0x0D,0x0D,0x09,0x0D,0x09,
    0x09,0x0D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x0D,0x09,0x09,0x0D,0x09,0x0D,
    0x0D,0x09,0x31,0x35,0x35,0x31,0x35,0x31,
    0x21,0x25,0x2D,0x29,0x29,0x2D,0x29,0x2D,
    0x2D,0x29,0x35,0x31,0x31,0x35,0x31,0x35,
    0x25,0x21,0x29,0x2D,0x2D,0x29,0x2D,0x29,
    0x29,0x2D,0x11,0x15,0x15,0x11,0x15,0x11,
    0x01,0x05,0x0D,0x09,0x09,0x0D,0x09,0x0D,
    0x0D,0x09,0x15,0x11,0x11,0x15,0x11,0x15,
    0x05,0x01,0x09,0x0D,0x0D,0x09,0x0D,0x09,
    0x09,0x0D,0x35,0x31,0x31,0x35,0x31,0x35,
    0x46,0x02,0x02,0x06,0x02,0x06,0x06,0x02,
    0x0A,0x0E,0x02,0x06,0x06,0x02,0x0A,0x0E,
    0x02,0x06,0x06,0x02,0x06,0x02,0x02,0x06,
    0x0E,0x0A,0x06,0x02,0x02,0x06,0x0E,0x0A,
    0x22,0x26,0x26,0x22,0x26,0x22,0x22,0x26,
    0x2E,0x2A,0x26,0x22,0x22,0x26,0x2E,0x2A,
    0x26,0x22,0x22,0x26,0x22,0x26,0x26,0x22,
    0x2A,0x2E,0x22,0x26,0x26,0x22,0x2A,0x2E,
    0x02,0x06,0x06,0x02,0x06,0x02,0x02,0x06,
    0x0E,0x0A,0x06,0x02,0x02,0x06,0x0E,0x0A,
    0x06,0x02,0x02,0x06,0x02,0x06,0x06,0x02,
    0x0A,0x0E,0x02,0x06,0x06,0x02,0x0A,0x0E,
    0x26,0x22,0x22,0x26,0x22,0x26,0x26,0x22,
    0x2A,0x2E,0x22,0x26,0x26,0x22,0x2A,0x2E,
    0x22,0x26,0x26,0x22,0x26,0x22,0x22,0x26,
    0x2E,0x2A,0x26,0x22,0x22,0x26,0x2E,0x2A,
    0x82,0x86,0x86,0x82,0x86,0x82,0x82,0x86,
    0x8E,0x8A,0x86,0x82,0x82,0x86,0x8E,0x8A,
    0x86,0x82,0x82,0x86,0x82,0x86,0x86,0x82,
    0x8A,0x8E,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x03,0x07,0x07,0x03,0x07,0x03,0x03,0x07,
    0x0F,0x0B,0x07,0x03,0x03,0x07,0x0F,0x0B,
    0x07,0x03,0x03,0x07,0x03,0x07,0x07,0x03,
    0x0B,0x0F,0x03,0x07,0x07,0x03,0x0B,0x0F,
    0x27,0x23,0x23,0x27,0x23,0x27,0x27,0x23,
    0x2B,0x2F,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x23,0x27,0x27,0x23,0x27,0x23,0x23,0x27,
    0x2F,0x2B,0x27,0x23,0x23,0x27,0x2F,0x2B,
    0x83,0x87,0x87,0x83,0x87,0x83,0x83,0x87,
    0x8F,0x8B,0x87,0x83,0x83,0x87,0x8F,0x8B,
    0x87,0x83,0x83,0x87,0x83,0x87,0x87,0x83,
    0x8B,0x8F,0x83,0x87,0x87,0x83,0x8B,0x8F,
    0xA7,0xA3,0xA3,0xA7,0xA3,0xA7,0xA7,0xA3,
    0xAB,0xAF,0xA3,0xA7,0xA7,0xA3,0xAB,0xAF,
    0xA3,0xA7,0xA7,0xA3,0xA7,0xA3,0xA3,0xA7,
    0xAF,0xAB,0xA7,0xA3,0xA3,0xA7,0xAF,0xAB,
    0x87,0x83,0x83,0x87,0x83,0x87,0x87,0x83,
    0x8B,0x8F,0x83,0x87,0x87,0x83,0x8B,0x8F,
    0x83,0x87,0x87,0x83,0x87,0x83,0x83,0x87,
    0x8F,0x8B,0x87,0x83,0x83,0x87,0x8F,0x8B,
    0xA3,0xA7,0xA7,0xA3,0xA7,0xA3,0xA3,0xA7,
    0xAF,0xAB,0xA7,0xA3,0xA3,0xA7,0xAF,0xAB,
    0xA7,0xA3,0xA3,0xA7,0xA3,0xA7,0xA7,0xA3,
    0xAB,0xAF,0xA3,0xA7,0xA7,0xA3,0xAB,0xAF,
    0x47,0x03,0x03,0x07,0x03,0x07,0x07,0x03,
    0x0B,0x0F,0x03,0x07,0x07,0x03,0x0B,0x0F,
    0x03,0x07,0x07,0x03,0x07,0x03,0x03,0x07,
    0x0F,0x0B,0x07,0x03,0x03,0x07,0x0F,0x0B,
    0x23,0x27,0x27,0x23,0x27,0x23,0x23,0x27,
    0x2F,0x2B,0x27,0x23,0x23,0x27,0x2F,0x2B,
    0x27,0x23,0x23,0x27,0x23,0x27,0x27,0x23,
    0x2B,0x2F,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x03,0x07,0x07,0x03,0x07,0x03,0x03,0x07,
    0x0F,0x0B,0x07,0x03,0x03,0x07,0x0F,0x0B,
    0x07,0x03,0x03,0x07,0x03,0x07,0x07,0x03,
    0x0B,0x0F,0x03,0x07,0x07,0x03,0x0B,0x0F,
    0x27,0x23,0x23,0x27,0x23,0x27,0x27,0x23,
    0x2B,0x2F,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x23,0x27,0x27,0x23,0x27,0x23,0x23,0x27,
    0x2F,0x2B,0x27,0x23,0x23,0x27,0x2F,0x2B,
    0x83,0x87,0x87,0x83,0x87,0x83,0x83,0x87,
    0x8F,0x8B,0x87,0x83,0x83,0x87,0x8F,0x8B,
    0x87,0x83,0x83,0x87,0x83,0x87,0x87,0x83,
    0x8B,0x8F,0x83,0x87,0x87,0x83,0x8B,0x8F,
    0xBE,0xBA,0xBE,0xBA,0xBA,0xBE,0x46,0x02,
    0x02,0x06,0x02,0x06,0x06,0x02,0x0A,0x0E,
    0x1E,0x1A,0x1E,0x1A,0x1A,0x1E,0x02,0x06,
    0x06,0x02,0x06,0x02,0x02,0x06,0x0E,0x0A,
    0x1A,0x1E,0x1A,0x1E,0x1E,0x1A,0x22,0x26,
    0x26,0x22,0x26,0x22,0x22,0x26,0x2E,0x2A,
    0x3A,0x3E,0x3A,0x3E,0x3E,0x3A,0x26,0x22,
    0x22,0x26,0x22,0x26,0x26,0x22,0x2A,0x2E,
    0x3E,0x3A,0x3E,0x3A,0x3A,0x3E,0x02,0x06,
    0x06,0x02,0x06,0x02,0x02,0x06,0x0E,0x0A,
    0x1A,0x1E,0x1A,0x1E,0x1E,0x1A,0x06,0x02,
    0x02,0x06,0x02,0x06,0x06,0x02,0x0A,0x0E,
    0x1E,0x1A,0x1E,0x1A,0x1A,0x1E,0x26,0x22,
    0x22,0x26,0x22,0x26,0x26,0x22,0x2A,0x2E,
    0x3E,0x3A,0x3E,0x3A,0x3A,0x3E,0x22,0x26,
    0x26,0x22,0x26,0x22,0x22,0x26,0x2E,0x2A,
    0x3A,0x3E,0x3A,0x3E,0x3E,0x3A,0x82,0x86,
    0x86,0x82,0x86,0x82,0x82,0x86,0x8E,0x8A,
    0x9A,0x9E,0x9A,0x9E,0x9E,0x9A,0x86,0x82,
    0x82,0x86,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x3F,0x3B,0x3F,0x3B,0x3B,0x3F,0x03,0x07,
    0x07,0x03,0x07,0x03,0x03,0x07,0x0F,0x0B,
    0x1B,0x1F,0x1B,0x1F,0x1F,0x1B,0x07,0x03,
    0x03,0x07,0x03,0x07,0x07,0x03,0x0B,0x0F,
    0x1F,0x1B,0x1F,0x1B,0x1B,0x1F,0x27,0x23,
    0x23,0x27,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x3F,0x3B,0x3F,0x3B,0x3B,0x3F,0x23,0x27,
    0x27,0x23,0x27,0x23,0x23,0x27,0x2F,0x2B,
    0x3B,0x3F,0x3B,0x3F,0x3F,0x3B,0x83,0x87,
    0x87,0x83,0x87,0x83,0x83,0x87,0x8F,0x8B,
    0x9B,0x9F,0x9B,0x9F,0x9F,0x9B,0x87,0x83,
    0x83,0x87,0x83,0x87,0x87,0x83,0x8B,0x8F,
    0x9F,0x9B,0x9F,0x9B,0x9B,0x9F,0xA7,0xA3,
    0xA3,0xA7,0xA3,0xA7,0xA7,0xA3,0xAB,0xAF,
    0xBF,0xBB,0xBF,0xBB,0xBB,0xBF,0xA3,0xA7,
    0xA7,0xA3,0xA7,0xA3,0xA3,0xA7,0xAF,0xAB,
    0xBB,0xBF,0xBB,0xBF,0xBF,0xBB,0x87,0x83,
    0x83,0x87,0x83,0x87,0x87,0x83,0x8B,0x8F,
    0x9F,0x9B,0x9F,0x9B,0x9B,0x9F,0x83,0x87,
    0x87,0x83,0x87,0x83,0x83,0x87,0x8F,0x8B,
    0x9B,0x9F,0x9B,0x9F,0x9F,0x9B,0xA3,0xA7,
    0xA7,0xA3,0xA7,0xA3,0xA3,0xA7,0xAF,0xAB,
    0xBB,0xBF,0xBB,0xBF,0xBF,0xBB,0xA7,0xA3,
    0xA3,0xA7,0xA3,0xA7,0xA7,0xA3,0xAB,0xAF,
    0xBF,0xBB,0xBF,0xBB,0xBB,0xBF,0x47,0x03,
    0x03,0x07,0x03,0x07,0x07,0x03,0x0B,0x0F,
    0x1F,0x1B,0x1F,0x1B,0x1B,0x1F,0x03,0x07,
    0x07,0x03,0x07,0x03,0x03,0x07,0x0F,0x0B,
    0x1B,0x1F,0x1B,0x1F,0x1F,0x1B,0x23,0x27,
    0x27,0x23,0x27,0x23,0x23,0x27,0x2F,0x2B,
    0x3B,0x3F,0x3B,0x3F,0x3F,0x3B,0x27,0x23,
    0x23,0x27,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x3F,0x3B,0x3F,0x3B,0x3B,0x3F,0x03,0x07,
    0x07,0x03,0x07,0x03,0x03,0x07,0x0F,0x0B,
    0x1B,0x1F,0x1B,0x1F,0x1F,0x1B,0x07,0x03,
    0x03,0x07,0x03,0x07,0x07,0x03,0x0B,0x0F,
    0x1F,0x1B,0x1F,0x1B,0x1B,0x1F,0x27,0x23,
    0x23,0x27,0x23,0x27,0x27,0x23,0x2B,0x2F,
    0x3F,0x3B,0x3F,0x3B,0x3B,0x3F,0x23,0x27,
    0x27,0x23,0x27,0x23,0x23,0x27,0x2F,0x2B,
    0x3B,0x3F,0x3B,0x3F,0x3F,0x3B,0x83,0x87,
    0x87,0x83,0x87,0x83,0x83,0x87,0x8F,0x8B,
    0x9B,0x9F,0x9B,0x9F,0x9F,0x9B,0x87,0x83,
    0x83,0x87,0x83,0x87,0x87,0x83,0x8B,0x8F
};
/* ------------------------------------------------------------------------- */

/* Interrupt handling.  */

#define DO_INTERRUPT(int_kind)                                               \
  do {                                                                       \
        BYTE ik = (int_kind);                                                \
                                                                             \
        if (ik & (IK_IRQ | IK_NMI)) {                                        \
            if ((ik & IK_NMI) && 0) {                                        \
            } else if ((ik & IK_IRQ) && (reg_iff & 0x01)) {                  \
                ADDRESS jumpdst;                                             \
                /*TRACE_IRQ();*/                                             \
                if (mon_mask[e_comp_space] & (MI_STEP)) {                    \
                    mon_check_icount_interrupt();                            \
                }                                                            \
                reg_iff &= 0xfe;                                             \
                CLK += 4;                                                    \
                --reg_sp;                                                    \
                STORE((reg_sp), ((BYTE)(z80_reg_pc >> 8)));                  \
                CLK += 4;                                                    \
                --reg_sp;                                                    \
                STORE((reg_sp), ((BYTE)(z80_reg_pc & 0xff)));                \
                jumpdst = (LOAD(reg_i << 8) << 8);                           \
                CLK += 4;                                                    \
                jumpdst |= (LOAD((reg_i << 8) + 1));                         \
                JUMP(jumpdst);                                               \
                CLK += 3;                                                    \
            }                                                                \
        }                                                                    \
        if (ik & (IK_TRAP | IK_RESET)) {                                     \
            if (ik & IK_TRAP) {                                              \
                interrupt_do_trap(cpu_int_status, (ADDRESS) z80_reg_pc);     \
                if (interrupt_check_pending_interrupt(cpu_int_status)        \
                                                      & IK_RESET)            \
                    ik |= IK_RESET;                                          \
            }                                                                \
            if (ik & IK_RESET) {                                             \
                interrupt_ack_reset(cpu_int_status);                         \
                maincpu_reset();                                             \
            }                                                                \
        }                                                                    \
        if (ik & (IK_MONITOR)) {                                             \
            caller_space = e_comp_space;                                     \
            if (mon_mask[e_comp_space] & (MI_BREAK)) {                       \
               if (check_breakpoints(e_comp_space, (ADDRESS) z80_reg_pc)) {  \
                  mon((ADDRESS) z80_reg_pc);                                 \
               }                                                             \
            }                                                                \
            if (mon_mask[e_comp_space] & (MI_STEP)) {                        \
               mon_check_icount((ADDRESS) z80_reg_pc);                       \
            }                                                                \
            if (mon_mask[e_comp_space] & (MI_WATCH)) {                       \
               mon_check_watchpoints((ADDRESS) z80_reg_pc);                  \
            }                                                                \
        }                                                                    \
                                                                             \
  } while (0)

/* ------------------------------------------------------------------------- */

/* Opcodes.  */

#define ADC(loadval, clk_inc1, clk_inc2, pc_inc)                    \
  do {                                                              \
      BYTE tmp, carry, value;                                       \
                                                                    \
      CLK += clk_inc1;                                              \
      value = loadval;                                              \
      carry = LOCAL_CARRY();                                        \
      tmp = reg_a + value + carry;                                  \
      reg_f = SZP[tmp];                                             \
      LOCAL_SET_CARRY((WORD)((WORD)reg_a + (WORD)value              \
                      + (WORD)(carry)) & 0x100);                    \
      LOCAL_SET_HALFCARRY((reg_a ^ value ^ tmp) & H_FLAG);          \
      LOCAL_SET_PARITY((~(reg_a ^ value)) & (reg_a ^ tmp) & 0x80);  \
      reg_a = tmp;                                                  \
      CLK += clk_inc2;                                              \
      INC_PC(pc_inc);                                               \
  } while (0)

#define ADCHLREG(reg_valh, reg_vall)                                  \
  do {                                                                \
      DWORD tmp, carry;                                               \
                                                                      \
      carry = LOCAL_CARRY();                                          \
      tmp = (DWORD)((reg_h << 8) + reg_l)                             \
            + (DWORD)((reg_valh << 8) + reg_vall) + carry;            \
      LOCAL_SET_ZERO(!(tmp & 0xffff));                                \
      LOCAL_SET_NADDSUB(0);                                           \
      LOCAL_SET_SIGN(tmp & 0x8000);                                   \
      LOCAL_SET_CARRY(tmp & 0x10000);                                 \
      LOCAL_SET_HALFCARRY(((tmp >> 8) ^ reg_valh ^ reg_h) & H_FLAG);  \
      LOCAL_SET_PARITY((~(reg_h ^ reg_valh)) &                        \
                       (reg_valh ^ (tmp >> 8)) & 0x80);               \
      reg_h = (BYTE)(tmp >> 8);                                       \
      reg_l = (BYTE)(tmp & 0xff);                                     \
      CLK += 15;                                                      \
      INC_PC(2);                                                      \
  } while (0)

#define ADCHLSP()                                                          \
  do {                                                                     \
      DWORD tmp, carry;                                                    \
                                                                           \
      carry = LOCAL_CARRY();                                               \
      tmp = (DWORD)((reg_h << 8) + reg_l) + (DWORD)(reg_sp) + carry;       \
      LOCAL_SET_ZERO(!(tmp & 0xffff));                                     \
      LOCAL_SET_NADDSUB(0);                                                \
      LOCAL_SET_SIGN(tmp & 0x8000);                                        \
      LOCAL_SET_CARRY(tmp & 0x10000);                                      \
      LOCAL_SET_HALFCARRY(((tmp >> 8) ^ (reg_sp >> 8) ^ reg_h) & H_FLAG);  \
      LOCAL_SET_PARITY((~(reg_h ^ (reg_sp >> 8))) &                        \
                       ((reg_sp >> 8) ^ (tmp >> 8)) & 0x80);               \
      reg_h = (BYTE)(tmp >> 8);                                            \
      reg_l = (BYTE)(tmp & 0xff);                                          \
      CLK += 15;                                                           \
      INC_PC(2);                                                           \
  } while (0)

#define ADD(loadval, clk_inc1, clk_inc2, pc_inc)                    \
  do {                                                              \
      BYTE tmp, value;                                              \
                                                                    \
      CLK += clk_inc1;                                              \
      value = loadval;                                              \
      tmp = reg_a + value;                                          \
      reg_f = SZP[tmp];                                             \
      LOCAL_SET_CARRY((WORD)((WORD)reg_a + (WORD)value) & 0x100);   \
      LOCAL_SET_HALFCARRY((reg_a ^ value ^ tmp) & H_FLAG);          \
      LOCAL_SET_PARITY((~(reg_a ^ value)) & (reg_a ^ tmp) & 0x80);  \
      reg_a = tmp;                                                  \
      CLK += clk_inc2;                                              \
      INC_PC(pc_inc);                                               \
  } while (0)

#define ADDXXREG(reg_dsth, reg_dstl, reg_valh, reg_vall, clk_inc, pc_inc)  \
  do {                                                                     \
      DWORD tmp;                                                           \
                                                                           \
      tmp = (DWORD)((reg_dsth << 8) + reg_dstl)                            \
            + (DWORD)((reg_valh << 8) + reg_vall);                         \
      LOCAL_SET_NADDSUB(0);                                                \
      LOCAL_SET_CARRY(tmp & 0x10000);                                      \
      LOCAL_SET_HALFCARRY(((tmp >> 8) ^ reg_valh ^ reg_dsth) & H_FLAG);    \
      reg_h = (BYTE)(tmp >> 8);                                            \
      reg_l = (BYTE)(tmp & 0xff);                                          \
      CLK += clk_inc;                                                      \
      INC_PC(pc_inc);                                                      \
  } while (0)

#define ADDXXSP(reg_dsth, reg_dstl, clk_inc, pc_inc)                          \
  do {                                                                        \
      DWORD tmp;                                                              \
                                                                              \
      tmp = (DWORD)((reg_dsth << 8) + reg_dstl) + (DWORD)(reg_sp);            \
      LOCAL_SET_NADDSUB(0);                                                   \
      LOCAL_SET_CARRY(tmp & 0x10000);                                         \
      LOCAL_SET_HALFCARRY(((tmp >> 8) ^ (reg_sp >> 8) ^ reg_dsth) & H_FLAG);  \
      reg_h = (BYTE)(tmp >> 8);                                               \
      reg_l = (BYTE)(tmp & 0xff);                                             \
      CLK += clk_inc;                                                         \
      INC_PC(pc_inc);                                                         \
  } while (0)

#define AND(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += clk_inc1;                          \
      reg_a &= (value);                         \
      reg_f = SZP[reg_a];                       \
      LOCAL_SET_HALFCARRY(1);                   \
      CLK += clk_inc2;                          \
      INC_PC(pc_inc);                           \
  } while (0)

#define BIT(reg_val, value, clk_inc)              \
  do {                                            \
      LOCAL_SET_NADDSUB(0);                       \
      LOCAL_SET_HALFCARRY(1);                     \
      LOCAL_SET_ZERO(!(reg_val & (1 << value)));  \
      /***LOCAL_SET_PARITY(LOCAL_ZERO());***/     \
      CLK += clk_inc;                             \
      INC_PC(2);                                  \
  } while (0)

#define BRANCH(cond, value)                                   \
  do {                                                        \
      if (cond) {                                             \
          unsigned int dest_addr;                             \
                                                              \
          dest_addr = z80_reg_pc + 2 + (signed char)(value);  \
          z80_reg_pc = dest_addr & 0xffff;                    \
          CLK += 7;                                           \
      } else {                                                \
          CLK += 7;                                           \
          INC_PC(2);                                          \
      }                                                       \
  } while (0)

#define CALL(reg_val, clk_inc, pc_inc)               \
  do {                                               \
      INC_PC(pc_inc);                                \
      CLK += 3;                                      \
      --reg_sp;                                      \
      STORE((reg_sp), ((BYTE)(z80_reg_pc >> 8)));    \
      CLK += 3;                                      \
      --reg_sp;                                      \
      STORE((reg_sp), ((BYTE)(z80_reg_pc & 0xff)));  \
      JUMP(reg_val);                                 \
      CLK += (clk_inc - 6);                          \
  } while (0)

#define CALL_COND(reg_value, cond, clk_inc1, clk_inc2, pc_inc)  \
  do {                                                          \
      if (cond) {                                               \
          CALL(reg_value, clk_inc1, pc_inc);                    \
      } else {                                                  \
          CLK += clk_inc2;                                      \
          INC_PC(3);                                            \
      }                                                         \
  } while (0)

#define CCF()                                 \
  do {                                        \
      LOCAL_SET_HALFCARRY((LOCAL_CARRY()));   \
      LOCAL_SET_CARRY(!(LOCAL_CARRY()));      \
      LOCAL_SET_NADDSUB(0);                   \
      CLK += 4;                               \
      INC_PC(1);                              \
  } while (0)

#define CP(loadval, clk_inc1, clk_inc2, pc_inc)                  \
  do {                                                           \
      BYTE tmp, value;                                           \
                                                                 \
      CLK += clk_inc1;                                           \
      value = loadval;                                           \
      tmp = reg_a - value;                                       \
      reg_f = N_FLAG | SZP[tmp];                                 \
      LOCAL_SET_CARRY(value > reg_a);                            \
      LOCAL_SET_HALFCARRY((reg_a ^ value ^ tmp) & H_FLAG);       \
      LOCAL_SET_PARITY((reg_a ^ value) & (reg_a ^ tmp) & 0x80);  \
      CLK += clk_inc2;                                           \
      INC_PC(pc_inc);                                            \
  } while (0)

#define CPDI(HL_FUNC)                                     \
  do {                                                    \
      BYTE val, tmp;                                      \
                                                          \
      CLK += 4;                                           \
      val = LOAD(HL_WORD());                              \
      tmp = reg_a - val;                                  \
      HL_FUNC;                                            \
      DEC_BC_WORD();                                      \
      reg_f = N_FLAG | SZP[tmp] | LOCAL_CARRY();          \
      /***LOCAL_SET_CARRY(val > reg_a);***/               \
      LOCAL_SET_HALFCARRY((reg_a ^ val ^ tmp) & H_FLAG);  \
      LOCAL_SET_PARITY(reg_b | reg_c);                    \
      CLK += 1;                                           \
      INC_PC(2);                                          \
  } while (0)

#define CPDIR(HL_FUNC)                                        \
  do {                                                        \
      BYTE val, tmp;                                          \
                                                              \
      CLK += 4;                                               \
      val = LOAD(HL_WORD());                                  \
      tmp = reg_a - val;                                      \
      HL_FUNC;                                                \
      DEC_BC_WORD();                                          \
      CLK += 17;                                              \
      if (!(BC_WORD() && tmp)) {                              \
          reg_f = N_FLAG | SZP[tmp] | LOCAL_CARRY();          \
          /***LOCAL_SET_CARRY(val > reg_a);***/               \
          LOCAL_SET_HALFCARRY((reg_a ^ val ^ tmp) & H_FLAG);  \
          LOCAL_SET_PARITY(reg_b | reg_c);                    \
          CLK += 5;                                           \
          INC_PC(2);                                          \
      }                                                       \
  } while (0)

#define CPL()                  \
  do {                         \
      reg_a ^= 0xff;           \
      LOCAL_SET_NADDSUB(1);    \
      LOCAL_SET_HALFCARRY(1);  \
      CLK += 4;                \
      INC_PC(1);               \
  } while (0)

#define DAA()                                    \
  do {                                           \
      WORD tmp;                                  \
                                                 \
      tmp = reg_a | (LOCAL_CARRY() ? 0x100 : 0)  \
              | (LOCAL_HALFCARRY() ? 0x200 : 0)  \
              | (LOCAL_NADDSUB() ? 0x400 : 0);   \
      reg_a = daa_reg_a[tmp];                    \
      reg_f = daa_reg_f[tmp];                    \
      CLK += 4;                                  \
      INC_PC(1);                                 \
  } while (0)

#define DECHLIND()                                  \
  do {                                              \
      BYTE tmp;                                     \
                                                    \
      CLK += 4;                                     \
      tmp = LOAD(HL_WORD());                        \
      tmp--;                                        \
      CLK += 4;                                     \
      STORE(HL_WORD(), tmp);                        \
      reg_f = N_FLAG | SZP[tmp] | LOCAL_CARRY();    \
      LOCAL_SET_PARITY((tmp == 0x7f));              \
      LOCAL_SET_HALFCARRY(((tmp & 0x0f) == 0x0f));  \
      CLK += 3;                                     \
      INC_PC(1);                                    \
  } while (0)

#define DECINC(FUNC)  \
  do {                \
      CLK += 6;       \
      FUNC;           \
      INC_PC(1);      \
  } while (0)

#define DECREG(reg_val)                                 \
  do {                                                  \
      reg_val--;                                        \
      reg_f = N_FLAG | SZP[reg_val] | LOCAL_CARRY();    \
      LOCAL_SET_PARITY((reg_val == 0x7f));              \
      LOCAL_SET_HALFCARRY(((reg_val & 0x0f) == 0x0f));  \
      CLK += 4;                                         \
      INC_PC(1);                                        \
  } while (0)

#define DJNZ(value)          \
  do {                       \
      reg_b--;               \
      /***LOCAL_SET_NADDSUB(1);***/ \
      BRANCH(reg_b, value);  \
  } while (0)

#define DI()            \
  do {                  \
      reg_iff &= 0xfe;  \
      CLK += 4;         \
      INC_PC(1);        \
  } while (0)

#define EI()            \
  do {                  \
      reg_iff |= 0x01;  \
      CLK += 4;         \
      INC_PC(1);        \
  } while (0)

#define EXAFAF()        \
  do {                  \
      BYTE tmpl, tmph;  \
                        \
      tmph = reg_a;     \
      tmpl = reg_f;     \
      reg_a = reg_a2;   \
      reg_f = reg_f2;   \
      reg_a2 = tmph;    \
      reg_f2 = tmpl;    \
      CLK += 8;         \
      INC_PC(1);        \
  } while (0)

#define EXX()           \
  do {                  \
      BYTE tmpl, tmph;  \
                        \
      tmph = reg_b;     \
      tmpl = reg_c;     \
      reg_b = reg_b2;   \
      reg_c = reg_c2;   \
      reg_b2 = tmph;    \
      reg_c2 = tmpl;    \
      tmph = reg_d;     \
      tmpl = reg_e;     \
      reg_d = reg_d2;   \
      reg_e = reg_e2;   \
      reg_d2 = tmph;    \
      reg_e2 = tmpl;    \
      tmph = reg_h;     \
      tmpl = reg_l;     \
      reg_h = reg_h2;   \
      reg_l = reg_l2;   \
      reg_h2 = tmph;    \
      reg_l2 = tmpl;    \
      CLK += 8;         \
      INC_PC(1);        \
  } while (0)

#define EXDEHL()        \
  do {                  \
      BYTE tmpl, tmph;  \
                        \
      tmph = reg_d;     \
      tmpl = reg_e;     \
      reg_d = reg_h;    \
      reg_e = reg_l;    \
      reg_h = tmph;     \
      reg_l = tmpl;     \
      CLK += 4;         \
      INC_PC(1);        \
  } while (0)

#define EXHLSP()                  \
  do {                            \
      BYTE tmpl, tmph;            \
                                  \
      tmph = reg_h;               \
      tmpl = reg_l;               \
      CLK += 4;                   \
      reg_h = LOAD(reg_sp + 1);   \
      CLK += 4;                   \
      reg_l = LOAD(reg_sp);       \
      CLK += 4;                   \
      STORE((reg_sp + 1), tmph);  \
      CLK += 4;                   \
      STORE(reg_sp, tmpl);        \
      CLK += 3;                   \
      INC_PC(1);                  \
  } while (0)

#define HALT()   \
  do {           \
      CLK += 4;  \
  } while (0)

#define IM(value)                        \
  do {                                   \
      reg_iff = value | (reg_iff & 0xf9);  \
      CLK += 8;                            \
      INC_PC(2);                           \
  } while (0)

#define INA(value)        \
  do {                    \
      CLK += 4;           \
      reg_a = IN(value);  \
      /***reg_a = IN((reg_a << 8) | value);***/  \
      CLK += 7;           \
      INC_PC(2);          \
  } while (0)

#define INBC(reg_val, clk_inc1, clk_inc2, pc_inc)   \
  do {                                              \
      CLK += clk_inc1;                              \
      reg_val = IN(BC_WORD());                      \
      reg_f = SZP[reg_val & 0xff] | LOCAL_CARRY();  \
      CLK += clk_inc2;                              \
      INC_PC(pc_inc);                               \
  } while (0)

#define INCHLIND()                         \
  do {                                     \
      BYTE tmp;                            \
                                           \
      CLK += 4;                            \
      tmp = LOAD(HL_WORD());               \
      tmp++;                               \
      CLK += 4;                            \
      STORE(HL_WORD(), tmp);               \
      reg_f = SZP[tmp] | LOCAL_CARRY();    \
      LOCAL_SET_PARITY((tmp == 0x80));     \
      LOCAL_SET_HALFCARRY(!(tmp & 0x0f));  \
      CLK += 3;                            \
      INC_PC(1);                           \
  } while (0)

#define INCREG(reg_val)                        \
  do {                                         \
      reg_val++;                               \
      reg_f = SZP[reg_val] | LOCAL_CARRY();    \
      LOCAL_SET_PARITY((reg_val == 0x80));     \
      LOCAL_SET_HALFCARRY(!(reg_val & 0x0f));  \
      CLK += 4;                                \
      INC_PC(1);                               \
  } while (0)

#define JMP(addr, clk_inc)  \
  do {                      \
      CLK += clk_inc;       \
      JUMP(addr);           \
  } while (0)

#define JMP_COND(addr, cond, clk_inc1, clk_inc2)  \
  do {                                            \
      if (cond) {                                 \
          JMP(addr, clk_inc1);                    \
      } else {                                    \
          CLK += clk_inc2;                        \
          INC_PC(3);                              \
      }                                           \
  } while (0)

#define LDAI()                             \
  do {                                     \
      CLK += 6;                            \
      reg_a = reg_i;                       \
      reg_f = SZP[reg_a] | LOCAL_CARRY();  \
      LOCAL_SET_PARITY(reg_iff & 1);       \
      CLK += 3;                            \
      INC_PC(2);                           \
  } while (0)

#define LDDI(DE_FUNC, HL_FUNC)          \
  do {                                  \
      BYTE tmp;                         \
                                        \
      CLK += 4;                         \
      tmp = LOAD(HL_WORD());            \
      CLK += 4;                         \
      STORE(DE_WORD(), tmp);            \
      DEC_BC_WORD();                    \
      DE_FUNC;                          \
      HL_FUNC;                          \
      LOCAL_SET_NADDSUB(0);             \
      LOCAL_SET_PARITY(reg_b | reg_c);  \
      LOCAL_SET_HALFCARRY(0);           \
      CLK += 12;                        \
      INC_PC(2);                        \
  } while (0)

#define LDDIR(DE_FUNC, HL_FUNC)    \
  do {                             \
      BYTE tmp;                    \
                                   \
      CLK += 4;                    \
      tmp = LOAD(HL_WORD());       \
      CLK += 4;                    \
      STORE(DE_WORD(), tmp);       \
      DEC_BC_WORD();               \
      DE_FUNC;                     \
      HL_FUNC;                     \
      CLK += 13;                   \
      if (!(BC_WORD())) {          \
          LOCAL_SET_NADDSUB(0);    \
          LOCAL_SET_PARITY(0);     \
          LOCAL_SET_HALFCARRY(0);  \
          CLK += 5;                \
          INC_PC(2);               \
      }                            \
  } while (0)

#define LDIND(val, reg_valh, reg_vall, clk_inc1, clk_inc2, clk_inc3, pc_inc)  \
  do {                                                                        \
      CLK += clk_inc1;                                                        \
      reg_vall = LOAD((val));                                                 \
      CLK += clk_inc2;                                                        \
      reg_valh = LOAD((val) + 1);                                             \
      CLK += clk_inc3;                                                        \
      INC_PC(pc_inc);                                                         \
  } while (0)

/* Can be replaced by LDREG */
#define LDINDD(reg_dst, reg_valh, reg_vall, disp)                \
  do {                                                           \
      ADDRESS tmp;                                               \
                                                                 \
      CLK += 8;                                                  \
      tmp = ((reg_valh << 8) | reg_vall) + (signed char)(disp);  \
      reg_dst = LOAD(tmp);                                       \
      CLK += 11;                                                 \
      INC_PC(3);                                                 \
  } while (0)

#define LDSP(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                           \
      CLK += clk_inc1;                           \
      reg_sp = (WORD)(value);                    \
      CLK += clk_inc2;                           \
      INC_PC(pc_inc);                            \
  } while (0)

#define LDSPIND(value, clk_inc1, clk_inc2, clk_inc3, pc_inc)  \
  do {                                                        \
      CLK += clk_inc1;                                        \
      reg_sp = LOAD(value);                                   \
      CLK += clk_inc2;                                        \
      reg_sp |= LOAD(value + 1) >> 8;                         \
      CLK += clk_inc3;                                        \
      INC_PC(pc_inc);                                         \
  } while (0)

#define LDREG(reg_dest, value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                                      \
      BYTE tmp;                                             \
                                                            \
      CLK += clk_inc1;                                      \
      tmp = (BYTE)(value);                                  \
      reg_dest = tmp;                                       \
      CLK += clk_inc2;                                      \
      INC_PC(pc_inc);                                       \
  } while (0)

#define LDW(value, reg_valh, reg_vall, clk_inc1, clk_inc2, pc_inc)  \
  do {                                                              \
      CLK += clk_inc1;                                              \
      reg_vall = (BYTE)((value) & 0xff);                            \
      reg_valh = (BYTE)((value) >> 8);                              \
      CLK += clk_inc2;                                              \
      INC_PC(pc_inc);                                               \
  } while (0)

#define NEG()                                      \
  do {                                             \
      BYTE tmp;                                    \
                                                   \
      tmp = 0 - reg_a;                             \
      reg_f = N_FLAG | SZP[tmp];                   \
      LOCAL_SET_HALFCARRY((reg_a ^ tmp) & H_FLAG); \
      LOCAL_SET_PARITY(reg_a & tmp & 0x80);        \
      LOCAL_SET_CARRY(reg_a > 0);                  \
      reg_a = tmp;                                 \
      CLK += 8;                                    \
      INC_PC(2);                                   \
  } while (0)

#define NOP()     \
  do {            \
      CLK += 4;   \
      INC_PC(1);  \
  } while (0)

#define OR(reg_val, clk_inc1, clk_inc2, pc_inc)  \
  do {                                           \
      CLK += clk_inc1;                           \
      reg_a |= reg_val;                          \
      reg_f = SZP[reg_a];                        \
      CLK += clk_inc2;                           \
      INC_PC(pc_inc);                            \
  } while (0)

#define OUTA(value)       \
  do {                    \
      CLK += 4;           \
      OUT(value, reg_a);  \
      /***OUT((reg_a << 8) | value, reg_a);***/ \
      CLK += 7;           \
      INC_PC(2);          \
  } while (0)

#define OUTBC(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                            \
      CLK += clk_inc1;                            \
      OUT(BC_WORD(), value);                      \
      CLK += clk_inc2;                            \
      INC_PC(pc_inc);                             \
  } while (0)

#define POP(reg_valh, reg_vall, pc_inc)  \
  do {                                   \
      CLK += 4;                          \
      reg_vall = LOAD(reg_sp);           \
      ++reg_sp;                          \
      CLK += 4;                          \
      reg_valh = LOAD(reg_sp);           \
      ++reg_sp;                          \
      CLK += 2;                          \
      INC_PC(pc_inc);                    \
  } while (0)

#define PUSH(reg_valh, reg_vall, pc_inc)  \
  do {                                    \
      CLK += 4;                           \
      --reg_sp;                           \
      STORE((reg_sp), (reg_valh));        \
      CLK += 4;                           \
      --reg_sp;                           \
      STORE((reg_sp), (reg_vall));        \
      CLK += 3;                           \
      INC_PC(pc_inc);                     \
  } while (0)

#define RES(reg_val, value)        \
  do {                             \
      reg_val &= (~(1 << value));  \
      CLK += 8;                    \
      INC_PC(2);                   \
  } while (0)

#define RESHL(value)           \
  do {                         \
      BYTE tmp;                \
                               \
      CLK += 4;                \
      tmp = LOAD(HL_WORD());   \
      tmp &= (~(1 << value));  \
      CLK += 4;                \
      STORE(HL_WORD(), tmp);   \
      CLK += 7;                \
      INC_PC(2);               \
  } while (0)

#define RET()                                          \
  do {                                                 \
      ADDRESS tmp;                                     \
                                                       \
      CLK += 4;                                        \
      tmp = LOAD(reg_sp);                              \
      CLK += 4;                                        \
      tmp |= LOAD((reg_sp + 1)) << 8;                  \
      reg_sp += 2;                                     \
      JUMP(tmp);                                       \
      CLK += 2;                                        \
  } while (0)

#define RET_COND(cond)  \
  do {                  \
      if (cond) {       \
          RET();        \
      } else {          \
          CLK += 5;     \
          INC_PC(1);    \
      }                 \
  } while (0)

#define RL(reg_val)                              \
  do {                                           \
      BYTE rot;                                  \
                                                 \
      rot = (reg_val & 0x80) ? C_FLAG : 0;       \
      reg_val = (reg_val << 1) | LOCAL_CARRY();  \
      reg_f = rot | SZP[reg_val];                \
      CLK += 8;                                  \
      INC_PC(2);                                 \
  } while (0)

#define RLA()                                \
  do {                                       \
      BYTE rot;                              \
                                             \
      rot = (reg_a & 0x80) ? C_FLAG : 0;     \
      reg_a = (reg_a << 1) | LOCAL_CARRY();  \
      LOCAL_SET_CARRY(rot);                  \
      LOCAL_SET_NADDSUB(0);                  \
      LOCAL_SET_HALFCARRY(0);                \
      CLK += 4;                              \
      INC_PC(1);                             \
  } while (0)

#define RLC(reg_val)                        \
  do {                                      \
      BYTE rot;                             \
                                            \
      rot = (reg_val & 0x80) ? C_FLAG : 0;  \
      reg_val = (reg_val << 1) | rot;       \
      reg_f = rot | SZP[reg_val];           \
      CLK += 8;                             \
      INC_PC(2);                            \
  } while (0)

#define RLCA()                            \
  do {                                    \
      BYTE rot;                           \
                                          \
      rot = (reg_a & 0x80) ? C_FLAG : 0;  \
      reg_a = (reg_a << 1) | rot;         \
      LOCAL_SET_CARRY(rot);               \
      LOCAL_SET_NADDSUB(0);               \
      LOCAL_SET_HALFCARRY(0);             \
      CLK += 4;                           \
      INC_PC(1);                          \
  } while (0)

#define RLCHL()                              \
  do {                                       \
      BYTE rot, tmp;                         \
                                             \
      CLK += 4;                              \
      tmp = LOAD(HL_WORD());                 \
      rot = (tmp & 0x80) ? C_FLAG : 0;       \
      tmp = (tmp << 1) | rot;                \
      CLK += 4;                              \
      STORE(HL_WORD(), tmp);                 \
      reg_f = rot | SZP[tmp];                \
      CLK += 7;                              \
      INC_PC(2);                             \
  } while (0)

#define RLHL()                           \
  do {                                   \
      BYTE rot, tmp;                     \
                                         \
      CLK += 4;                          \
      tmp = LOAD(HL_WORD());             \
      rot = (tmp & 0x80) ? C_FLAG : 0;   \
      tmp = (tmp << 1) | LOCAL_CARRY();  \
      CLK += 4;                          \
      STORE(HL_WORD(), tmp);             \
      reg_f = rot | SZP[tmp];            \
      CLK += 7;                          \
      INC_PC(2);                         \
  } while (0)

#define RR(reg_val)                                           \
  do {                                                        \
      BYTE rot;                                               \
                                                              \
      rot = reg_val & C_FLAG;                                 \
      reg_val = (reg_val >> 1) | (LOCAL_CARRY() ? 0x80 : 0);  \
      reg_f = rot | SZP[reg_val];                             \
      CLK += 8;                                               \
      INC_PC(2);                                              \
  } while (0)

#define RRA()                                             \
  do {                                                    \
      BYTE rot;                                           \
                                                          \
      rot = reg_a & C_FLAG;                               \
      reg_a = (reg_a >> 1) | (LOCAL_CARRY() ? 0x80 : 0);  \
      LOCAL_SET_CARRY(rot);                               \
      LOCAL_SET_NADDSUB(0);                               \
      LOCAL_SET_HALFCARRY(0);                             \
      CLK += 4;                                           \
      INC_PC(1);                                          \
  } while (0)

#define RRC(reg_val)                                  \
  do {                                                \
      BYTE rot;                                       \
                                                      \
      rot = reg_val & C_FLAG;                         \
      reg_val = (reg_val >> 1) | ((rot) ? 0x80 : 0);  \
      reg_f = rot | SZP[reg_val];                     \
      CLK += 8;                                       \
      INC_PC(2);                                      \
  } while (0)

#define RRCA()                                    \
  do {                                            \
      BYTE rot;                                   \
                                                  \
      rot = reg_a & C_FLAG;                       \
      reg_a = (reg_a >> 1) | ((rot) ? 0x80 : 0);  \
      LOCAL_SET_CARRY(rot);                       \
      LOCAL_SET_NADDSUB(0);                       \
      LOCAL_SET_HALFCARRY(0);                     \
      CLK += 4;                                   \
      INC_PC(1);                                  \
  } while (0)

#define RRCHL()                               \
  do {                                        \
      BYTE rot, tmp;                          \
                                              \
      CLK += 4;                               \
      tmp = LOAD(HL_WORD());                  \
      rot = tmp & C_FLAG;                     \
      tmp = (tmp >> 1) | ((rot) ? 0x80 : 0);  \
      CLK += 4;                               \
      STORE(HL_WORD(), tmp);                  \
      reg_f = rot | SZP[tmp];                 \
      CLK += 7;                               \
      INC_PC(2);                              \
  } while (0)

#define RRHL()                                        \
  do {                                                \
      BYTE rot, tmp;                                  \
                                                      \
      CLK += 4;                                       \
      tmp = LOAD(HL_WORD());                          \
      rot = tmp & C_FLAG;                             \
      tmp = (tmp >> 1) | (LOCAL_CARRY() ? 0x80 : 0);  \
      CLK += 4;                                       \
      STORE(HL_WORD(), tmp);                          \
      reg_f = rot | SZP[tmp];                         \
      CLK += 7;                                       \
      INC_PC(2);                                      \
  } while (0)

#define SBCHLREG(reg_valh, reg_vall)                                         \
  do {                                                                       \
      DWORD tmp;                                                             \
      BYTE carry;                                                            \
                                                                             \
      carry = LOCAL_CARRY();                                                 \
      tmp = (DWORD)(HL_WORD()) - (DWORD)((reg_valh << 8) + reg_vall)         \
            - (DWORD)(carry);                                                \
      reg_f = N_FLAG;                                                        \
      LOCAL_SET_CARRY(tmp & 0x10000);                                        \
      LOCAL_SET_HALFCARRY((reg_h ^ reg_valh ^ (tmp >> 8)) & H_FLAG);         \
      LOCAL_SET_PARITY(((reg_h ^ (tmp >> 8)) & (reg_h ^ reg_valh)) & 0x80);  \
      LOCAL_SET_ZERO(!(tmp & 0xffff));                                       \
      LOCAL_SET_SIGN(tmp & 0x8000);                                          \
      reg_h = (BYTE)(tmp >> 8);                                              \
      reg_l = (BYTE)(tmp & 0xff);                                            \
      CLK += 15;                                                             \
      INC_PC(2);                                                             \
  } while (0)

#define SBCHLSP()                                                          \
  do {                                                                     \
      DWORD tmp;                                                           \
      BYTE carry;                                                          \
                                                                           \
      carry = LOCAL_CARRY();                                               \
      tmp = (DWORD)(HL_WORD()) - (DWORD)reg_sp - (DWORD)(carry);           \
      reg_f = N_FLAG;                                                      \
      LOCAL_SET_CARRY(tmp & 0x10000);                                      \
      LOCAL_SET_HALFCARRY((reg_h ^ (reg_sp >> 8) ^ (tmp >> 8)) & H_FLAG);  \
      LOCAL_SET_PARITY(((reg_h ^ (tmp >> 8))                               \
                       & (reg_h ^ (reg_sp >> 8))) & 0x80);                 \
      LOCAL_SET_ZERO(!(tmp & 0xffff));                                     \
      LOCAL_SET_SIGN(tmp & 0x8000);                                        \
      reg_h = (BYTE)(tmp >> 8);                                            \
      reg_l = (BYTE)(tmp & 0xff);                                          \
      CLK += 15;                                                           \
      INC_PC(2);                                                           \
  } while (0)

#define SBC(loadval, clk_inc1, clk_inc2, pc_inc)                 \
  do {                                                           \
      BYTE tmp, carry, value;                                    \
                                                                 \
      CLK += clk_inc1;                                           \
      value = loadval;                                           \
      carry = LOCAL_CARRY();                                     \
      tmp = reg_a - value - carry;                               \
      reg_f = N_FLAG | SZP[tmp];                                 \
      LOCAL_SET_HALFCARRY((reg_a ^ value ^ tmp) & H_FLAG);       \
      LOCAL_SET_PARITY((reg_a ^ value) & (reg_a ^ tmp) & 0x80);  \
      LOCAL_SET_CARRY((WORD)((WORD)value                         \
                      + (WORD)(carry)) > reg_a);                 \
      reg_a = tmp;                                               \
      CLK += clk_inc2;                                           \
      INC_PC(pc_inc);                                            \
  } while (0)

#define SCF()                  \
  do {                         \
      LOCAL_SET_CARRY(1);      \
      LOCAL_SET_HALFCARRY(0);  \
      LOCAL_SET_NADDSUB(0);    \
      CLK += 4;                \
      INC_PC(1);               \
  } while (0)

#define SET(reg_val, value)     \
  do {                          \
      reg_val |= (1 << value);  \
      CLK += 8;                 \
      INC_PC(2);                \
  } while (0)

#define SETHL(value)          \
  do {                        \
      BYTE tmp;               \
                              \
      CLK += 4;               \
      tmp = LOAD(HL_WORD());  \
      tmp |= (1 << value);    \
      CLK += 4;               \
      STORE(HL_WORD(), tmp);  \
      CLK += 7;               \
      INC_PC(2);              \
  } while (0)


#define SLA(reg_val)                        \
  do {                                      \
      BYTE rot;                             \
                                            \
      rot = (reg_val & 0x80) ? C_FLAG : 0;  \
      reg_val <<= 1;                        \
      reg_f = rot | SZP[reg_val];           \
      CLK += 8;                             \
      INC_PC(2);                            \
  } while (0)

#define SLAHL()                         \
  do {                                  \
      BYTE rot, tmp;                    \
                                        \
      CLK += 4;                         \
      tmp = LOAD(HL_WORD());            \
      rot = (tmp & 0x80) ? C_FLAG : 0;  \
      tmp <<= 1;                        \
      CLK += 4;                         \
      STORE(HL_WORD(), tmp);            \
      reg_f = rot | SZP[tmp];           \
      CLK += 7;                         \
      INC_PC(2);                        \
  } while (0)

#define SLL(reg_val)                        \
  do {                                      \
      BYTE rot;                             \
                                            \
      rot = (reg_val & 0x80) ? C_FLAG : 0;  \
      reg_val = (reg_val << 1) | 1;         \
      reg_f = rot | SZP[reg_val];           \
      CLK += 8;                             \
      INC_PC(2);                            \
  } while (0)

#define SLLHL()                         \
  do {                                  \
      BYTE rot, tmp;                    \
                                        \
      CLK += 4;                         \
      tmp = LOAD(HL_WORD());            \
      rot = (tmp & 0x80) ? C_FLAG : 0;  \
      tmp = (tmp << 1) | 1;             \
      CLK += 4;                         \
      STORE(HL_WORD(), tmp);            \
      reg_f = rot | SZP[tmp];           \
      CLK += 7;                         \
      INC_PC(2);                        \
  } while (0)

#define SRA(reg_val)                                \
  do {                                              \
      BYTE rot;                                     \
                                                    \
      rot = reg_val & C_FLAG;                       \
      reg_val = (reg_val >> 1) | (reg_val & 0x80);  \
      reg_f = rot | SZP[reg_val];                   \
      CLK += 8;                                     \
      INC_PC(2);                                    \
  } while (0)

#define SRAHL()                         \
  do {                                  \
      BYTE rot, tmp;                    \
                                        \
      CLK += 4;                         \
      tmp = LOAD(HL_WORD());            \
      rot = tmp & C_FLAG;               \
      tmp = (tmp >> 1) | (tmp & 0x80);  \
      CLK += 4;                         \
      STORE(HL_WORD(), tmp);            \
      reg_f = rot | SZP[tmp];           \
      CLK += 7;                         \
      INC_PC(2);                        \
  } while (0)

#define SRL(reg_val)               \
  do {                             \
      BYTE rot;                    \
                                   \
      rot = reg_val & C_FLAG;      \
      reg_val >>= 1;               \
      reg_f = rot | SZP[reg_val];  \
      CLK += 8;                    \
      INC_PC(2);                   \
  } while (0)

#define SRLHL()                \
  do {                         \
      BYTE rot, tmp;           \
                               \
      CLK += 4;                \
      tmp = LOAD(HL_WORD());   \
      rot = tmp & C_FLAG;      \
      tmp >>= 1;               \
      CLK += 4;                \
      STORE(HL_WORD(), tmp);   \
      reg_f = rot | SZP[tmp];  \
      CLK += 11;               \
      INC_PC(2);               \
  } while (0)

#define STW(addr, reg_valh, reg_vall, clk_inc1, clk_inc2, clk_inc3, pc_inc)  \
  do {                                                                       \
      CLK += clk_inc1;                                                       \
      STORE((ADDRESS)(addr), reg_vall);                                      \
      CLK += clk_inc2;                                                       \
      STORE((ADDRESS)(addr + 1), reg_valh);                                  \
      CLK += clk_inc3;                                                       \
      INC_PC(pc_inc);                                                        \
  } while (0)

#define STSPW(addr, clk_inc1, clk_inc2, clk_inc3, pc_inc)  \
  do {                                                     \
      CLK += clk_inc1;                                     \
      STORE((ADDRESS)(addr), (reg_sp & 0xff));             \
      CLK += clk_inc2;                                     \
      STORE((ADDRESS)(addr + 1), (reg_sp >> 8));           \
      CLK += clk_inc3;                                     \
      INC_PC(pc_inc);                                      \
  } while (0)

#define STREG(addr, reg_val, clk_inc1, clk_inc2, pc_inc)  \
  do {                                                    \
      CLK += clk_inc1;                                    \
      STORE(addr, reg_val);                               \
      CLK += clk_inc2;                                    \
      INC_PC(pc_inc);                                     \
  } while (0)

#define SUB(loadval, clk_inc1, clk_inc2, pc_inc)                 \
  do {                                                           \
      BYTE tmp, value;                                           \
                                                                 \
      CLK += clk_inc1;                                           \
      value = loadval;                                           \
      tmp = reg_a - value;                                       \
      reg_f = N_FLAG | SZP[tmp];                                 \
      LOCAL_SET_HALFCARRY((reg_a ^ value ^ tmp) & H_FLAG);       \
      LOCAL_SET_PARITY((reg_a ^ value) & (reg_a ^ tmp) & 0x80);  \
      LOCAL_SET_CARRY(value > reg_a);                            \
      reg_a = tmp;                                               \
      CLK += clk_inc2;                                           \
      INC_PC(pc_inc);                                            \
  } while (0)

#define XOR(value, clk_inc1, clk_inc2, pc_inc)  \
  do {                                          \
      CLK += clk_inc1;                          \
      reg_a ^= value;                           \
      reg_f = SZP[reg_a];                       \
      CLK += clk_inc2;                          \
      INC_PC(pc_inc);                           \
  } while (0)


/* ------------------------------------------------------------------------- */

/* Extented opcodes.  */

inline void opcode_cb(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0x00: /* RLC B */
        RLC(reg_b);
        break;
      case 0x01: /* RLC C */
        RLC(reg_c);
        break;
      case 0x02: /* RLC D */
        RLC(reg_d);
        break;
      case 0x03: /* RLC E */
        RLC(reg_e);
        break;
      case 0x04: /* RLC H */
        RLC(reg_h);
        break;
      case 0x05: /* RLC L */
        RLC(reg_l);
        break;
      case 0x06: /* RLC (HL) */
        RLCHL();
        break;
      case 0x07: /* RLC A */
        RLC(reg_a);
        break;
      case 0x08: /* RRC B */
        RRC(reg_b);
        break;
      case 0x09: /* RRC C */
        RRC(reg_c);
        break;
      case 0x0a: /* RRC D */
        RRC(reg_d);
        break;
      case 0x0b: /* RRC E */
        RRC(reg_e);
        break;
      case 0x0c: /* RRC H */
        RRC(reg_h);
        break;
      case 0x0d: /* RRC L */
        RRC(reg_l);
        break;
      case 0x0e: /* RRC (HL) */
        RRCHL();
        break;
      case 0x0f: /* RRC A */
        RRC(reg_a);
        break;
      case 0x10: /* RL B */
        RL(reg_b);
        break;
      case 0x11: /* RL C */
        RL(reg_c);
        break;
      case 0x12: /* RL D */
        RL(reg_d);
        break;
      case 0x13: /* RL E */
        RL(reg_e);
        break;
      case 0x14: /* RL H */
        RL(reg_h);
        break;
      case 0x15: /* RL L */
        RL(reg_l);
        break;
      case 0x16: /* RL (HL) */
        RLHL();
        break;
      case 0x17: /* RL A */
        RL(reg_a);
        break;
      case 0x18: /* RR B */
        RR(reg_b);
        break;
      case 0x19: /* RR C */
        RR(reg_c);
        break;
      case 0x1a: /* RR D */
        RR(reg_d);
        break;
      case 0x1b: /* RR E */
        RR(reg_e);
        break;
      case 0x1c: /* RR H */
        RR(reg_h);
        break;
      case 0x1d: /* RR L */
        RR(reg_l);
        break;
      case 0x1e: /* RR (HL) */
        RRHL();
        break;
      case 0x1f: /* RR A */
        RR(reg_a);
        break;
      case 0x20: /* SLA B */
        SLA(reg_b);
        break;
      case 0x21: /* SLA C */
        SLA(reg_c);
        break;
      case 0x22: /* SLA D */
        SLA(reg_d);
        break;
      case 0x23: /* SLA E */
        SLA(reg_e);
        break;
      case 0x24: /* SLA H */
        SLA(reg_h);
        break;
      case 0x25: /* SLA L */
        SLA(reg_l);
        break;
      case 0x26: /* SLA (HL) */
        SLAHL();
        break;
      case 0x27: /* SLA A */
        SLA(reg_a);
        break;
      case 0x28: /* SRA B */
        SRA(reg_b);
        break;
      case 0x29: /* SRA C */
        SRA(reg_c);
        break;
      case 0x2a: /* SRA D */
        SRA(reg_d);
        break;
      case 0x2b: /* SRA E */
        SRA(reg_e);
        break;
      case 0x2c: /* SRA H */
        SRA(reg_h);
        break;
      case 0x2d: /* SRA L */
        SRA(reg_l);
        break;
      case 0x2e: /* SRA (HL) */
        SRAHL();
        break;
      case 0x2f: /* SRA A */
        SRA(reg_a);
        break;
      case 0x30: /* SLL B */
        SLL(reg_b);
        break;
      case 0x31: /* SLL C */
        SLL(reg_c);
        break;
      case 0x32: /* SLL D */
        SLL(reg_d);
        break;
      case 0x33: /* SLL E */
        SLL(reg_e);
        break;
      case 0x34: /* SLL H */
        SLL(reg_h);
        break;
      case 0x35: /* SLL L */
        SLL(reg_l);
        break;
      case 0x36: /* SLL (HL) */
        SLLHL();
        break;
      case 0x37: /* SLL A */
        SLL(reg_a);
        break;
      case 0x38: /* SRL B */
        SRL(reg_b);
        break;
      case 0x39: /* SRL C */
        SRL(reg_c);
        break;
      case 0x3a: /* SRL D */
        SRL(reg_d);
        break;
      case 0x3b: /* SRL E */
        SRL(reg_e);
        break;
      case 0x3c: /* SRL H */
        SRL(reg_h);
        break;
      case 0x3d: /* SRL L */
        SRL(reg_l);
        break;
      case 0x3e: /* SRL (HL) */
        SRLHL();
        break;
      case 0x3f: /* SRL A */
        SRL(reg_a);
        break;
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
        BIT(reg_a, 0, 8);
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
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x79: /* BIT C 7 */
        BIT(reg_c, 7, 8);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x7a: /* BIT D 7 */
        BIT(reg_d, 7, 8);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x7b: /* BIT E 7 */
        BIT(reg_e, 7, 8);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x7c: /* BIT H 7 */
        BIT(reg_h, 7, 8);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x7d: /* BIT L 7 */
        BIT(reg_l, 7, 8);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x7e: /* BIT (HL) 7 */
        BIT(LOAD(HL_WORD()), 7, 12);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
        break;
      case 0x7f: /* BIT A 7 */
        BIT(reg_a, 7, 8);
        /***LOCAL_SET_SIGN(LOCAL_ZERO() ? LOCAL_SIGN() : 1);***/
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
        RES(reg_a, 0);
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
        SET(reg_a, 0);
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
                    CLK, z80_reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
        INC_PC(2);
   }
}

inline void opcode_dd(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0x01: /* LD BC # */
        LDW(ip23, reg_b, reg_c, 10, 0, 4);
        break;
      case 0x09: /* ADD IX BC */
        ADDXXREG(reg_ixh, reg_ixl, reg_b, reg_c, 15, 2);
        break;
      case 0x11: /* LD DE # */
        LDW(ip23, reg_d, reg_e, 10, 0, 4);
        break;
      case 0x19: /* ADD IX DE */
        ADDXXREG(reg_ixh, reg_ixl, reg_d, reg_e, 15, 2);
        break;
      case 0x21: /* LD IX # */
        LDW(ip23, reg_ixh, reg_ixl, 10, 4, 4);
        break;
      case 0x22: /* LD (WORD) IX */
        STW(ip23, reg_ixh, reg_ixl, 4, 9, 7, 4);
        break;
      case 0x29: /* ADD IX IX */
        ADDXXREG(reg_ixh, reg_ixl, reg_ixh, reg_ixl, 15, 2);
        break;
      case 0x2a: /* LD IX (WORD) */
        LDIND(ip23, reg_ixh, reg_ixl, 4, 4, 12, 4);
        break;
      case 0x31: /* LD SP # */
        LDSP(ip23, 10, 0, 4);
        break;
      case 0x39: /* ADD IX SP */
        ADDXXSP(reg_ixh, reg_ixl, 15, 2);
        break;
      case 0x40: /* LD B B */
        LDREG(reg_b, reg_b, 0, 4, 2);
        break;
      case 0x41: /* LD B C */
        LDREG(reg_b, reg_c, 0, 4, 2);
        break;
      case 0x42: /* LD B D */
        LDREG(reg_b, reg_d, 0, 4, 2);
        break;
      case 0x43: /* LD B E */
        LDREG(reg_b, reg_e, 0, 4, 2);
        break;
      case 0x44: /* LD B H */
        LDREG(reg_b, reg_h, 0, 4, 2);
        break;
      case 0x45: /* LD B L */
        LDREG(reg_b, reg_l, 0, 4, 2);
        break;
      case 0x46: /* LD B (IX+d) */
        LDINDD(reg_b, reg_ixh, reg_ixl, ip2);
        break;
      case 0x47: /* LD B A */
        LDREG(reg_b, reg_a, 0, 4, 2);
        break;
      case 0x48: /* LD C B */
        LDREG(reg_c, reg_b, 0, 4, 2);
        break;
      case 0x49: /* LD C C */
        LDREG(reg_c, reg_c, 0, 4, 2);
        break;
      case 0x4a: /* LD C D */
        LDREG(reg_c, reg_d, 0, 4, 2);
        break;
      case 0x4b: /* LD C E */
        LDREG(reg_c, reg_e, 0, 4, 2);
        break;
      case 0x4c: /* LD C H */
        LDREG(reg_c, reg_h, 0, 4, 2);
        break;
      case 0x4d: /* LD C L */
        LDREG(reg_c, reg_l, 0, 4, 2);
        break;
      case 0x4e: /* LD C (IX+d) */
        LDINDD(reg_c, reg_ixh, reg_ixl, ip2);
        break;
      case 0x4f: /* LD C A */
        LDREG(reg_c, reg_a, 0, 4, 2);
        break;
      case 0x50: /* LD D B */
        LDREG(reg_d, reg_b, 0, 4, 2);
        break;
      case 0x51: /* LD D C */
        LDREG(reg_d, reg_c, 0, 4, 2);
        break;
      case 0x52: /* LD D D */
        LDREG(reg_d, reg_d, 0, 4, 2);
        break;
      case 0x53: /* LD D E */
        LDREG(reg_d, reg_e, 0, 4, 2);
        break;
      case 0x54: /* LD D H */
        LDREG(reg_d, reg_h, 0, 4, 2);
        break;
      case 0x55: /* LD D L */
        LDREG(reg_d, reg_l, 0, 4, 2);
        break;
      case 0x56: /* LD D (IX+d) */
        LDINDD(reg_d, reg_ixh, reg_ixl, ip2);
        break;
      case 0x57: /* LD D A */
        LDREG(reg_d, reg_a, 0, 4, 2);
        break;
      case 0x58: /* LD E B */
        LDREG(reg_e, reg_b, 0, 4, 2);
        break;
      case 0x59: /* LD E C */
        LDREG(reg_e, reg_c, 0, 4, 2);
        break;
      case 0x5a: /* LD E D */
        LDREG(reg_e, reg_d, 0, 4, 2);
        break;
      case 0x5b: /* LD E E */
        LDREG(reg_e, reg_e, 0, 4, 2);
        break;
      case 0x5c: /* LD E H */
        LDREG(reg_e, reg_h, 0, 4, 2);
        break;
      case 0x5d: /* LD E L */
        LDREG(reg_e, reg_l, 0, 4, 2);
        break;
      case 0x5e: /* LD E (IX+d) */
        LDINDD(reg_e, reg_ixh, reg_ixl, ip2);
        break;
      case 0x5f: /* LD E A */
        LDREG(reg_e, reg_a, 0, 4, 2);
        break;
      case 0x60: /* LD H B */
        LDREG(reg_h, reg_b, 0, 4, 2);
        break;
      case 0x61: /* LD H C */
        LDREG(reg_h, reg_c, 0, 4, 2);
        break;
      case 0x62: /* LD H D */
        LDREG(reg_h, reg_d, 0, 4, 2);
        break;
      case 0x63: /* LD H E */
        LDREG(reg_h, reg_e, 0, 4, 2);
        break;
      case 0x64: /* LD H H */
        LDREG(reg_h, reg_h, 0, 4, 2);
        break;
      case 0x65: /* LD H L */
        LDREG(reg_h, reg_l, 0, 4, 2);
        break;
      case 0x66: /* LD H (IX+d) */
        LDINDD(reg_h, reg_ixh, reg_ixl, ip2);
        break;
      case 0x67: /* LD H A */
        LDREG(reg_h, reg_a, 0, 4, 2);
        break;
      case 0x68: /* LD L B */
        LDREG(reg_l, reg_b, 0, 4, 2);
        break;
      case 0x69: /* LD L C */
        LDREG(reg_l, reg_c, 0, 4, 2);
        break;
      case 0x6a: /* LD L D */
        LDREG(reg_l, reg_d, 0, 4, 2);
        break;
      case 0x6b: /* LD L E */
        LDREG(reg_l, reg_e, 0, 4, 2);
        break;
      case 0x6c: /* LD L H */
        LDREG(reg_l, reg_h, 0, 4, 2);
        break;
      case 0x6d: /* LD L L */
        LDREG(reg_l, reg_l, 0, 4, 2);
        break;
      case 0x6e: /* LD L (IX+d) */
        LDINDD(reg_l, reg_ixh, reg_ixl, ip2);
        break;
      case 0x6f: /* LD L A */
        LDREG(reg_l, reg_a, 0, 4, 2);
        break;
      case 0x70: /* LD (IX+d) B */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_b, 8, 11, 3);
        break;
      case 0x71: /* LD (IX+d) C */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_c, 8, 11, 3);
        break;
      case 0x72: /* LD (IX+d) D */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_d, 8, 11, 3);
        break;
      case 0x73: /* LD (IX+d) E */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_e, 8, 11, 3);
        break;
      case 0x74: /* LD (IX+d) H */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_h, 8, 11, 3);
        break;
      case 0x75: /* LD (IX+d) L */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_l, 8, 11, 3);
        break;
      case 0x76: /* HALT */
        HALT();
        break;
      case 0x77: /* LD (IX+d) A */
        STREG(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2), reg_a, 8, 11, 3);
        break;
      case 0x78: /* LD A B */
        LDREG(reg_a, reg_b, 0, 4, 2);
        break;
      case 0x79: /* LD A C */
        LDREG(reg_a, reg_c, 0, 4, 2);
        break;
      case 0x7a: /* LD A D */
        LDREG(reg_a, reg_d, 0, 4, 2);
        break;
      case 0x7b: /* LD A E */
        LDREG(reg_a, reg_e, 0, 4, 2);
        break;
      case 0x7c: /* LD A H */
        LDREG(reg_a, reg_h, 0, 4, 2);
        break;
      case 0x7d: /* LD A L */
        LDREG(reg_a, reg_l, 0, 4, 2);
        break;
      case 0x7e: /* LD A (IX+d) */
        LDINDD(reg_a, reg_ixh, reg_ixl, ip2);
        break;
      case 0x7f: /* LD A A */
        LDREG(reg_a, reg_a, 0, 4, 2);
        break;
      case 0x80: /* ADD B */
        ADD(reg_b, 0, 4, 2);
        break;
      case 0x81: /* ADD C */
        ADD(reg_c, 0, 4, 2);
        break;
      case 0x82: /* ADD D */
        ADD(reg_d, 0, 4, 2);
        break;
      case 0x83: /* ADD E */
        ADD(reg_e, 0, 4, 2);
        break;
      case 0x84: /* ADD H */
        ADD(reg_h, 0, 4, 2);
        break;
      case 0x85: /* ADD L */
        ADD(reg_l, 0, 4, 2);
        break;
      case 0x86: /* ADD (IX+d) */
        ADD(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x87: /* ADD A */
        ADD(reg_a, 0, 4, 2);
        break;
      case 0x88: /* ADC B */
        ADC(reg_b, 0, 4, 2);
        break;
      case 0x89: /* ADC C */
        ADC(reg_c, 0, 4, 2);
        break;
      case 0x8a: /* ADC D */
        ADC(reg_d, 0, 4, 2);
        break;
      case 0x8b: /* ADC E */
        ADC(reg_e, 0, 4, 2);
        break;
      case 0x8c: /* ADC H */
        ADC(reg_h, 0, 4, 2);
        break;
      case 0x8d: /* ADC L */
        ADC(reg_l, 0, 4, 2);
        break;
      case 0x8e: /* ADC (IX+d) */
        ADC(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x8f: /* ADC A */
        ADC(reg_a, 0, 4, 2);
        break;
      case 0x90: /* SUB B */
        SUB(reg_b, 0, 4, 2);
        break;
      case 0x91: /* SUB C */
        SUB(reg_c, 0, 4, 2);
        break;
      case 0x92: /* SUB D */
        SUB(reg_d, 0, 4, 2);
        break;
      case 0x93: /* SUB E */
        SUB(reg_e, 0, 4, 2);
        break;
      case 0x94: /* SUB H */
        SUB(reg_h, 0, 4, 2);
        break;
      case 0x95: /* SUB L */
        SUB(reg_l, 0, 4, 2);
        break;
      case 0x96: /* SUB (IX+d) */
        SUB(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x97: /* SUB A */
        SUB(reg_a, 0, 4, 2);
        break;
      case 0x98: /* SBC B */
        SBC(reg_b, 0, 4, 2);
        break;
      case 0x99: /* SBC C */
        SBC(reg_c, 0, 4, 2);
        break;
      case 0x9a: /* SBC D */
        SBC(reg_d, 0, 4, 2);
        break;
      case 0x9b: /* SBC E */
        SBC(reg_e, 0, 4, 2);
        break;
      case 0x9c: /* SBC H */
        SBC(reg_h, 0, 4, 2);
        break;
      case 0x9d: /* SBC L */
        SBC(reg_l, 0, 4, 2);
        break;
      case 0x9e: /* SBC (IX+d) */
        SBC(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x9f: /* SBC A */
        SBC(reg_a, 0, 4, 2);
        break;
      case 0xa0: /* AND B */
        AND(reg_b, 0, 4, 2);
        break;
      case 0xa1: /* AND C */
        AND(reg_c, 0, 4, 2);
        break;
      case 0xa2: /* AND D */
        AND(reg_d, 0, 4, 2);
        break;
      case 0xa3: /* AND E */
        AND(reg_e, 0, 4, 2);
        break;
      case 0xa4: /* AND H */
        AND(reg_h, 0, 4, 2);
        break;
      case 0xa5: /* AND L */
        AND(reg_l, 0, 4, 2);
        break;
      case 0xa6: /* AND (IX+d) */
        AND(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xa7: /* AND A */
        AND(reg_a, 0, 4, 2);
        break;
      case 0xa8: /* XOR B */
        XOR(reg_b, 0, 4, 2);
        break;
      case 0xa9: /* XOR C */
        XOR(reg_c, 0, 4, 2);
        break;
      case 0xaa: /* XOR D */
        XOR(reg_d, 0, 4, 2);
        break;
      case 0xab: /* XOR E */
        XOR(reg_e, 0, 4, 2);
        break;
      case 0xac: /* XOR H */
        XOR(reg_h, 0, 4, 2);
        break;
      case 0xad: /* XOR L */
        XOR(reg_l, 0, 4, 2);
        break;
      case 0xae: /* XOR (IX+d) */
        XOR(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xaf: /* XOR A */
        XOR(reg_a, 0, 4, 2);
        break;
      case 0xb0: /* OR B */
        OR(reg_b, 0, 4, 2);
        break;
      case 0xb1: /* OR C */
        OR(reg_c, 0, 4, 2);
        break;
      case 0xb2: /* OR D */
        OR(reg_d, 0, 4, 2);
        break;
      case 0xb3: /* OR E */
        OR(reg_e, 0, 4, 2);
        break;
      case 0xb4: /* OR H */
        OR(reg_h, 0, 4, 2);
        break;
      case 0xb5: /* OR L */
        OR(reg_l, 0, 4, 2);
        break;
      case 0xb6: /* OR (IX+d) */
        OR(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xb7: /* OR A */
        OR(reg_a, 0, 4, 2);
        break;
      case 0xb8: /* CP B */
        CP(reg_b, 0, 4, 2);
        break;
      case 0xb9: /* CP C */
        CP(reg_c, 0, 4, 2);
        break;
      case 0xba: /* CP D */
        CP(reg_d, 0, 4, 2);
        break;
      case 0xbb: /* CP E */
        CP(reg_e, 0, 4, 2);
        break;
      case 0xbc: /* CP H */
        CP(reg_h, 0, 4, 2);
        break;
      case 0xbd: /* CP L */
        CP(reg_l, 0, 4, 2);
        break;
      case 0xbe: /* CP (IX+d) */
        CP(LOAD(((reg_ixh << 8) | reg_ixl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xbf: /* CP A */
        CP(reg_a, 0, 4, 2);
        break;
      case 0xc1: /* POP BC */
        POP(reg_b, reg_c, 2);
        break;
      case 0xc5: /* PUSH BC */
        PUSH(reg_b, reg_c, 2);
        break;
      case 0xd1: /* POP DE */
        POP(reg_d, reg_e, 2);
        break;
      case 0xd5: /* PUSH DE */
        PUSH(reg_d, reg_e, 2);
        break;
      case 0xe1: /* POP IX */
        POP(reg_ixh, reg_ixl, 2);
        break;
      case 0xe5: /* PUSH IX */
        PUSH(reg_ixh, reg_ixl, 2);
        break;
      case 0xf1: /* POP AF */
        POP(reg_a, reg_f, 2);
        break;
      case 0xf5: /* PUSH AF */
        PUSH(reg_a, reg_f, 2);
        break;
      default:
        log_message(LOG_DEFAULT,
                    "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                    "H%02x L%02x SP%04x OP %02x %02x %02x.",
                    CLK, z80_reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
        INC_PC(2);
   }
}

inline void opcode_ed(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0x40: /* IN B BC */
        INBC(reg_b, 4, 8, 2);
        break;
      case 0x41: /* OUT BC B */
        OUTBC(reg_b, 4, 8, 2);
        break;
      case 0x42: /* SBC HL BC */
        SBCHLREG(reg_b, reg_c);
        break;
      case 0x43: /* LD (WORD) BC */
        STW(ip23, reg_b, reg_c, 4, 13, 3, 4);
        break;
      case 0x44: /* NEG */
        NEG();
        break;
      case 0x46: /* IM0 */
        IM(0);
        break;
      case 0x47: /* LD I A */
        LDREG(reg_i, reg_a, 6, 3, 2);
        break;
      case 0x48: /* IN C BC */
        INBC(reg_c, 4, 8, 2);
        break;
      case 0x49: /* OUT BC C */
        OUTBC(reg_c, 4, 8, 2);
        break;
      case 0x4a: /* ADC HL BC */
        ADCHLREG(reg_b, reg_c);
        break;
      case 0x4b: /* LD BC (WORD) */
        LDIND(ip23, reg_b, reg_c, 4, 4, 12, 4);
        break;
      case 0x50: /* IN D BC */
        INBC(reg_d, 4, 8, 2);
        break;
      case 0x51: /* OUT BC D */
        OUTBC(reg_d, 4, 8, 2);
        break;
      case 0x52: /* SBC HL DE */
        SBCHLREG(reg_d, reg_e);
        break;
      case 0x53: /* LD (WORD) DE */
        STW(ip23, reg_d, reg_e, 4, 13, 3, 4);
        break;
      case 0x56: /* IM1 */
        IM(2);
        break;
#if 0
      case 0x57: /* LD A I */
        LDAI();
        break;
#endif
      case 0x58: /* IN E BC */
        INBC(reg_e, 4, 8, 2);
        break;
      case 0x59: /* OUT BC E */
        OUTBC(reg_e, 4, 8, 2);
        break;
      case 0x5a: /* ADC HL DE */
        ADCHLREG(reg_d, reg_e);
        break;
      case 0x5b: /* LD DE (WORD) */
        LDIND(ip23, reg_d, reg_e, 4, 4, 12, 4);
        break;
      case 0x5e: /* IM2 */
        IM(4);
        break;
      case 0x60: /* IN H BC */
        INBC(reg_h, 4, 8, 2);
        break;
      case 0x61: /* OUT BC H */
        OUTBC(reg_h, 4, 8, 2);
        break;
      case 0x62: /* SBC HL HL */
        SBCHLREG(reg_h, reg_l);
        break;
      case 0x63: /* LD (WORD) HL */
        STW(ip23, reg_h, reg_l, 4, 13, 3, 4);
        break;
      case 0x68: /* IN L BC */
        INBC(reg_l, 4, 8, 2);
        break;
      case 0x69: /* OUT BC L */
        OUTBC(reg_l, 4, 8, 2);
        break;
      case 0x6a: /* ADC HL HL */
        ADCHLREG(reg_h, reg_l);
        break;
      case 0x6b: /* LD HL (WORD) */
        LDIND(ip23, reg_h, reg_l, 4, 4, 12, 4);
        break;
#if 0
      case 0x70: /* IN F BC */
        INBC(reg_f, 4, 8, 2);
        break;
#endif
      case 0x72: /* SBC HL SP */
        SBCHLSP();
        break;
      case 0x73: /* LD (WORD) SP */
        STSPW(ip23, 4, 13, 3, 4);
        break;
      case 0x78: /* IN A BC */
        INBC(reg_a, 4, 8, 2);
        break;
      case 0x79: /* OUT BC A */
        OUTBC(reg_a, 4, 8, 2);
        break;
      case 0x7a: /* ADC HL SP */
        ADCHLSP();
        break;
      case 0x7b: /* LD SP (WORD) */
        LDSPIND(ip23, 4, 4, 12, 4);
        break;
      case 0xa0: /* LDI */
        LDDI(INC_DE_WORD(), INC_HL_WORD());
        break;
      case 0xa1: /* CPI */
        CPDI(INC_HL_WORD());
        break;
      case 0xa8: /* LDD */
        LDDI(DEC_DE_WORD(), DEC_HL_WORD());
        break;
      case 0xa9: /* CPD */
        CPDI(DEC_HL_WORD());
        break;
      case 0xb0: /* LDIR */
        LDDIR(INC_DE_WORD(), INC_HL_WORD());
        break;
      case 0xb1: /* CPIR */
        CPDIR(INC_HL_WORD());
        break;
      case 0xb8: /* LDDR */
        LDDIR(DEC_DE_WORD(), DEC_HL_WORD());
        break;
      case 0xb9: /* CPDR */
        CPDIR(DEC_HL_WORD());
        break;
      default:
        log_message(LOG_DEFAULT,
                    "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                    "H%02x L%02x SP%04x OP %02x %02x %02x.",
                    CLK, z80_reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
        INC_PC(2);
   }
}

inline void opcode_fd(BYTE ip0, BYTE ip1, BYTE ip2, BYTE ip3, WORD ip12,
                      WORD ip23)
{
    switch (ip1) {
      case 0x01: /* LD BC # */
        LDW(ip23, reg_b, reg_c, 10, 0, 4);
        break;
      case 0x09: /* ADD IY BC */
        ADDXXREG(reg_iyh, reg_iyl, reg_b, reg_c, 15, 2);
        break;
      case 0x11: /* LD DE # */
        LDW(ip23, reg_d, reg_e, 10, 0, 4);
        break;
      case 0x19: /* ADD IY DE */
        ADDXXREG(reg_iyh, reg_iyl, reg_d, reg_e, 15, 2);
        break;
      case 0x21: /* LD IY # */
        LDW(ip23, reg_iyh, reg_iyl, 10, 4, 4);
        break;
      case 0x22: /* LD (WORD) IY */
        STW(ip23, reg_iyh, reg_iyl, 4, 9, 7, 4);
        break;
      case 0x29: /* ADD IY IY */
        ADDXXREG(reg_iyh, reg_iyl, reg_iyh, reg_iyl, 15, 2);
        break;
      case 0x2a: /* LD IY (WORD) */
        LDIND(ip23, reg_iyh, reg_iyl, 4, 4, 12, 4);
        break;
      case 0x31: /* LD SP # */
        LDSP(ip23, 10, 0, 4);
        break;
      case 0x39: /* ADD IY SP */
        ADDXXSP(reg_iyh, reg_iyl, 15, 2);
        break;
      case 0x40: /* LD B B */
        LDREG(reg_b, reg_b, 0, 4, 2);
        break;
      case 0x41: /* LD B C */
        LDREG(reg_b, reg_c, 0, 4, 2);
        break;
      case 0x42: /* LD B D */
        LDREG(reg_b, reg_d, 0, 4, 2);
        break;
      case 0x43: /* LD B E */
        LDREG(reg_b, reg_e, 0, 4, 2);
        break;
      case 0x44: /* LD B H */
        LDREG(reg_b, reg_h, 0, 4, 2);
        break;
      case 0x45: /* LD B L */
        LDREG(reg_b, reg_l, 0, 4, 2);
        break;
      case 0x46: /* LD B (IY+d) */
        LDINDD(reg_b, reg_iyh, reg_iyl, ip2);
        break;
      case 0x47: /* LD B A */
        LDREG(reg_b, reg_a, 0, 4, 2);
        break;
      case 0x48: /* LD C B */
        LDREG(reg_c, reg_b, 0, 4, 2);
        break;
      case 0x49: /* LD C C */
        LDREG(reg_c, reg_c, 0, 4, 2);
        break;
      case 0x4a: /* LD C D */
        LDREG(reg_c, reg_d, 0, 4, 2);
        break;
      case 0x4b: /* LD C E */
        LDREG(reg_c, reg_e, 0, 4, 2);
        break;
      case 0x4c: /* LD C H */
        LDREG(reg_c, reg_h, 0, 4, 2);
        break;
      case 0x4d: /* LD C L */
        LDREG(reg_c, reg_l, 0, 4, 2);
        break;
      case 0x4e: /* LD C (IY+d) */
        LDINDD(reg_c, reg_iyh, reg_iyl, ip2);
        break;
      case 0x4f: /* LD C A */
        LDREG(reg_c, reg_a, 0, 4, 2);
        break;
      case 0x50: /* LD D B */
        LDREG(reg_d, reg_b, 0, 4, 2);
        break;
      case 0x51: /* LD D C */
        LDREG(reg_d, reg_c, 0, 4, 2);
        break;
      case 0x52: /* LD D D */
        LDREG(reg_d, reg_d, 0, 4, 2);
        break;
      case 0x53: /* LD D E */
        LDREG(reg_d, reg_e, 0, 4, 2);
        break;
      case 0x54: /* LD D H */
        LDREG(reg_d, reg_h, 0, 4, 2);
        break;
      case 0x55: /* LD D L */
        LDREG(reg_d, reg_l, 0, 4, 2);
        break;
      case 0x56: /* LD D (IY+d) */
        LDINDD(reg_d, reg_iyh, reg_iyl, ip2);
        break;
      case 0x57: /* LD D A */
        LDREG(reg_d, reg_a, 0, 4, 2);
        break;
      case 0x58: /* LD E B */
        LDREG(reg_e, reg_b, 0, 4, 2);
        break;
      case 0x59: /* LD E C */
        LDREG(reg_e, reg_c, 0, 4, 2);
        break;
      case 0x5a: /* LD E D */
        LDREG(reg_e, reg_d, 0, 4, 2);
        break;
      case 0x5b: /* LD E E */
        LDREG(reg_e, reg_e, 0, 4, 2);
        break;
      case 0x5c: /* LD E H */
        LDREG(reg_e, reg_h, 0, 4, 2);
        break;
      case 0x5d: /* LD E L */
        LDREG(reg_e, reg_l, 0, 4, 2);
        break;
      case 0x5e: /* LD E (IY+d) */
        LDINDD(reg_e, reg_iyh, reg_iyl, ip2);
        break;
      case 0x5f: /* LD E A */
        LDREG(reg_e, reg_a, 0, 4, 2);
        break;
      case 0x60: /* LD H B */
        LDREG(reg_h, reg_b, 0, 4, 2);
        break;
      case 0x61: /* LD H C */
        LDREG(reg_h, reg_c, 0, 4, 2);
        break;
      case 0x62: /* LD H D */
        LDREG(reg_h, reg_d, 0, 4, 2);
        break;
      case 0x63: /* LD H E */
        LDREG(reg_h, reg_e, 0, 4, 2);
        break;
      case 0x64: /* LD H H */
        LDREG(reg_h, reg_h, 0, 4, 2);
        break;
      case 0x65: /* LD H L */
        LDREG(reg_h, reg_l, 0, 4, 2);
        break;
      case 0x66: /* LD H (IY+d) */
        LDINDD(reg_h, reg_iyh, reg_iyl, ip2);
        break;
      case 0x67: /* LD H A */
        LDREG(reg_h, reg_a, 0, 4, 2);
        break;
      case 0x68: /* LD L B */
        LDREG(reg_l, reg_b, 0, 4, 2);
        break;
      case 0x69: /* LD L C */
        LDREG(reg_l, reg_c, 0, 4, 2);
        break;
      case 0x6a: /* LD L D */
        LDREG(reg_l, reg_d, 0, 4, 2);
        break;
      case 0x6b: /* LD L E */
        LDREG(reg_l, reg_e, 0, 4, 2);
        break;
      case 0x6c: /* LD L H */
        LDREG(reg_l, reg_h, 0, 4, 2);
        break;
      case 0x6d: /* LD L L */
        LDREG(reg_l, reg_l, 0, 4, 2);
        break;
      case 0x6e: /* LD L (IY+d) */
        LDINDD(reg_l, reg_iyh, reg_iyl, ip2);
        break;
      case 0x6f: /* LD L A */
        LDREG(reg_l, reg_a, 0, 4, 2);
        break;
      case 0x70: /* LD (IY+d) B */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_b, 8, 11, 3);
        break;
      case 0x71: /* LD (IY+d) C */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_c, 8, 11, 3);
        break;
      case 0x72: /* LD (IY+d) D */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_d, 8, 11, 3);
        break;
      case 0x73: /* LD (IY+d) E */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_e, 8, 11, 3);
        break;
      case 0x74: /* LD (IY+d) H */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_h, 8, 11, 3);
        break;
      case 0x75: /* LD (IY+d) L */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_l, 8, 11, 3);
        break;
      case 0x76: /* HALT */
        HALT();
        break;
      case 0x77: /* LD (IY+d) A */
        STREG(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2), reg_a, 8, 11, 3);
        break;
      case 0x78: /* LD A B */
        LDREG(reg_a, reg_b, 0, 4, 2);
        break;
      case 0x79: /* LD A C */
        LDREG(reg_a, reg_c, 0, 4, 2);
        break;
      case 0x7a: /* LD A D */
        LDREG(reg_a, reg_d, 0, 4, 2);
        break;
      case 0x7b: /* LD A E */
        LDREG(reg_a, reg_e, 0, 4, 2);
        break;
      case 0x7c: /* LD A H */
        LDREG(reg_a, reg_h, 0, 4, 2);
        break;
      case 0x7d: /* LD A L */
        LDREG(reg_a, reg_l, 0, 4, 2);
        break;
      case 0x7e: /* LD A (IY+d) */
        LDINDD(reg_a, reg_iyh, reg_iyl, ip2);
        break;
      case 0x7f: /* LD A A */
        LDREG(reg_a, reg_a, 0, 4, 2);
        break;
      case 0x80: /* ADD B */
        ADD(reg_b, 0, 4, 2);
        break;
      case 0x81: /* ADD C */
        ADD(reg_c, 0, 4, 2);
        break;
      case 0x82: /* ADD D */
        ADD(reg_d, 0, 4, 2);
        break;
      case 0x83: /* ADD E */
        ADD(reg_e, 0, 4, 2);
        break;
      case 0x84: /* ADD H */
        ADD(reg_h, 0, 4, 2);
        break;
      case 0x85: /* ADD L */
        ADD(reg_l, 0, 4, 2);
        break;
      case 0x86: /* ADD (IY+d) */
        ADD(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x87: /* ADD A */
        ADD(reg_a, 0, 4, 2);
        break;
      case 0x88: /* ADC B */
        ADC(reg_b, 0, 4, 2);
        break;
      case 0x89: /* ADC C */
        ADC(reg_c, 0, 4, 2);
        break;
      case 0x8a: /* ADC D */
        ADC(reg_d, 0, 4, 2);
        break;
      case 0x8b: /* ADC E */
        ADC(reg_e, 0, 4, 2);
        break;
      case 0x8c: /* ADC H */
        ADC(reg_h, 0, 4, 2);
        break;
      case 0x8d: /* ADC L */
        ADC(reg_l, 0, 4, 2);
        break;
      case 0x8e: /* ADC (IY+d) */
        ADC(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x8f: /* ADC A */
        ADC(reg_a, 0, 4, 2);
        break;
      case 0x90: /* SUB B */
        SUB(reg_b, 0, 4, 2);
        break;
      case 0x91: /* SUB C */
        SUB(reg_c, 0, 4, 2);
        break;
      case 0x92: /* SUB D */
        SUB(reg_d, 0, 4, 2);
        break;
      case 0x93: /* SUB E */
        SUB(reg_e, 0, 4, 2);
        break;
      case 0x94: /* SUB H */
        SUB(reg_h, 0, 4, 2);
        break;
      case 0x95: /* SUB L */
        SUB(reg_l, 0, 4, 2);
        break;
      case 0x96: /* SUB (IY+d) */
        SUB(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x97: /* SUB A */
        SUB(reg_a, 0, 4, 2);
        break;
      case 0x98: /* SBC B */
        SBC(reg_b, 0, 4, 2);
        break;
      case 0x99: /* SBC C */
        SBC(reg_c, 0, 4, 2);
        break;
      case 0x9a: /* SBC D */
        SBC(reg_d, 0, 4, 2);
        break;
      case 0x9b: /* SBC E */
        SBC(reg_e, 0, 4, 2);
        break;
      case 0x9c: /* SBC H */
        SBC(reg_h, 0, 4, 2);
        break;
      case 0x9d: /* SBC L */
        SBC(reg_l, 0, 4, 2);
        break;
      case 0x9e: /* SBC (IY+d) */
        SBC(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0x9f: /* SBC A */
        SBC(reg_a, 0, 4, 2);
        break;
      case 0xa0: /* AND B */
        AND(reg_b, 0, 4, 2);
        break;
      case 0xa1: /* AND C */
        AND(reg_c, 0, 4, 2);
        break;
      case 0xa2: /* AND D */
        AND(reg_d, 0, 4, 2);
        break;
      case 0xa3: /* AND E */
        AND(reg_e, 0, 4, 2);
        break;
      case 0xa4: /* AND H */
        AND(reg_h, 0, 4, 2);
        break;
      case 0xa5: /* AND L */
        AND(reg_l, 0, 4, 2);
        break;
      case 0xa6: /* AND (IY+d) */
        AND(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xa7: /* AND A */
        AND(reg_a, 0, 4, 2);
        break;
      case 0xa8: /* XOR B */
        XOR(reg_b, 0, 4, 2);
        break;
      case 0xa9: /* XOR C */
        XOR(reg_c, 0, 4, 2);
        break;
      case 0xaa: /* XOR D */
        XOR(reg_d, 0, 4, 2);
        break;
      case 0xab: /* XOR E */
        XOR(reg_e, 0, 4, 2);
        break;
      case 0xac: /* XOR H */
        XOR(reg_h, 0, 4, 2);
        break;
      case 0xad: /* XOR L */
        XOR(reg_l, 0, 4, 2);
        break;
      case 0xae: /* XOR (IY+d) */
        XOR(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xaf: /* XOR A */
        XOR(reg_a, 0, 4, 2);
        break;
      case 0xb0: /* OR B */
        OR(reg_b, 0, 4, 2);
        break;
      case 0xb1: /* OR C */
        OR(reg_c, 0, 4, 2);
        break;
      case 0xb2: /* OR D */
        OR(reg_d, 0, 4, 2);
        break;
      case 0xb3: /* OR E */
        OR(reg_e, 0, 4, 2);
        break;
      case 0xb4: /* OR H */
        OR(reg_h, 0, 4, 2);
        break;
      case 0xb5: /* OR L */
        OR(reg_l, 0, 4, 2);
        break;
      case 0xb6: /* OR (IY+d) */
        OR(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xb7: /* OR A */
        OR(reg_a, 0, 4, 2);
        break;
      case 0xb8: /* CP B */
        CP(reg_b, 0, 4, 2);
        break;
      case 0xb9: /* CP C */
        CP(reg_c, 0, 4, 2);
        break;
      case 0xba: /* CP D */
        CP(reg_d, 0, 4, 2);
        break;
      case 0xbb: /* CP E */
        CP(reg_e, 0, 4, 2);
        break;
      case 0xbc: /* CP H */
        CP(reg_h, 0, 4, 2);
        break;
      case 0xbd: /* CP L */
        CP(reg_l, 0, 4, 2);
        break;
      case 0xbe: /* CP (IY+d) */
        CP(LOAD(((reg_iyh << 8) | reg_iyl) + (signed char)(ip2)), 8, 11, 3);
        break;
      case 0xbf: /* CP A */
        CP(reg_a, 0, 4, 2);
        break;
      case 0xc1: /* POP BC */
        POP(reg_b, reg_c, 2);
        break;
      case 0xc5: /* PUSH BC */
        PUSH(reg_b, reg_c, 2);
        break;
      case 0xd1: /* POP DE */
        POP(reg_d, reg_e, 2);
        break;
      case 0xd5: /* PUSH DE */
        PUSH(reg_d, reg_e, 2);
        break;
      case 0xe1: /* POP IY */
        POP(reg_iyh, reg_iyl, 2);
        break;
      case 0xe5: /* PUSH IY */
        PUSH(reg_iyh, reg_iyl, 2);
        break;
      case 0xf1: /* POP AF */
        POP(reg_a, reg_f, 2);
        break;
      case 0xf5: /* PUSH AF */
        PUSH(reg_a, reg_f, 2);
        break;
      default:
        log_message(LOG_DEFAULT,
                    "%i PC %04x A%02x F%02x B%02x C%02x D%02x E%02x "
                    "H%02x L%02x SP%04x OP %02x %02x %02x.",
                    CLK, z80_reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, ip0, ip1, ip2);
        INC_PC(2);
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

            pending_interrupt = interrupt_check_pending_interrupt(cpu_int_status);
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
                    CLK, z80_reg_pc, reg_a, reg_f, reg_b, reg_c, reg_d, reg_e,
                    reg_h, reg_l, reg_sp, p0, p1, p2, p3);
#endif

        switch (p0) {

          case 0x00: /* NOP */
            NOP();
            break;
          case 0x01: /* LD BC # */
            LDW(p12, reg_b, reg_c, 10, 0, 3);
            break;
          case 0x02: /* LD (BC) A */
            STREG(BC_WORD(), reg_a, 4, 3, 1);
            break;
          case 0x03: /* INC BC */
            DECINC(INC_BC_WORD());
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
          case 0x08: /* EX AF AF' */
            EXAFAF();
            break;
          case 0x09: /* ADD HL BC */
            ADDXXREG(reg_h, reg_l, reg_b, reg_c, 11, 1);
            break;
          case 0x0a: /* LD A (BC) */
            LDREG(reg_a, LOAD(BC_WORD()), 4, 3, 1);
            break;
          case 0x0b: /* DEC BC */
            DECINC(DEC_BC_WORD());
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
            LDW(p12, reg_d, reg_e, 10, 0, 3);
            break;
          case 0x12: /* LD (DE) A */
            STREG(DE_WORD(), reg_a, 4, 3, 1);
            break;
          case 0x13: /* INC DE */
            DECINC(INC_DE_WORD());
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
            ADDXXREG(reg_h, reg_l, reg_d, reg_e, 11, 1);
            break;
          case 0x1a: /* LD A DE */
            LDREG(reg_a, LOAD(DE_WORD()), 4, 3, 1);
            break;
          case 0x1b: /* DEC DE */
            DECINC(DEC_DE_WORD());
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
            LDW(p12, reg_h, reg_l, 10, 0, 3);
            break;
          case 0x22: /* LD (WORD) HL */
            STW(p12, reg_h, reg_l, 4, 9, 3, 3);
            break;
          case 0x23: /* INC HL */
            DECINC(INC_HL_WORD());
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
          case 0x27: /* DAA */
            DAA();
            break;
          case 0x28: /* JR Z */
            BRANCH(LOCAL_ZERO(), p1);
            break;
          case 0x29: /* ADD HL HL */
            ADDXXREG(reg_h, reg_l, reg_h, reg_l, 11, 1);
            break;
          case 0x2a: /* LD HL (WORD) */
            LDIND(p12, reg_h, reg_l, 4, 4, 8, 3);
            break;
          case 0x2b: /* DEC HL */
            DECINC(DEC_HL_WORD());
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
          case 0x32: /* LD (WORD) A */
            STREG(p12, reg_a, 10, 3, 3);
            break;
          case 0x33: /* INC SP */
            DECINC(reg_sp++);
            break;
          case 0x34: /* INC (HL) */
            INCHLIND();
            break;
          case 0x35: /* DEC (HL) */
            DECHLIND();
            break;
          case 0x36: /* LD (HL) # */
            STREG(HL_WORD(), p1, 8, 2, 2);
            break;
          case 0x37: /* SCF */
            SCF();
            break;
          case 0x38: /* JR C */
            BRANCH(LOCAL_CARRY(), p1);
            break;
          case 0x39: /* ADD HL SP */
            ADDXXSP(reg_h, reg_l, 11, 1);
            break;
          case 0x3a: /* LD A (WORD) */
            LDREG(reg_a, LOAD(p12), 10, 3, 3);
            break;
          case 0x3b: /* DEC SP */
            DECINC(reg_sp--);
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
            LDREG(reg_b, reg_b, 0, 4, 1);
            break;
          case 0x41: /* LD B C */
            LDREG(reg_b, reg_c, 0, 4, 1);
            break;
          case 0x42: /* LD B D */
            LDREG(reg_b, reg_d, 0, 4, 1);
            break;
          case 0x43: /* LD B E */
            LDREG(reg_b, reg_e, 0, 4, 1);
            break;
          case 0x44: /* LD B H */
            LDREG(reg_b, reg_h, 0, 4, 1);
            break;
          case 0x45: /* LD B L */
            LDREG(reg_b, reg_l, 0, 4, 1);
            break;
          case 0x46: /* LD B (HL) */
            LDREG(reg_b, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x47: /* LD B A */
            LDREG(reg_b, reg_a, 0, 4, 1);
            break;
          case 0x48: /* LD C B */
            LDREG(reg_c, reg_b, 0, 4, 1);
            break;
          case 0x49: /* LD C C */
            LDREG(reg_c, reg_c, 0, 4, 1);
            break;
          case 0x4a: /* LD C D */
            LDREG(reg_c, reg_d, 0, 4, 1);
            break;
          case 0x4b: /* LD C E */
            LDREG(reg_c, reg_e, 0, 4, 1);
            break;
          case 0x4c: /* LD C H */
            LDREG(reg_c, reg_h, 0, 4, 1);
            break;
          case 0x4d: /* LD C L */
            LDREG(reg_c, reg_l, 0, 4, 1);
            break;
          case 0x4e: /* LD C (HL) */
            LDREG(reg_c, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x4f: /* LD C A */
            LDREG(reg_c, reg_a, 0, 4, 1);
            break;
          case 0x50: /* LD D B */
            LDREG(reg_d, reg_b, 0, 4, 1);
            break;
          case 0x51: /* LD D C */
            LDREG(reg_d, reg_c, 0, 4, 1);
            break;
          case 0x52: /* LD D D */
            LDREG(reg_d, reg_d, 0, 4, 1);
            break;
          case 0x53: /* LD D E */
            LDREG(reg_d, reg_e, 0, 4, 1);
            break;
          case 0x54: /* LD D H */
            LDREG(reg_d, reg_h, 0, 4, 1);
            break;
          case 0x55: /* LD D L */
            LDREG(reg_d, reg_l, 0, 4, 1);
            break;
          case 0x56: /* LD D (HL) */
            LDREG(reg_d, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x57: /* LD D A */
            LDREG(reg_d, reg_a, 0, 4, 1);
            break;
          case 0x58: /* LD E B */
            LDREG(reg_e, reg_b, 0, 4, 1);
            break;
          case 0x59: /* LD E C */
            LDREG(reg_e, reg_c, 0, 4, 1);
            break;
          case 0x5a: /* LD E D */
            LDREG(reg_e, reg_d, 0, 4, 1);
            break;
          case 0x5b: /* LD E E */
            LDREG(reg_e, reg_e, 0, 4, 1);
            break;
          case 0x5c: /* LD E H */
            LDREG(reg_e, reg_h, 0, 4, 1);
            break;
          case 0x5d: /* LD E L */
            LDREG(reg_e, reg_l, 0, 4, 1);
            break;
          case 0x5e: /* LD E (HL) */
            LDREG(reg_e, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x5f: /* LD E A */
            LDREG(reg_e, reg_a, 0, 4, 1);
            break;
          case 0x60: /* LD H B */
            LDREG(reg_h, reg_b, 0, 4, 1);
            break;
          case 0x61: /* LD H C */
            LDREG(reg_h, reg_c, 0, 4, 1);
            break;
          case 0x62: /* LD H D */
            LDREG(reg_h, reg_d, 0, 4, 1);
            break;
          case 0x63: /* LD H E */
            LDREG(reg_h, reg_e, 0, 4, 1);
            break;
          case 0x64: /* LD H H */
            LDREG(reg_h, reg_h, 0, 4, 1);
            break;
          case 0x65: /* LD H L */
            LDREG(reg_h, reg_l, 0, 4, 1);
            break;
          case 0x66: /* LD H (HL) */
            LDREG(reg_h, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x67: /* LD H A */
            LDREG(reg_h, reg_a, 0, 4, 1);
            break;
          case 0x68: /* LD L B */
            LDREG(reg_l, reg_b, 0, 4, 1);
            break;
          case 0x69: /* LD L C */
            LDREG(reg_l, reg_c, 0, 4, 1);
            break;
          case 0x6a: /* LD L D */
            LDREG(reg_l, reg_d, 0, 4, 1);
            break;
          case 0x6b: /* LD L E */
            LDREG(reg_l, reg_e, 0, 4, 1);
            break;
          case 0x6c: /* LD L H */
            LDREG(reg_l, reg_h, 0, 4, 1);
            break;
          case 0x6d: /* LD L L */
            LDREG(reg_l, reg_l, 0, 4, 1);
            break;
          case 0x6e: /* LD L (HL) */
            LDREG(reg_l, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x6f: /* LD L A */
            LDREG(reg_l, reg_a, 0, 4, 1);
            break;
          case 0x70: /* LD (HL) B */
            STREG(HL_WORD(), reg_b, 4, 3, 1);
            break;
          case 0x71: /* LD (HL) C */
            STREG(HL_WORD(), reg_c, 4, 3, 1);
            break;
          case 0x72: /* LD (HL) D */
            STREG(HL_WORD(), reg_d, 4, 3, 1);
            break;
          case 0x73: /* LD (HL) E */
            STREG(HL_WORD(), reg_e, 4, 3, 1);
            break;
          case 0x74: /* LD (HL) H */
            STREG(HL_WORD(), reg_h, 4, 3, 1);
            break;
          case 0x75: /* LD (HL) L */
            STREG(HL_WORD(), reg_l, 4, 3, 1);
            break;
          case 0x76: /* HALT */
            HALT();
            break;
          case 0x77: /* LD (HL) A */
            STREG(HL_WORD(), reg_a, 4, 3, 1);
            break;
          case 0x78: /* LD A B */
            LDREG(reg_a, reg_b, 0, 4, 1);
            break;
          case 0x79: /* LD A C */
            LDREG(reg_a, reg_c, 0, 4, 1);
            break;
          case 0x7a: /* LD A D */
            LDREG(reg_a, reg_d, 0, 4, 1);
            break;
          case 0x7b: /* LD A E */
            LDREG(reg_a, reg_e, 0, 4, 1);
            break;
          case 0x7c: /* LD A H */
            LDREG(reg_a, reg_h, 0, 4, 1);
            break;
          case 0x7d: /* LD A L */
            LDREG(reg_a, reg_l, 0, 4, 1);
            break;
          case 0x7e: /* LD A (HL) */
            LDREG(reg_a, LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x7f: /* LD A A */
            LDREG(reg_a, reg_a, 0, 4, 1);
            break;
          case 0x80: /* ADD B */
            ADD(reg_b, 0, 4, 1);
            break;
          case 0x81: /* ADD C */
            ADD(reg_c, 0, 4, 1);
            break;
          case 0x82: /* ADD D */
            ADD(reg_d, 0, 4, 1);
            break;
          case 0x83: /* ADD E */
            ADD(reg_e, 0, 4, 1);
            break;
          case 0x84: /* ADD H */
            ADD(reg_h, 0, 4, 1);
            break;
          case 0x85: /* ADD L */
            ADD(reg_l, 0, 4, 1);
            break;
          case 0x86: /* ADD (HL) */
            ADD(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x87: /* ADD A */
            ADD(reg_a, 0, 4, 1);
            break;
          case 0x88: /* ADC B */
            ADC(reg_b, 0, 4, 1);
            break;
          case 0x89: /* ADC C */
            ADC(reg_c, 0, 4, 1);
            break;
          case 0x8a: /* ADC D */
            ADC(reg_d, 0, 4, 1);
            break;
          case 0x8b: /* ADC E */
            ADC(reg_e, 0, 4, 1);
            break;
          case 0x8c: /* ADC H */
            ADC(reg_h, 0, 4, 1);
            break;
          case 0x8d: /* ADC L */
            ADC(reg_l, 0, 4, 1);
            break;
          case 0x8e: /* ADC (HL) */
            ADC(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x8f: /* ADC A */
            ADC(reg_a, 0, 4, 1);
            break;
          case 0x90: /* SUB B */
            SUB(reg_b, 0, 4, 1);
            break;
          case 0x91: /* SUB C */
            SUB(reg_c, 0, 4, 1);
            break;
          case 0x92: /* SUB D */
            SUB(reg_d, 0, 4, 1);
            break;
          case 0x93: /* SUB E */
            SUB(reg_e, 0, 4, 1);
            break;
          case 0x94: /* SUB H */
            SUB(reg_h, 0, 4, 1);
            break;
          case 0x95: /* SUB L */
            SUB(reg_l, 0, 4, 1);
            break;
          case 0x96: /* SUB (HL) */
            SUB(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x97: /* SUB A */
            SUB(reg_a, 0, 4, 1);
            break;
          case 0x98: /* SBC B */
            SBC(reg_b, 0, 4, 1);
            break;
          case 0x99: /* SBC C */
            SBC(reg_c, 0, 4, 1);
            break;
          case 0x9a: /* SBC D */
            SBC(reg_d, 0, 4, 1);
            break;
          case 0x9b: /* SBC E */
            SBC(reg_e, 0, 4, 1);
            break;
          case 0x9c: /* SBC H */
            SBC(reg_h, 0, 4, 1);
            break;
          case 0x9d: /* SBC L */
            SBC(reg_l, 0, 4, 1);
            break;
          case 0x9e: /* SBC (HL) */
            SBC(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0x9f: /* SBC A */
            SBC(reg_a, 0, 4, 1);
            break;
          case 0xa0: /* AND B */
            AND(reg_b, 0, 4, 1);
            break;
          case 0xa1: /* AND C */
            AND(reg_c, 0, 4, 1);
            break;
          case 0xa2: /* AND D */
            AND(reg_d, 0, 4, 1);
            break;
          case 0xa3: /* AND E */
            AND(reg_e, 0, 4, 1);
            break;
          case 0xa4: /* AND H */
            AND(reg_h, 0, 4, 1);
            break;
          case 0xa5: /* AND L */
            AND(reg_l, 0, 4, 1);
            break;
          case 0xa6: /* AND (HL) */
            AND(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0xa7: /* AND A */
            AND(reg_a, 0, 4, 1);
            break;
          case 0xa8: /* XOR B */
            XOR(reg_b, 0, 4, 1);
            break;
          case 0xa9: /* XOR C */
            XOR(reg_c, 0, 4, 1);
            break;
          case 0xaa: /* XOR D */
            XOR(reg_d, 0, 4, 1);
            break;
          case 0xab: /* XOR E */
            XOR(reg_e, 0, 4, 1);
            break;
          case 0xac: /* XOR H */
            XOR(reg_h, 0, 4, 1);
            break;
          case 0xad: /* XOR L */
            XOR(reg_l, 0, 4, 1);
            break;
          case 0xae: /* XOR (HL) */
            XOR(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0xaf: /* XOR A */
            XOR(reg_a, 0, 4, 1);
            break;
          case 0xb0: /* OR B */
            OR(reg_b, 0, 4, 1);
            break;
          case 0xb1: /* OR C */
            OR(reg_c, 0, 4, 1);
            break;
          case 0xb2: /* OR D */
            OR(reg_d, 0, 4, 1);
            break;
          case 0xb3: /* OR E */
            OR(reg_e, 0, 4, 1);
            break;
          case 0xb4: /* OR H */
            OR(reg_h, 0, 4, 1);
            break;
          case 0xb5: /* OR L */
            OR(reg_l, 0, 4, 1);
            break;
          case 0xb6: /* OR (HL) */
            OR(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0xb7: /* OR A */
            OR(reg_a, 0, 4, 1);
            break;
          case 0xb8: /* CP B */
            CP(reg_b, 0, 4, 1);
            break;
          case 0xb9: /* CP C */
            CP(reg_c, 0, 4, 1);
            break;
          case 0xba: /* CP D */
            CP(reg_d, 0, 4, 1);
            break;
          case 0xbb: /* CP E */
            CP(reg_e, 0, 4, 1);
            break;
          case 0xbc: /* CP H */
            CP(reg_h, 0, 4, 1);
            break;
          case 0xbd: /* CP L */
            CP(reg_l, 0, 4, 1);
            break;
          case 0xbe: /* CP (HL) */
            CP(LOAD(HL_WORD()), 4, 3, 1);
            break;
          case 0xbf: /* CP A */
            CP(reg_a, 0, 4, 1);
            break;
          case 0xc0: /* RET NZ */
            RET_COND(!LOCAL_ZERO());
            break;
          case 0xc1: /* POP BC */
            POP(reg_b, reg_c, 1);
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
            PUSH(reg_b, reg_c, 1);
            break;
          case 0xc6: /* ADD # */
            ADD(p1, 4, 3, 2);
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
            opcode_cb((BYTE)p0, (BYTE)p1, (BYTE)p2, (BYTE)p3, (WORD)p12,
                      (WORD)p23);
            break;
          case 0xcc: /* CALL Z */
            CALL_COND(p12, LOCAL_ZERO(), 10, 10, 3);
            break;
          case 0xcd: /* CALL */
            CALL(p12, 7, 3);
            break;
          case 0xce: /* ADC # */
            ADC(p1, 4, 3, 2);
            break;
          case 0xcf: /* RST 08 */
            CALL(0x08, 11, 1);
            break;
          case 0xd0: /* RET NC */
            RET_COND(!LOCAL_CARRY());
            break;
          case 0xd1: /* POP DE */
            POP(reg_d, reg_e, 1);
            break;
          case 0xd2: /* JP NC */
            JMP_COND(p12, !LOCAL_CARRY(), 10, 10);
            break;
          case 0xd3: /* OUT A */
            OUTA(p1);
            break;
          case 0xd4: /* CALL NC */
            CALL_COND(p12, !LOCAL_CARRY(), 10, 10, 3);
            break;
          case 0xd5: /* PUSH DE */
            PUSH(reg_d, reg_e, 1);
            break;
          case 0xd6: /* SUB # */
            SUB(p1, 4, 3, 2);
            break;
          case 0xd7: /* RST 10 */
            CALL(0x10, 11, 1);
            break;
          case 0xd8: /* RET C */
            RET_COND(LOCAL_CARRY());
            break;
          case 0xd9: /* EXX */
            EXX();
            break;
          case 0xda: /* JP C */
            JMP_COND(p12, LOCAL_CARRY(), 10, 10);
            break;
          case 0xdb: /* IN A */
            INA(p1);
            break;
          case 0xdc: /* CALL C */
            CALL_COND(p12, LOCAL_CARRY(), 10, 10, 3);
            break;
          case 0xdd: /*  OPCODE DD */
            opcode_dd((BYTE)p0, (BYTE)p1, (BYTE)p2, (BYTE)p3, (WORD)p12,
                      (WORD)p23);
            break;
            break;
          case 0xde: /* SBC # */
            SBC(p1, 4, 3, 2);
            break;
          case 0xdf: /* RST 18 */
            CALL(0x18, 11, 1);
            break;
          case 0xe0: /* RET PO */
            RET_COND(!LOCAL_PARITY());
            break;
          case 0xe1: /* POP HL */
            POP(reg_h, reg_l, 1);
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
            PUSH(reg_h, reg_l, 1);
            break;
          case 0xe6: /* AND # */
            AND(p1, 4, 3, 2);
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
            opcode_ed((BYTE)p0, (BYTE)p1, (BYTE)p2, (BYTE)p3, (WORD)p12,
                      (WORD)p23);
            break;
          case 0xee: /* XOR # */
            XOR(p1, 4, 3, 2);
            break;
          case 0xef: /* RST 28 */
            CALL(0x28, 11, 1);
            break;
          case 0xf0: /* RET P */
            RET_COND(!LOCAL_SIGN());
            break;
          case 0xf1: /* POP AF */
            POP(reg_a, reg_f, 1);
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
            PUSH(reg_a, reg_f, 1);
            break;
          case 0xf6: /* OR # */
            OR(p1, 4, 3, 2);
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
            opcode_fd((BYTE)p0, (BYTE)p1, (BYTE)p2, (BYTE)p3, (WORD)p12,
                      (WORD)p23);
            break;
          case 0xfe: /* CP # */
            CP(p1, 4, 3, 2);
            break;
          case 0xff: /* RST 38 */
            CALL(0x38, 11, 1);
            break;
        }
    } while (!dma_request);
}

