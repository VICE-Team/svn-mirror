/*
 * mon.c - Built-in monitor for VICE.
 *
 * Written by
 *  Vesa-Matti Puro  (vmp@lut.fi)
 *  Jarkko Sonninen  (sonninen@lut.fi)
 *  Jouko Valta      (jopi@stekt.oulu.fi)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Patches by
 *  Frank Prindle    (Frank.Prindle@lambada.oit.unc.edu)  /FCP/
 *  Teemu Rantanen   (tvr@cs.hut.fi) /TVR/
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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "asm.h"
#include "misc.h"
#include "mshell.h"
#include "maincpu.h"
#include "mem.h"
#include "vmachine.h"
#include "serial.h"
#include "macro.h"
#include "interrupt.h"
#include "ui.h"
#include "video.h"
#include "file.h"
#include "charsets.h"
#include "traps.h"
#include "resources.h"

#if defined (C128)
#include "vicii.h"
#include "vdc.h"
#endif

#ifdef GEMDOS
typedef unsigned	size_t;
#endif

#define MAXADDR		(ADDRESS)65535
#ifdef PET
#define	MAXBANKADDR	MAXADDR
#else
#define MAXBANKADDR	(RAM_SIZE-1)  /* (11 occurrences) */
#endif

#ifdef C128
#define MASKVAL		0x3ffff	/* >= MAXBANKADDR  (used once) */
#else
#define MASKVAL		0x1ffff	/* bank 1 == ram0 */
#endif


/* Dump/Undump return values */

#define BAD_CORE_FORMAT		-1
#define BAD_CORE_IO		-2
#define BAD_CORE_CPU		-3


/* extern */

extern void iowr(ADDRESS, BYTE);
extern BYTE iord(ADDRESS);
/*extern int floppy_hexdump_block(int, int, int, int);*/


#ifdef VICE_DUMP
int     dump ( char *name );
int     undump ( char *name );
#endif


#ifdef HAS_ZILOG_Z80
#include "cpm.h"
#include "z80/Z80.h"

extern reg  *get_z80_regs (void);
extern void  put_z80_regs (reg *r);
extern int   disassemble_line ( int counter, BYTE *inbuf );

static int  mon_zdis ( void );
static int  mon_zreg ( void );
static int  mon_zcall ( void );
static int  mon_z80_showreg (void);
#endif


/*
 *  Machine Language Monitor
 */

static int    mode;
static ADDRESS  address;	/* Default place to put code in Commodore 64. */

static char  *args[MAXARG+1];
static int    values[MAXARG+1];
static int    types[MAXARG+1];
static int    nargs;


void    mon( ADDRESS adr );

static BYTE get_mem ( ADDRESS adr );
static int  put_mem ( ADDRESS adr, BYTE *byte, int count );
static void pack_args( int start );
static int  mon_fill ( void );
static int  mon_hunt ( void );
static int  mon_compare ( void );
static int  mon_move ( void );
static int  mon_number ( void );
static int  mon_asm ( void );
static int  mon_disassemble ( void );
static int  mon_memdump ( void );
static int  mon_bitdump ( void );
static int  mon_memstore ( void );
static int  mon_bitstore ( void );

/* Basic */

#ifdef CBM64
static int  mon_type ( void );
static int  mon_screen ( void );
#endif

/* Debugger */

static int  mon_jump ( void );
static void mon_times( int set );
#ifdef TRACE
static int  mon_trace( void );
#endif
#if 0
static int  mon_step( void );
#endif
static int  mon_setreg ( void );
static int  mon_showreg ( void );
static int  mon_up ( void );
static int  mon_down ( void );
#if 0
static int  mon_set ( void );
static int  mon_unset ( void );
#endif
static int  mon_help ( void );
static int  mon_help_mnem( char *line );
#ifdef NEW_TIMER_CODE
static int  mon_break ( void );
static int  mon_clear ( void );
#endif
#ifdef  TRAP_WRITES
static int  mon_trap ( void );
#endif
static int  mon_chdir ( void );
static int  mon_save ( void );
static int  mon_load ( void );
#ifdef VICE_DUMP
static int  mon_dump ( void );
static int  mon_undump ( void );
#endif
static int  mon_attach ( void );
static int  mon_quit ( void );
static int  mon_exit ( void );

/* ------------------------------------------------------------------------- */

int runflg = 0;

struct ms_table mon_cmds[] = {
    {"fill", 3, MAXARG, mon_fill,
    "fill    start end|+len bytes|'string'\n"},
    {"hunt", 3, MAXARG, mon_hunt,
    "hunt    start end|+len bytes|'string'\n"},
    {"compare", 3, 3, mon_compare,
    "compare  start end|+len target\n"},
    {"transfer", 3, 3, mon_move,
    "transfer start  end|+len  dest\t(move memory area)\n"},
    {"number", 3, 5, mon_number,
    "number  start  end|+len  orig.st [orig.end [disp]]\t(relink code)\n"},
    {"asm", 0, 1, mon_asm,
    "asm \t [addr]\t\t\t\t(assemble)\n"},
    {"disassemble", 0, 2, mon_disassemble,
    "disassemble [addr] [end|+len]\n"},
    {"memory", 0, 2, mon_memdump,
    "memory  [addr] [end|+len]\t\t(show memory in hex format)\n"},
    {"image", 0, 2, mon_bitdump,
    "image   [addr] [end|+len]\t\t(show memory in bit format)\n"},
    {">", 1, MAXARG, mon_memstore,
    ">\t  addr  bytes|'string'\t\t(write data to memory)\n"},
    {":", 1, MAXARG, mon_bitstore,
    ":\t  addr  bytes|'string'\t\t(write image data to memory)\n"},

