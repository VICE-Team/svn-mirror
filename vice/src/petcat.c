/*
 * $Id: petcat.c,v 2.3 1997/05/04 16:23:04 ettore Exp ettore $
 *
 * This file is part of Commodore 64 emulator
 *      and Program Development System.
 *
 *   Copyright (C) 1993-1995,1996, Jouko Valta
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * This program converts your SEQ files as well as expands tokenized
 * C64/128 BASIC programs into 7-bit ASCII text. Unprintable characters
 * can be shown as hexadecimal codes in parenthesis, via `-c' option.
 * It is also possible to convert programs from ascii listings into
 * tokenized basic v2.0, v3.5, v4.0, v7.0 or simon's basic programs. This
 * program also replaces certain control code names with the actual codes.
 *
 * A list of Toolkit Basic (published by Compute! Books) token codes
 * would be greatly appreciated. (-:
 *
 * Runs on UNIX or Atari ST.
 *
 * In shell:
 *  tr '\015A-Za-z\301-\332\\\|\[\{\]\}' '\012a-zA-ZA-Z\|\\\{\[\}\]'
 * or
 *  tr '\015A-Za-z\\\|\[\{\]\}' '\012a-zA-Z\|\\\{\[\}\]'
 *
 *
 * Written by
 *   Jouko Valta (jopi@stekt.oulu.fi)
 *
 *
 * $Log: petcat.c,v $
 * Revision 2.3  1997/05/04 16:23:04  ettore
 * Assignment of const pointer to non-const one fixed.
 *
 * Revision 2.2  1996/06/07  20:52:35  jopi
 * All conversions sped up significantly.
 *
 * Revision 2.1  1996/04/01  09:01:41  jopi
 * AtBasic added
 * P00 support implemented
 * offset for listing directly from archives.
 *
 * Revision 2.0  1995/06/28  19:48:14  jopi
 * Turtle Basic V1.0 and PET BASIC 1.0
 * CBM-x Graphic characters named
 *
 * Revision 1.9  1995/04/01  07:54:09  jopi
 * X64 0.3 PL 0
 * Suppressing header added.
 *
 * Revision 1.8  1994/12/12  16:59:44  jopi
 * PET Basic V4.0, Basic 10.0 and Speech Basic added.
 * New options for selecting version. Can set load address and
 * output file.
 *
 * Revision 1.7  1994/06/16  17:19:26  jopi
 * Alternate Control code set.
 *
 * Revision 1.6  1994/06/08  16:19:50  jopi
 * X64 version 0.2 PL 2
 * Operation improved, Basic 3.5 and new options.
 * Ported to Atari ST. Control code cruncher added.
 *
 * Revision 1.5  1994/01/26  16:08:37  jopi
 * X64 version 0.2 PL 1
 * Text mode, directory mode, Super Expander and Basic 4 ext. added.
 * Arithmetics tokenizing fixed, and crunches appreviated keywords too.
 *
 * Revision 1.4  1993/11/10  01:55:34  jopi
 * Text converter. Unused tokens skipped.
 *
 * Revision 1.3  93/06/21  13:38:03  jopi
 * X64 version 0.2 PL 0
 *
 * Revision 1.2  1993/06/13  08:19:36  sonninen
 * *** empty log message ***
 *
 *
 */

/* Ettore Perazzoli (ettore@comm2000.it) 1997/10/27: Renamed `p_toascii' to
   `_p_toascii' to avoid conflicts with `charsets.h'. */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef GEMDOS			/* Atari ST */
#define strchr index
#else
#include <string.h>
#endif

#include "patchlevel.h"
#include "charsets.h"		/* ctrl1, ctrl2, cbmkeys */

#ifndef SEEK_SET
#define SEEK_SET 0
#endif


#define B_1		 1
#define B_2		 2
#define B_SUPER		 3
#define B_TURTLE	 4

#define B_SIMON		 5
#define B_SPEECH	 6
#define B_ATBAS		 7
#define B_4		 8
#define B_4E		 9	/* C64 extension, Expand only */

#define B_35		10
#define B_7		11
#define B_10		12


/* Limits */

#define NUM_B_1		75	/* Basic 1.0 */

#define NUM_COMM	76	/* common for all versions 2.0 ... 10.0 */
#define NUM_SECC	18	/* VIC-20 extension */
#define NUM_TUCC	34	/* VIC-20 Turtle Basic extension */

#define NUM_V4CC	15	/* PET 4.0 */
#define NUM_4ECC	24	/* 4.0 extension (C64) */
#define NUM_SPECC	27	/* Speech Basic */
#define NUM_ATBCC	43	/* Atbasic */

#define NUM_KWCE	11
#define NUM_V7FE	39
#define NUM_V10FE	62


#define MAX_COMM	0xCB	/* common for all versions */
#define MAX_SECC	0xDD	/* VIC-20 extension */
#define MAX_TUCC	0xED	/* VIC-20 Turtle Basic extension */

#define MAX_V4CC	0xDA	/* PET 4.0 */
#define MAX_4ECC	0xE3	/* 4.0 extension (C64) */
#define MAX_SPECC	0xE6	/* Speech Basic */
#define MAX_ATBCC	0xF6	/* Atbasic */

#define MAX_KWCE	0x0A
#define MAX_V7FE	0x26
#define MAX_V10FE	0x3D

#define KW_NONE		0xFE	/* flag unused token */


