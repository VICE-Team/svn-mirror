/*
 * serial.c - IEEE and serial device implementation.
 *
 * Written by
 *  Teemu Rantanen   (tvr@cs.hut.fi)
 *  Andre Fachat     (a.fachat@physik.tu-chemnitz.de)
 *
 * Patches by
 *  Ettore Perazzoli (ettore@comm2000.it)
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
 * The serial device interface consists of traps in serial access routines
 * TALK, ACPTR, Send Data, and Get Data, so that actual operation can be
 * controlled by C routines.  Serial.c implements the I/F control whereas
 * the required acknowledge is supplied with the peripheral emulators.
 *
 * The PET goes another way. Here the hardware is emulated directly and
 * then the routines from this file are called.
 * As the PET has a parallel IEEE 488 interface, this doesn't slow
 * down.
 *
 */

/*#define	DEBUG_SERIAL*/

#include "vice.h"

#include <stdio.h>
#include <ctype.h>

#include "maincpu.h"
#include "serial.h"
#include "traps.h"
#include "parallel.h"
#include "utils.h"

#include "drive.h"
#include "tape.h"
#include "tapeunit.h"

#ifdef PRINTER
#include "printer.h"
#endif

/* Warning: these are only valid for the VIC20, C64 and C128, but *not* for
   the PET.  (FIXME?)  */
#define SET_ST(b)		STORE(0x90, (LOAD(0x90) | b))
#define BSOUR			0x95    /* Buffered Character for IEEE Bus */
#define TMP_IN			0xA4

/* Initialized serial devices.  */

static serial_t serialdevices[MAXDEVICES];

/* Store name here for serial-open.  */
#define SERIAL_NAMELENGTH 255

static BYTE SerialBuffer[SERIAL_NAMELENGTH +1];
static int SerialPtr;


/* Flag: On which channel did listen happen to ?  */
static int TrapDevice;
static int TrapSecondary;

/* Flag: Have traps been installed?  */
static int traps_installed = 0;

/* Pointer to list of traps we are using.  */
static const trap_t *serial_traps;

/* ------------------------------------------------------------------------- */

/* Call this if device is not attached: -128 == device not present.  */
static int fn()
{
    return (0x80);
}

/* Handle Serial Bus Commands under Attention.  */
static int  serialcommand()
{
    serial_t *p;
    BYTE    b;
    int     channel;
    int     i, st = 0;

#ifdef DEBUG_SERIAL
    printf("***SerialCommand %02x %02x %02x  BUF(%d) '%s'\n\n",
	   TrapDevice, TrapSecondary, ram[BSOUR],  SerialPtr, SerialBuffer);
#endif

    /*
     * which device ?
     */
    p = &serialdevices[TrapDevice & 0x0f];
    channel = TrapSecondary & 0x0f;

    /* if command on a channel, reset output buffer... */
    if((TrapSecondary & 0xf0)!=0x60) {
	p -> nextok[channel] = 0;
    }

    switch (TrapSecondary & 0xf0) {
	/*
	 * Open Channel
	 */
      case 0x60:
	if (!p -> isopen[channel]) {
	    p -> isopen[channel] = 1;
	    st = (*(p ->openf))(p ->info, NULL, 0, channel);
	    for (i = 0; i < SerialPtr; i++)
		(*(p -> putf))(p -> info, SerialBuffer[i], channel);
	    SerialPtr = 0;
	}
	if (p -> flushf)
	    (*(p -> flushf))(p -> info, channel);
	break;

	/*
	 * Close File
	 */
      case 0xE0:
	p -> isopen[channel] = 0;
	st = (*(p -> closef))(p -> info, channel);
	break;

	/*
	 * Open File
	 */
      case 0xF0:
	if (!p -> isopen[channel]) {
	    p -> isopen[channel] = 1;
	    SerialBuffer[SerialPtr] = 0;
	    st = (*(p -> openf))(p -> info, (char *)SerialBuffer, SerialPtr,
				 channel);
	    SerialPtr = 0;

	    if (st) {
		p -> isopen[channel] = 0;
		(*(p -> closef))(p -> info, channel);

		fprintf(stderr, "Serial: Cannot open file. Status $%02x\n",
                        st);
	    }
	}
	if (p -> flushf)
	    (*(p -> flushf))(p -> info, channel);
	break;

      default:
	printf("Unknown command %02X\n\n", TrapSecondary & 0xff);
    }

    maincpu_regs.psp.i = 0;
    return st;
}


