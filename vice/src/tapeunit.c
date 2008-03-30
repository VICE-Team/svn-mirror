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

/*
 * tapeunit.c -- Cassette drive interface.
 * The cassette interface consists of traps in tape access routines
 * Find, Write Header, Send Data, and Get Data, so that actual operation
 *  can be controlled by C routines.
 * To support turboloaders, it would be necessary to emulate the tape
 * encoding used. That is much slower though. :(
 *
 */

/* Warning!  This implementation works only on the C64.  Before trying to use
   with the other machines, check out all the `FIXME's and the constants
   #defined after the #includes.  */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "tapeunit.h"
#include "maincpu.h"
#include "serial.h"
#include "file.h"
#include "drive.h"
#include "tape.h"
#include "c1541.h"
#include "traps.h"
#include "zfile.h"
#include "tape.h"
#include "utils.h"
#include "resources.h"
#include "charsets.h"

/* #define  DEBUG_TAPE */

/* Warning: this only works for C64!  (FIXME!)  */
#define SET_ST(b)	   STORE(0x90, (LOAD(0x90) | b))
#define BSOUR		   0x95	/* Buffered Character for Serial Bus */
#define TMP_IN		   0xA4	/* Temp Data Area */
#define CAS_BUFFER_OFFSET  (ram[0xB2] | (ram[0xB3] << 8))  /* C64,128 TAPE1 */
#define VERCK		   0x93
#define IRQTMP             0x02A0

static int  fn (void);
static int  t64_find_next (TAPE *tape, char *pattern, int type, BYTE *cbuf);

static const trap_t *tape_traps;
static TAPE  *tape = NULL;
static char *typenames[] = {
    "Tape Index", "CBM Binary", "SFX Archive", "LYNX Archive",
    "P00", "T64 Image", "X64 Disk Image"
};

/* ------------------------------------------------------------------------- */