#define CLARIF_LP	'<'	/* control code left delimiter */
#define CLARIF_RP	'>'	/* control code right delimiter */


#if (!defined(GEMDOS) && defined(__STDC__))
static int    parse_version ( char *str );
static void   pet_2_asc ( int ctrls );
static void   _p_toascii ( int c, int ctrls);
static void   list_keywords ( int version );
static int    p_expand ( int version, int addr, int ctrls );
static void   p_tokenize ( int version, int addr, int ctrls );
static unsigned char sstrcmp ( unsigned char *, char**, int, int );

/* extern */
extern long filelength ( int handle );
extern int  is_pc64name ( char *name );
extern int  write_pc64header ( FILE *fd, char *name, int reclen );
extern int  read_pc64header ( FILE *fd, char *name, int *reclen );
extern char *pc_get_cbmname ( FILE *fd, char *fsname );
extern int  DirP00 ( FILE *fd, char *CbmName, int *start, int *end );

extern int  check_t64_header (FILE *f);

#else
static int    parse_version();
static void   pet_2_asc();
static void   _p_toascii();
static void   list_keywords();
static int    p_expand();
static void   p_tokenize();
static unsigned char sstrcmp();

/* extern */
extern long filelength ();
extern int  is_pc64name ();
extern int  write_pc64header ();
extern int  read_pc64header ();
extern char *pc_get_cbmname ();
extern int  DirP00 ();

extern int  check_t64_header (FILE *f);

#endif


/* ------------------------------------------------------------------------- */

/* PC64 files need this */

char *slot_type[] =
{
    "DEL", "SEQ", "PRG", "USR", "REL", "CBM", "DJJ", "FAB"
};

static const unsigned char MagicHeaderP00[8] = "C64File\0";


#define NUM_VERSIONS  12

char   *VersNames[] = {
    "Basic 1.0",
    "Basic 2.0",
    "Basic 2.0 with Super Expander",
    "Basic 2.0 with Turtle Basic v1.0",

    "Basic 2.0 and Simon's Basic",
    "Basic 2.0 with Speech Basic v2.7",
    "Basic 2.0 with @Basic",

    "Basic 4.0",
    "Basic 4.0 extension",
    "Basic 3.5",
    "Basic 7.0",
    "Basic 10.0",
    ""
};


/*
 * CBM Basic Keywords
 */

char   *keyword[] = {
    /* Common Keywords, 80 - cb */
    "end",   "for",  "next", "data", "input#", "input",  "dim", "read",
    "let",   "goto", "run",  "if",   "restore", "gosub", "return", "rem",
    "stop",  "on",   "wait", "load", "save",   "verify", "def", "poke",
    "print#", "print", "cont", "list", "clr",  "cmd", "sys", "open",
    "close", "get",  "new",  "tab(", "to",    "fn",  "spc(", "then",
    "not",   "step", "+",    "-",    "*",     "/",   "^",    "and",
    "or",    ">",    "=",    "<",    "sgn",   "int", "abs",  "usr",
    "fre",   "pos",  "sqr",  "rnd",  "log",   "exp", "cos",  "sin",
    "tan",   "atn",  "peek", "len",  "str$",  "val", "asc",  "chr$",
    "left$", "right$", "mid$", "go",
    /*
     * The following codes (0xcc- 0xfe) are for 3.5, 7.0, and 10.0 only.
     * On 10.0 gshape, sshape, and draw are replaced with paste, cut, and line
     * respectively. */
    "rgr",  "rlcr", "rlum" /* 0xce -- v7 prefix */, "joy",
    "rdot", "dec",  "hex$", "err$", "instr", "else", "resume", "trap",
    "tron", "troff", "sound", "vol", "auto", "pudef", "graphic", "paint",
    "char", "box", "circle", "gshape", "sshape", "draw", "locate", "color",
    "scnclr", "scale", "help", "do", "loop", "exit", "directory", "dsave",
    "dload",  "header", "scratch", "collect", "copy", "rename", "backup",
    "delete", "renumber", "key", "monitor", "using", "until", "while",
    /* 0xfe -- prefix */ "", "~" /* '~' is ASCII for 'pi' */
};


/*
 * 7.0 and 10.0 only.
 * On 10.0 stash, fetch, and swap are replaced with dma.
 */

char   *kwce[] = {
    "", "", "pot", "bump", "pen", "rsppos", "rsprite", "rspcolor",
    "xor", "rwindow", "pointer"
};


char   *kwfe[] = {
    "", "",  "bank", "filter",  "play", "tempo", "movspr", "sprite",
    "sprcolor", "rreg", "envelope", "sleep", "catalog", "dopen", "append",
    "dclose", "bsave",
     "bload", "record", "concat", "dverify", "dclear", "sprsav", "collision",
    "begin", "bend",  "window", "boot", "width", "sprdef", "quit", "stash",
    "",  "fetch", "",  "swap",  "off",  "fast",  "slow",
    /* Basic 10.0 only (fe27 - fe3d) */
    "type",
    "bverify", "ectory", "erase", "find", "change", "set", "screen", "polygon",
    "ellipse", "viewport", "gcopy", "pen", "palette", "dmode", "dpat", "pic",
    "genlock", "foreground", "", "background", "border", "highlight"
};