/* ------------------------------------------------------------------------- */

/* Command Serial Bus to TALK, LISTEN, UNTALK, or UNLISTEN, and send the
   Secondary Address to Serial Bus under Attention.  */
void serialattention()
{
    int     b;
    int     st;

    /*
     * Which Secondary Address ?
     */
    b = LOAD(BSOUR);		/* BSOUR - character for serial bus */

#ifdef DEBUG_SERIAL
    printf("SerialSaListen(%02x)\n", b);
#endif

    /* do a flush if unlisten for close and command channel */
    if (b == 0x3f
	    && (((TrapSecondary & 0xf0) == 0xf0)
		|| ((TrapSecondary & 0x0f) == 0x0f))) {
	st = serialcommand();
	SET_ST(st);
    }
    else switch (b & 0xf0) {
      case 0x20:
      case 0x40:
	TrapDevice = b;
	break;

      case 0x60:
      case 0xe0:
	TrapSecondary = b;
	st = serialcommand();
	SET_ST(st);
	break;

      case 0xf0:	/* Open File needs the filename first */
	TrapSecondary = b;
	serialdevices[TrapDevice & 0x0f].isopen[TrapSecondary & 0x0f] = 0;
#ifdef DEBUG_SERIAL
	printf("close output.\n");
#endif
	break;
    }

    if (!(serialdevices[TrapDevice & 0x0f].inuse))
	SET_ST(0x80);

    maincpu_regs.psp.c = 0;
    maincpu_regs.psp.i = 0;
}

/* Send one byte on the serial bus.  */
void serialsendbyte()
{
    int     data, st;
    serial_t *p;

    /*
     * Get data to send from address 0x95
     */
    data = LOAD(BSOUR);		/* BSOUR - character for serial bus */

    p = &serialdevices[TrapDevice & 0x0f];

    if (p -> inuse) {
	if (!p -> isopen[TrapSecondary & 0x0f]) {

#ifdef DEBUG_SERIAL
	    printf("SerialSendByte[%2d] = %02x\n", SerialPtr, data);
#endif
	    /* Store name here */
	    if (SerialPtr < SERIAL_NAMELENGTH)
		SerialBuffer[SerialPtr++] = data;
	} else {

#ifdef DEBUG_SERIAL_1
	    printf("SerialSendByte(%02x)\n", data);
#endif
	    /* Send to device */
	    st = (*(p -> putf))(p -> info, data, TrapSecondary & 0x0f);
	    SET_ST(st);
	}
    }
    else {	/* Not present */
	SET_ST(0x83);
    }

    maincpu_regs.psp.c = 0;
    maincpu_regs.psp.i = 0;
}

/* Receive one byte from the serial bus.  */
void serialreceivebyte()
{
    int     st = 0, secadr = TrapSecondary & 0x0f;
    BYTE    data;
    serial_t *p;

    p = &serialdevices[TrapDevice & 0x0f];

    /* get next byte if necessary */
    if(! (p -> nextok[secadr]) ) {
      st = (*(p -> getf))(p -> info, &(p -> nextbyte[secadr]), secadr);
    }
    /* move byte from buffer to output */
    data = p -> nextbyte[secadr];
    p -> nextok[secadr] = 0;
    /* fill buffer again */
    if(!st) {
      st = (*(p -> getf))(p -> info, &(p -> nextbyte[secadr]), secadr);
      if(!st) p -> nextok[secadr] = 1;
    }

#ifdef DEBUG_SERIAL_1
    printf("SerialReceiveByte(%02x '%c', st=%02x)\n",
		data, isprint(data)?data:' ', st);
#endif

    /* Set up serial success / data.  */
    if (st)
        SET_ST(st);
    STORE(TMP_IN, data);

    /* Set registers (PC, AC and CARRY) like the Kernal routine does.  */
    maincpu_regs.a = data;
    maincpu_regs.psp.n = (data & 0x80) ? 1 : 0;
    maincpu_regs.psp.z = data ? 0 : 1;
    maincpu_regs.psp.c = 0;
    maincpu_regs.psp.i = 0;
}


/* Kernal loops serial-port (0xdd00) to see when serial is ready: fake it.
   EEA9 Get serial data and clk in (TKSA subroutine).  */

