/*
 * Copyright 2001 by Arto Salmi and Joze Fabcic
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of GCC6809.
 * This file is part of VICE.
 *
 * VICE is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VICE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VICE; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "vice.h"

#include <stdarg.h>

#include "6809.h"
#include "alarm.h"
#include "h6809regs.h"
#include "interrupt.h"
#include "monitor.h"
#include "petmem.h"

#define CLK maincpu_clk
#define CPU_INT_STATUS maincpu_int_status
#define ALARM_CONTEXT maincpu_alarm_context

/* ------------------------------------------------------------------------- */
/* Hook for additional delay.  */

#ifndef CPU_DELAY_CLK
#define CPU_DELAY_CLK
#endif

#ifndef CPU_REFRESH_CLK
#define CPU_REFRESH_CLK
#endif

#ifndef DMA_ON_RESET
#define DMA_ON_RESET
#endif

#ifndef DMA_FUNC
#define DMA_FUNC
#endif

#define CALLER  e_comp_space
#define LAST_OPCODE_ADDR iPC
#define GLOBAL_REGS     h6809_regs

h6809_regs_t h6809_regs;

/* Export the local version of the registers.  */
#define EXPORT_REGISTERS()            \
  do {                                \
      GLOBAL_REGS.reg_x  = get_x();   \
      GLOBAL_REGS.reg_y  = get_y();   \
      GLOBAL_REGS.reg_u  = get_u();   \
      GLOBAL_REGS.reg_s  = get_s();   \
      GLOBAL_REGS.reg_pc = get_pc();  \
      GLOBAL_REGS.reg_dp = get_dp();  \
      GLOBAL_REGS.reg_cc = get_cc();  \
      GLOBAL_REGS.reg_a  = get_a();   \
      GLOBAL_REGS.reg_b  = get_b();   \
  } while (0)

/* Import the public version of the registers.  */
#define IMPORT_REGISTERS()         \
  do {                             \
      set_x( GLOBAL_REGS.reg_x );  \
      set_y( GLOBAL_REGS.reg_y );  \
      set_u( GLOBAL_REGS.reg_u );  \
      set_s( GLOBAL_REGS.reg_s );  \
      set_pc(GLOBAL_REGS.reg_pc);  \
      set_dp(GLOBAL_REGS.reg_dp);  \
      set_cc(GLOBAL_REGS.reg_cc);  \
      set_a (GLOBAL_REGS.reg_a );  \
      set_b (GLOBAL_REGS.reg_b );  \
  } while (0)
#define JUMP(pc)

#define DO_INTERRUPT(int_kind)  do {                                  \
        BYTE ik = (int_kind);                                         \
        if (ik & (IK_TRAP | IK_RESET)) {                              \
            if (ik & IK_TRAP) {                                       \
                EXPORT_REGISTERS();                                   \
                interrupt_do_trap(CPU_INT_STATUS, (WORD)PC);          \
                IMPORT_REGISTERS();                                   \
                if (CPU_INT_STATUS->global_pending_int & IK_RESET)    \
                    ik |= IK_RESET;                                   \
            }                                                         \
            if (ik & IK_RESET) {                                      \
                interrupt_ack_reset(CPU_INT_STATUS);                  \
                cpu6809_reset();                                      \
                DMA_ON_RESET;                                         \
            }                                                         \
        }                                                             \
        if (ik & (IK_MONITOR | IK_DMA)) {                             \
            if (ik & IK_MONITOR) {                                    \
                if (monitor_force_import(CALLER))                     \
                    IMPORT_REGISTERS();                               \
                if (monitor_mask[CALLER])                             \
                    EXPORT_REGISTERS();                               \
                if (monitor_mask[CALLER] & (MI_BREAK)) {              \
                    if (monitor_check_breakpoints(CALLER,             \
                            (WORD)PC)) {                              \
                        monitor_startup(CALLER);                      \
                        IMPORT_REGISTERS();                           \
                    }                                                 \
                }                                                     \
                if (monitor_mask[CALLER] & (MI_STEP)) {               \
                    monitor_check_icount((WORD)PC);                   \
                    IMPORT_REGISTERS();                               \
                }                                                     \
                if (monitor_mask[CALLER] & (MI_WATCH)) {              \
                    monitor_check_watchpoints(LAST_OPCODE_ADDR, (WORD)PC); \
                    IMPORT_REGISTERS();                               \
                }                                                     \
            }                                                         \
            if (ik & IK_DMA) {                                        \
                EXPORT_REGISTERS();                                   \
                DMA_FUNC;                                             \
                interrupt_ack_dma(CPU_INT_STATUS);                    \
                IMPORT_REGISTERS();                                   \
                JUMP(PC);                                             \
            }                                                         \
        }                                                             \
        if (ik & IK_NMI) {                                            \
            request_nmi(0);                                           \
        } else if (ik & IK_IRQ) {                                     \
            request_irq(0);                                           \
        }                                                             \
    } while (0)

#ifdef LAST_OPCODE_ADDR
#define SET_LAST_ADDR(x) LAST_OPCODE_ADDR = (x)
#else
#error "please define LAST_OPCODE_ADDR"
#endif

union regs {
    DWORD reg_l;
    WORD reg_s[2];
    BYTE reg_c[4];
} regs6309;

#define Q regs6309_reg_l
#ifndef WORDS_BIGENDIAN
#define W regs6309.reg_s[0]
#define D regs6309.reg_s[1]
#define F regs6309.reg_c[0]
#define E regs6309.reg_c[1]
#define B regs6309.reg_c[2]
#define A regs6309.reg_c[3]
#else
#define W regs6309.reg_s[1]
#define D regs6309.reg_s[0]
#define F regs6309.reg_c[3]
#define E regs6309.reg_c[2]
#define B regs6309.reg_c[1]
#define A regs6309.reg_c[0]
#endif

static WORD X, Y, S, U, DP, PC, iPC;
static BYTE EFI;
static DWORD H, N, Z, OV, C;

#ifdef H6309
static BYTE MD;
static WORD V;

#define MD_NATIVE 0x1		/* if 1, execute in 6309 mode */
#define MD_FIRQ_LIKE_IRQ 0x2	/* if 1, FIRQ acts like IRQ */
#define MD_ILL 0x40		/* illegal instruction */
#define MD_DBZ 0x80		/* divide by zero */
#endif /* H6309 */


static WORD ea = 0;
static unsigned int irqs_pending = 0;
static unsigned int firqs_pending = 0;
static unsigned int cc_changed = 0;

static WORD *index_regs[4] = { &X, &Y, &U, &S };

extern void nmi(void);
extern void irq(void);
extern void firq(void);

void request_nmi(unsigned int source)
{
    /* If the interrupt is not masked, generate
     * IRQ immediately.  Else, mark it pending and
     * we'll check it later when the flags change.
     */
    nmi();
}

void request_irq(unsigned int source)
{
    /* If the interrupt is not masked, generate
     * IRQ immediately.  Else, mark it pending and
     * we'll check it later when the flags change.
     */
    irqs_pending |= (1 << source);
    if (!(EFI & I_FLAG)) {
        irq();
    }
}

void release_irq(unsigned int source)
{
    irqs_pending &= ~(1 << source);
}


void request_firq (unsigned int source)
{
    /* If the interrupt is not masked, generate
     * IRQ immediately.  Else, mark it pending and
     * we'll check it later when the flags change.
     */
    firqs_pending |= (1 << source);
    if (!(EFI & F_FLAG)) {
        firq();
    }
}

void release_firq(unsigned int source)
{
    firqs_pending &= ~(1 << source);
}