char  *superkwcc[] = { /* VIC Super Expander commands 0xcc - 0xdd */
    "key", "graphic", "scnclr", "circle", "draw", "region", "color", "point",
    "sound", "char",  "paint",  "rpot",   "rpen", "rsnd",   "rcolr", "rgr",
    "rjoy",  "rdot"
};


char *petkwcc[] = {	/* PET Basic 4.0 0xcc - 0xda */
    "concat", "dopen", "dclose", "record",
    "header", "collect", "backup", "copy","append", "dsave","dload", "catalog",
    "rename", "scratch", "directory",
    /* Basic 4 Extension for C64 (0xdb - 0xe3) */
    "color", "cold", "key", "dverify", "delete",
    "auto", "merge", "old", "monitor"
};


/* ------------------------------------------------------------------------- */

/*
 * Third party products for VIC-20
 */

/* Turtle Basic v1.0 Keywords  -- Craig Bruce */

char   *turtlekwcc[] = {
    "graphic", "old",  "turn",   "pen",  "draw",  "move", "point", "kill",
    "write", "repeat", "screen", "doke", "reloc", "fill", "rtime", "base",
    "pause", "pop",    "color", "merge", "char",  "take", "sound", "vol",
    "put",   "place",  "cls",  "accept", "reset", "grab", "rdot",  "plr$",
    "deek",  "joy"
};


/*
 * Third party products for C=64
 */


/* Speech Basic v2.7  Keywords (Tokens CC - E6) */

char   *speechkwcc[] = {
    "reset",  "basic", "help",  "key",
    "himem",  "disk",  "dir",   "bload", "bsave",  "map",    "mem", "pause",
    "block",  "hear", "record", "play",  "voldef", "coldef", "hex", "dez",
    "screen", "exec",  "mon",   "<-",    "from",   "speed",  "off"
};


/* @Basic (Atbasic) Keywords (Tokens CC - F6) -- Andre Fachat */

char   *atbasickwcc[] = {
    "trace", "delete",  "auto", "old", "dump", "find", "renumber", "dload",
    "dsave", "dverify", "directory", "catalog", "scratch", "collect",
    "rename", "copy", "backup", "disk", "header", "append", "merge", "mload",
    "mverify", "msave", "key", "basic", "reset", "exit", "enter", "doke",
    "set", "help", "screen", "lomem", "himem", "colour", "type", "time",
    "deek", "hex$", "bin$", "off", "alarm"
};


/* Simon's Basic Keywords */

char   *simonskw[] = {
    "", "hires", "plot", "line", "block", "fchr", "fcol", "fill",
    "rec", "rot", "draw", "char", "hi col", "inv", "frac",  "move",
    "place", "upb", "upw", "leftw", "leftb", "downb", "downw", "rightb",
    "rightw", "multi", "colour", "mmob", "bflash", "mob set", "music", "flash",
    "repeat", "play", ">>", "centre", "envelope", "cgoto", "wave", "fetch",
    "at(",   "until", ">>", ">>", "use", ">>", "global", ">>",
    "reset", "proc", "call", "exec", "end proc", "exit", "end loop", "on key",
    "disable", "resume", "loop", "delay", ">>",  ">>",  ">>",  ">>",
    "secure", "disapa", "circle", "on error","no error","local","rcomp","else",
    "retrace", "trace", "dir", "page", "dump", "find", "option", "auto",
    "old",  "joy", "mod", "div", ">>", "dup", "inkey", "inst",
    "test", "lin", "exor", "insert", "pot", "penx", ">>", "peny",
    "sound", "graphics", "design", "rlocmob", "cmob", "bckgnds", "pause","nrm",
    "mob off", "off", "angl", "arc", "cold", "scrsv", "scrld", "text",
    "cset",  "vol",  "disk", "hrdcpy", "key", "paint", "low col", "copy",
    "merge", "renumber", "mem", "detect", "check", "display", "err", "out"
};


/* ------------------------------------------------------------------------- */


/*
 * Alternate mnemonics for control codes
 * These are used by MikroBITTI for clarification
 */

char   *a_ctrl1[] = {
    "", "", "", "", "", "wht", "", "",
    "up/lo lock on", "up/lo lock off", "", "", "", "return", "lower case", "",
    "", "down", "rvs on", "home", "delete", "", "", "",
    "",  "",  "",  "esc", "red", "right", "grn", "blu"
};

char   *a_ctrl2[] = {
    "", "orange", "", "", "", "f1", "f3", "f5",
    "f7", "f2", "f4", "f6", "f8", "shift return", "upper case", "",
    "blk",  "up", "rvs off", "clr", "insert", "brown", "lt red", "grey1",
    "grey2", "lt green", "lt blue", "grey3", "pur", "left", "yel", "cyn"
};


/* ------------------------------------------------------------------------- */

static FILE    *source, *dest;
static int     kwlen;