    /* debugger */
    {"jump", 0, 6, mon_jump,
    "jump\t [addr [registers]]\n"},
    {"call", 0, 1, mon_jump,
    "call\t [addr]\t\t\t\t(run code at address specified)\n"},
#if 0
    {"time", 0, 6, mon_jump,
    "time\t [addr]\t\t\t\t(time code until next BRK)\n"},
#endif
    {"registers", 0, 6, mon_setreg,
    "registers [register list]\t\t(set PC AC AX YR PF SP)\n"},
    {";", 0, 6, mon_setreg,
    ";\t\t\t\t\t(same as registers)\n"},
#ifdef TRACE
    {"trace", 0, 1, mon_trace,
    "trace\t [0|1]\t\t\t\t(switch debug mode)\n"},
#endif
    /*{"ss", 0, 1, mon_step,
    "ss\t\t\t\t\t(single step program)\n"},*/
#ifdef NEW_TIMER_CODE
    {"break", 0, 2, mon_break,
     "break  [start] [end]\t\t\t(set up breakpoints (region))\n"},
    {"clear", 0, 2, mon_clear,
     "clear   start [end]\t\t\t(clear breakpoints (region))\n"},
#endif
#ifdef  TRAP_WRITES
    {"trap", 0, 2, mon_trap,
     "trap   [start] [end]\t\t\t(set up read/write traps (region))\n"},
#endif
    {"down", 0, 1, mon_down,
    "down\t [levels]\t\t\t(move down on stack)\n"},
    {"up", 0, 1, mon_up,
    "up\t [levels]\n"},

    /* Zilog Z80 */

#ifdef HAS_ZILOG_Z80
    {"zdis", 0, 2, mon_zdis,
    "zdis\t [addr] [end|+len]\n"},
    {"zasm", 0, 1, mon_asm,		/* Note: same function. */
     "zasm\t [addr]\t\t\t\t(enter to Z80 line interpreter)\n"},
    {"zcall", 0, 1, mon_zcall,
     "zcall\t [addr]\t\t\t\t(run Z80 code at address specified)\n"},
    {"zreg", 0, 9, mon_zreg,
     "zreg\t [register list]\t\t(show Z80 registers)\n"},
#endif

    {"cd", 1, 1, mon_chdir,
    "cd\t path\n"},
    {"save", 3, 3, mon_save,
    "save\t filename  addr  end|+len\t(work dir)\n"},
    {"load", 1, 2, mon_load,
    "load\t filename [addr]\t\t(work dir)\n"},
#ifdef VICE_DUMP
    {"dump", 0, 1, mon_dump, /*FCP*/
    "dump\t [filename]\t\t\t(save freezed program and state)\n"},
    {"undump", 0, 1, mon_undump, /*FCP*/
    "undump [filename]\t\t\t(load freezed ram image from file)\n"},
#endif
    {"attach", 0, 2, mon_attach, /*TVR*/
     "attach [#dev] [imagename]\t\t(attach image for use)\n"}, /*TVR*/
#if 0
    {"block", 0, 4, mon_sectordump,
    "block [drive:] [track] [sector] [disp] (show disk blocks in hex format)\n"},
#endif

#if 0
    {"set", 0, 2, mon_set,
    "set\t [variable [value]]\t\t(set system variable)\n"},
    {"unset", 0, 1, mon_unset,
    "unset\t [variable [value]]\t\t(clear system variable)\n"},
#endif

    {"help", 0, 1, mon_help,
    "help\t [topic]\t\t\t(describe command or mnemonic)\n"},
    {"?", 0, 1, mon_help,
    "?\t [topic]\t\t\t(same as help)\n"},
    {"x", 0, 0, mon_exit,
    "x\t\t\t\t\t(exit monitor)\n"},
    {"quit", 0, 0, mon_quit,
    "quit\t\t\t\t\t(exits the emulator)\n"},

    /* basic -- lowest priority commands */
#ifdef CBM64
    {"type", 0, MAXARG, mon_type,
    "type\t  [bytes|'string']\t\t(manipulate basic memory)\n"},
    {"screen", 0, 1, mon_screen,
    "screen  [n|addr]\t\t\t(display video memory)\n"},
#endif

    {NULL, 0, 0, 0, NULL}
};


/*
 * Functions for reading data from memory and writing back.
 * Any MMU memory configurations are ignored, if address > FFFF.
 */

static BYTE get_mem(ADDRESS adr)
{
    /* switch (bank) {
       case 0: */
    if (adr <= 0xffff)		/* Computer's own memory configuration */
	return (LOAD((ADDRESS)(adr & MAXADDR)) );
    else
	return (ram[adr & MAXBANKADDR]);	/* Ignore MMU */
    /* case 1:
       default:
       }
     */
}