void sim_error(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    fprintf(stderr, "m6809-run: (at PC=%04X) ", iPC);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

static inline void change_pc(WORD newPC)
{
    PC = newPC;
}

static inline BYTE imm_byte(void)
{
    BYTE val = read8(PC);

    PC++;
    return val;
}

static inline WORD imm_word(void)
{
    WORD val = read16(PC);

    PC += 2;
    return val;
}

#define WRMEM(addr, data) write8(addr, data)

static void WRMEM16(WORD addr, WORD data)
{
    WRMEM(addr, data >> 8);
    CLK++;
    WRMEM(addr + 1, data & 0xff);
}

#define RDMEM(addr) read8(addr)

static WORD RDMEM16(WORD addr)
{
    WORD val = RDMEM(addr) << 8;

    CLK++;
    val |= RDMEM(addr + 1);
    return val;
}

#define write_stack WRMEM
#define read_stack  RDMEM

static void write_stack16(WORD addr, WORD data)
{
    write_stack(addr + 1, data & 0xff);
    write_stack(addr, data >> 8);
}

static unsigned read_stack16(WORD addr)
{
    return (read_stack(addr) << 8) | read_stack(addr + 1);
}

static void direct(void)
{
    ea = read8(PC) | DP;
    PC++;
}

static void indexed(void)		/* note take 1 extra cycle */
{
    BYTE post = imm_byte();
    WORD *R = index_regs[(post >> 5) & 0x3];

    if (post & 0x80) {
        switch (post & 0x1f) {
            case 0x00:	/* ,R+ */
                ea = *R;
                *R += 1;
                CLK += 6;
                break;
            case 0x01:	/* ,R++ */
                ea = *R;
                *R += 2;
                CLK += 7;
                break;
            case 0x02:	/* ,-R */
                *R -= 1;
                ea = *R;
                CLK += 6;
                break;
            case 0x03:	/* ,--R */
                *R -= 2;
                ea = *R;
                CLK += 7;
                break;
            case 0x04:	/* ,R */
                ea = *R;
                CLK += 4;
                break;
            case 0x05:	/* B,R */
                ea = *R + (INT8)B;
                CLK += 5;
                break;
            case 0x06:	/* A,R */
                ea = *R + (INT8)A;
                CLK += 5;
                break;
            case 0x08:	/* 8bit,R */
                ea = *R + (INT8)imm_byte();
                CLK += 5;
                break;
            case 0x09:	/* 16bit,R */
                ea = *R + imm_word();
                CLK += 8;
                break;
            case 0x0b:	/* D,R */
                ea = *R + D;
                CLK += 8;
                break;
            case 0x0c:	/* 8bit,PC */
                ea = (INT8)imm_byte();
                ea += PC;
                CLK += 5;
                break;
            case 0x0d:	/* 16bit,PC */
                ea = imm_word();
                ea += PC;
                CLK += 9;
                break;
            case 0x11:	/* [,R++] */
                ea = *R;
                *R += 2;
                CLK += 7;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x13:	/* [,--R] */
                *R -= 2;
                ea = *R;
                CLK += 7;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x14:	/* [,R] */
                ea = *R;
                CLK += 4;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x15:	/* [B,R] */
                ea = *R + (INT8)B;
                CLK += 5;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x16:	/* [A,R] */
                ea = *R + (INT8)A;
                CLK += 5;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x18:	/* [8bit,R] */
                ea = *R + (INT8)imm_byte();
                CLK += 5;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x19:	/* [16bit,R] */
                ea = *R + imm_word();
                CLK += 8;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x1b:	/* [D,R] */
                ea = *R + D;
                CLK += 8;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x1c:	/* [8bit,PC] */
                ea = (INT8)imm_byte();
                ea += PC;
                CLK += 5;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x1d:	/* [16bit,PC] */
                ea = imm_word();
                ea += PC;
                CLK += 9;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            case 0x1f:	/* [16bit] */
                ea = imm_word();
                CLK += 6;
                ea = RDMEM16(ea);
                CLK += 2;
                break;
            default:
                ea = 0;
                sim_error("invalid index post $%02X\n", post);
                break;
        }
    } else {
        if (post & 0x10) {
            ea = *R + (post | 0xfff0);
        } else {
            ea = *R + (post & 0x000f);
        }
        CLK += 5;
    }
}

static void extended(void)
{
    ea = read16(PC);
    PC += 2;
}

/* external register functions */

static BYTE get_a(void)
{
    return A;
}

static BYTE get_b(void)
{
    return B;
}

static BYTE get_dp(void)
{
    return DP >> 8;
}

static WORD get_x(void)
{
    return X;
}

static WORD get_y(void)
{
    return Y;
}

static WORD get_s(void)
{
    return S;
}

static WORD get_u(void)
{
    return U;
}

static WORD get_pc(void)
{
    return PC;
}

static WORD get_d(void)
{
    return D;
}

static BYTE get_flags(void)
{
    return EFI;
}

#ifdef H6309
static BYTE get_e(void)
{
    return E;
}

static BYTE get_f(void)
{
    return F;
}

static WORD get_w(void)
{
    return W;
}

static DWORD get_q(void)
{
    return Q;
}

static WORD get_v(void)
{
    return V;
}

static BYTE get_md(void)
{
    return MD;
}
#endif

static void set_a(BYTE val)
{
    A = val;
}

static void set_b(BYTE val)
{
    B = val;
}

static void set_dp(BYTE val)
{
    DP = val << 8;
}

static void set_x(WORD val)
{
    X = val;
}

static void set_y(WORD val)
{
    Y = val;
}

static void set_s(WORD val)
{
    S = val;
}

static void set_u(WORD val)
{
    U = val;
}

static void set_pc(WORD val)
{
    PC = val;
}

static void set_d(WORD val)
{
    D = val;
}

#ifdef H6309
static void set_e(BYTE val)
{
    E = val;
}

static void set_f(BYTE val)
{
    F = val;
}

static void set_w(WORD val)
{
    W = val;
}

static void set_q(DWORD val)
{
    Q = val;
}

static void set_v(WORD val)
{
    V = val;
}

static void set_md(BYTE val)
{
    MD = val;
}
#endif


/* handle condition code register */

static BYTE get_cc(void)
{
    BYTE res = EFI & (E_FLAG | F_FLAG | I_FLAG);

    if (H & 0x10) {
        res |= H_FLAG;
    }
    if (N & 0x80) {
        res |= N_FLAG;
    }
    if (Z == 0) {
        res |= Z_FLAG;
    }
    if (OV & 0x80) {
        res |= V_FLAG;
    }
    if (C != 0) {
        res |= C_FLAG;
    }
    return res;
}

static void set_cc(BYTE arg)
{
    EFI = arg & (E_FLAG | F_FLAG | I_FLAG);
    H = (arg & H_FLAG ? 0x10 : 0);
    N = (arg & N_FLAG ? 0x80 : 0);
    Z = (~arg) & Z_FLAG;
    OV = (arg & V_FLAG ? 0x80 : 0);
    C = arg & C_FLAG;
    cc_changed = 1;
}

static void cc_modified(void)
{
    /* Check for pending interrupts */
    if (firqs_pending && !(EFI & F_FLAG)) {
        firq();
    } else if (irqs_pending && !(EFI & I_FLAG)) {
        irq();
    }
    cc_changed = 0;
}

/* Undocumented: When the 6809 transfers an 8bit register
   to a 16bit register the LSB is filled with the 8bit
   register value and the MSB is filled with 0xff.
 */

static WORD get_reg(BYTE nro)
{
    WORD val;

    switch (nro) {
        case 0:
            val = D;
            break;
        case 1:
            val = X;
            break;
        case 2:
            val = Y;
            break;
        case 3:
            val = U;
            break;
        case 4:
            val = S;
            break;
        case 5:
            val = PC;
            break;
#ifdef H6309
        case 6:
            val = W;
            break;
        case 7:
            val = V;
            break;
#endif
        case 8:
            val = A & 0xffff;
            break;
        case 9:
            val = B & 0xffff;
            break;
        case 10:
            val = get_cc() & 0xffff;
            break;
        case 11:
            val = (DP >> 8) & 0xffff;
            break;
#ifdef H6309
        case 14:
            val = E & 0xffff;
            break;
        case 15:
            val = F & 0xffff;
            break;
#endif
    }
    return val;
}

static void set_reg(BYTE nro, WORD val)
{
    switch (nro) {
        case 0:
            D = val;
            break;
        case 1:
            X = val;
            break;
        case 2:
            Y = val;
            break;
        case 3:
            U = val;
            break;
        case 4:
            S = val;
            break;
        case 5:
            PC = val;
            break;
#ifdef H6309
        case 6:
            W = val;
            break;
        case 7:
            V = val;
            break;
#endif
        case 8:
            A = val & 0xff;
            break;
        case 9:
            B = val & 0xff;
            break;
        case 10:
            set_cc(val & 0xff);
            break;
        case 11:
            DP = (val & 0xff) << 8;
            break;
#ifdef H6309
        case 14:
            E = val & 0xff;
            break;
        case 15:
            F = val & 0xff;
            break;
#endif
    }
}

/* 8-Bit Accumulator and Memory Instructions */

static BYTE adc(BYTE arg, BYTE val)
{
    WORD res = arg + val + (C != 0);

    C = (res >> 1) & 0x80;
    N = Z = res &= 0xff;
    OV = H = arg ^ val ^ res ^ C;

    return res;
}

static BYTE add(BYTE arg, BYTE val)
{
    WORD res = arg + val;

    C = (res >> 1) & 0x80;
    N = Z = res &= 0xff;
    OV = H = arg ^ val ^ res ^ C;

    return res;
}

static BYTE and(BYTE arg, BYTE val)
{
    BYTE res = arg & val;

    N = Z = res;
    OV = 0;

    return res;
}

static BYTE asl(BYTE arg)		/* same as lsl */
{
    WORD res = arg << 1;

    C = res & 0x100;
    N = Z = res &= 0xff;
    OV = arg ^ res;
    CLK += 2;

    return res;
}

static BYTE asr(BYTE arg)
{
    WORD res = (INT8)arg;

    C = res & 1;
    N = Z = res = (res >> 1) & 0xff;
    CLK += 2;

    return res;
}

static void bit(BYTE arg, BYTE val)
{
    N = Z = arg & val;
    OV = 0;
}

static BYTE clr(BYTE arg)
{
    C = N = Z = OV = 0;
    CLK += 2;

    return 0;
}

static void cmp(BYTE arg, BYTE val)
{
    WORD res = arg - val;

    C = res & 0x100;
    N = Z = res &= 0xff;
    OV = (arg ^ val) & (arg ^ res);
}

static BYTE com(BYTE arg)
{
    BYTE res = ~arg;

    N = Z = res;
    OV = 0;
    C = 1;
    CLK += 2;

    return res;
}

static void daa(void)
{
    WORD res = A;
    BYTE msn = res & 0xf0;
    BYTE lsn = res & 0x0f;

    if (lsn > 0x09 || (H & 0x10)) {
        res += 0x06;
    }
    if (msn > 0x80 && lsn > 0x09) {
        res += 0x60;
    }
    if (msn > 0x90 || (C != 0)) {
        res += 0x60;
    }

    C |= (res & 0x100);
    A = N = Z = res &= 0xff;

    CLK += 2;
}

static BYTE dec(BYTE arg)
{
    BYTE res = arg - 1;

    N = Z = res;
    OV = arg & ~res;
    CLK += 2;

    return res;
}

static BYTE eor(BYTE arg, BYTE val)
{
    BYTE res = arg ^ val;

    N = Z = res;
    OV = 0;

    return res;
}

static void exg(void)
{
    WORD tmp1 = 0xff;
    WORD tmp2 = 0xff;
    BYTE post = imm_byte();

    if (((post ^ (post << 4)) & 0x80) == 0) {
        tmp1 = get_reg(post >> 4);
        tmp2 = get_reg(post & 15);
    }

    set_reg(post & 15, tmp1);
    set_reg(post >> 4, tmp2);

    CLK += 8;
}

static BYTE inc(BYTE arg)
{
    BYTE res = arg + 1;

    N = Z = res;
    OV = ~arg & res;
    CLK += 2;

    return res;
}

static BYTE ld(BYTE arg)
{
    N = Z = arg;
    OV = 0;

    return arg;
}

static BYTE lsr(BYTE arg)
{
    BYTE res = arg >> 1;

    N = 0;
    Z = res;
    C = arg & 1;
    CLK += 2;

    return res;
}

static void mul(void)
{
    WORD res = A * B;

    Z = D = res;
    C = res & 0x80;
    CLK += 11;
}

static BYTE neg(BYTE arg)
{
    BYTE res = ~arg + 1;

    C = N = Z = res;
    OV = res & arg;
    CLK += 2;

    return res;
}

static BYTE or(BYTE arg, BYTE val)
{
    BYTE res = arg | val;

    N = Z = res;
    OV = 0;

    return res;
}

static BYTE rol(BYTE arg)
{
    WORD res = (arg << 1) + (C != 0);

    C = res & 0x100;
    N = Z = res &= 0xff;
    OV = arg ^ res;
    CLK += 2;

    return res;
}

static BYTE ror(BYTE arg)
{
    BYTE res = (arg >> 1) | ((C != 0) << 7);

    C = arg & 1;
    N = Z = res;
    CLK += 2;

    return res;
}

static BYTE sbc(BYTE arg, BYTE val)
{
    WORD res = arg - val - (C != 0);

    C = res & 0x100;
    N = Z = res &= 0xff;
    OV = (arg ^ val) & (arg ^ res);

    return res;
}

static void st(BYTE arg)
{
    N = Z = arg;
    OV = 0;

    WRMEM(ea, arg);
}

static BYTE sub(BYTE arg, BYTE val)
{
    WORD res = arg - val;

    C = res & 0x100;
    N = Z = res &= 0xff;
    OV = (arg ^ val) & (arg ^ res);

    return res;
}

static void tst(BYTE arg)
{
    N = Z = arg;
    OV = 0;
    CLK += 2;
}

static void tfr(void)
{
    WORD tmp1 = 0xff;
    BYTE post = imm_byte();

    if (((post ^ (post << 4)) & 0x80) == 0) {
        tmp1 = get_reg (post >> 4);
    }

    set_reg(post & 15, tmp1);

    CLK += 6;
}

/* 16-Bit Accumulator Instructions */

/* ------------------------marco------------------------------- */

static void abx(void)
{
    X += B;
    CLK += 3;
}

static void addd(WORD val)
{
    DWORD res = D + val;

    C = res & 0x10000;
    Z = res &= 0xffff;
    OV = ((D ^ res) & (val ^ res)) >> 8;
    N = res >> 8;
    D = res;
}

static void cmp16(WORD arg, WORD val)
{
    DWORD res = arg - val;

    C = res & 0x10000;
    Z = res &= 0xffff;
    N = res >> 8;
    OV = ((arg ^ val) & (arg ^ res)) >> 8;
}

static void ldd(WORD arg)
{
    Z = D = arg;
    N = arg >> 8;
    OV = 0;
}

static WORD ld16(WORD arg)
{
    Z = arg;
    N = arg >> 8;
    OV = 0;

    return arg;
}

static void sex(void)
{
    D = (INT8)B;

    Z = D;
    N = D >> 8;
    CLK += 2;
}

static void std(void)
{
    Z = D;
    N = D >> 8;
    OV = 0;
    WRMEM16(ea, D);
}

static void st16(WORD arg)
{
    Z = arg;
    N = arg >> 8;
    OV = 0;
    WRMEM16(ea, arg);
}

static void subd(WORD val)
{
    DWORD res = D - val;

    C = res & 0x10000;
    Z = res &= 0xffff;
    OV = ((D ^ val) & (D ^ res)) >> 8;
    N = res >> 8;
    D = res;
}

/* stack instructions */

static void pshs(void)
{
    BYTE post = imm_byte();

    CLK += 5;

    if (post & 0x80) {
        CLK += 2;
        S -= 2;
        write_stack16(S, PC);
    }
    if (post & 0x40) {
        CLK += 2;
        S -= 2;
        write_stack16(S, U);
    }
    if (post & 0x20) {
        CLK += 2;
        S -= 2;
        write_stack16(S, Y);
    }
    if (post & 0x10) {
        CLK += 2;
        S -= 2;
        write_stack16(S, X);
    }
    if (post & 0x08) {
        CLK++;
        S--;
        write_stack(S, DP >> 8);
    }
    if (post & 0x04) {
        CLK++;
        S--;
        write_stack(S, B);
    }
    if (post & 0x02) {
        CLK++;
        S--;
        write_stack(S, A);
    }
    if (post & 0x01) {
        CLK++;
        S--;
        write_stack(S, get_cc());
    }
}

static void pshu(void)
{
    BYTE post = imm_byte();

    CLK += 5;

    if (post & 0x80) {
        CLK += 2;
        U -= 2;
        write_stack16(U, PC);
    }
    if (post & 0x40) {
        CLK += 2;
        U -= 2;
        write_stack16(U, S);
    }
    if (post & 0x20) {
        CLK += 2;
        U -= 2;
        write_stack16(U, Y);
    }
    if (post & 0x10) {
        CLK += 2;
        U -= 2;
        write_stack16(U, X);
    }
    if (post & 0x08) {
        CLK++;
        U--;
        write_stack(U, DP >> 8);
    }
    if (post & 0x04) {
        CLK++;
        U--;
        write_stack(U, B);
    }
    if (post & 0x02) {
        CLK++;
        U--;
        write_stack(U, A);
    }
    if (post & 0x01) {
        CLK++;
        U--;
        write_stack(U, get_cc());
    }
}

static void puls(void)
{
    BYTE post = imm_byte();

    CLK += 5;

    if (post & 0x01) {
        CLK++;
        set_cc(read_stack(S++));
    }
    if (post & 0x02) {
        CLK++;
        A = read_stack(S++);
    }
    if (post & 0x04) {
        CLK++;
        B = read_stack(S++);
    }
    if (post & 0x08) {
        CLK++;
        DP = read_stack(S++) << 8;
    }
    if (post & 0x10) {
        CLK += 2;
        X = read_stack16(S);
        S += 2;
    }
    if (post & 0x20) {
        CLK += 2;
        Y = read_stack16(S);
        S += 2;
    }
    if (post & 0x40) {
        CLK += 2;
        U = read_stack16(S);
        S += 2;
    }
    if (post & 0x80) {
        CLK += 2;
        PC = read_stack16(S);
        S += 2;
    }
}

static void pulu(void)
{
    BYTE post = imm_byte();

    CLK += 5;

    if (post & 0x01) {
        CLK++;
        set_cc(read_stack(U++));
    }
    if (post & 0x02) {
        CLK++;
        A = read_stack(U++);
    }
    if (post & 0x04) {
        CLK++;
        B = read_stack(U++);
    }
    if (post & 0x08) {
        CLK++;
        DP = read_stack(U++) << 8;
    }
    if (post & 0x10) {
        CLK += 2;
        X = read_stack16(U);
        U += 2;
    }
    if (post & 0x20) {
        CLK += 2;
        Y = read_stack16(U);
        U += 2;
    }
    if (post & 0x40) {
        CLK += 2;
        S = read_stack16(U);
        U += 2;
    }
    if (post & 0x80) {
        CLK += 2;
        PC = read_stack16(U);
        U += 2;
    }
}

/* Miscellaneous Instructions */

static void nop(void)
{
    CLK += 2;
}

static void jsr(void)
{
    S -= 2;
    write_stack16(S, PC);
    change_pc(ea);
}

static void rti(void)
{
    CLK += 6;
    set_cc(read_stack(S));
    S++;

    if ((EFI & E_FLAG) != 0) {
        CLK += 9;
        A = read_stack(S++);
        B = read_stack(S++);
        DP = read_stack(S++) << 8;
        X = read_stack16(S);
        S += 2;
        Y = read_stack16(S);
        S += 2;
        U = read_stack16(S);
        S += 2;
    }
    PC = read_stack16(S);
    S += 2;
}

static void rts(void)
{
    CLK += 5;
    PC = read_stack16(S);
    S += 2;
}

void nmi(void)
{
    EFI |= E_FLAG;
    S -= 2;
    write_stack16(S, PC);
    S -= 2;
    write_stack16(S, U);
    S -= 2;
    write_stack16(S, Y);
    S -= 2;
    write_stack16(S--, X);
    write_stack(S--, DP >> 8);
    write_stack(S--, B);
    write_stack(S--, A);
    write_stack(S, get_cc());
    EFI |= I_FLAG;

    change_pc(read16(0xfffc));
}

void irq(void)
{
    EFI |= E_FLAG;
    S -= 2;
    write_stack16(S, PC);
    S -= 2;
    write_stack16(S, U);
    S -= 2;
    write_stack16(S, Y);
    S -= 2;
    write_stack16(S--, X);
    write_stack(S--, DP >> 8);
    write_stack(S--, B);
    write_stack(S--, A);
    write_stack(S, get_cc());
    EFI |= I_FLAG;

    change_pc(read16(0xfff8));
    irqs_pending = 0;
}

void firq(void)
{
    EFI &= ~E_FLAG;
    S -= 2;
    write_stack16(S--, PC);
    write_stack(S, get_cc());
    EFI |= (I_FLAG | F_FLAG);

    change_pc(read16(0xfff6));
    firqs_pending = 0;
}

void swi(void)
{
    CLK += 19;
    //CLK++;        /* /VMA cycle */
    EFI |= E_FLAG;
    S -= 2;
    write_stack16(S, PC);
    S -= 2;
    write_stack16(S, U);
    S -= 2;
    write_stack16(S, Y);
    S -= 2;
    write_stack16(S--, X);
    write_stack(S--, DP >> 8);
    write_stack(S--, B);
    write_stack(S--, A);
    write_stack(S, get_cc());
    EFI |= (I_FLAG | F_FLAG);
    //CLK++;        /* /VMA cycle */

    change_pc(read16(0xfffa));
}

void swi2(void)
{
    CLK += 20;
    //CLK++;        /* /VMA cycle */
    EFI |= E_FLAG;
    S -= 2;
    write_stack16(S, PC & 0xffff);
    S -= 2;
    write_stack16(S, U);
    S -= 2;
    write_stack16(S, Y);
    S -= 2;
    write_stack16(S--, X);
    write_stack(S--, DP >> 8);
    write_stack(S--, B);
    write_stack(S--, A);
    write_stack(S, get_cc());
    //CLK++;        /* /VMA cycle */

    change_pc(read16(0xfff4));
}

void swi3(void)
{
    CLK += 20;
    //CLK++;        /* /VMA cycle */
    EFI |= E_FLAG;
    S -= 2;
    write_stack16(S, PC & 0xffff);
    S -= 2;
    write_stack16(S, U);
    S -= 2;
    write_stack16(S, Y);
    S -= 2;
    write_stack16(S--, X);
    write_stack(S--, DP >> 8);
    write_stack(S--, B);
    write_stack(S--, A);
    write_stack(S, get_cc());
    //CLK++;        /* /VMA cycle */

    change_pc(read16(0xfff2));
}

#ifdef H6309
void trap(void)
{
    CLK += 20;
    //CLK++;        /* /VMA cycle */
    EFI |= E_FLAG;
    S -= 2;
    write_stack16(S, PC & 0xffff);
    S -= 2;
    write_stack16(S, U);
    S -= 2;
    write_stack16(S, Y);
    S -= 2;
    write_stack16(S--, X);
    write_stack(S--, DP >> 8);
    write_stack(S--, B);
    write_stack(S--, A);
    write_stack(S, get_cc());
    //CLK++;        /* /VMA cycle */

    change_pc(read16(0xfff0));
}
#endif

void cwai(void)
{
    sim_error("CWAI - not supported yet!");
}

void sync(void)
{
    CLK += 4;
    sim_error("SYNC - not supported yet!");
}

static void orcc(void)
{
    BYTE tmp = imm_byte();

    set_cc(get_cc() | tmp);
    CLK += 3;
}

static void andcc(void)
{
    BYTE tmp = imm_byte();

    set_cc(get_cc() & tmp);
    CLK += 3;
}

/* Branch Instructions */

#define cond_HI() ((Z != 0) && (C == 0))
#define cond_LS() ((Z == 0) || (C != 0))
#define cond_HS() (C == 0)
#define cond_LO() (C != 0)
#define cond_NE() (Z != 0)
#define cond_EQ() (Z == 0)
#define cond_VC() ((OV & 0x80) == 0)
#define cond_VS() ((OV & 0x80) != 0)
#define cond_PL() ((N & 0x80) == 0)
#define cond_MI() ((N & 0x80) != 0)
#define cond_GE() (((N^OV) & 0x80) == 0)
#define cond_LT() (((N^OV) & 0x80) != 0)
#define cond_GT() ((((N^OV) & 0x80) == 0) && (Z != 0))
#define cond_LE() ((((N^OV) & 0x80) != 0) || (Z == 0))

static void bra(void)
{
    INT8 tmp = (INT8)imm_byte();
    change_pc(PC + tmp);
}

static void branch(unsigned cond)
{
    if (cond) {
        bra();
    } else {
        change_pc(PC + 1);
    }

    CLK += 3;
}

static void long_bra(void)
{
    INT16 tmp = (INT16)imm_word();
    change_pc(PC + tmp);
}

static void long_branch(unsigned cond)
{
    if (cond) {
        long_bra();
        CLK += 6;
    } else {
        change_pc(PC + 2);
        CLK += 5;
    }
}

static void long_bsr(void)
{
    INT16 tmp = (INT16)imm_word();
    ea = PC + tmp;
    S -= 2;
    write_stack16(S, PC);
    CLK += 9;
    change_pc(ea);
}

static void bsr(void)
{
    INT8 tmp = (INT8)imm_byte();
    ea = PC + tmp;
    S -= 2;
    write_stack16(S, PC);
    CLK += 7;
    change_pc(ea);
}


/* Execute 6809 code for a certain number of cycles. */
void h6809_mainloop (struct interrupt_cpu_status_s *maincpu_int_status, alarm_context_t *maincpu_alarm_context)
{
  unsigned opcode;

  do
    {

#ifndef CYCLE_EXACT_ALARM
    while (CLK >= alarm_context_next_pending_clk(ALARM_CONTEXT)) {
        alarm_context_dispatch(ALARM_CONTEXT, CLK);
        CPU_DELAY_CLK
    }
#endif

    {
        enum cpu_int pending_interrupt;

        if (!(CPU_INT_STATUS->global_pending_int & IK_IRQ)
            && (CPU_INT_STATUS->global_pending_int & IK_IRQPEND)
            && CPU_INT_STATUS->irq_pending_clk <= CLK) {
            interrupt_ack_irq(CPU_INT_STATUS);
            release_irq(0);
        }

        pending_interrupt = CPU_INT_STATUS->global_pending_int;
        if (pending_interrupt != IK_NONE) {
            DO_INTERRUPT(pending_interrupt);
            if (!(CPU_INT_STATUS->global_pending_int & IK_IRQ)
                && CPU_INT_STATUS->global_pending_int & IK_IRQPEND)
                    CPU_INT_STATUS->global_pending_int &= ~IK_IRQPEND;
            CPU_DELAY_CLK
#ifndef CYCLE_EXACT_ALARM
            while (CLK >= alarm_context_next_pending_clk(ALARM_CONTEXT)) {
                alarm_context_dispatch(ALARM_CONTEXT, CLK);
                CPU_DELAY_CLK
            }
#endif
        }
    }

      SET_LAST_ADDR(PC);
      opcode = imm_byte ();

      switch (opcode)
	{
	case 0x00:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, neg (RDMEM (ea)));
	  break;		/* NEG direct */
#ifdef H6309
	case 0x01:		/* OIM */
	  break;
	case 0x02:		/* AIM */
	  break;
#endif
	case 0x03:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, com (RDMEM (ea)));
	  break;		/* COM direct */
	case 0x04:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, lsr (RDMEM (ea)));
	  break;		/* LSR direct */
#ifdef H6309
	case 0x05:		/* EIM */
	  break;
#endif
	case 0x06:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, ror (RDMEM (ea)));
	  break;		/* ROR direct */
	case 0x07:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, asr (RDMEM (ea)));
	  break;		/* ASR direct */
	case 0x08:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, asl (RDMEM (ea)));
	  break;		/* ASL direct */
	case 0x09:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, rol (RDMEM (ea)));
	  break;		/* ROL direct */
	case 0x0a:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, dec (RDMEM (ea)));
	  break;		/* DEC direct */