void trap_serial_ready(void)
{
    maincpu_regs.a = 1;
    maincpu_regs.psp.n = 0;
    maincpu_regs.psp.z = 0;
    maincpu_regs.psp.i = 0;
}

/* ------------------------------------------------------------------------- */

/* These routines work for IEEE488 emulation on both C64 and PET.  */
static int  parallelcommand()
{
    serial_t *p;
    BYTE    b;
    int     channel;
    int     i, st = 0;

#ifdef DEBUG_SERIAL
    printf("***ParallelCommand %02x %02x %02x  BUF(%d) '%s'\n\n",
	   TrapDevice, TrapSecondary, ram[BSOUR],  SerialPtr, SerialBuffer);
#endif

    /* which device ? */
    p = &serialdevices[TrapDevice & 0x0f];
    channel = TrapSecondary & 0x0f;

    /* if command on a channel, reset output buffer... */
    if((TrapSecondary & 0xf0)!=0x60) {
	p -> nextok[channel] = 0;
	p -> lastok[channel] = 0;
    }

    switch (TrapSecondary & 0xf0) {
      case 0x60:
	/* Open Channel */
	if (!p -> isopen[channel]) {
	    p -> isopen[channel] = 1;
	    st = (*(p ->openf))(p ->info, NULL, 0, channel);
	    for (i = 0; i < SerialPtr; i++)
		(*(p -> putf))(p -> info, SerialBuffer[i], channel);
	    SerialPtr = 0;
	}
	if (p -> flushf)
	    (*(p -> flushf))(p -> info, channel);

	if((!st) && ((TrapDevice&0xf0) == 0x40) ) {
	    st = parallelreceivebyte(&b, 1) & 0xbf; /* any error, except eof */
	}

	break;

      case 0xE0:
	/* Close File */
	p -> isopen[channel] = 0;
	st = (*(p -> closef))(p -> info, channel);
	break;

      case 0xF0:
	/* Open File */
	if (!p -> isopen[channel]) {
	    p -> isopen[channel] = 1;
	    SerialBuffer[SerialPtr] = 0;
	    st = (*(p -> openf))(p -> info, (char *)SerialBuffer, SerialPtr,
				 channel);
	    SerialPtr = 0;

	    if (st) {
		p -> isopen[channel] = 0;
		(*(p -> closef))(p -> info, channel);
		fprintf(stderr, "Serial: Cannot open file. Status $%02x\n",
                        st);
	    }
	}
	if (p -> flushf)
	    (*(p -> flushf))(p -> info, channel);
	break;

      default:
	printf("Unknown command %02X\n\n", TrapSecondary & 0xff);
    }
    return (st);
}

int parallelattention(int b)
{
    int     st=0;

    if (pardebug)
	printf("ParallelAttention(%02x)\n", b);

    if (b == 0x3f
        && (((TrapSecondary & 0xf0) == 0xf0)
            || ((TrapSecondary & 0x0f) == 0x0f))) {
	st = parallelcommand();
    }
    else switch (b & 0xf0) {
      case 0x20:
      case 0x40:
	TrapDevice = b;
	break;

      case 0x60:
      case 0xe0:
	TrapSecondary = b;
	st |= parallelcommand();
	break;

      case 0xf0:	/* Open File needs the filename first */
	TrapSecondary = b;
	serialdevices[TrapDevice & 0x0f].isopen[TrapSecondary & 0x0f] = 0;
	if (pardebug) printf("close output.\n");
	break;
    }

    if (!(serialdevices[TrapDevice & 0x0f].inuse))
	st |= 0x80;

    if ((b==0x3f) || (b==0x5f) ) {
	TrapDevice = 0;
	TrapSecondary = 0;
    }

    if (pardebug)
        printf("ParallelAttention(%02x)->TrapDevice=%02x, st=%04x\n",
               b, TrapDevice, st+(TrapDevice<<8));

    st |= TrapDevice<<8;
    return st;
}