static int  put_mem(ADDRESS adr, BYTE *byte, int count)
{
    /* switch (bank) {
       case 0: */

    if (adr <= 0xffff)		/* Computer's own memory configuration */
      for (; count-- > 0; adr++, byte++)
	{ STORE((ADDRESS)(adr & MAXADDR), *byte); }

    else
	memcpy(ram + (adr & MAXBANKADDR), byte, count); /* Ignore MMU */

    /* case 1:
       default:
       }
     */

   return (0);
}


#if 0
int     dump(char *name)
{
    ADDRESS  a;
    FILE   *fp;

    if (NULL == (fp = fopen(name, WRITE)) ||
	1 != fwrite((char *) ram, (size_t)RAM_SIZE, 1, fp))
	return (0);

    for (a = 0xd000; ((a < 0xe000) && (1 == fprintf(fp, "%c", iord(a)))); a++);

    fprintf(fp, "%c%c%c%c%c%c%c", LOWER(PC),UPPER(PC),
	(int) AC, (int) XR, (int) YR, GET_SR(), SP);

    (void) fclose(fp);

    return (0);
}


/* Handle the actual Core resume. */

int     undump(char *name)
{
    ADDRESS  a;
    int     i;
    BYTE    buf[8];
    FILE   *fp;

    /* FIXME: Should use magic byte here...  */

    if (NULL == (fp = fopen(name, READ)) ||
	1 != fread((char *) ram, (size_t)RAM_SIZE, 1, fp)) {
	perror(name);
	return (BAD_CORE_FORMAT);
    }

    for (a = 0xd000; a < 0xe000; a++) {
	if ((i= getc(fp)) == EOF) {
	    /*printf("IO status read failed %04X\n", a);*/
	    (void) fclose(fp);
	    return (0);
	}
	iowr(a, (BYTE) i);
    }

    /* FIXME: Should use magic byte here...  */

    if (1 == fread(buf, 7, 1, fp)) {
	PC = buf[0];
	PC |= (buf[1] <<8);
	AC = buf[2];
	XR = buf[3];
	YR = buf[4];
	SET_SR(buf[5]);
	SP = buf[6];

	printf("Restoring main CPU...\n");
	mon_showreg();
    } else {
	printf("Main CPU status read failed\n");
	(void) fclose(fp);
	return BAD_CORE_CPU;
    }

    (void) fclose(fp);

    return (0);
}
#endif

/* ------------------------------------------------------------------------- */

/*
 * Memory manipulation
 */


/* function packs arguments after 'start' to single piece of data */

static void pack_args(int start)
{
    int     i;
    static char buf[256];
    char   *p = buf;

    for (i = start; i < nargs; i++) {
	if (types[i] == T_QUOTED) {
	    memcpy(p, args[i], values[i]);
	    p += values[i];
	} else
	    *p++ = values[i];
    }
    *p = '\0';
    types[start] = T_QUOTED;
    args[start] = buf;
    values[start] = (int) p - (int) buf;  /* warning: size */

    /* return values[start]; */
}


static int  mon_fill(void)
{
    ADDRESS  adr, end = values[2]-1;
    pack_args(3);

    if (values[3] > 0)
	for (adr = values[1]; adr <= end; adr += values[3])
	    put_mem(adr, (BYTE *)args[3], values[3]);
    return (0);
}


static int  mon_hunt(void)
{
    ADDRESS  adr = values[1], end = values[2];

    pack_args(3);
    if (values[3] > 0)
	for (; adr <= end;) {
	    if (!memcmp(ram + adr, args[3], values[3])) {
		printf("%04X\t", adr);
		adr += values[3];
	    } else
		adr++;
	}
    printf("\n");
    return (0);
}


static int  mon_compare(void)
{
    ADDRESS  adr = values[1], end = values[2], trg = values[3];

    for (; adr <= end; adr++, trg++)
	if (get_mem(adr) != get_mem(trg))
	    printf("%04X\t", adr);
    printf("\n");
    return (0);
}


static int  mon_move(void)
{
    ADDRESS  adr = values[1], end = values[2], trg = values[3];
    BYTE    c;

    if (adr > trg)
	for (; adr <= end; trg++, adr++) {
	    c = get_mem(adr);
	    put_mem(trg, &c, 1);
	}
    else if (adr < trg)
	for (trg += (end - adr); adr <= end; trg--, end-- ) {
	    c = get_mem(end);
	    put_mem(trg, &c, 1);
	}
    return (0);
}