#ifdef H6309
	case 0x0B:		/* TIM */
	  break;
#endif
	case 0x0c:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, inc (RDMEM (ea)));
	  break;		/* INC direct */
	case 0x0d:
	  direct ();
	  CLK += 4;
	  tst (RDMEM (ea));
	  break;		/* TST direct */
	case 0x0e:
	  direct ();
	  CLK += 3;
	  PC = ea;
	  break;		/* JMP direct */
	case 0x0f:
	  direct ();
	  CLK += 4;
	  WRMEM (ea, clr (RDMEM (ea)));
	  break;		/* CLR direct */
	case 0x10:
	  {
            page_10:
	    opcode = imm_byte ();

	    switch (opcode)
	      {
              case 0x10:        /* ignore further prefix bytes (UNDOC) */
              case 0x11:
                goto page_10;
	      case 0x21:
		CLK += 5;
		PC += 2;
		break;
	      case 0x22:
		long_branch (cond_HI ());
		break;
	      case 0x23:
		long_branch (cond_LS ());
		break;
	      case 0x24:
		long_branch (cond_HS ());
		break;
	      case 0x25:
		long_branch (cond_LO ());
		break;
	      case 0x26:
		long_branch (cond_NE ());
		break;
	      case 0x27:
		long_branch (cond_EQ ());
		break;
	      case 0x28:
		long_branch (cond_VC ());
		break;
	      case 0x29:
		long_branch (cond_VS ());
		break;
	      case 0x2a:
		long_branch (cond_PL ());
		break;
	      case 0x2b:
		long_branch (cond_MI ());
		break;
	      case 0x2c:
		long_branch (cond_GE ());
		break;
	      case 0x2d:
		long_branch (cond_LT ());
		break;
	      case 0x2e:
		long_branch (cond_GT ());
		break;
	      case 0x2f:
		long_branch (cond_LE ());
		break;
#ifdef H6309
	      case 0x30:	/* ADDR */
		break;
	      case 0x31:	/* ADCR */
		break;
	      case 0x32:	/* SUBR */
		break;
	      case 0x33:	/* SBCR */
		break;
	      case 0x34:	/* ANDR */
		break;
	      case 0x35:	/* ORR */
		break;
	      case 0x36:	/* EORR */
		break;
	      case 0x37:	/* CMPR */
		break;
	      case 0x38:	/* PSHSW */
		break;
	      case 0x39:	/* PULSW */
		break;
	      case 0x3a:	/* PSHUW */
		break;
	      case 0x3b:	/* PULUW */
		break;
#endif
	      case 0x3f:
		swi2 ();
		break;
#ifdef H6309
	      case 0x40:	/* NEGD */
		break;
	      case 0x43:	/* COMD */
		break;
	      case 0x44:	/* LSRD */
		break;
	      case 0x46:	/* RORD */
		break;
	      case 0x47:	/* ASRD */
		break;
	      case 0x48:	/* ASLD/LSLD */
		break;
	      case 0x49:	/* ROLD */
		break;
	      case 0x4a:	/* DECD */
		break;
	      case 0x4c:	/* INCD */
		break;
	      case 0x4d:	/* TSTD */
		break;
	      case 0x4f:	/* CLRD */
		break;
	      case 0x53:	/* COMW */
		break;
	      case 0x54:	/* LSRW */
		break;
	      case 0x56:	/* ??RORW */
		break;
	      case 0x59:	/* ROLW */
		break;
	      case 0x5a:	/* DECW */
		break;
	      case 0x5c:	/* INCW */
		break;
	      case 0x5d:	/* TSTW */
		break;
	      case 0x5f:	/* CLRW */
		break;
	      case 0x80:	/* SUBW */
		break;
	      case 0x81:	/* CMPW */
		break;
	      case 0x82:	/* SBCD */
		break;
#endif
	      case 0x83:
		CLK += 5;
		cmp16 (get_d (), imm_word ());
		break;
#ifdef H6309
	      case 0x84:	/* ANDD */
		break;
	      case 0x85:	/* BITD */
		break;
	      case 0x86:	/* LDW */
		break;
	      case 0x88:	/* EORD */
		break;
	      case 0x89:	/* ADCD */
		break;
	      case 0x8a:	/* ORD */
		break;
	      case 0x8b:	/* ADDW */
		break;
#endif
	      case 0x8c:
		CLK += 5;
		cmp16 (Y, imm_word ());
		break;
	      case 0x8e:
		CLK += 4;
		Y = ld16 (imm_word ());
		break;
#ifdef H6309
	      case 0x90:	/* SUBW */
		break;
	      case 0x91:	/* CMPW */
		break;
	      case 0x92:	/* SBCD */
		break;
#endif
	      case 0x93:
		direct ();
		CLK += 5;
		cmp16 (get_d (), RDMEM16 (ea));
		CLK++;
		break;
	      case 0x9c:
		direct ();
		CLK += 5;
		cmp16 (Y, RDMEM16 (ea));
		CLK++;
		break;
	      case 0x9e:
		direct ();
		CLK += 5;
		Y = ld16 (RDMEM16 (ea));
		break;
	      case 0x9f:
		direct ();
		CLK += 5;
		st16 (Y);
		break;
	      case 0xa3:
		CLK++;
		indexed ();
		cmp16 (get_d (), RDMEM16 (ea));
		CLK++;
		break;
	      case 0xac:
		CLK++;
		indexed ();
		cmp16 (Y, RDMEM16 (ea));
		CLK++;
		break;
	      case 0xae:
		CLK++;
		indexed ();
		Y = ld16 (RDMEM16 (ea));
		break;
	      case 0xaf:
		CLK++;
		indexed ();
		st16 (Y);
		break;
	      case 0xb3:
		extended ();
		CLK += 6;
		cmp16 (get_d (), RDMEM16 (ea));
		CLK++;
		break;
	      case 0xbc:
		extended ();
		CLK += 6;
		cmp16 (Y, RDMEM16 (ea));
		CLK++;
		break;
	      case 0xbe:
		extended ();
		CLK += 6;
		Y = ld16 (RDMEM16 (ea));
		break;
	      case 0xbf:
		extended ();
		CLK += 6;
		st16 (Y);
		break;
	      case 0xce:
		CLK += 4;
		S = ld16 (imm_word ());
		break;
	      case 0xde:
		direct ();
		CLK += 5;
		S = ld16 (RDMEM16 (ea));
		break;
	      case 0xdf:
		direct ();
		CLK += 5;
		st16 (S);
		break;
	      case 0xee:
		CLK++;
		indexed ();
		S = ld16 (RDMEM16 (ea));
		break;
	      case 0xef:
		CLK++;
		indexed ();
		st16 (S);
		break;
	      case 0xfe:
		extended ();
		CLK += 6;
		S = ld16 (RDMEM16 (ea));
		break;
	      case 0xff:
		extended ();
		CLK += 6;
		st16 (S);
		break;
	      default:
	        sim_error ("invalid opcode (1) at %X\n", iPC);
		break;
	      }
	  }
	  break;

	case 0x11:
	  {
          page_11:
	    opcode = imm_byte ();

	    switch (opcode)
	      {
              case 0x10:        /* ignore further prefix bytes (UNDOC) */
              case 0x11:
                goto page_11;
	      case 0x3f:
		swi3 ();
		break;
#ifdef H6309
			case 0x80: /* SUBE */
			case 0x81: /* CMPE */
#endif
	      case 0x83:
		CLK += 5;
		cmp16 (U, imm_word ());
		break;
#ifdef H6309
			case 0x86: /* LDE */
			case 0x8B: /* ADDE */
#endif
	      case 0x8c:
		CLK += 5;
		cmp16 (S, imm_word ());
		break;
#ifdef H6309
			case 0x8D: /* DIVD */
			case 0x8E: /* DIVQ */
			case 0x8F: /* MULD */
			case 0x90: /* SUBE */
			case 0x91: /* CMPE */
#endif
	      case 0x93:
		direct ();
		CLK += 5;
		cmp16 (U, RDMEM16 (ea));
		CLK++;
		break;
	      case 0x9c:
		direct ();
		CLK += 5;
		cmp16 (S, RDMEM16 (ea));
		CLK++;
		break;
	      case 0xa3:
		CLK++;
		indexed ();
		cmp16 (U, RDMEM16 (ea));
		CLK++;
		break;
	      case 0xac:
		CLK++;
		indexed ();
		cmp16 (S, RDMEM16 (ea));
		CLK++;
		break;
	      case 0xb3:
		extended ();
		CLK += 6;
		cmp16 (U, RDMEM16 (ea));
		CLK++;
		break;
	      case 0xbc:
		extended ();
		CLK += 6;
		cmp16 (S, RDMEM16 (ea));
		CLK++;
		break;
	      default:
	        sim_error ("invalid opcode (2) at %X\n", iPC);
		break;
	      }
	  }
	  break;

	case 0x12:
	  nop ();
	  break;
	case 0x13:
	  sync ();
	  break;