int parallelsendbyte(int data)
{
    int st = 0;
    serial_t *p;

    p = &serialdevices[TrapDevice & 0x0f];

    if (p -> inuse) {
	if (!p -> isopen[TrapSecondary & 0x0f]) {

	    if (pardebug)
		printf("SerialSendByte[%2d] = %02x\n", SerialPtr, data);
	    /* Store name here */
	    if (SerialPtr < SERIAL_NAMELENGTH)
		SerialBuffer[SerialPtr++] = data;
	} else {
	    /* Send to device */
	    st = (*(p -> putf))(p -> info, data, TrapSecondary & 0x0f);
	}
    } else {	/* Not present */
	st = 0x83;
    }

    return st + (TrapDevice<<8);
}

int parallelreceivebyte(BYTE *data, int fake)
{
    int     st = 0, secadr = TrapSecondary & 0x0f;
    serial_t *p;

    p = &serialdevices[TrapDevice & 0x0f];

    /* first fill up buffers */
    if (!p->lastok[secadr]) {
        p->lastok[secadr] = p->nextok[secadr];
        p->lastbyte[secadr] = p->nextbyte[secadr];
        p->lastst[secadr] = p->nextst[secadr];
        p->nextok[secadr] = 0;
        if (!p->lastok[secadr]) {
            p->lastst[secadr] =
		(*(p -> getf))(p -> info, &(p -> lastbyte[secadr]), secadr);
            p->lastok[secadr] = 1;
        }
    }
    if ((!p->nextok[secadr]) && (!p->lastst[secadr])) {
        p->nextst[secadr] =
            (*(p -> getf))(p -> info, &(p -> nextbyte[secadr]), secadr);
        p->nextok[secadr] = 1;
    }

    *data = p->lastbyte[secadr];
    if (!fake)
        p->lastok[secadr] = 0;
    st = p->nextok[secadr] ? p->nextst[secadr] :
        ( p->lastok[secadr] ? p->lastst[secadr] : 2);
    st += TrapDevice<<8;

    if (pardebug)
        printf("receive: sa=%02x lastb = %02x,  ok=%s, st=%04x, nextb = %02x, "
               "ok=%s, st=%04x\n", secadr,
               p->lastbyte[secadr], p->lastok[secadr]?"ok":"no",
               p->lastst[secadr],
               p->nextbyte[secadr], p->nextok[secadr]?"ok":"no",
               p->nextst[secadr]);

    if ((!fake) && p->nextok[secadr] && p->nextst[secadr])
        p->nextok[secadr] = 0;

    if (pardebug)
        printf("faked = %d, return data =%02x ('%c'), st = %04x\n",
               fake, *data, isprint(*data)?*data:'_', st);

    return st;
}

/* ------------------------------------------------------------------------- */

int initialize_serial(const trap_t *trap_list)
{
    int     i;

    /* Remove installed traps, if any.  */
    remove_serial_traps();

    /* Install specified traps.  */
    serial_traps = trap_list;
    install_serial_traps();

    /*
     * Clear serial device functions
     */
    for (i = 0; i < MAXDEVICES; i++) {
	serial_t *p;

	p = &serialdevices[i];

	p -> getf   = (int (*)(void *, BYTE *, int))fn;
	p -> putf   = (int (*)(void *, BYTE, int))fn;
	p -> openf  = (int (*)(void *, char *, int , int))fn;
	p -> closef = (int (*)(void *, int))fn;
	p -> flushf = (void (*)(void *, int))NULL;
    }

    return 0;
}

int	install_serial_traps(void)
{
    if (!traps_installed && serial_traps != NULL) {
        const trap_t *p;

        for (p = serial_traps; p->func != NULL; p++)
            set_trap(p);
	traps_installed = 1;
    }
    return 0;
}


int	remove_serial_traps(void)
{
    if (traps_installed && serial_traps != NULL) {
        const trap_t *p;

        for (p = serial_traps; p->func != NULL; p++)
            remove_trap(p);
	traps_installed = 0;
    }
    return 0;
}


int     attach_serial_device(int device, char *var, char *name,
			     int (*getf)(void *, BYTE * , int),
			     int (*putf)(void *, BYTE , int),
			     int (*openf)(void *, char *, int , int),
			     int (*closef)(void *, int),
			     void (*flushf)(void *, int))
{
    serial_t *p;
    int i;

    if (device < 0 || device >= MAXDEVICES)
	return 1;

    p = &serialdevices[device];

    if (p -> inuse) {
	printf("warning. serial device %d (%s) in use\n", device, name);
	return 1;
    }
    p -> getf  = getf;
    p -> putf  = putf;
    p -> openf = openf;
    p -> closef = closef;
    p -> flushf = flushf;

    p -> inuse = 1;
    p -> info  = var; /* run-time data storage */
    p -> name  = stralloc(name);

    for(i = 0; i < 16; i++) {
	p -> nextok[i] = 0;
    }
    return 0;
}