static int  mon_number(void)
{
    ADDRESS  adr = values[1], end =  values[2], origs =  values[3];
    ADDRESS  orige, val;
    int     c, disp;

    switch (nargs) {
      case 4:
	orige = end - adr + origs;
	disp  = adr - origs;
	break;

      case 5:
	orige = values[4];
	disp  = adr - origs;
	break;

      case 6:
	orige = values[4];
	disp  = values[5];

      default:
	return 0;
    }
    origs &= 0xffff;		/* Keep within bank */
    orige &= 0xffff;

    printf("Numbering %04X-%04X: %04X-%04X -> %04X-%04X\n",
	   adr, end, origs, orige,
	   (origs+disp) & MAXBANKADDR, (orige+disp) & MAXBANKADDR);

    while (adr <= end) {
	if ((c= clength[lookup[ram[adr]].addr_mode]) == 3 &&
	    (val = (ram[adr+2]<<8)+ram[adr+1]) >= origs && val <= orige) {
	    val += disp;
	    ram[adr+1] = (val & 0xff);
	    ram[adr+2] = (val>>8 & 0xff);
	    printf(". %04X\t%s\n", adr, sprint_opcode(adr, 1));
	}
	adr += c;
    }

    return (0);
}


static int  mon_asm(void)
{
    int asmode = mode;


#ifdef HAS_ZILOG_Z80
    if (*args[0] == 'z')
	asmode |= MODE_ZILOG;
#endif

    if (nargs > 1)
	ass(values[1], asmode);
    else
	ass(address, asmode);
    return (0);
}


static int  mon_disassemble(void)
{
    ADDRESS  p, start = values[1], end =  values[2];
    BYTE    op, p1, p2;
    int     cnt;

    switch (nargs) {
      case 1:
	start = address;
	/* Drop through.  */

      case 2:
	for (cnt = 20, p = start; cnt-- > 0;) {
	    op = get_mem(p);
	    p1 = get_mem(p + 1);
	    p2 = get_mem(p + 2);
	    printf(". %04X\t%s\n",
		   p, sprint_disassembled(p & MAXADDR, op, p1, p2, mode));
	    p += clength[lookup[get_mem(p)].addr_mode];
	}
	break;

      case 3:
	for (p = start; p <= end && p >= start;) {
	    op = get_mem(p);
	    p1 = get_mem(p + 1);
	    p2 = get_mem(p + 2);
	    printf((mode & MODE_HEX) ? ". %04X\t%s\n" : ". %05d\t%s\n",
		   p, sprint_disassembled(p & MAXADDR, op, p1, p2, mode));
	    p += clength[lookup[get_mem(p)].addr_mode];
	}

      default:
	/* (cannot happen) */
	fprintf(stderr, "Wrong number of arguments!\n");
	return -1;
    }

    address = p & MAXBANKADDR;
    return (0);
}


static int  mon_memdump(void)
{
    ADDRESS  p, start = values[1], end = values[2];
    BYTE    c, arg[17];
    int     cnt;

    switch (nargs) {
      case 1:
	start = address;
	/* Drop through.  */

      case 2:
	end = start + 254;
	break;
    }

    arg[16] = 0;

    for (p = start; p < end && p >= start;) {
	printf((mode & MODE_HEX) ? "> %04X " : "> %05d ", p);

	for (cnt = 0; cnt < 16; cnt++, p++) {
	    c = get_mem(p);
	    printf(" %02X", c);

	    arg[cnt] = p_toascii (c, 1);
	}
	printf(" ;%s\n", arg);
    }

    address = p & MAXBANKADDR;
    return (0);
}


static int  mon_bitdump(void)
{
    ADDRESS  adr = values[1], end = values[2];
    char    arg[17];
    int     cnt;

    switch (nargs) {
      case 1:
	adr = address;

      case 2:
	end = adr + 60;
	break;
    }

    arg[3] = 0;

    while (adr <= end) {
	unsigned int n, val, mask;
	printf((mode & MODE_HEX) ? ": %04X " : ": %05d ", adr);
	for (cnt = 0; cnt < 3; cnt++, adr++) {
	    val = get_mem(adr&0xffff) & 0xff;
	    for (n = 8, mask = 0x80; n--; mask >>= 1)
		printf((val & mask) ? "*" : "-");
	}
	printf("\n");
	if ((adr & 0x3f) == 0x3f)
	    adr++;	/* Skip the unused byte */
    }

    address = adr & MAXBANKADDR;
    return (0);
}


static int  mon_memstore(void)
{
    address = values[1] & MAXBANKADDR;
    pack_args(2);

    if (values[2] > 0)
	put_mem(address, (BYTE *)args[2], values[2]);

    address = (address + values[2]) & MAXBANKADDR;
    return (0);
}


static int  mon_bitstore(void)
{
    unsigned long val;
    int i, j, n=0;
    BYTE *c, buf[4];

    address = values[1];

    /* convert image string into binary */
    for (i = 2; i < nargs; i++) {
	if (types[i] != T_NUMBER) {
	    for (c = (BYTE *)args[i], n = 0; *c; c++, n++)
		*c = ((*c == '1' || *c == '*') ? '1' : '0');
	    val= strtol(args[i], NULL, 2);
	    n = (n + 7) / 8;		/* n bytes */

	    if (n > 3)
		printf("Argument too large.\n");
	    n &= 3;
	}
	else {
	    val = values[i];
	    n = ((val > 0xff) ? 2 : 1);		/* values <= FFFF expected */
	}

	for (c = (BYTE *)(buf + n), j = n; j-- > 0;) {
	    *--c = val & 0xff;
	    val >>= 8;
	}

	address = ((address + n) & MAXBANKADDR);
	put_mem(address, buf, n);
    }

    return (0);
}


