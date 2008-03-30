/*
 * tapeunit.c - (Guess what?) Tape unit emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *
 * Patches by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

/* tapeunit.c -- Cassette drive interface.  The cassette interface consists
   of traps in tape access routines Find, Write Header, Send Data, and Get
   Data, so that actual operation can be controlled by C routines.  To
   support turboloaders, it would be necessary to emulate the tape encoding
   used. That is much slower though. :( */

/* Warning!  This implementation works only on the C64.  Before trying to use
   with the other machines, check out all the `FIXME's and the constants
   #defined after the #includes.  */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef __riscos
#include "ROlib.h"
#else
#include <unistd.h>
#endif
#endif

#include "tapeunit.h"
#include "maincpu.h"
#include "serial.h"
#include "file.h"
#include "vdrive.h"
#include "tape.h"
#include "c1541.h"
#include "traps.h"
#include "zfile.h"
#include "tape.h"
#include "utils.h"
#include "resources.h"
#include "charsets.h"

/* #define  DEBUG_TAPE */

#define SET_ST(b)	   mem_store(status, (mem_read(status) | b))
#define CAS_BUFFER_OFFSET  (mem_read(bufpaddr) | (mem_read(bufpaddr+1) << 8))

/* CPU addresses for tape routine variables */
static int bufpaddr;
static int status;
static int verfl;
static int irqtmp;
static int stal;
static int eal;
static int kbdbuf;
static int nkeys;
static ADDRESS irqval;

static int tape_is_initialized = 0;

static int fn(void);
static int t64_find_next(TAPE *tape, char *pattern, int type, BYTE *cbuf);

static const trap_t *tape_traps;
static TAPE *tape = NULL;
static char *typenames[] = {
    "Tape Index", "CBM Binary", "SFX Archive", "LYNX Archive",
    "P00", "T64 Image", "X64 Disk Image"
};

/* ------------------------------------------------------------------------- */

/* Initialize the tape emulation, using the traps in `trap_list'.  */
int tape_init(int _bufpaddr, int _status, int _verfl, int _irqtmp,
              ADDRESS _irqval, int _stal, int _eal, const trap_t *trap_list,
	      int _kbdbuf, int _nkeys)
{
    const trap_t *p;

    /* Set addresses of tape routine variables.  */
    status = _status;
    bufpaddr = _bufpaddr;
    verfl = _verfl;
    irqtmp = _irqtmp;
    irqval = _irqval;
    stal = _stal;
    eal = _eal;
    kbdbuf = _kbdbuf;
    nkeys = _nkeys;

    tape_traps = trap_list;
    if (tape_traps != 0) {
	for (p = tape_traps; p->func != NULL; p++)
	    traps_add(p);
    }
    if (tape_is_initialized)
	return 0;
    tape_is_initialized = 1;

    /* Create instance of the tape.  */

    tape = (TAPE *) xmalloc(sizeof(TAPE));
    memset(tape, 0, sizeof(TAPE));	/* init all pointers */

    tape->type = DT_TAPE;


    /* Add cassette drive to serial device list, so that Monitor
       can handle it.  The device number (1) is hardcoded.  */
    if (serial_attach_device(1, (char *) tape, "Cassette unit",
			     (int (*)(void *, BYTE *, int)) fn,
			     (int (*)(void *, BYTE, int)) fn,
			     (int (*)(void *, char *, int, int)) fn,
			     (int (*)(void *, int)) fn,
			     (void (*)(void *, int)) fn)) {
	fprintf(logfile, "could not initialize Cassette ????\n");
	return -1;
    }
    return 0;
}

/* ------------------------------------------------------------------------- */

/* Functions to attach the tape image files.  */

void tape_detach_image(TAPE *tape)
{
    if (!tape)
	return;

    if (tape->FileDs != NULL) {
	fprintf(logfile, "Detaching tape image %s\n", tape->ActiveName);
	zfclose(tape->FileDs);
	tape->FileDs = NULL;
	tape->ActiveName[0] = 0;	/* Name is used as flag */
    }
    if (tape->directory)
	free(tape->directory);
    tape->directory = NULL;

    /* Tape detached, depress play. */
    mem_set_tape_sense(0);
}