int    main(argc, argv)
    int     argc;
    char   *argv[];
{
    char   *progname, *outfilename = NULL;
    char    realname[24];
    int     reclen;
    int     c;

    long    offset = 0;
    int     wr_mode = 0, version = B_7;		/* best defaults */
    int     load_addr = 0, ctrls= -1, hdr = 1, show_words = 0;
    int     fil = 0, outf = 0, overwrt = 0, textmode = 0;
    int     flg = 0;				/* files on stdin */


    /* Parse arguments */

    progname = argv[0];
    while (--argc && ((*++argv)[0] == '-')) {

	if (!strcmp(argv[0], "--")) {
	    --argc; ++argv;
	    break;
	}

	if (!strcmp(argv[0], "-l")) {		/* load address */
	    if (argc > 1 && sscanf(argv[1], "%x", &load_addr) == 1) {
		--argc; ++argv;
	    continue;
	    }
	    /* Fall to error */
	}

	if (!strcmp(argv[0], "-c")) {
	    ctrls = 1;
	    continue;
	}
	else if (!strcmp(argv[0], "-nc")) {
	    ctrls = 0;
	    continue;
	}

	if (!strcmp(argv[0], "-h")) {
	    hdr = 1;
	    continue;
	}
	else if (!strcmp(argv[0], "-nh")) {
	    hdr = 0;
	    continue;
	}

	else if (!strcmp(argv[0], "-f")) {	/* force overwrite */
	    ++overwrt;
	    continue;
	}

	else if (!strcmp(argv[0], "-o")) {
	    if (argc > 1) {
		outfilename = argv[1];
		++outf;
		--argc; ++argv;
	    continue;
	    }
	    fprintf (stderr, "\nOutput filename missing\n");
	    /* Fall to error */
	}

	/* reading offset */
	if (!strcmp(argv[0], "-skip") || !strcmp(argv[0], "-offset")) {
	    if (argc > 1 && sscanf(argv[1], "%lx", &offset) == 1)
		{
		    --argc; ++argv;
		    continue;
		}
	    /* Fall to error */
	}

	else if (!strcmp(argv[0], "-text")) {	/* force text mode */
	    ++textmode;
	    continue;
	}

	else if (!strcmp(argv[0], "-help") ||
		 !strncmp(argv[0], "-v", 2)) {	/* version ID */
	    fprintf(stderr,
		    "\n\t%s V%4.2f PL %d -- Commodore Basic list/crunch utility.\n",
		    progname, (float)PETCATVERSION, PETCATLEVEL );

	    /* Fall to error for Usage */
	}


	/* Basic version */

	else if (!strncmp(argv[0], "-w", 2) && !wr_mode) {
	    version = parse_version((strlen(argv[0]) > 2 ? &argv[0][2] : NULL));
	    ++wr_mode;
	    continue;
	}

	else if (!strncmp(argv[0], "-k", 2) && !wr_mode) {
	    version = parse_version((strlen(argv[0]) > 2 ? &argv[0][2] : NULL));
	    ++show_words;
	    continue;
	}

	else if ((version = parse_version(&argv[0][1])) >= 0) {
	    continue;
	}

	fprintf(stderr,
		"\nUsage: %7s  [-c | -nc]  [-text | -<version> | -w<version>]\
\n\t\t[-skip bytes] [-l hex]  [--] [file list]\n\t\t[-k[<version>]]\n",
		progname);

	fprintf(stderr, "\n\
   -c\t\tcontrols (interpret also control codes)\n\
   -nc\t\tno controls (suppress control codes in printout)\n\
   -skip <n>\tSkip <n> bytes in the beginning of input file. Ignored on P00.\n\
   -<version>\tuse keywords for <version> instead of the v7.0 ones\n\
   -w<version>\t\ttokenize using keywords on specified Basic version.\n\
   -k<version>\tlist all keywords for the specified Basic version\n\
\t\t  without <version>, list all Basic versions available.\n\
   -l\t\tSpecify load address for program.\n");

	fprintf(stderr, "\n\tVersions:\n\
\t1\t\tPET Basic V1.0\n\
\t2\t\tBasic v2.0\n\
\tsuper\tv2 with Super Expander (VIC)\n\
\tturtle\tv2 with Turtle Basic by Craig Bruce (VIC)\n\
\ta\tv2 with AtBasic (C64)\n\
\tsimon\tlike previous but using Simon's Basic extension (C64)\n\
\tspeech\tlike previous but using Speech Basic v2.7 (C64)\n\
\t4 -w4e\tPET Basic v4.0 program (PET/C64)\n\
\t3\t\tBasic v3.5 program (C16)\n\
\t7\t\tBasic v7.0 program (C128)\n\
\t10\t\tBasic v10.0 program (C64DX)\n\n");

	exit(1);
    }


/*
 * Check parameters
 */

    if (argc)
	fil++;

    if (outf)
	hdr = 0;

    if (version == B_10) {
	keyword[0x63] = "paste";
	keyword[0x64] = "cut";
	keyword[0x65] = "line";
	keyword[0x6e] = "dir";
	kwfe[0x1f] = "dma";
	kwfe[0x21] = "dma";
	kwfe[0x23] = "dma";
    }

    if (show_words) {
	list_keywords(version);
	return (0);
    }

    if (ctrls < 0)
	ctrls = (textmode ? 0 : 1);	/*default ON for prgs, OFF for text */


    if (!load_addr) {
	switch (version) {
	  case B_SUPER:
	    load_addr = 0x0401; break;
	  case B_TURTLE:
	    load_addr = 0x3701; break;
	  case B_35:
	    load_addr = 0x1001; break;
	  case B_7:
	    load_addr = 0x1c01; break;
	  case B_10:
	    load_addr = 0x2001; break;
	  default:
	    load_addr = 0x0801;
	}
    }

    if (wr_mode) {
	fprintf (stderr, "\nLoad address %04x\n", load_addr);
	if ((load_addr & 255) != 1) {
	    fprintf (stderr, "I'm afraid I cannot accept that.\n");
	    exit(1);
	}

	fprintf (stderr, "Control code set: %s\n\n",
		 (ctrls ? "enabled" : "disabled"));
    }


    /*
     * Loop all files
     */

    do {
	int  plen = 0;
	flg = 0;	/* stdin loop flag */


	/*
	 * Try to figure out whether input file is in P00 format or not.
	 * If the header is found, the real filaname is feches and any other
	 * offset specified is overruled.
	 * This is particularly difficult on <stdin>, as only _one_ character
	 * of pushback is guaranteed on all cases. So, the lost bytes "C64File"
	 * are recreated from the magic header while printing the (text) file.
	 */

	if (!fil) {
	    const unsigned char *p;

	    source = stdin;

	    for (plen = 0, p = MagicHeaderP00; plen < 8 &&
		 (c = getc(source)) != EOF && (unsigned)c == *p; ++plen, ++p);

	    if (plen == 8) {
		/* skip the rest of header */
		for (plen = 18; plen > 0 && getc(source) != EOF; --plen);
	    }
	    else {
		/*printf("P00 failed at location %d.\n", plen);*/
		ungetc(c, source);
	    }
	}

	else {
	    if ((source = fopen(argv[0], "rb")) == NULL) {
		fprintf(stderr,
			"\n%s: Can't open file %s\n", progname, argv[0]);
		exit(1);
	    }

	    if (read_pc64header(source, realname, &reclen) == 0) {
		printf ("p00 file.\n");

		/*outfilename = realname;*/
	    }
	    else {
		fseek(source, offset, SEEK_SET); /* rewind or skip beginning */
	    }
	}



	if (!outf)
	    dest = stdout;
	else {
	    /*if (extension && *extension == '.') {
	        sprintf (outfilename, "%s%s", argv[0], extension);
	    }*/

	    if ((dest = fopen(outfilename, "wb")) == NULL) {
		fprintf(stderr,
			"\n%s: Can't open output file %s\n", progname, outfilename);
		exit(1);
	    }
	}


	if (wr_mode) {

	    /* Write P00 header ?  -- Available on tokenizer only */

	    if (is_pc64name(outfilename) >= 0) {
		printf("writing PC64 header.\n");
		write_pc64header(dest, argv[0], 0);
		/*write_pc64header(dest, pc_get_cbmname(argv[0]), 0);*/
	    }

	    p_tokenize(version, load_addr, ctrls);
	}
	else {
	    if (hdr)
		printf("\n\n%s ", (fil ? argv[0] : "<stdin>"));

	    /*
	     * Use TEXT mode if the offset doesn't equal BASIC load addresses
	     * and the first bytes to be read do not contain load address.
	     * Explicitly selected textmode overrules these conditions.
	     */

	    if (textmode || ((offset & 255) !=1 &&
		((c = getc(source)) != EOF && ungetc(c, source) != EOF &&
		c && c != 1)) ) {

		/* Print the bytes lost in header check */
		if (plen > 0 && plen < 8)
		    for (c = 0; c < plen; ++c)
		    fputc (MagicHeaderP00[(int)c], dest);

		pet_2_asc(ctrls);
	    }
	    else {

		/* get load address */
		load_addr =(getc(source) & 0xff) + ((getc(source) & 0xff)<< 8);
		fprintf(dest, "==%04x==\n", load_addr);

		if (p_expand(version, load_addr, ctrls)) {
		    printf ("\n*** Machine language part skipped. ***\n");
		}
		else	/* End of BASIC on stdin. Is there more ? */
		    if (!fil && (c = getc(source)) != EOF &&
			ungetc(c, source) != EOF && c) {
			++flg;
			++hdr;
		    }
	    }

	    if (hdr)
		fputc('\n', dest);
	}


	if (fil)
	    fclose(source);
	if (outf)
	    fclose(dest);

    } while (flg || (fil && --argc && ++argv));		/* next file */
    return(0);
}