#ifdef H6309
	case 0x14:		/* SEXW */
	  break;
#endif
	case 0x16:
	  long_bra ();
	  CLK += 5;
	  break;
	case 0x17:
	  long_bsr ();
	  break;
	case 0x19:
	  daa ();
	  break;
	case 0x1a:
	  orcc ();
	  break;
	case 0x1c:
	  andcc ();
	  break;
	case 0x1d:
	  sex ();
	  break;
	case 0x1e:
	  exg ();
	  break;
	case 0x1f:
	  tfr ();
	  break;

	case 0x20:
	  bra ();
	  CLK += 3;
	  break;
	case 0x21:
	  PC++;
	  CLK += 3;
	  break;
	case 0x22:
	  branch (cond_HI ());
	  break;
	case 0x23:
	  branch (cond_LS ());
	  break;
	case 0x24:
	  branch (cond_HS ());
	  break;
	case 0x25:
	  branch (cond_LO ());
	  break;
	case 0x26:
	  branch (cond_NE ());
	  break;
	case 0x27:
	  branch (cond_EQ ());
	  break;
	case 0x28:
	  branch (cond_VC ());
	  break;
	case 0x29:
	  branch (cond_VS ());
	  break;
	case 0x2a:
	  branch (cond_PL ());
	  break;
	case 0x2b:
	  branch (cond_MI ());
	  break;
	case 0x2c:
	  branch (cond_GE ());
	  break;
	case 0x2d:
	  branch (cond_LT ());
	  break;
	case 0x2e:
	  branch (cond_GT ());
	  break;
	case 0x2f:
	  branch (cond_LE ());
	  break;

	case 0x30:
	  indexed ();
	  Z = X = ea;
	  break;		/* LEAX indexed */
	case 0x31:
	  indexed ();
	  Z = Y = ea;
	  break;		/* LEAY indexed */
	case 0x32:
	  indexed ();
	  S = ea;
	  break;		/* LEAS indexed */
	case 0x33:
	  indexed ();
	  U = ea;
	  break;		/* LEAU indexed */
	case 0x34:
	  pshs ();
	  break;		/* PSHS implied */
	case 0x35:
	  puls ();
	  break;		/* PULS implied */
	case 0x36:
	  pshu ();
	  break;		/* PSHU implied */
	case 0x37:
	  pulu ();
	  break;		/* PULU implied */
	case 0x39:
	  rts ();
	  break;		/* RTS implied  */
	case 0x3a:
	  abx ();
	  break;		/* ABX implied  */
	case 0x3b:
	  rti ();
	  break;		/* RTI implied  */
	case 0x3c:
	  cwai ();
	  break;		/* CWAI implied */
	case 0x3d:
	  mul ();
	  break;		/* MUL implied  */
	case 0x3f:
	  swi ();
	  break;		/* SWI implied  */

	case 0x40:
	  A = neg (A);
	  break;		/* NEGA implied */
	case 0x43:
	  A = com (A);
	  break;		/* COMA implied */
	case 0x44:
	  A = lsr (A);
	  break;		/* LSRA implied */
	case 0x46:
	  A = ror (A);
	  break;		/* RORA implied */
	case 0x47:
	  A = asr (A);
	  break;		/* ASRA implied */
	case 0x48:
	  A = asl (A);
	  break;		/* ASLA implied */
	case 0x49:
	  A = rol (A);
	  break;		/* ROLA implied */
	case 0x4a:
	  A = dec (A);
	  break;		/* DECA implied */
	case 0x4c:
	  A = inc (A);
	  break;		/* INCA implied */
	case 0x4d:
	  tst (A);
	  break;		/* TSTA implied */
	case 0x4f:
	  A = clr (A);
	  break;		/* CLRA implied */

	case 0x50:
	  B = neg (B);
	  break;		/* NEGB implied */
	case 0x53:
	  B = com (B);
	  break;		/* COMB implied */
	case 0x54:
	  B = lsr (B);
	  break;		/* LSRB implied */
	case 0x56:
	  B = ror (B);
	  break;		/* RORB implied */
	case 0x57:
	  B = asr (B);
	  break;		/* ASRB implied */
	case 0x58:
	  B = asl (B);
	  break;		/* ASLB implied */
	case 0x59:
	  B = rol (B);
	  break;		/* ROLB implied */
	case 0x5a:
	  B = dec (B);
	  break;		/* DECB implied */
	case 0x5c:
	  B = inc (B);
	  break;		/* INCB implied */
	case 0x5d:
	  tst (B);
	  break;		/* TSTB implied */
	case 0x5f:
	  B = clr (B);
	  break;		/* CLRB implied */
	case 0x60:
	  indexed ();
	  WRMEM (ea, neg (RDMEM (ea)));
	  break;		/* NEG indexed */