/* Check if this image is a CBM binary program file.  */
static int is_valid_prgfile(FILE *fd)
{
    int len, ret = 0;
    BYTE tmp[2];

    /* size is right? */
    fseek(fd, 0, SEEK_SET);
    len = file_length(fileno(fd));
    if (len >= 65536 || len < 2)
	return 0;

    /* check load address */
    if (fread(tmp, 1, 2, fd) != 2)
	return 1;

    if (tmp[0] == 1 && (tmp[1] == 8	/* C64 */
			|| tmp[1] == 4	/* PET */
			|| tmp[1] == 0x12 || tmp[1] == 0x10	/* VIC20 */
			|| tmp[1] == 0x1c || tmp[1] == 0x40))	/* C128 */
	ret = 1;

    /* rewind */
    fseek(fd, 0, SEEK_SET);
    return ret;
}

/* Attach a tape image.  */
int tape_attach_image(TAPE *tape, const char *name, int mode)
{
    char realname[24];
    int reclen = 0;
    int format = -1, entries = 0;
    long flen;

    tape_detach_image(tape);

    if (!name || !*name) {
	fprintf(logfile, "No name, detaching image.\n");
	return -1;
    }
    tape->FileDs = zfopen(name, mode ? "rwb" : "rb");
    if ((tape->FileDs == NULL) || ((flen = file_length(fileno(tape->FileDs))) < 0)) {
	perror(name);

	fprintf(logfile, "Tape: cannot open file `%s'.\n", name);
	return FD_NOTRD;
    }

    /* Create new file for writing ?  If so, write image header.  IF no
       format is specified, try to figure it out of the extension.  */

    if (flen == 0) {
	if (!mode) {
	    fprintf(logfile, "Tape: Cannot open file `%s'.\n", name);
	    return FD_BADIMAGE;
	}
	/* Writing: Try to get format out of filename ... */

	if (format < 0) {
	    if (is_pc64name(name) >= 0)
		format = TFF_P00;
	    else if (is_t64name(name) > 0)
		format = TFF_TAPE;
	}
	switch (format) {

	  case TFF_CBM:	/* Do nothing */
            break;

	  case TFF_P00:	/* Write P00 image header */

	  default:
            fprintf(errfile, "cannot create tape %s: %s format specified.\n",
                    name, (format < 0) ? "No" : "Illegal");

            return FD_BADIMAGE;
	}			/* switch */

	return FD_OK;
    }

    /* Define the format of Image File attached.  */
    if (read_pc64header(tape->FileDs, realname, &reclen) == FD_OK) {
	format = TFF_P00;
	entries = 1;
    }

    /* Check if the tape image contains valid T64 header.  */
    else if ((entries = check_t64_header(tape->FileDs)) > 0) {

	format = TFF_TAPE;

	/* Read the tape directory entries into memory.  */
	tape->directory = (BYTE *) malloc(TAPE_DIR_SIZE * entries);
	assert(tape->directory);

	fread(tape->directory, TAPE_DIR_SIZE, entries, tape->FileDs);
	tape->entry = -1;	/* Number of current entry slot.  */
    }
    if (format < 0) {		/* There is no header.  */
	if (is_valid_prgfile(tape->FileDs)) {
	    fprintf(errfile, "Tape: Not Relocatable Program File.\n");
	    format = TFF_CBM;
	} else {
#ifdef DEBUG_TAPE
	    fprintf(logfile, "Tape: realtype %d  entries %d.\n", format, entries);
#endif
	    fprintf(errfile, "Tape: Invalid fileformat.\n");
	    return -1;
	}
    }
    tape->ImageFormat = format;
    tape->entries = entries;
    strcpy(tape->ActiveName, name);

#ifdef DEBUG_TAPE
    fprintf(logfile, "Tape: realtype %d  entries %d.\n", format, entries);
#endif

    fprintf(logfile, "Tape file '%s' [%s] attached.\n", name, typenames[format]);

    /* Tape attached: press play. */
    mem_set_tape_sense(1);

    return 0;
}