/* ------------------------------------------------------------------------- */


/* Parse given version name and return its code, or -1 if not recognized. */

static int    parse_version(str)
    char   *str;
{
    int version = -1;


    if (str == NULL || !*str)
	return 0;

    switch (toupper(*str)) {
      case '1':		/* Basic 1.0 and Basic 10.0 */
	if (str[1] == '0')
	    version = B_10;
	else if (!str[1])
	    version = B_1;
	break;

      case '2':
	version = B_2;
	break;

      case 'A':
	version = B_ATBAS;
	break;

      case 'S':
	switch (str[1]) {
	  case 'u':
	  case 'e':
	    version = B_SUPER;
	    break;
	  case 'i':
	    version = B_SIMON;
	    break;
	  case 'p':
	    version = B_SPEECH;
	    break;
	  default:
	    fprintf (stderr,
		"Please, select one of the following: super, simon, speech\n");
	}
	break;

      case 'T':
	version = B_TURTLE;
	break;

      case '4':
	version = ((str[1]=='e') ? B_4E : B_4);	/* Basic 4.0 */
	break;

      case '3':
	version = B_35;	/* 3.5 */
	break;
      case '7':
	version = B_7;
	break;

      default:
	fprintf (stderr, "\nUnimplemented version '%s'\n", str);
	version = -1;
    }

    return (version);
}


