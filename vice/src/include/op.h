/*
 * $Id: op.h,v 1.8 1997/05/22 21:28:44 ettore Exp $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice
 *
 * This file contains declarations for MOS6502 functions.
 *
 * Written by
 *   Vesa-Matti Puro (vmp@lut.fi)
 *   Jarkko Sonninen (sonninen@lut.fi)
 *   Jouko Valta (jopi@stekt.oulu.fi)
 *
 * $Log: op.h,v $
 * Revision 1.8  1997/05/22 21:28:44  ettore
 * *** empty log message ***
 *
 * Revision 1.7  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Prototypes.
 *
 * Revision 1.6  1994/08/10  18:34:13  jopi
 * More changeability
 *
 * Revision 1.5  1994/06/16  17:19:26  jopi
 * Code reorganized and cleaned up
 *
 * Revision 1.4  1993/11/10  01:55:34  jopi
 *
 * Revision 1.3  93/06/21  13:38:56  jopi
 *  X64 version 0.2 PL 0
 *
 * Revision 1.2  1993/06/13  08:21:50  sonninen
 * *** empty log message ***
 *
 *
 */

#if defined (__STDC__)
#define FUNC(x) unsigned int x(unsigned int)
#else
#define FUNC(x) unsigned int x()
#endif

FUNC(load_reg);
FUNC(store);
FUNC(inc);
FUNC(dec);
FUNC(setf);
FUNC(clrf);
FUNC(ora);
FUNC(and);
FUNC(eor);
FUNC(adc);
FUNC(sbc);
FUNC(cmp);
FUNC(cpy);
FUNC(cpx);
FUNC(bit);
FUNC(asl);
FUNC(lsr);
FUNC(rol);
FUNC(ror);
FUNC(nop);
FUNC(bcc);
FUNC(bcs);
FUNC(bne);
FUNC(beq);
FUNC(bpl);
FUNC(bmi);
FUNC(bvc);
FUNC(bvs);
FUNC(push);
FUNC(pull);
FUNC(plp);
FUNC(jsr);
FUNC(rts);
FUNC(rti);
FUNC(brki);
FUNC(u_anc);

FUNC(noop);
FUNC(forb);

#ifndef NO_UNDOC_CMDS
#define u_and    and
#define u_asr    lsr
#define u_load   load_reg
#define u_store  store
#define u_nop    nop
#define u_ror    ror
#define u_sbc    sbc

FUNC(u_decp);
FUNC(u_isbc);
FUNC(u_sbx);
FUNC(u_rlan);
FUNC(u_rrad);
FUNC(u_slor);
FUNC(u_sreo);
#else

#define u_and    noop
#define u_asr    noop
#define u_decp   noop
#define u_isbc   noop
#define u_load   noop
#define u_nop    noop
#define u_rlan   noop
#define u_ror    noop
#define u_rrad   noop
#define u_sbc    noop
#define u_sbx    noop
#define u_slor   noop
#define u_sreo   noop
#define u_store  noop
#endif

#define u_undoc  noop