/* ------------------------------------------------------------------------- */

/*
 * Z80 functions.
 * "zasm" is the same function as 6502 "asm".
 */

#ifdef HAS_ZILOG_Z80

static int  mon_zdis (void)
{
    ADDRESS  adr = values[1], end =  values[2];
    int     cnt;


    switch (nargs) {
      case 1:
	adr = address;

      case 2:
	cnt = 20;
	while (cnt-- > 0) {
	    adr += disassemble_line(adr, ((bios_rom && adr < BIOSROM_SIZE) ?
			  bios_rom + adr : ram + adr));
	}
	break;

      case 3:
	while (adr <= end) {
	    adr += disassemble_line(adr, ((bios_rom && adr < BIOSROM_SIZE) ?
			  bios_rom + adr : ram + adr));

	}
    }
    address = adr & MAXBANKADDR;
    return (0);
}


static int  mon_zreg (void)
{
    reg *r = get_z80_regs();    /* set registers if more than one parameter */

    switch (nargs) {
      case 9:
	r->SP.W = values[6];
      case 8:
	r->IY.W = values[4];
      case 7:
	r->IX.W = values[4];
      case 6:
	r->HL.W = values[4];
      case 5:
	r->DE.W = values[4];
      case 4:
	r->BC.W = values[3];
      case 3:
	r->AF.W = values[2];
      case 2:
	r->PC.W = address = values[1];
	put_z80_regs (r);
    }

    mon_z80_showreg();
    return (0);
}


static int  mon_z80_showreg(void)
{
    reg *r = get_z80_regs();

    printf("\n PC    AF    BC    DE    HL    IX    IY    SP    OPCODE\n");
    printf((mode & MODE_HEX) ?
	   "%04X  %04X  %04X  %04X  %04X  %04X  %04X  %04X   %02X  %s\n\n" :
	   "%05d %05d %05d %05d %05d %05d %05d %05d  %03d  %s\n\n",
	r->PC.W, r->AF.W, r->BC.W, r->DE.W, r->HL.W, r->IX.W, r->IY.W, r->SP.W,
	   M_RDMEM((ADDRESS)r->PC.W), "---" /*sprint_opcode(PC, mode & MODE_HEX)*/);
    return (0);
}


/*    {"zcall", 0, 1, mon_zcall,
     "zcall\t [addr]\t\t\t\t(run Z80 code at address specified)\n"},
*/

static int  mon_zcall ( void )
{
    printf ("Not implemented.\n");
    return (0);
}

#endif


/* ------------------------------------------------------------------------- */

/*
 * File handling functions
 */

static int  mon_chdir(void)
{
    if (chdir(args[1]) < 0) {
	perror(args[1]);
    }

    return (0);
}


static int  mon_save(void)
{
    FILE   *fp;
    ADDRESS  adr = values[2];
    ADDRESS end = values[3];

    if (NULL == (fp = fopen(args[1], WRITE))) {
	perror(args[1]);
	printf("Saving failed.\n");
    } else {
	printf("Saving file `%s'...\n", args[1]);
	fputc((BYTE) adr & 0xff, fp);
	fputc((BYTE) (adr >> 8) & 0xff, fp);
	fwrite((char *) (ram + adr), 1, end - adr, fp);
	fclose(fp);
    }
    return (0);
}


static int  mon_load(void)
{
    FILE   *fp;
    ADDRESS  adr = values[2];
    char    flag = 0;
    int     b1, b2;
    int     ch;

    if (*args[0] == 'l')
	flag++;

    if (NULL == (fp = fopen(args[1], READ))) {
	perror(args[1]);
	printf("Loading failed.\n");
	return (0);
    }

    b1 = fgetc(fp);
    b2 = fgetc(fp);

    if (nargs < 3) {	/* No Load address given */
	if (b1 == 1)	/* Load Basic */
	    mem_get_basic_text(&adr, NULL);
	else
	    adr = LOHI ((BYTE)b1,(BYTE)b2);
    }

    printf((flag ? "Loading %s" : "Verifying %s"), args[1]);
    printf(" from %04X\n", adr);

    if (flag) {
	ch = fread (ram + adr, 1, RAM_SIZE - adr, fp);
	printf ("%x bytes\n", ch);

	/* set end of load addresses like kernal load */ /*FCP*/
	mem_set_basic_text(adr, adr + ch);
    }
    else {
	while ((ch = fgetc(fp)) != EOF)
	    if (ram[adr++] != ch)
		printf("%04X\t", adr - 1);
	printf("\n");
    }


    fclose(fp);
    return (0);
}


#ifdef VICE_DUMP
static int  mon_dump(void)
{
    if (nargs <= 1) /*FCP*/
	dump(create_name(app_resources.directory, app_resources.ramName));
    else
	dump(create_name(".", args[1]));

    printf("RAM image saved.\n");
    return (0);
}