static void   list_keywords(version)
     int version;
{
    int   n, max;

    if (version <= 0 || version > NUM_VERSIONS) {
	printf ("\n  The following versions are supported on  %s V%4.2f\n\n",
		"petcat", (float)PETCATVERSION );

	for (n = 0; n < NUM_VERSIONS; n++)
	    printf ("\t%s\n", VersNames[n]);
	printf ("\n");
	return;
    }


    printf ("\n  Available Keywords on %s\n\n", VersNames[version - 1]);

    if (version == B_1)
	max = NUM_B_1;
    else if (version==B_35 || version==B_7 || version == B_10)
	max = 0x7E;
    else
	max = NUM_COMM;

    for (n = 0; n < max; n++) {
	if (version == B_35 || n != 0x4e)	/* Skip prefix marker */
	    printf("%s\t", keyword[n] /*, n | 0x80*/);
    }
    printf("%s\n", keyword[127]);


    if (version == B_7 || version == B_10) {
	for (n = 2; n < ((version == B_10) ? NUM_V10FE : NUM_V7FE); n++)
	    printf("%s\t", kwfe[n] /*, 0xfe, n*/);

	for (n = 2; n < NUM_KWCE; n++)
	    printf("%s\t", kwce[n] /*, 0xce, n*/);
    }

    else
      switch (version) {
      case B_SUPER:
	for (n = 0; n < NUM_SECC; n++)
	    printf("%s\t", superkwcc[n] /*, n + 0xcc*/);
	break;

      case B_TURTLE:
	for (n = 0; n < NUM_TUCC; n++)
	    printf("%s\t", turtlekwcc[n] /*, n + 0xcc*/);
	break;

	case B_4:
	case B_4E:
	for (n = 0; n < ((version == B_4) ? NUM_V4CC : NUM_4ECC); n++)
	    printf("%s\t", petkwcc[n] /*, n + 0xcc*/);
	break;

	case B_SIMON:
	for (n = 1; n < 0x80; n++)
	    printf("%s\t", simonskw[n] /*, 0x64, n*/);
	break;

      case B_SPEECH:
	for (n = 0; n < NUM_SPECC; n++)
	    printf("%s\t", speechkwcc[n] /*, n + 0xcc*/);
	break;

      case B_ATBAS:
	for (n = 0; n < NUM_ATBCC; n++)
	    printf("%s\t", atbasickwcc[n] /*, n + 0xcc*/);
    }  /* switch */

    printf("\n\n");
}


/* ------------------------------------------------------------------------- */

/*
 * Conversion Routines
 */

static void   pet_2_asc (ctrls)
     int ctrls;
{
    int c;


    while ((c = getc(source)) != EOF) {
	_p_toascii(c, ctrls);		/* convert character */
    }      /* line */
}


static void   _p_toascii(c, ctrls)
     int c;
     int ctrls;
{
    switch (c) {
    case 0x00:				/* 00 for SEQ */
    case 0x0a:
    case 0x0d:
      fputc ('\n', dest);
      break;
    case 0x60:
      fprintf(dest, "(SHIFT-*)");
      break;
    case 0x7c:
      fprintf(dest, "(CBM--)");		/* Conflicts with Scandinavian Chars */
      break;
    case 0x7f:
      fprintf(dest, "(CBM-*)");
      break;
    case 0xa0:				/* CBM: Shifted Space */
    case 0xe0:
      if (!ctrls)
	  fputc (' ', dest);
      else
	  fprintf(dest, "($%02x)", c & 0xff);
      break;
    case 0xff:
      fputc (0x7e, dest);
      break;

    default:
      switch (c & 0xe0) {
      case 0x40:		/* 41 - 7F */
      case 0x60:
	fputc (c ^ 0x20, dest);
	break;
      case 0xa0:		/* A1 - BF */
      case 0xe0:		/* E1 - FE */
	fprintf(dest, "(%s)", cbmkeys[c & 0x1f]);
	break;
      case 0xc0:		/* C0 - DF */
	fputc (c ^ 0x80, dest);
	break;

      default:
	if (isprint(c))
	   fputc (c, dest);

	else if (ctrls) {
	  if ((c < 0x20) && *ctrl1[c])
	     fprintf(dest, "(%s)", ctrl1[c]);
	  else if ((c > 0x7f) && (c < 0xa0) && *ctrl2[c & 0x1f])
	     fprintf(dest, "(%s)", ctrl2[c & 0x1f]);
	  else
	     fprintf(dest, "($%02x)", c & 0xff);
	}  /* ctrls */
      }  /* switch */
    }  /* switch */
}


/*
 * This routine starts from the beginning of Basic, and not from the
 * load address included on program files. That way it can list from
 * RAM dump.
 */

