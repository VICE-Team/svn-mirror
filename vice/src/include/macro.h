/*
 * $Id: macro.h,v 1.12 1997/05/22 21:24:09 ettore Exp $
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice
 *
 * Written by
 *   Vesa-Matti Puro (vmp@lut.fi)
 *   Jarkko Sonninen (sonninen@lut.fi)
 *
 * $Log: macro.h,v $
 * Revision 1.12  1997/05/22 21:24:09  ettore
 * Code cleaned up and memory access macros moved to maincpu.h.
 *
 * Revision 1.11  1996/07/29 21:18:05  ettore
 * New memory access.
 *
 * Revision 1.10  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Typedef for signed char.
 *
 * Revision 1.9  1994/12/12  16:59:44  jopi
 * *** empty log message ***
 *
 * Revision 1.8  1994/08/10  18:34:13  jopi
 * More changeability
 *
 * Revision 1.7  1994/06/16  17:19:26  jopi
 * Code reorganized and cleaned up
 *
 * Revision 1.6  1994/06/07  22:34:40  jopi
 * Patchlevel 2
 *
 * Revision 1.5  1994/02/18  16:10:57  jopi
 * New debugger & arg parser and x128
 *
 * Revision 1.4  1993/11/10  01:55:34  jopi
 * REL_ADDR macro and 1541 made more portable
 *
 * Revision 1.3  93/06/21  13:38:33  jopi
 *  X64 version 0.2 PL 0
 *
 * Revision 1.2  1993/06/13  08:21:50  sonninen
 * *** empty log message ***
 *
 *
 */

#ifndef X64_MACRO_H
#define X64_MACRO_H


#include "types.h"

/* FIXME: This should all go into maincpu.h. */

#define PUSH(b) 		ram[0x100 + SP]=(b),SP--
#define PULL()		        ram[0x100 + (++SP)]

#define UPPER(ad)		(((ad)>>8)&0xff)
#define LOWER(ad)		((ad)&0xff)
#define LOHI(lo,hi)             ((lo)|((hi)<<8))

#define REL_ADDR(pc,src) 	(pc+((SIGNED_CHAR)src))

#define SET_SIGN(a)		(SF=(a)&S_SIGN)
#define SET_ZERO(a)		(ZF=!(a))
#define SET_CARRY(a)  		(CF=(a))

#define SET_INTERRUPT(a)	(IF=(a))
#define SET_DECIMAL(a)		(DF=(a))
#define SET_OVERFLOW(a)		(OF=(a))
#define SET_BREAK(a)		(BF=(a))

#define SET_SR(a)		(SF=(a) & S_SIGN,\
				 ZF=(a) & S_ZERO,\
				 CF=(a) & S_CARRY,\
				 IF=(a) & S_INTERRUPT,\
				 DF=(a) & S_DECIMAL,\
				 OF=(a) & S_OVERFLOW,\
				 BF=(a) & S_BREAK)

#define GET_SR()		((SF ? S_SIGN : 0) |\
				 (ZF ? S_ZERO : 0) |\
				 (CF ? S_CARRY : 0) |\
				 (IF ? S_INTERRUPT : 0) |\
				 (DF ? S_DECIMAL : 0) |\
				 (OF ? S_OVERFLOW : 0) |\
				 (BF ? S_BREAK : 0) | S_UNUSED)

#define IF_SIGN()		SF
#define IF_ZERO()		ZF
#define IF_CARRY()		CF
#define IF_INTERRUPT()		IF
#define IF_DECIMAL()		DF
#define IF_OVERFLOW()		OF
#define IF_BREAK()		BF


#define sprint_status()	 sprint_binary(GET_SR())


#endif  /* X64_MACRO_H */