static int  mon_undump(void)
{
    int err;

    if (nargs <= 1) /*FCP*/
	err = undump(create_name(app_resources.directory,
				 app_resources.ramName));
    else
	err = undump(create_name(".", args[1]));

    if (err)
	printf("Core loaded.\n");
    else
	printf("Core load failed.\n");

    return (0);
}
#endif

static int  mon_attach(void)
{
    char    *name = NULL;
    int      dev  = 8;

    switch (nargs) {
      case 2:
	name = args[1];
	break;
      case 3:
	dev  = values[1];
	name = args[2];
    }

    if (dev >= 8)
	serial_select_file(DT_DISK | DT_1541, dev, name);
    else
	serial_select_file(DT_PRINTER, dev, name);
    printf("\n");
    return (0);
}


/* ------------------------------------------------------------------------- */

/*
 * Basic input and output.
 * To list a basic prgram, use "petcat" with appropriate offset option.
 */

static void f_chrin ( void );

static FILE   *chrin_fp;

static trap_t chrin_trap = {
	"ChrIn",
#ifdef PET
	0x00,			/* FIXME: PET Traps not implemented */
	{0x0, 0x0, 0x0},
#else
#ifndef C128
#ifdef VIC20
	0xE5CF,
	{0xAC, 0x77, 0x02},	/* VIC20 Trap */
#else
	0xE5B4,
	{0xAC, 0x77, 0x02},	/* C64 Trap */
#endif  /* VIC20 */
#else
	0xC244,
	{0xAC, 0x4A, 0x03},	/* C128 Trap */
#endif
#endif  /* Not PET */
	f_chrin
};


/*
 * This function reads one byte from the file, pretending it came from
 * the keyboard buffer.
 */

static void f_chrin(void)
{
    int     b;

    if ((b = fgetc(chrin_fp)) == EOF) {
	printf ("Read complete.\n");
	remove_trap(&chrin_trap);
	fclose(chrin_fp);
    }

    if (b == 0x0a)
	b = 0x0d;
    else if ( ((b & 0xc0) == 0x40) && ((b & 0x1f) < 0x1e) )
	b ^= 0x20;

    AC = YR = (BYTE)b;
    SET_INTERRUPT(0);
    SET_CARRY(0);
    PC += 18;
}


#ifdef CBM64
static int  mon_type(void)
{
    BYTE   *p, *kbd, *cnt;
    int     n;

#ifdef PET
	kbd = ram + 0x026f;
	cnt = ram + 0x9e;
#else
	kbd = ram + 0x0277;
	cnt = ram + 0xc6;
#endif

    /* Send command to KBD buffer if more than one parameter */

    if (nargs == 3 && *args[1] == '<') {

	if (NULL == (chrin_fp = fopen(args[2], READ))) {
	    perror(args[2]);
	    printf("Cannot read file.\n");
	    return (0);
	}

	if (set_trap(&chrin_trap) != 0) {
	    printf("Driver not installed.\nCannot read file.\n");
	    fclose(chrin_fp);
	}
	else {
	    ++(*cnt);
	    printf("Emulator reads file '%s'\n", args[2]);
	}
    }

    else if (nargs > 1) {
	pack_args(1);			/* string argument must be quoted */

	for (n = 0, p = (BYTE *)args[1]; n < values[1]; ++n, ++p) {
	    if ( ((*p & 0xc0) == 0x40) && ((*p & 0x1f) < 0x1e) )
		*p ^= 0x20;

	    kbd[(*cnt)++] = *p;

	    if (*cnt > 10) {
		printf ("10 byte KBD buffer full.\n");

		/*  IERROR trap */

		return (0);
	    }
	}  /* for */
    }

    else
	mon_screen();			/* Display default screen */


    return (0);
}
#endif


/* Display screen */

#ifdef CBM64

static int  mon_screen(void)
{
    ADDRESS a, vbase;
    int     col;


    if (nargs > 1) {
	vbase = (values[1] < 0x100) ?		/* VIC-II screen address */
	  (values[1] & 0x3f) << 10 : values[1];
    }
    else {
	vbase = (LOAD(0xd018) & 0xf0) << 6;	/* VIC-II screen base */
	vbase |= (~(LOAD(0xdd00) & 3) << 14);	/* VIC-II screen bank */
    }

    printf ("%04X\n", vbase);			/* ccVVVVxxxxxxxxxx */
    for (a = vbase, col = 0; a < vbase + 0x3e8; ++a) {
	putchar (p_toascii(ram[a], 2));
	if (++col >= 40) {
	    putchar('\n');
	    col = 0;
	}
    }

    return (0);
}
#endif  /* CBM64 */


/* ------------------------------------------------------------------------- */

/*
 * Debugger functions
 */

static int  mon_setreg(void)
{
    /* set registers if more than one parameter */

    switch (nargs) {
      case 7:
	SP = values[6];
      case 6:
	SET_SR(values[5]);
      case 5:
	YR = values[4];
      case 4:
	XR = values[3];
      case 3:
	AC = values[2];
      case 2:
	PC = address = values[1];
    }

    mon_showreg();
    return (0);
}