#ifdef H6309
	case 0x61:		/* OIM indexed */
	  break;
	case 0x62:		/* AIM indexed */
	  break;
#endif
	case 0x63:
	  indexed ();
	  WRMEM (ea, com (RDMEM (ea)));
	  break;		/* COM indexed */
	case 0x64:
	  indexed ();
	  WRMEM (ea, lsr (RDMEM (ea)));
	  break;		/* LSR indexed */
#ifdef H6309
	case 0x65:		/* EIM indexed */
	  break;
#endif
	case 0x66:
	  indexed ();
	  WRMEM (ea, ror (RDMEM (ea)));
	  break;		/* ROR indexed */
	case 0x67:
	  indexed ();
	  WRMEM (ea, asr (RDMEM (ea)));
	  break;		/* ASR indexed */
	case 0x68:
	  indexed ();
	  WRMEM (ea, asl (RDMEM (ea)));
	  break;		/* ASL indexed */
	case 0x69:
	  indexed ();
	  WRMEM (ea, rol (RDMEM (ea)));
	  break;		/* ROL indexed */
	case 0x6a:
	  indexed ();
	  WRMEM (ea, dec (RDMEM (ea)));
	  break;		/* DEC indexed */
#ifdef H6309
	case 0x6b:		/* TIM indexed */
	  break;