/* Argh.  We could probably get rid of this.  FIXME!  */
int check_tape(void)
{
    return 1;
}

/* ------------------------------------------------------------------------- */

/* Cassette device is not attached on the Serial Bus.  */

static int fn()
{
    return 0x80;
}

/*
   Find the next Tape Header and load it onto the Tape Buffer.
   Both Find-Any-Header and Find-Specific-Header (which calls the previous)
   only need to trap the Tape-Read-Block routine.

   An interface to select the image file is provided.

   Entry Format:
   0    File type flag:
   1    Binary Program or RAM image
   3    Relocatable Program
   4    Data File
   5    End-of-Tape (EOT)
   1-2  Start Address
   3-4  End Address
   5-21 Filename

   FYI: Simon's Basic supports filenames much longer than 16 characters.
 */

void findheader(void)
{
    BYTE *s;
    int start, end;
    int sense = 0;
    int err = 0;

    if (tape->FileDs == NULL) {
	fprintf(errfile, "No Tape.\n");
	err++;
    }
#ifdef DEBUG_TAPE
    fprintf(errfile, "findheader\n");
#endif

    /*
     * Load next filename record
     */

    s = ram + CAS_BUFFER_OFFSET;

    if (tape->FileDs != NULL) {
	++sense;

	switch (tape->ImageFormat) {
	  case TFF_P00:
            DirP00(tape->FileDs, (char *) (s + CAS_NAME_OFFSET), &start, &end);
            s[CAS_TYPE_OFFSET] = CAS_TYPE_PRG;
            s[CAS_STAD_OFFSET] = start & 0xFF;
            s[CAS_STAD_OFFSET + 1] = (start >> 8) & 0xFF;
            s[CAS_ENAD_OFFSET] = end & 0xFF;
            s[CAS_ENAD_OFFSET + 1] = (end >> 8) & 0xFF;
            break;

	  case TFF_INDEX:
            ++err;
            break;

            /*case FS: */
	  case TFF_CBM:
            fseek(tape->FileDs, 0, SEEK_SET);
            if (fread(s + CAS_STAD_OFFSET, 1, 2, tape->FileDs) != 2)
                ++err;
            else {
                start = s[CAS_STAD_OFFSET] + s[CAS_STAD_OFFSET + 1] * 256;
                end = start + file_length(fileno(tape->FileDs)) - 2;
                s[CAS_TYPE_OFFSET] = CAS_TYPE_PRG;
                s[CAS_ENAD_OFFSET] = end & 0xff;
                s[CAS_ENAD_OFFSET + 1] = (end >> 8) & 0xff;
                s[CAS_NAME_OFFSET] = 0;
            }
            break;

	  case TFF_SFX:
	  case TFF_LYNX:
            ++err;
            break;

	  case TFF_TAPE:
            if (t64_find_next(tape, NULL, 1, ram + CAS_BUFFER_OFFSET) < 0)
                ++err;

            break;

	  case TFF_DISK:

            ++err;
            sense = 0;
            break;

	  default:
            fprintf(errfile, "Tape: Invalid fileformat.\n");

	}			/* switch */
    }
    if (err) {
	s[CAS_TYPE_OFFSET] = CAS_TYPE_EOF;
    }
#ifdef DEBUG_TAPE
    fprintf(errfile, "Tape: find next header (type %d).\n",
	    s[CAS_TYPE_OFFSET]);

    fprintf(logfile, " BUF (b2/b3) %04X   SAL  ac/ad %02X%02X     EAL ae/af %02X%02X\n\
\t\t    STAL c1/c2 %02X%02X  MEMUSS c3/c4 %02X%02X\n",
	   CAS_BUFFER_OFFSET,
	   ram[0xad], ram[0xac], ram[eal + 1], ram[eal],
	   ram[stal + 1], ram[stal], ram[0xc4], ram[0xc3]);
#endif

    mem_store(status, 0);	/* Clear the STATUS word */

    mem_store(verfl, 0);
    if (irqtmp) {
	mem_store(irqtmp, irqval & 0xff);
	mem_store(irqtmp + 1, (irqval >> 8) & 0xff);
    }
    /* Check if STOP has been pressed.  FIXME: only works with C64.  */
    {
	int i, n = mem_read(nkeys);

        MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
	for (i = 0; i < n; i++) {
	    if (mem_read(kbdbuf + i) == 0x3) {
                MOS6510_REGS_SET_CARRY(&maincpu_regs, 1);
		break;
	    }
        }
    }

    MOS6510_REGS_SET_ZERO(&maincpu_regs, 1);
}