static int   p_expand(version, addr, ctrls)
     int addr, ctrls, version;
{
    static char line[4];
    unsigned int c;
    int quote, spnum, directory = 0;
    int sysflg = 0;


    /*
     * It seems to be common mistake not to terminate BASIC properly
     * before the machine language part, so we don't check for the
     * low byte of line link here.
     * Line link and line number are read separately to leave possible
     * next file on stdin intact.
     */

    while ((fread(line, 1, 2, source) == 2) && (/*line[0] ||*/ line[1]) &&
	   fread(line + 2, 1, 2, source) == 2) {
	quote = 0;
	fprintf(dest, " %4d ",
		(spnum = (line[2] & 0xff) + ((line[3] & 0xff) << 8)) );

	if (directory) {
	    if (spnum >= 100)
		spnum = 0;
	    else if (spnum >= 10)
		spnum = 1;
	    else
		spnum = 2;
	}

	/* prevent list protection from terminating listing */

	while ((c = getc(source)) != EOF && !c);

	if (c == 0x12 && !line[2] && !line[3]) {  /* 00 00 12 22 */
	    directory++;
	}

	do {
	    if (c == 0x22)
		quote ^= c;

	    /*
	     * Simon's basic. Any flag for this is not needed since it is
	     * mutually exclusive with all other implemented modes.
	     */

	    if (!quote && (c == 0x64)) {
		if ((c = getc(source)) < 0x80) {
		    fprintf(dest, "%s", simonskw[c]);
		    continue;
		} else
		    fprintf(dest, "($64)");	/* it wasn't prefix */
	    }

	    /* basic 2.0, 7.0 & 10.0 and extensions */

	    if (!quote && c > 0x7f) {
		if (c <= MAX_COMM) {
		    fprintf(dest, "%s", keyword[c & 0x7f]);

		    if (c == 0x9E) {
			++sysflg;
		    }
		    continue;
		}
		if (version != B_35) {
		    if (c == 0xce) {		/* 'rlum' on V3.5*/
			if ((c = getc(source)) <= MAX_KWCE)
			    fprintf(dest,"%s", kwce[c]);
			else
			    fprintf(dest, "($ce%02x)", c);
			continue;
		    } else if (c == 0xfe) {
			if ((c = getc(source)) <= MAX_V10FE)
			    fprintf(dest, "%s", kwfe[c]);
			else
			    fprintf(dest, "($fe%02x)", c);
			continue;
		    }
		}

		switch (version) {
		  case B_2:
		  case B_SUPER:		/* VIC extension */
		    if (c >= 0xcc && c <= MAX_SECC)
			fprintf(dest, "%s", superkwcc[c - 0xcc]);
		    break;

		  case B_TURTLE:	/* VIC extension as well */
		    if (c >= 0xcc && c <= MAX_TUCC)
			fprintf(dest, "%s", turtlekwcc[c - 0xcc]);
		    break;

		  case B_SPEECH:	/* C64 Speech basic */
		    if (c >= 0xcc && c <= MAX_SPECC)
			fprintf(dest, "%s", speechkwcc[c - 0xcc]);
		    break;

		  case B_ATBAS:		/* C64 Atbasic */
		    if (c >= 0xcc && c <= MAX_ATBCC)
			fprintf(dest, "%s", atbasickwcc[c - 0xcc]);
		    break;

		  case B_4:		/* PET V4.0 */
		  case B_4E:	/* V4.0 extension */
		    if (c >= 0xcc && c <= MAX_4ECC)
			fprintf(dest, "%s", petkwcc[c - 0xcc]);
		    break;

		  default:		/* C128 */
		    fprintf(dest, "%s", keyword[c & 0x7f]);
		}
		continue;
	    } /* quote */

	    if (directory && spnum && c == 0x20) {
		spnum--;	  /* eat spaces to adjust directory lines */
		continue;
	    }

	    _p_toascii(c, ctrls);	/* convert character */

	} while ((c = getc(source)) != EOF && c);
	printf("\n");

    }      /* line */

#ifdef DEBUG
    printf("\n c %02x  EOF %d  *line %d  sysflg %d\n",
	   c, feof(source), *line, sysflg);
#endif

    return (!feof(source) && (*line | line[1]) && sysflg);
}