/* Attach a file for use to specified peripheral.  The device numbed is
   searched in filename if device # == -1.  Note that tape and RS-232 are
   handled here as well.  */
int  serial_select_file(int type, int number, const char *file)
{
    serial_t *p;


    if (number < 0 && ( (number = find_devno(-1, file)) < 0) && type ) {
	if (type & DT_TAPE)
	    number = 1;
#ifdef PRINTER
	if (type & DT_PRINTER)
	    number = 4;
#endif
	else if (type & DT_DISK)
	    number = 8;
    }
    if (number < 0 || number >= MAXDEVICES) {
	fprintf (stderr, "\nIllegal device number %d.\n", number);
	return -1;
    }

    p = &serialdevices[number];

    /* WARNING: In older versions of this code, we initialized the devices
       which had not been initialized yet, but this is not done anymore.  All
       the devices must have been explicitly initialized before we get here.
       EP 98-04-24.  */
    if (!p || !(p->inuse))
        return -1;

    /* should be based on p -> info.type */

    switch (number) {

      case 1:
	return (attach_tape_image ((TAPE *)p -> info, file, 0));

#ifdef PRINTER
      case 4:
      case 5:
        return attach_printout ((PRINTER *)p -> info, file, 0);
#endif

      case 8:
      case 9:
      case 10:
      case 11:
        return attach_floppy_image((DRIVE *)(p->info), file, 0);

      default:
        return -1;
    }

    return 0;
}


/* Detach and kill serial devices.  Detach all (shutdown) if device# == -1.  */
int  remove_serial(int number)
{
    serial_t *p;
    int  i;

    if (number < 0) {
	for (i = 0; i < MAXDEVICES; i++) {

	    p = &serialdevices[i];

	    if (p && p->inuse) {
		if (number == -2) {	/* QUERY mode */
		    printf("    Unit #%d: %s.\n", i, p -> name); /* add file */
		}
		else {
#if (defined(DEBUG) || defined(DEBUG_SERIAL))
		    printf("remove unit #%d.\n", i);
#endif
		    remove_serial(i);
		}
	    }
	}
    }		/* number < 0 */
    else {
	if (number < 0 || number >= MAXDEVICES) {
	    fprintf (stderr, "\nIllegal device number %d.\n", number);
	    return -1;
	}

	p = &serialdevices[number];

	if (!p || !(p->inuse)) {
	    printf("I'm not here.  ;)\n");
	}
	else
	    switch (number) {  /* should be based on actual type ... */
	      case 1:
		detach_tape_image ((TAPE *)p -> info);
		break;
#ifdef PRINTER
	      case 4:
	      case 5:
		detach_printout((PRINTER *)p -> info);
		break;
#endif
	      case 8:
	      case 9:
	      case 10:
	      case 11:
		detach_floppy_image((DRIVE *)p -> info);
		break;

	      default:
		printf ("Orphan device #%d\n", number);
		return -1;
	    }

    } /* else */

    return (0);
}


/* Return the name of the image attached at unit `number', NULL if none.  */
char *image_file_name(int number)
{
    char *p;

    if (!serialdevices[number].inuse)
	return NULL;

    switch (number) {
      case 1:
	p = ((TAPE *)serialdevices[number].info)->ActiveName;
	if (*p != 0)
	    return p;
	else
	    return NULL;
      case 8:
      case 9:
      case 10:
	p = ((DRIVE *)serialdevices[number].info)->ActiveName;
	if (*p != 0)
	    return p;
	else
	    return NULL;
      default:
	return NULL;
    }
}


/* Close all files.  */
void serial_reset(void)
{
    int i,j;
    serial_t *p;

    for (i = 0; i < MAXDEVICES; i++) {
	if (serialdevices[i].inuse) {
	    p = &serialdevices[i];
	    for (j = 0; j < 16; j++) {
		if (p->isopen[j]) {
		    p->isopen[j] = 0;
		    (*(p->closef))(p->info, j);
		}
	    }
	}
    }
}