static int  mon_showreg(void)
{
    printf("\n PC   AC  XR  YR  nv1bdizc  SP    OPCODE\n");
    printf((mode & MODE_HEX) ?
	"%04X  %02X  %02X  %02X  %s  %02X   %3X  %s\n\n" :
	"%04d %03d %03d %03d %s %03d  %3d  %s\n\n",
	(int) PC, (int) AC, (int) XR, (int) YR,
	sprint_status(), (int) SP,
	get_mem(PC), sprint_opcode(PC, mode & MODE_HEX));
    return (0);
}


static int  mon_exit(void)
{
#if 0
    unsigned char c;

    /* Check for BRK and JAM */
    if (!(c = get_mem(PC)) || (c & 0x8f) == 2 || (c & 0x9f) == 0x92) {
	printf("Start BASIC.\n");
	/* FIXME: This has to be done differently... */
	/* intr(I_BRK, PC); */	/* Restart BASIC via BREAK interrupt */
    }
#endif
    runflg = 0;
    return (1);
}


static int  mon_quit(void)
{
    printf("Quit.\n");
    exit (-1);
    exit (0);
}


static int  mon_jump(void)
{
    if (nargs > 1)
	mon_setreg();

    if (*args[0] == 't') {		/* time */
	if (nargs >1)
	    runflg = 2;
	mon_times(runflg);
    }
    else
	runflg = 1;

    return (runflg);
}


static void mon_times(int set)
{
    static time_t time_real = 0;
    static long   cycles = 0;

    /* set or show program timing */

    if (set) {
	time(&time_real); /* seconds, use hrtime for better resolution */
	cycles = clk;
    }
    else {
	long   c,v;
	time_t t = 0;

	if (time_real) {
	    time(&t);
	    t -= time_real;
	}
	c = clk - cycles;
	v = c / (CYCLES_PER_SEC/100);		/* 1/100 sec */

	printf("\t-- instr.  %8ld cycles  %02d:%05.2f virtual  %2d:%02d:%02d real\n\n",
	    c,
	    (int)(v/6000), (float)(v%6000)/100,
	    (int)(t/3600), (int)((t%3600)/60), (int)(t%60));
    }
}

#ifdef TRACE
static int  mon_trace(void)
{
    if (nargs == 2)
	traceflg = values[1];		/* trace CPU */

    printf ("Trace %s.\n", (traceflg ? "on" : "off") );
    return (0);
}
#endif

/* Execute single instruction */

#if 0
static int  mon_step(void)
{

    printf("Monitor break.\n");
    mon_showreg();
    return (0);
}
#endif

static int  mon_down(void)
{
    int level = values[1];
    if (nargs == 0)
	level = 1;

    while (level-- > 0) {
	address = ram[(++SP)+0x100] + 1;
	address |= ram[(++SP)+0x100] << 8;
    }

    printf ("Down stack.\n");
    mon_showreg();
    return (0);
}


static int  mon_up(void)
{
    int level = values[1];
    if (nargs == 0)
	level = 1;

    while (level-- > 0) {
	address = ram[(SP--)+0x100] << 8;
	address |= ram[(SP--)+0x100] + 1;
    }

    printf ("Up stack.\n");
    mon_showreg();
    return (0);
}


#ifdef NEW_TIMER_CODE
static BYTE break_bits[65536/8];
static int  break_alarm = 0;

static void break_alarm_cb(int i)
{
    /*
     * This will be called twice / cycle. This way we can be sure
     * breakpoint is tested if interrupt happens on same cycle on other
     * alarm.
     */
    static int old_pc = -1;
    int    kludge;


    kludge = (PC == old_pc) ? 1 : 0;

    old_pc = PC;

    set_alarm(break_alarm, clk + kludge, break_alarm_cb, 0);

    if (kludge)
	return;

    if (break_bits[PC/8] & (1 << (PC%8)))
    {
	printf("Monitor break");
	mon(PC);
    }
}


static void set_break_alarm(void)
{
    int i;

    if (!break_alarm)
	break_alarm = new_alarm();

    for (i=0; i<65536/8; i++)
	if (break_bits[i])
	{
	    break_alarm_cb(0);
	    return;
	}

    clear_alarm(break_alarm);
}


static int  mon_break(void)
{
    ADDRESS  adr = values[1], end = values[2];
    int found = 0;
    int i;
    int started;


    /*
     * Show current breakpoints
     */

    if (nargs == 1)
    {
	for (i=0; i<65536; i++)
	{
	    if (break_bits[i/8] & (1 << (i % 8)))
	    {
		if (!found)
		{
		    printf("Breakpoints: ");
		    found = 1;
		}
		started = i;

		while (++i < 65536 && (break_bits[i/8] & (1 << (i % 8)) ));
		i--;

		if (found == 1)
		    found = 2;
		else
		    printf(",");

		if (i > started + 1)
		    printf("%04x-%04x", started, i);
		else if (i == started + 1)
		    printf("%04x,%04x", started, i);
		else
		    printf("%04x", started);
	    }
	}
	printf( (found ? "\n" : "No breakpoints set.\n"));
	return 0;
    }


    if (nargs == 2)
	end = adr;

    while (adr <= end)
    {
	break_bits[adr/8] |= (1 << (adr % 8));
	adr++;
    }

    set_break_alarm();
    return (0);
}