#endif
	case 0x6c:
	  indexed ();
	  WRMEM (ea, inc (RDMEM (ea)));
	  break;		/* INC indexed */
	case 0x6d:
	  indexed ();
	  tst (RDMEM (ea));
	  break;		/* TST indexed */
	case 0x6e:
	  indexed ();
	  CLK += 1;
	  PC = ea;
	  break;		/* JMP indexed */
	case 0x6f:
	  indexed ();
	  WRMEM (ea, clr (RDMEM (ea)));
	  break;		/* CLR indexed */
	case 0x70:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, neg (RDMEM (ea)));
	  break;		/* NEG extended */
#ifdef H6309
	case 0x71:		/* OIM extended */
	  break;
	case 0x72:		/* AIM extended */
	  break;
#endif
	case 0x73:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, com (RDMEM (ea)));
	  break;		/* COM extended */
	case 0x74:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, lsr (RDMEM (ea)));
	  break;		/* LSR extended */
#ifdef H6309
	case 0x75:		/* EIM extended */
	  break;
#endif
	case 0x76:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, ror (RDMEM (ea)));
	  break;		/* ROR extended */
	case 0x77:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, asr (RDMEM (ea)));
	  break;		/* ASR extended */
	case 0x78:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, asl (RDMEM (ea)));
	  break;		/* ASL extended */
	case 0x79:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, rol (RDMEM (ea)));
	  break;		/* ROL extended */
	case 0x7a:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, dec (RDMEM (ea)));
	  break;		/* DEC extended */