/* Write the Tape Header onto the Tape Buffer and pad the buffer with $20's.
   Then send it to tape along with a 10-second sync leader (short pulses).
   We only need to trap the Tape-Write-Block routine.  For details on the
   Entry format see the description above.

   Always confirm the file to save onto.  */

void writeheader(void)
{
    BYTE *s;
    int sense = 0;
#ifdef DEBUG_TAPE
    fprintf(errfile, "writeheader\n");
#endif

    if (tape->FileDs == NULL) {
	fprintf(errfile, "No Tape.\n");
    }
    /* Write the filename record on tape */

    s = ram + CAS_BUFFER_OFFSET;

    if (tape->FileDs != NULL) {
	++sense;

	switch (tape->ImageFormat) {

	  case TFF_CBM:	/* Plain file - Write Load Address */
            fwrite(s + CAS_STAD_OFFSET, 1, 2, tape->FileDs);	/* Load Addr */
            break;


	  case TFF_P00:	/* Write P00 image header */
            write_pc64header(tape->FileDs, (char *) (s + CAS_NAME_OFFSET), 0);
            fwrite(s + CAS_STAD_OFFSET, 1, 2, tape->FileDs);	/* Load Addr */
            break;


	  case TFF_INDEX:
            sense = 0;
            break;


            /*case FS: */
	  case TFF_SFX:
	  case TFF_LYNX:
	  case TFF_TAPE:
            fprintf(errfile, "Tape: No writing allowed on format specified.\n");
            sense = 0;
            break;

	  case TFF_DISK:
            sense = 0;
            break;

	  default:
            fprintf(errfile, "Tape: Invalid fileformat.\n");

	}			/* switch */
    }
#ifdef DEBUG_TAPE
    fprintf(errfile, "Tape: write file header.\n");

    fprintf(logfile, " BUF (b2/b3) %04X   SAL  ac/ad %02X%02X     EAL ae/af %02X%02X\n\
\t\t    STAL c1/c2 %02X%02X  MEMUSS c3/c4 %02X%02X\n",
	   CAS_BUFFER_OFFSET,
	   ram[0xad], ram[0xac], ram[eal + 1], ram[eal],
	   ram[stal + 1], ram[stal], ram[0xc4], ram[0xc3]);
#endif


    if (irqtmp) {
	mem_store(irqtmp, irqval & 0xff);
	mem_store(irqtmp + 1, (irqval >> 8) & 0xff);
    }

    /* Carry flag sets BREAK error */
    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_ZERO(&maincpu_regs, 1);
}


/* This function is unreliable (open/close) and thus not used. It's easier
   to just check for any transitions on the Motor Control line.  */

#if 0
void checkplay(void)
{

#ifdef DEBUG_TAPE
    fprintf(errfile, "Tape: check play on tape.\n");
#endif

}

#endif


/* ------------------------------------------------------------------------- */

/*
   Cassette Data transfer trap.

   XR flags the function to be performed on IRQ:

   08   Write tape
   0a   Write tape leader
   0c   Normal keyscan
   0e   Read tape

   Luckily enough, these offset values are valid for PETs, C64 and C128.

 */