static int  mon_clear(void)
{
    ADDRESS adr = values[1], end = values[2];


    switch (nargs) {
      case 1:
	for (adr = 0; adr <= 65535/8; ++adr)
	    break_bits[adr] = 0;
	break;

      case 2:
	end = adr;

      case 3:
	while (adr <= end) {
	    break_bits[adr/8] &= ~(1 << (adr % 8));
	    adr++;
	}
    }


    printf ("Breakpoint%c cleared.\n", ((nargs == 2) ? ' ' : 's'));

    set_break_alarm();
    return (0);
}

#endif  /* NEW_TIMER_CODE */


/* ------------------------------------------------------------------------- */

/*
 * Set operand's effective address (i.e. memory location read or written)
 * to look for. Notice the similarity to mon_break.
 */

#ifdef  TRAP_WRITES

static BYTE trap_bits[65536/8];

static int  mon_trap(void)
{
    ADDRESS  adr = values[1], end = values[2];
    int found = 0;
    int i;
    int started;


    /*
     * Show current read/write traps
     */

    if (nargs == 1)
    {
	for (i=0; i<65536; i++)
	{
	    if (trap_bits[i/8] & (1 << (i % 8)))
	    {
		if (!found)
		{
		    printf("Traps: ");
		    found = 1;
		}
		started = i;

		while (++i <= 65535 && (trap_bits[i/8] & (1 << (i % 8)) ));
		i--;

		if (found == 1)
		    found = 2;
		else
		    printf(",");

		if (i > started + 1)
		    printf("%04x-%04x", started, i);
		else if (i == started + 1)
		    printf("%04x,%04x", started, i);
		else
		    printf("%04x", started);
	    }
	}
	printf( (found ? "\n" : "No traps set.\n"));
	return 0;
    }


    if (nargs == 2)
	end = adr;

    while (adr <= end)
    {
	trap_bits[adr/8] |= (1 << (adr % 8));
	adr++;
    }

    return (0);
}

#endif


/* ------------------------------------------------------------------------- */


/*
 * Help
 */

static int  mon_help(void)
{
    int     i, n;
    int     f = 0;

    if (nargs == 2) {
	n = strlen(args[1]);
	for (i = 0; mon_cmds[i].command; i++)
	    if (!strncmp(mon_cmds[i].command, args[1], n)) {
		printf("\n  %s\n", mon_cmds[i].help_line);
		/* add help page here */
		f++;
		break;
	    }
	if (!f)
	    mon_help_mnem(args[1]);	/* Show 65xx mnemonic */
    }
    else {
	printf("The following commands are available:\n\
  #%%&$\t\t\t\t\t(base conversions)\n");
	    for (i = 0; mon_cmds[i].command; i++)
		printf("  %s", mon_cmds[i].help_line);
	printf("\nAll commands may be abbreviated.\n");
    }
    return (0);
}


static int  mon_help_mnem(char *line)
{
    int     i;

    /*
     * These routines are translated from 'monstar'
     */

    printf("\n");

    for (i = 0; line[i] && i < 3; i++) {
	if (isalpha (line[i]))
	    line[i] = toupper (line[i]);
    }

    for (i = 0; i < TOTAL_CODES; i++)
	if (0 == strncmp(lookup[i].mnemonic, line, 3)) {

	    printf("\t%02X  %s\t  %d\n", i,
		   sprint_disassembled(0, i, 0, 0, 1),
		   lookup[i].cycles);
	}

/*
    for (i = 0; i < TOTAL_CODES; i++)
	if (n == lookup[i].addr_mode) {

	    printf("\t%02X  %s\n", i,
		   sprint_disassembled(0, i, 0, 0, 1));
	}
*/
    printf("\n");
    return(0);
}


/*
 * Monitor main program
 */

void    mon(ADDRESS adr)
{
    char   *line, prompt[10];

    /* Make banked address */
    /* address = (ADDRESS)adr; */
    address = program_counter;

    printf("\nMonitor.\n");
    mode = MODE_SPACE | (app_resources.hexFlag ? MODE_HEX : 0);

    mon_showreg();

    if (runflg == 2)
	mon_times(0);	/* show accumulated count */

    runflg = 0;		/* terminate RUN mode */


    /* monitor main loop */

    do {
	sprintf(prompt, "(%04x) ", address);
	if ((line = read_line(prompt, MODE_MON)) == NULL)
	    exit (-1);

	if (*line) {
	    nargs = split_args(line, mode,
			       MAXARG, MASKVAL, args, values, types);

	    switch ( eval_command(args[0], nargs, mon_cmds) ) {
	      case 0:
		continue;
	      case 1:
		disable_text();
		return;
	    }

	    switch (*line) {
		/*case '+':*/
	      case '-':
	      case '%':
	      case '&':
	      case '#':
	      case '$':
		show_bases(line, mode);		/* Base conversions. */
		break;

	      case '*':
		address = sconv(line + 1, 0, mode) & MAXBANKADDR;
		break;

	      default:
		printf(" Unknown command. Try 'help'\n");
	    }
	} /* line */
    } while (!runflg);

    printf ("Resuming emulation.\n");
    disable_text();
}