#ifdef H6309
	case 0x7b:		/* TIM indexed */
	  break;
#endif
	case 0x7c:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, inc (RDMEM (ea)));
	  break;		/* INC extended */
	case 0x7d:
	  extended ();
	  CLK += 5;
	  tst (RDMEM (ea));
	  break;		/* TST extended */
	case 0x7e:
	  extended ();
	  CLK += 4;
	  PC = ea;
	  break;		/* JMP extended */
	case 0x7f:
	  extended ();
	  CLK += 5;
	  WRMEM (ea, clr (RDMEM (ea)));
	  break;		/* CLR extended */
	case 0x80:
	  CLK += 2;
	  A = sub (A, imm_byte ());
	  break;
	case 0x81:
	  CLK += 2;
	  cmp (A, imm_byte ());
	  break;
	case 0x82:
	  CLK += 2;
	  A = sbc (A, imm_byte ());
	  break;
	case 0x83:
	  CLK += 4;
	  subd (imm_word ());
	  break;
	case 0x84:
	  CLK += 2;
	  A = and (A, imm_byte ());
	  break;
	case 0x85:
	  CLK += 2;
	  bit (A, imm_byte ());
	  break;
	case 0x86:
	  CLK += 2;
	  A = ld (imm_byte ());
	  break;
	case 0x88:
	  CLK += 2;
	  A = eor (A, imm_byte ());
	  break;
	case 0x89:
	  CLK += 2;
	  A = adc (A, imm_byte ());
	  break;
	case 0x8a:
	  CLK += 2;
	  A = or (A, imm_byte ());
	  break;
	case 0x8b:
	  CLK += 2;
	  A = add (A, imm_byte ());
	  break;
	case 0x8c:
	  CLK += 4;
	  cmp16 (X, imm_word ());
	  break;
	case 0x8d:
	  bsr ();
	  break;
	case 0x8e:
	  CLK += 3;
	  X = ld16 (imm_word ());
	  break;

	case 0x90:
	  direct ();
	  CLK += 4;
	  A = sub (A, RDMEM (ea));
	  break;
	case 0x91:
	  direct ();
	  CLK += 4;
	  cmp (A, RDMEM (ea));
	  break;
	case 0x92:
	  direct ();
	  CLK += 4;
	  A = sbc (A, RDMEM (ea));
	  break;
	case 0x93:
	  direct ();
	  CLK += 4;
	  subd (RDMEM16 (ea));
	  CLK++;
	  break;
	case 0x94:
	  direct ();
	  CLK += 4;
	  A = and (A, RDMEM (ea));
	  break;
	case 0x95:
	  direct ();
	  CLK += 4;
	  bit (A, RDMEM (ea));
	  break;
	case 0x96:
	  direct ();
	  CLK += 4;
	  A = ld (RDMEM (ea));
	  break;
	case 0x97:
	  direct ();
	  CLK += 4;
	  st (A);
	  break;
	case 0x98:
	  direct ();
	  CLK += 4;
	  A = eor (A, RDMEM (ea));
	  break;
	case 0x99:
	  direct ();
	  CLK += 4;
	  A = adc (A, RDMEM (ea));
	  break;
	case 0x9a:
	  direct ();
	  CLK += 4;
	  A = or (A, RDMEM (ea));
	  break;
	case 0x9b:
	  direct ();
	  CLK += 4;
	  A = add (A, RDMEM (ea));
	  break;
	case 0x9c:
	  direct ();
	  CLK += 4;
	  cmp16 (X, RDMEM16 (ea));
	  CLK++;
	  break;
	case 0x9d:
	  direct ();
	  CLK += 7;
	  jsr ();
	  break;
	case 0x9e:
	  direct ();
	  CLK += 4;
	  X = ld16 (RDMEM16 (ea));
	  break;
	case 0x9f:
	  direct ();
	  CLK += 4;
	  st16 (X);
	  break;

	case 0xa0:
	  indexed ();
	  A = sub (A, RDMEM (ea));
	  break;
	case 0xa1:
	  indexed ();
	  cmp (A, RDMEM (ea));
	  break;
	case 0xa2:
	  indexed ();
	  A = sbc (A, RDMEM (ea));
	  break;
	case 0xa3:
	  indexed ();
	  subd (RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xa4:
	  indexed ();
	  A = and (A, RDMEM (ea));
	  break;
	case 0xa5:
	  indexed ();
	  bit (A, RDMEM (ea));
	  break;
	case 0xa6:
	  indexed ();
	  A = ld (RDMEM (ea));
	  break;
	case 0xa7:
	  indexed ();
	  st (A);
	  break;
	case 0xa8:
	  indexed ();
	  A = eor (A, RDMEM (ea));
	  break;
	case 0xa9:
	  indexed ();
	  A = adc (A, RDMEM (ea));
	  break;
	case 0xaa:
	  indexed ();
	  A = or (A, RDMEM (ea));
	  break;
	case 0xab:
	  indexed ();
	  A = add (A, RDMEM (ea));
	  break;
	case 0xac:
	  indexed ();
	  cmp16 (X, RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xad:
	  indexed ();
	  CLK += 3;
	  jsr ();
	  break;
	case 0xae:
	  indexed ();
	  X = ld16 (RDMEM16 (ea));
	  break;
	case 0xaf:
	  indexed ();
	  st16 (X);
	  break;

	case 0xb0:
	  extended ();
	  CLK += 5;
	  A = sub (A, RDMEM (ea));
	  break;
	case 0xb1:
	  extended ();
	  CLK += 5;
	  cmp (A, RDMEM (ea));
	  break;
	case 0xb2:
	  extended ();
	  CLK += 5;
	  A = sbc (A, RDMEM (ea));
	  break;
	case 0xb3:
	  extended ();
	  CLK += 5;
	  subd (RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xb4:
	  extended ();
	  CLK += 5;
	  A = and (A, RDMEM (ea));
	  break;
	case 0xb5:
	  extended ();
	  CLK += 5;
	  bit (A, RDMEM (ea));
	  break;
	case 0xb6:
	  extended ();
	  CLK += 5;
	  A = ld (RDMEM (ea));
	  break;
	case 0xb7:
	  extended ();
	  CLK += 5;
	  st (A);
	  break;
	case 0xb8:
	  extended ();
	  CLK += 5;
	  A = eor (A, RDMEM (ea));
	  break;
	case 0xb9:
	  extended ();
	  CLK += 5;
	  A = adc (A, RDMEM (ea));
	  break;
	case 0xba:
	  extended ();
	  CLK += 5;
	  A = or (A, RDMEM (ea));
	  break;
	case 0xbb:
	  extended ();
	  CLK += 5;
	  A = add (A, RDMEM (ea));
	  break;
	case 0xbc:
	  extended ();
	  CLK += 5;
	  cmp16 (X, RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xbd:
	  extended ();
	  CLK += 8;
	  jsr ();
	  break;
	case 0xbe:
	  extended ();
	  CLK += 5;
	  X = ld16 (RDMEM16 (ea));
	  break;
	case 0xbf:
	  extended ();
	  CLK += 5;
	  st16 (X);
	  break;

	case 0xc0:
	  CLK += 2;
	  B = sub (B, imm_byte ());
	  break;
	case 0xc1:
	  CLK += 2;
	  cmp (B, imm_byte ());
	  break;
	case 0xc2:
	  CLK += 2;
	  B = sbc (B, imm_byte ());
	  break;
	case 0xc3:
	  CLK += 4;
	  addd (imm_word ());
	  break;
	case 0xc4:
	  CLK += 2;
	  B = and (B, imm_byte ());
	  break;
	case 0xc5:
	  CLK += 2;
	  bit (B, imm_byte ());
	  break;
	case 0xc6:
	  CLK += 2;
	  B = ld (imm_byte ());
	  break;
	case 0xc8:
	  CLK += 2;
	  B = eor (B, imm_byte ());
	  break;
	case 0xc9:
	  CLK += 2;
	  B = adc (B, imm_byte ());
	  break;
	case 0xca:
	  CLK += 2;
	  B = or (B, imm_byte ());
	  break;
	case 0xcb:
	  CLK += 2;
	  B = add (B, imm_byte ());
	  break;
	case 0xcc:
	  CLK += 3;
	  ldd (imm_word ());
	  break;
#ifdef H6309
	case 0xcd:		/* LDQ immed */
	  break;
#endif
	case 0xce:
	  CLK += 3;
	  U = ld16 (imm_word ());
	  break;

	case 0xd0:
	  direct ();
	  CLK += 4;
	  B = sub (B, RDMEM (ea));
	  break;
	case 0xd1:
	  direct ();
	  CLK += 4;
	  cmp (B, RDMEM (ea));
	  break;
	case 0xd2:
	  direct ();
	  CLK += 4;
	  B = sbc (B, RDMEM (ea));
	  break;
	case 0xd3:
	  direct ();
	  CLK += 4;
	  addd (RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xd4:
	  direct ();
	  CLK += 4;
	  B = and (B, RDMEM (ea));
	  break;
	case 0xd5:
	  direct ();
	  CLK += 4;
	  bit (B, RDMEM (ea));
	  break;
	case 0xd6:
	  direct ();
	  CLK += 4;
	  B = ld (RDMEM (ea));
	  break;
	case 0xd7:
	  direct ();
	  CLK += 4;
	  st (B);
	  break;
	case 0xd8:
	  direct ();
	  CLK += 4;
	  B = eor (B, RDMEM (ea));
	  break;
	case 0xd9:
	  direct ();
	  CLK += 4;
	  B = adc (B, RDMEM (ea));
	  break;
	case 0xda:
	  direct ();
	  CLK += 4;
	  B = or (B, RDMEM (ea));
	  break;
	case 0xdb:
	  direct ();
	  CLK += 4;
	  B = add (B, RDMEM (ea));
	  break;
	case 0xdc:
	  direct ();
	  CLK += 4;
	  ldd (RDMEM16 (ea));
	  break;
	case 0xdd:
	  direct ();
	  CLK += 4;
	  std ();
	  break;
	case 0xde:
	  direct ();
	  CLK += 4;
	  U = ld16 (RDMEM16 (ea));
	  break;
	case 0xdf:
	  direct ();
	  CLK += 4;
	  st16 (U);
	  break;

	case 0xe0:
	  indexed ();
	  B = sub (B, RDMEM (ea));
	  break;
	case 0xe1:
	  indexed ();
	  cmp (B, RDMEM (ea));
	  break;
	case 0xe2:
	  indexed ();
	  B = sbc (B, RDMEM (ea));
	  break;
	case 0xe3:
	  indexed ();
	  addd (RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xe4:
	  indexed ();
	  B = and (B, RDMEM (ea));
	  break;
	case 0xe5:
	  indexed ();
	  bit (B, RDMEM (ea));
	  break;
	case 0xe6:
	  indexed ();
	  B = ld (RDMEM (ea));
	  break;
	case 0xe7:
	  indexed ();
	  st (B);
	  break;
	case 0xe8:
	  indexed ();
	  B = eor (B, RDMEM (ea));
	  break;
	case 0xe9:
	  indexed ();
	  B = adc (B, RDMEM (ea));
	  break;
	case 0xea:
	  indexed ();
	  B = or (B, RDMEM (ea));
	  break;
	case 0xeb:
	  indexed ();
	  B = add (B, RDMEM (ea));
	  break;
	case 0xec:
	  indexed ();
	  ldd (RDMEM16 (ea));
	  break;
	case 0xed:
	  indexed ();
	  std ();
	  break;
	case 0xee:
	  indexed ();
	  U = ld16 (RDMEM16 (ea));
	  break;
	case 0xef:
	  indexed ();
	  st16 (U);
	  break;

	case 0xf0:
	  extended ();
	  CLK += 5;
	  B = sub (B, RDMEM (ea));
	  break;
	case 0xf1:
	  extended ();
	  CLK += 5;
	  cmp (B, RDMEM (ea));
	  break;
	case 0xf2:
	  extended ();
	  CLK += 5;
	  B = sbc (B, RDMEM (ea));
	  break;
	case 0xf3:
	  extended ();
	  CLK += 5;
	  addd (RDMEM16 (ea));
	  CLK++;
	  break;
	case 0xf4:
	  extended ();
	  CLK += 5;
	  B = and (B, RDMEM (ea));
	  break;
	case 0xf5:
	  extended ();
	  CLK += 5;
	  bit (B, RDMEM (ea));
	  break;
	case 0xf6:
	  extended ();
	  CLK += 5;
	  B = ld (RDMEM (ea));
	  break;
	case 0xf7:
	  extended ();
	  CLK += 5;
	  st (B);
	  break;
	case 0xf8:
	  extended ();
	  CLK += 5;
	  B = eor (B, RDMEM (ea));
	  break;
	case 0xf9:
	  extended ();
	  CLK += 5;
	  B = adc (B, RDMEM (ea));
	  break;
	case 0xfa:
	  extended ();
	  CLK += 5;
	  B = or (B, RDMEM (ea));
	  break;
	case 0xfb:
	  extended ();
	  CLK += 5;
	  B = add (B, RDMEM (ea));
	  break;
	case 0xfc:
	  extended ();
	  CLK += 5;
	  ldd (RDMEM16 (ea));
	  break;
	case 0xfd:
	  extended ();
	  CLK += 5;
	  std ();
	  break;
	case 0xfe:
	  extended ();
	  CLK += 5;
	  U = ld16 (RDMEM16 (ea));
	  break;
	case 0xff:
	  extended ();
	  CLK += 5;
	  st16 (U);
	  break;

	default:
	  CLK += 2;
          sim_error ("invalid opcode '%02X'\n", opcode);
          //PC = iPC;
	  break;
	}

	if (cc_changed)
		cc_modified ();
    }
  while (1 /*CLK > 0*/);

cpu_exit:
   return;
}

void
cpu6809_reset (void)
{
  X = Y = S = U = A = B = DP = 0;
  H = N = OV = C = 0;
  Z = 1;
  EFI = F_FLAG | I_FLAG;
#ifdef H6309
  MD = E = F = V = 0;
#endif

  change_pc (read16 (0xfffe));
}