static void   p_tokenize(version, addr, ctrls)
     int version;
     int addr;
     int ctrls;
{
    static char    line[256];
    unsigned char *p1, *p2, quote, c;
    int            len = 0, linum = 10, match;

    fprintf(dest, "%c%c", (addr & 255), ((addr >> 8) & 255) );


    /* Copies from p2 to p1 */

    while((p1 = p2 = (unsigned char *)fgets(line, 255, source)) != NULL) {

#ifndef GEMDOS
	if (sscanf(line, "%d%n", &linum, &len) == 1)
	    p2 += len;
#else
	if (sscanf(line, "%d", &linum) == 1)
	    while (isspace(*p2) || isdigit(*p2)) p2++;
#endif
	quote = 0;
	while (isspace(*p2)) p2++;

	while (*p2) {
	    if (*p2 == 0x22) {
		quote ^= *p2;
	    }
	    if (*p2 == 0x0a || *p2 == 0x0d)
		break;

	    match = 0;
	    if (quote) {
		/*
		 * control code evaluation
		 * only strings that appear inside quotes are
		 * interpreted -- they should not be used elsewhere
		 */

		if (ctrls && (*p2 == CLARIF_LP)) {
		    unsigned char *p;
		    p = p2;

		    /* repetition count */
		    len = 1;
#ifndef GEMDOS
		    if (sscanf((char *)++p, "%d%n", &len, &kwlen) == 1) {
			p += kwlen;
#else
		    if (sscanf(++p, "%d", &len) == 1) {
			while (isspace(*p) || isdigit(*p)) p++;
#endif
			if (*p == ' ')
			    ++p;
		    }
		    /*fprintf(stderr, "count %d\n", len);*/

		    if (( ((c = sstrcmp(p, ctrl1, 0, 0x20)) != KW_NONE) ||
			 ((c = sstrcmp(p, a_ctrl1, 0, 0x20)) !=KW_NONE) ||

			((((c = sstrcmp(p, ctrl2, 0, 0x20)) != KW_NONE) ||
			 ((c = sstrcmp(p, a_ctrl2, 0, 0x20)) !=KW_NONE)) &&
			 (c |= 0x80)) ||

			( ((c = sstrcmp(p, cbmkeys, 0, 0x20)) != KW_NONE) &&
			 (c |= 0xA0)) ) &&	/* CBM-x images */

			p[kwlen] == CLARIF_RP) {

			for (; len-- > 0;)
			    *p1++ = c;
			p2 = p + (++kwlen);
			continue;
		    }
		}
	    }
	    else if (isalpha(*p2) || strchr("+-*/^>=<", *p2)) {

		/* FE and CE prefixes are checked first */
		if (version == B_7 || version == B_10) {
		    if ((c = sstrcmp(p2, kwfe, 2,
			((version == B_10) ? NUM_V10FE : NUM_V7FE))) != KW_NONE) {
			*p1++ = 0xfe;
			*p1++ = c;
			p2 += kwlen;
			match++;
		    }
		    else if ((c = sstrcmp(p2, kwce, 2, NUM_KWCE)) != KW_NONE) {
			*p1++ = 0xce;
			*p1++ = c;
			p2 += kwlen;
			match++;
		    }
		}

		/* Common Keywords
		 * Note:  ~ (pi) is tested later */


		if (!match) {
		    int max;

		    if (version == B_1)
			max = NUM_B_1;
		    else if (version==B_35 || version== B_7 || version == B_10)
			max = 0x7E;
		    else
			max = NUM_COMM;

		    if ((c = sstrcmp(p2, keyword, 0, max)) != KW_NONE) {

			if (version == B_35 || c != 0x4e) {  /* Skip prefix */
			    *p1++ = c | 0x80;
			    p2 += kwlen;
			    match++;
			}
		    }
		}

		if (!match)
		  switch (version) {
		  case B_SUPER:
		    if ((c = sstrcmp(p2, superkwcc, 0, NUM_SECC)) != KW_NONE) {
			*p1++ = c + 0xcc;
			p2 += kwlen;
			match++;
		    }
		    break;

		  case B_TURTLE:
		    if ((c = sstrcmp(p2, turtlekwcc, 0, NUM_TUCC)) !=KW_NONE) {
			*p1++ = c + 0xcc;
			p2 += kwlen;
			match++;
		    }
		    break;

		  case B_4:
		  case B_4E:
		    if ((c = sstrcmp(p2, petkwcc, 0,
			((version == B_4) ? NUM_V4CC : NUM_4ECC))) != KW_NONE) {
			*p1++ = c + 0xcc;
			p2 += kwlen;
			match++;
		    }
		    break;

		  case B_SIMON:
		    if ((c = sstrcmp(p2, simonskw, 1, 0x80)) != KW_NONE) {
			*p1++ = 0x64;
			*p1++ = c;
			p2 += kwlen;
			match++;
		    }
		    break;

		  case B_SPEECH:
		    if ((c = sstrcmp(p2, speechkwcc, 0, NUM_SPECC)) !=KW_NONE) {
			*p1++ = c + 0xcc;
			p2 += kwlen;
			match++;
		    }
		    break;

		  case B_ATBAS:
		    if ((c = sstrcmp(p2, atbasickwcc, 0, NUM_ATBCC)) !=KW_NONE) {
			*p1++ = c + 0xcc;
			p2 += kwlen;
			match++;
		    }
		}  /* switch */
	    } /* !quote */

	    if (!match) {
		if (*p2 == 0x7e)		/*  '~' is ASCII for 'pi' */
		    *p1++ = 0xff;

		else if ((*p2 >= 0x5b) && (*p2 <= 0x7e))
		    *p1++ = *p2 ^ 0x20;

		else if ((*p2 >= 'A') && (*p2 <= 'Z'))
		    *p1++ = *p2 | 0x80;

		else
		    *p1++ = *p2;

		++p2;
	    } /* match */
	} /* while */

	*p1 = 0;
	if ((len = strlen(line) ) > 0) {
	    addr += len + 5;
	    fprintf(dest, "%c%c%c%c%s%c", addr & 255, (addr>>8) & 255,
		   linum & 255, (linum>>8) & 255, line, '\0');

	    linum += 2; /* auto line numbering by default */
	}
    } /* while */

    fprintf(dest, "%c%c", 0, 0);	/* program end marker */
}


static unsigned char sstrcmp(line, wordlist, token, maxitems)
     unsigned char *line;
     char **wordlist;
     int    token, maxitems;
{
    int     j;
    char   *p, *q;

    kwlen = 1;
    /* search for keyword */
    for (; token < maxitems; token++) {
	for (p = wordlist[token], q = (char *)line, j = 0;
	     *p && *q && *p == *q; p++, q++, j++);

	/*fprintf (stderr,
		 "compare %s %s - %d %d\n", wordlist[token], line, j, kwlen);*/

	/* found an exact or abbreviated keyword
	 */
	if (j && (!*p || (*p && (*p ^ *q) == 0x20 && j++)) ) {
	    kwlen = j;
	    /*fprintf (stderr, "found %s %2x\n", wordlist[token], token);*/
	    return token;
	}
    } /* for */

    return (KW_NONE);
}