/* Initialize the tape emulation, using the traps in `trap_list'.  */
int  initialize_tape (const trap_t *trap_list)
{
    const trap_t *p;

    tape_traps = trap_list;
    if (tape_traps != 0) {
        for (p = tape_traps; p->func != NULL; p++)
            set_trap(p);
    }

    /*
     * Create instance of the tape
     */

    tape = (TAPE *) xmalloc(sizeof(TAPE));
    memset (tape, 0, sizeof(TAPE));		/* init all pointers */

    tape->type = DT_TAPE;


    /* Add cassette drive to serial device list, so that Monitor
       can handle it.  The device number (1) is hardcoded.  */
    if (attach_serial_device (1, (char *)tape, "Cassette unit",
                              (int (*)(void *, BYTE *, int))fn,
                              (int (*)(void *, BYTE, int))fn,
                              (int (*)(void *, char *, int, int))fn,
                              (int (*)(void *, int))fn,
                              (void (*)(void *, int))fn)) {
	printf("could not initialize Cassette ????\n");
	return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Functions to attach the tape image files.  */

void  detach_tape_image (TAPE *tape)
{
    if (!tape)
	return;

    if (tape->FileDs != NULL) {
	printf("Detaching tape image %s\n", tape->ActiveName);
	zfclose (tape->FileDs);
	tape->FileDs = NULL;
	tape->ActiveName[0] = 0;		/* Name is used as flag */
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
    if (tmp[0] == 1 && (tmp[1] == 8 || tmp[1] == 4))
	ret = 1;

    /* rewind */
    fseek(fd, 0, SEEK_SET);
    return ret;
}

/* Attach a tape image.  */
int   attach_tape_image (TAPE *tape, const char *name, int mode)
{
    char   realname[24];
    int    reclen = 0;
    int    format = -1, entries = 0;
    long   flen;

    detach_tape_image(tape);

    if (!name || !*name) {
	printf("No name, detaching image.\n");
	return -1;
    }

    if ((tape->FileDs = zfopen (name, mode ? "rwb" : "rb")) == NULL ||
	(flen = file_length(fileno(tape->FileDs))) < 0) {
	perror (name);

	printf("Tape: cannot open file `%s'.\n", name);
	return FD_NOTRD;
    }


    /*
     * Create new file for writing ?  If so, write image header.
     * IF no format is specified, try to figure it out of the extension.
     */

    if (flen == 0) {
	if (!mode) {
	    printf("Tape: Cannot open file `%s'.\n", name);
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

	case TFF_CBM:			/* Do nothing */
	  break;

	case TFF_P00:			/* Write P00 image header */

	default:
	  fprintf(stderr, "cannot create tape %s: %s format specified.\n",
		  name, (format < 0) ? "No" : "Illegal");

	  return FD_BADIMAGE;
	}  /* switch */

	return FD_OK;
    }


    /*
     * Define the format of Image File attached.
     */

    if (read_pc64header(tape->FileDs, realname, &reclen) == FD_OK) {
	format = TFF_P00;
	entries= 1;
    }

    /* Check if the tape image contains valid T64 header. */

    else if ((entries= check_t64_header(tape->FileDs)) > 0) {

	format = TFF_TAPE;

	/* Read the tape directory entries into memory */

	tape->directory = (BYTE *)malloc(/*tapebuf,*/ TAPE_DIR_SIZE * entries);
	assert(tape->directory);

	fread(tape->directory, TAPE_DIR_SIZE, entries, tape->FileDs);
	tape->entry = -1;		/* Number of current entry slot */
    }

    if (format < 0) {			/* There is no header */
	if (is_valid_prgfile(tape->FileDs)) {
	    fprintf (stderr, "Tape: Not Relocatable Program File.\n");
	    format = TFF_CBM;
	} else {
#ifdef DEBUG_TAPE
	    printf ("Tape: realtype %d  entries %d.\n", realtype, entries);
#endif
	    fprintf (stderr, "Tape: Invalid fileformat.\n");
	    return -1;
	}
    }


    tape->ImageFormat = format;
    tape->entries   = entries;
    strcpy(tape->ActiveName, name);

#ifdef DEBUG_TAPE
    printf ("Tape: realtype %d  entries %d.\n", realtype, entries);
#endif

    printf ("Tape file '%s' [%s] attached.\n", name, typenames[format]);

    /* Tape attached: press play. */
    mem_set_tape_sense(1);

    return 0;
}


/* Argh.  We could probably get rid of this.  FIXME!  */
int   check_tape (void)
{
    return 1;
}


/* ------------------------------------------------------------------------- */

/* Cassette device is not attached on the Serial Bus.  */

static int  fn()
{
    return 0x80;
}

/*
   Find the next Tape Header and load it onto the Tape Buffer.
   Both Find-Any-Header and Find-Specific-Header (which calls the previous)
   only need to trap the Tape-Read-Block routine.

   An interface to select the image file is provided.

   Entry Format:
                0	File type flag:
                       1	Binary Program or RAM image
                       3	Relocatable Program
                       4	Data File
                       5	End-of-Tape (EOT)
               1-2	Start Address
               3-4	End Address
               5-21	Filename

    FYI: Simon's Basic supports filenames much longer than 16 characters.
 */

void findheader (void)
{
    BYTE  *s;
    int    start, end;
    int    sense = 0;
    int    err = 0;

    if (tape->FileDs == NULL) {
	fprintf(stderr, "No Tape.\n");
	err++;
    }

    /*
     * Load next filename record
     */

    s = ram + CAS_BUFFER_OFFSET;

    if (tape->FileDs != NULL) {
	++sense;

	switch (tape->ImageFormat) {
	case TFF_P00:
	  DirP00 (tape->FileDs, (char *)(s + CAS_NAME_OFFSET), &start, &end);
	  s[CAS_TYPE_OFFSET]  = CAS_TYPE_BAS;
	  s[CAS_STAD_OFFSET]   = start & 0xFF;
	  s[CAS_STAD_OFFSET+1] = (start >> 8) & 0xFF;
	  s[CAS_ENAD_OFFSET]   = end & 0xFF;
	  s[CAS_ENAD_OFFSET+1] = (end >> 8) & 0xFF;
	  break;

	case TFF_INDEX:
	  ++err;
	  break;

	  /*case FS:*/
	case TFF_CBM:
	  fseek(tape->FileDs, 0, SEEK_SET);
	  if (fread(s + CAS_STAD_OFFSET, 1, 2, tape->FileDs) != 2)
	      ++err;
	  else
	  {
	      start = s[CAS_STAD_OFFSET] + s[CAS_STAD_OFFSET+1]*256;
	      end = start + file_length(fileno(tape->FileDs)) - 2;
	      s[CAS_TYPE_OFFSET] = CAS_TYPE_BAS;
	      s[CAS_ENAD_OFFSET] = end & 0xff;
	      s[CAS_ENAD_OFFSET+1] = (end >> 8) & 0xff;
	      s[CAS_NAME_OFFSET] = 0;
	  }
	  break;

	case TFF_SFX:
	case TFF_LYNX:
	  ++err;
	  break;

	case TFF_TAPE:
	  if (t64_find_next (tape, NULL, 1, ram + CAS_BUFFER_OFFSET) < 0)
	      ++err;

	  break;

	case TFF_DISK:

	  ++err;
	  sense = 0;
	  break;

	default:
	  fprintf (stderr, "Tape: Invalid fileformat.\n");

	}  /* switch */
    }


    if (err)
	s[CAS_TYPE_OFFSET] = CAS_TYPE_EOF;


#ifdef DEBUG_TAPE
    fprintf (stderr, "Tape: find next header (type %d).\n",
	     s[CAS_TYPE_OFFSET]);

    printf (" BUF (b2/b3) %04X   SAL  ac/ad %02X%02X     EAL ae/af %02X%02X\n\
\t\t    STAL c1/c2 %02X%02X  MEMUSS c3/c4 %02X%02X\n",
	   CAS_BUFFER_OFFSET,
	   ram[0xad], ram[0xac],  ram[0xaf], ram[0xae],
	   ram[0xc2], ram[0xc1],  ram[0xc4], ram[0xc3]);
#endif


    /* FIXME: Only works for C64 */
    STORE(0x90, 0);			/* Clear the STATUS word */

    ram[VERCK] = 0;
    ram[IRQTMP] = 0;

    /* Check if STOP has been pressed.  FIXME: only works with C64.  */
    {
	int i, n = LOAD_ZERO(0xC6);

	maincpu_regs.psp.c = 0;
	for (i = 0; i < n; i++)
	    if (LOAD(0x277 + i) == 0x3) {
		maincpu_regs.psp.c = 1;	/* Carry set flags BREAK error. */
		break;
	    }
    }

    maincpu_regs.psp.z = 1;
}


/* Write the Tape Header onto the Tape Buffer and pad the buffer with $20's.
   Then send it to tape along with a 10-second sync leader (short pulses).
   We only need to trap the Tape-Write-Block routine.  For details on the
   Entry format see the description above.

   Always confirm the file to save onto.  */

void writeheader (void)
{
    BYTE  *s;
    int    sense = 0;

    if (tape->FileDs == NULL)
	fprintf(stderr, "No Tape.\n");

    /* Write the filename record on tape */

    s = ram + CAS_BUFFER_OFFSET;

    if (tape->FileDs != NULL) {
	++sense;

	switch (tape->ImageFormat) {

	case TFF_CBM:			/* Plain file - Write Load Address */
	  fwrite (s + CAS_STAD_OFFSET, 1, 2, tape->FileDs);	/* Load Addr */
	  break;


	case TFF_P00:			/* Write P00 image header */
	  if (app_resources.debugFlag)
	      printf("writing PC64 header.\n");

	  write_pc64header(tape->FileDs, (char *)(s + CAS_NAME_OFFSET), 0);
	  fwrite (s + CAS_STAD_OFFSET, 1, 2, tape->FileDs);	/* Load Addr */
	  break;


	case TFF_INDEX:
	  sense = 0;
	  break;


	/*case FS:*/
	case TFF_SFX:
	case TFF_LYNX:
	case TFF_TAPE:
	  fprintf (stderr, "Tape: No writing allowed on format specified.\n");
	  sense = 0;
	  break;

	case TFF_DISK:
	  sense = 0;
	  break;

	default:
	  fprintf (stderr, "Tape: Invalid fileformat.\n");

	}  /* switch */
    }


#ifdef DEBUG_TAPE
    fprintf (stderr, "Tape: write file header.\n");

    printf (" BUF (b2/b3) %04X   SAL  ac/ad %02X%02X     EAL ae/af %02X%02X\n\
\t\t    STAL c1/c2 %02X%02X  MEMUSS c3/c4 %02X%02X\n",
	   CAS_BUFFER_OFFSET,
	   ram[0xad], ram[0xac],  ram[0xaf], ram[0xae],
	   ram[0xc2], ram[0xc1],  ram[0xc4], ram[0xc3]);
#endif


    ram[IRQTMP] = 0;

    maincpu_regs.psp.c = 0;     /* Carry flag sets BREAK error */
    maincpu_regs.psp.z = 1;
}


/* This function is unreliable (open/close) and thus not used. It's easier
   to just check for any transitions on the Motor Control line.  */

#if 0
void checkplay (void)
{

#ifdef DEBUG_TAPE
    fprintf (stderr, "Tape: check play on tape.\n");
#endif

}
#endif


/* ------------------------------------------------------------------------- */

/*
   Cassette Data transfer trap.

   XR flags the function to be performed on IRQ:

               08	Write tape
               0a	Write tape leader
               0c	Normal keyscan
               0e	Read tape

   Luckily enough, these offset values are valid for PETs, C64 and C128.

 */

void tapereceive (void)
{
    int   start, end, len;
    int   st = 0;


    start = (ram[0xc1] | (ram[0xc2] << 8)); /* C64: STAL */
    end   = (ram[0xae] | (ram[0xaf] << 8)); /* C64: EAL */

#ifdef DEBUG_TAPE
    fprintf (stderr, "Tape: start %04X  end %04X  vector %02X\n",
	   start, end, XR);

    printf (" BUF (b2/b3) %04X   SAL  ac/ad %02X%02X     EAL ae/af %02X%02X\n\
\t\t    STAL c1/c2 %02X%02X  MEMUSS c3/c4 %02X%02X\n",
	   CAS_BUFFER_OFFSET,
	   ram[0xad], ram[0xac],  ram[0xaf], ram[0xae],
	   ram[0xc2], ram[0xc1],  ram[0xc4], ram[0xc3]);
#endif


    switch (maincpu_regs.x) {
    case 0x0a:		/* Write Leader */
      break;


    case 0x08:		/* Write Block */

#ifdef DEBUG_TAPE
      fprintf (stderr, "Tape: send next block.\n");
#endif

      len = end - start;

      if (fwrite(ram + start, len, 1, tape->FileDs) == 1) {
	  st |= 0x40;		/* EOF */
      }
      else {
	  st |= 0xB4;		/* All possible errors... */

	  fprintf(stderr, "Error: Tape write failed.\n");
      }
      break;


    case 0x0e:		/* Read Block */

#ifdef DEBUG_TAPE
      fprintf (stderr, "Tape: get next block.\n");
#endif

      len = end - start;

      if (fread(ram + start, len, 1, tape->FileDs) == 1) {
	  st |= 0x40;		/* EOF */
      }
      else {
	  st |= 0x10;

	  fprintf(stderr,
		"Error: Unexpected end of tape. File may be truncated.\n");
      }

      break;

    default:
      fprintf (stderr, "Tape: unknown command.\n");

    }  /* switch */


    /*
     * Set registers and flags like the ML routine does
     * Set up serial success / data
     */

    ram[IRQTMP] = 0;

    SET_ST(st);			/* EOF and possible errors */
    maincpu_regs.psp.c = 0;
}

/* ------------------------------------------------------------------------- */

/* Find the next applicable file on a *.T64 tape.
   On success, filename and both start and end addresses are copied to
   the cassette input buffer.  */

static int  t64_find_next (TAPE *tape, char *pattern, int type, BYTE *cbuf)
{
    char  asciiname[20], newname[20];
    BYTE *dirp;
    long  loc;


    if (tape->entry >= tape->entries) {
	return -1;
    }

    while (1) {

	if (tape->entry < tape->entries - 1) {
	    tape->entry ++;
	} else {
	    tape->entry = 0;
	}

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
		while (*cf == 0x20)  *cf-- = 0;
	    }
	    strcpy(asciiname, newname);
	    petconvstring(asciiname, 1);	/* to ascii */


	    /* Check filename match against search pattern if one is given */

	    if (pattern && *pattern) {
		if (!compare_filename(asciiname, pattern)) {

		    continue;			/* No match, skip the file */
		}
	    }  /* pattern */

	    cbuf[CAS_TYPE_OFFSET] = CAS_TYPE_BAS;
	    memcpy (cbuf + CAS_STAD_OFFSET, dirp + 2, 4);
	    memcpy (cbuf + CAS_NAME_OFFSET, dirp + 16, 16);

	    loc = (dirp[8] | (dirp[9] << 8) | (dirp[10] << 16)
		   | (dirp[11] << 24));
            fseek(tape->FileDs, loc, 0);
	    break;		/* Found a match */

	} /* if dirp */

    }

    return 0;
}