void tapereceive(void)
{
    int start, end, len;
    int st = 0;


    start = (mem_read(stal) | (mem_read(stal + 1) << 8));	/* C64: STAL */
    end = (mem_read(eal) | (mem_read(eal + 1) << 8));	/* C64: EAL */

#ifdef DEBUG_TAPE
    fprintf(errfile, "TapeReceive: start %04X  end %04X\n",
	    start, end);

    fprintf(logfile, " BUF (b2/b3) %04X   SAL  ac/ad %02X%02X     EAL ae/af %02X%02X\n\
\t\t    STAL c1/c2 %02X%02X  MEMUSS c3/c4 %02X%02X\n",
	   CAS_BUFFER_OFFSET,
	   ram[0xad], ram[0xac], ram[eal + 1], ram[eal],
	   ram[stal + 1], ram[stal], ram[0xc4], ram[0xc3]);
#endif


    switch (MOS6510_REGS_GET_X(&maincpu_regs)) {
      case 0x0a:		/* Write Leader */
        break;


      case 0x08:		/* Write Block */

#ifdef DEBUG_TAPE
        fprintf(errfile, "Tape: send next block.\n");
#endif

        len = end - start;

        if (fwrite(ram + start, len, 1, tape->FileDs) == 1) {
            st |= 0x40;	/* EOF */
        } else {
            st |= 0xB4;	/* All possible errors... */

            fprintf(errfile, "Error: Tape write failed.\n");
        }
        break;


      case 0x0e:		/* Read Block */

#ifdef DEBUG_TAPE
        fprintf(errfile, "Tape: get next block.\n");
#endif

        len = end - start;

        if (fread(ram + start, len, 1, tape->FileDs) == 1) {
            st |= 0x40;	/* EOF */
        } else {
            st |= 0x10;

            fprintf(errfile,
                    "Error: Unexpected end of tape. File may be truncated.\n");
        }

        break;

      default:
        fprintf(errfile, "Tape: unknown command.\n");

    }				/* switch */


    /*
     * Set registers and flags like the ML routine does
     * Set up serial success / data
     */

    if (irqtmp) {
	mem_store(irqtmp, irqval & 0xff);
	mem_store(irqtmp + 1, (irqval >> 8) & 0xff);
    }
    SET_ST(st);			/* EOF and possible errors */

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

/* ------------------------------------------------------------------------- */

/* Find the next applicable file on a *.T64 tape.
   On success, filename and both start and end addresses are copied to
   the cassette input buffer.  */

static int t64_find_next(TAPE *tape, char *pattern, int type, BYTE *cbuf)
{
    char asciiname[20], newname[20];
    BYTE *dirp;
    long loc;
    int start_entry = -1;

    if (tape->entry >= tape->entries) {
	return -1;
    }

    while (1) {

	if (tape->entry < tape->entries - 1)
	    tape->entry++;
	else
	    tape->entry = 0;

        if (start_entry < 0)
            start_entry = tape->entry;
        else if (start_entry == tape->entry)
            break;

	dirp = tape->directory + TAPE_DIR_SIZE * tape->entry;

	if (!type && *dirp == 0) {	/* found empty slot */
	    return 0;
	}
	if (type && *dirp == 1) {	/* found a file */

	    /* Get filename and remove trailing spaces */
	    memccpy(newname, &dirp[16], 0xa0, 16);
	    newname[16] = 0;
	    {
		char *cf = &newname[15];
		while (*cf == 0x20)
		    *cf-- = 0;
	    }
	    strcpy(asciiname, newname);
	    petconvstring(asciiname, 1);	/* to ascii */


	    /* Check filename match against search pattern if one is given */

	    if (pattern && *pattern) {
		if (!compare_filename(asciiname, pattern)) {

		    continue;	/* No match, skip the file */
		}
	    }			/* pattern */
	    cbuf[CAS_TYPE_OFFSET] = CAS_TYPE_PRG;
	    memcpy(cbuf + CAS_STAD_OFFSET, dirp + 2, 4);
	    memcpy(cbuf + CAS_NAME_OFFSET, dirp + 16, 16);

	    loc = (dirp[8] | (dirp[9] << 8) | (dirp[10] << 16)
		   | (dirp[11] << 24));
	    fseek(tape->FileDs, loc, 0);
	    break;		/* Found a match */

	}			/* if dirp */
    }

    return 0;
}
