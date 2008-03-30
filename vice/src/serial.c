/*
 * serial.c - IEEE and serial device implementation.
 *
 * Written by
 *  Teemu Rantanen   (tvr@cs.hut.fi)
 *  André Fachat     (a.fachat@physik.tu-chemnitz.de)
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

/* FIXME: The `parallel*()' functions definitely need to get outta here!  */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <ctype.h>
#endif

#include "drive.h"
#include "log.h"
#include "maincpu.h"
#include "parallel.h"
#include "serial.h"
#include "tape.h"
#include "traps.h"
#include "utils.h"
#include "vdrive.h"
#include "prdevice.h"

/* Warning: these are only valid for the VIC20, C64 and C128, but *not* for
   the PET.  (FIXME?)  */
#define SET_ST(b)		mem_store(0x90, (mem_read(0x90) | b))
#define BSOUR			0x95	/* Buffered Character for IEEE Bus */
#define TMP_IN			0xA4

/* Initialized serial devices.  */

static serial_t serialdevices[MAXDEVICES];

/* Store name here for serial-open.  */
#define SERIAL_NAMELENGTH 255

static BYTE SerialBuffer[SERIAL_NAMELENGTH + 1];
static int SerialPtr;

/* On which channel did listen happen to?  */
static int TrapDevice;
static int TrapSecondary;

/* Flag: Have traps been installed?  */
static int traps_installed = 0;

/* Pointer to list of traps we are using.  */
static const trap_t *serial_traps;

/* Logging goes here.  */
static log_t serial_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

/* This is just a kludge for the autostart code (see `autostart.c').  */

/* Function to call when EOF happens in `serialreceivebyte()'.  */
static void (*eof_callback_func)(void);

/* Function to call when the `serialattention()' trap is called.  */
static void (*attention_callback_func)(void);

/* ------------------------------------------------------------------------- */

/* Call this if device is not attached: -128 == device not present.  */
static int fn(void)
{
    return (0x80);
}

/* Handle Serial Bus Commands under Attention.  */
static int serialcommand(void)
{
    serial_t *p;
    int channel;
    int i, st = 0;

    /*
     * which device ?
     */
    p = &serialdevices[TrapDevice & 0x0f];
    channel = TrapSecondary & 0x0f;

    /* if command on a channel, reset output buffer... */
    if ((TrapSecondary & 0xf0) != 0x60) {
	p->nextok[channel] = 0;
    }
    switch (TrapSecondary & 0xf0) {
        /*
         * Open Channel
         */
      case 0x60:
        if (p->isopen[channel] == 1) {
            p->isopen[channel] = 2;
            st = (*(p->openf)) (p->info, NULL, 0, channel);
            for (i = 0; i < SerialPtr; i++)
                (*(p->putf)) (p->info, SerialBuffer[i], channel);
            SerialPtr = 0;
        }
        if (p->flushf)
            (*(p->flushf)) (p->info, channel);
        break;

        /*
         * Close File
         */
      case 0xE0:
        p->isopen[channel] = 0;
        st = (*(p->closef)) (p->info, channel);
        break;

        /*
         * Open File
         */
      case 0xF0:
        if (p->isopen[channel]) {
	    if(p->isopen[channel] == 2) {
                log_warning(serial_log, "Bogus close?");
		(*(p->closef)) (p->info, channel);
	    }
            p->isopen[channel] = 2;
            SerialBuffer[SerialPtr] = 0;
            st = (*(p->openf)) (p->info, (char *) SerialBuffer, SerialPtr,
                                channel);
            SerialPtr = 0;

            if (st) {
                p->isopen[channel] = 0;
                (*(p->closef)) (p->info, channel);

                log_error(serial_log,
                          "Cannot open file. Status $%02x.",
                          st);
            }
        }
        if (p->flushf)
            (*(p->flushf)) (p->info, channel);
        break;

      default:
        log_error(serial_log, "Unknown command %02X.", TrapSecondary & 0xff);
    }

    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
    return st;
}

/* ------------------------------------------------------------------------- */

/* Command Serial Bus to TALK, LISTEN, UNTALK, or UNLISTEN, and send the
   Secondary Address to Serial Bus under Attention.  */
void serialattention(void)
{
    int b;
    int st;
    serial_t *p;

    /*
     * Which Secondary Address ?
     */
    b = mem_read(BSOUR);	/* BSOUR - character for serial bus */

    /* do a flush if unlisten for close and command channel */
    if (b == 0x3f && (((TrapSecondary & 0xf0) == 0xf0)
		      || ((TrapSecondary & 0x0f) == 0x0f))) {
	st = serialcommand();
	SET_ST(st);
    } else {
	switch (b & 0xf0) {
	  case 0x20:
	  case 0x40:
            TrapDevice = b;
            break;

	  case 0x60:		/* secondary address */
	  case 0xe0:		/* close a file */
            TrapSecondary = b;
            st = serialcommand();
            SET_ST(st);
            break;

	  case 0xf0:		/* Open File needs the filename first */
            TrapSecondary = b;
	    p = &(serialdevices[TrapDevice & 0x0f]);
            if (p->isopen[b & 0x0f] == 2) {
		(*(p->closef)) (p->info, b & 0x0f);
	    }
            p->isopen[b & 0x0f] = 1;
            break;
	}
    }

    if (!(serialdevices[TrapDevice & 0x0f].inuse))
	SET_ST(0x80);

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);

    if (attention_callback_func)
        attention_callback_func();
}

/* Send one byte on the serial bus.  */
void serialsendbyte(void)
{
    int data, st;
    serial_t *p;

    /*
     * Get data to send from address 0x95
     */
    data = mem_read(BSOUR);	/* BSOUR - character for serial bus */

    p = &serialdevices[TrapDevice & 0x0f];

    if (p->inuse) {
	if (p->isopen[TrapSecondary & 0x0f] == 1) {
	    /* Store name here */
	    if (SerialPtr < SERIAL_NAMELENGTH)
		SerialBuffer[SerialPtr++] = data;
	} else {
	    /* Send to device */
	    st = (*(p->putf)) (p->info, data, TrapSecondary & 0x0f);
	    SET_ST(st);
	}
    } else {			/* Not present */
	SET_ST(0x83);
    }

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

/* Receive one byte from the serial bus.  */
void serialreceivebyte(void)
{
    int st = 0, secadr = TrapSecondary & 0x0f;
    BYTE data;
    serial_t *p;

    p = &serialdevices[TrapDevice & 0x0f];

    /* get next byte if necessary */
    if (!(p->nextok[secadr]))
	st = (*(p->getf)) (p->info, &(p->nextbyte[secadr]), secadr);

    /* move byte from buffer to output */
    data = p->nextbyte[secadr];
    p->nextok[secadr] = 0;
    /* fill buffer again */
    if (!st) {
	st = (*(p->getf)) (p->info, &(p->nextbyte[secadr]), secadr);
	if (!st)
	    p->nextok[secadr] = 1;
    }

    /* Set up serial success / data.  */
    if (st)
	SET_ST(st);
    mem_store(TMP_IN, data);

    /* If at EOF, call specified callback function.  */
    if ((st & 0x40) && eof_callback_func != NULL)
        eof_callback_func();

    /* Set registers like the Kernal routine does.  */
    MOS6510_REGS_SET_A(&maincpu_regs, data);
    MOS6510_REGS_SET_SIGN(&maincpu_regs, (data & 0x80) ? 1 : 0);
    MOS6510_REGS_SET_ZERO(&maincpu_regs, data ? 0 : 1);
    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}


/* Kernal loops serial-port (0xdd00) to see when serial is ready: fake it.
   EEA9 Get serial data and clk in (TKSA subroutine).  */

void trap_serial_ready(void)
{
    MOS6510_REGS_SET_A(&maincpu_regs, 1);
    MOS6510_REGS_SET_SIGN(&maincpu_regs, 0);
    MOS6510_REGS_SET_ZERO(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

/* ------------------------------------------------------------------------- */

/* These routines work for IEEE488 emulation on both C64 and PET.  */
static int parallelcommand(void)
{
    serial_t *p;
    BYTE b;
    int channel;
    int i, st = 0;

    if ( ((TrapDevice & 0x0f) == 8 && drive[0].enable)
	|| ((TrapDevice & 0x0f) == 9 && drive[1].enable) ) {
	return 0x83;	/* device not present */
    }

    /* which device ? */
    p = &serialdevices[TrapDevice & 0x0f];
    channel = TrapSecondary & 0x0f;

    /* if command on a channel, reset output buffer... */
    if ((TrapSecondary & 0xf0) != 0x60) {
	p->nextok[channel] = 0;
	p->lastok[channel] = 0;
    }
    switch (TrapSecondary & 0xf0) {
      case 0x60:
	  /* Open Channel */
	  if (!p->isopen[channel] == 1) {
	      p->isopen[channel] = 2;
	      st = (*(p->openf)) (p->info, NULL, 0, channel);
	      for (i = 0; i < SerialPtr; i++)
		  (*(p->putf)) (p->info, SerialBuffer[i], channel);
	      SerialPtr = 0;
	  }
	  if (p->flushf)
	      (*(p->flushf)) (p->info, channel);

	  if ((!st) && ((TrapDevice & 0xf0) == 0x40)) {
	      st = parallelreceivebyte(&b, 1) & 0xbf;	/* any error, except eof */
	  }
	  break;

      case 0xE0:
	  /* Close File */
	  p->isopen[channel] = 0;
	  st = (*(p->closef)) (p->info, channel);
	  break;

      case 0xF0:
	  /* Open File */
	  if (p->isopen[channel]) {
	      if (p->isopen[channel] == 2) {
                  log_warning(serial_log, "Bogus close?");
                  (*(p->closef)) (p->info, channel);
              }
	      p->isopen[channel] = 2;
	      SerialBuffer[SerialPtr] = 0;
	      st = (*(p->openf)) (p->info, (char *) SerialBuffer, SerialPtr,
				  channel);
	      SerialPtr = 0;

	      if (st) {
		  p->isopen[channel] = 0;
		  (*(p->closef)) (p->info, channel);
		  log_error(serial_log, "Cannot open file. Status $%02x.",
                            st);
	      }
	  }
	  if (p->flushf)
	      (*(p->flushf)) (p->info, channel);
	  break;

      default:
	  log_error(serial_log, "Unknown command %02X.", TrapSecondary & 0xff);
    }
    return (st);
}

int parallelattention(int b)
{
    int st = 0;
    serial_t *p;

    if (parallel_debug)
	log_message(serial_log, "ParallelAttention(%02x).", b);

    if (b == 0x3f
	&& (((TrapSecondary & 0xf0) == 0xf0)
	    || ((TrapSecondary & 0x0f) == 0x0f))) {
	st = parallelcommand();
    } else {
	switch (b & 0xf0) {
	  case 0x20:
	  case 0x40:
	      TrapDevice = b;
	      break;

	  case 0x60:		/* secondary address */
	  case 0xe0:		/* close a file */
	      TrapSecondary = b;
	      st |= parallelcommand();
	      break;

	  case 0xf0:		/* Open File needs the filename first */
	      TrapSecondary = b;
              p = &(serialdevices[TrapDevice & 0x0f]);
              if (p->isopen[b & 0x0f] == 2) {
                 (*(p->closef)) (p->info, b & 0x0f);
              }
              p->isopen[b & 0x0f] = 1;
	      break;
	}
    }

    if (!(serialdevices[TrapDevice & 0x0f].inuse))
	st |= 0x80;

    if ((b == 0x3f) || (b == 0x5f)) {
	TrapDevice = 0;
	TrapSecondary = 0;
    }

    st |= TrapDevice << 8;

    if (attention_callback_func)
        attention_callback_func();

    return st;
}

int parallelsendbyte(int data)
{
    int st = 0;
    serial_t *p;

    if ( ((TrapDevice & 0x0f) == 8 && drive[0].enable)
	|| ((TrapDevice & 0x0f) == 9 && drive[1].enable) ) {
	return 0x83;	/* device not present */
    }

    p = &serialdevices[TrapDevice & 0x0f];

    if (p->inuse) {
	if (p->isopen[TrapSecondary & 0x0f] == 1) {

	    if (parallel_debug)
		log_message(serial_log,
                            "SerialSendByte[%2d] = %02x.", SerialPtr, data);
	    /* Store name here */
	    if (SerialPtr < SERIAL_NAMELENGTH)
		SerialBuffer[SerialPtr++] = data;
	} else {
	    /* Send to device */
	    st = (*(p->putf)) (p->info, data, TrapSecondary & 0x0f);
	}
    } else {			/* Not present */
	st = 0x83;
    }

    return st + (TrapDevice << 8);
}

int parallelreceivebyte(BYTE * data, int fake)
{
    int st = 0, secadr = TrapSecondary & 0x0f;
    serial_t *p;

    if ( ((TrapDevice & 0x0f) == 8 && drive[0].enable)
	|| ((TrapDevice & 0x0f) == 9 && drive[1].enable) ) {
	return 0x83;	/* device not present */
    }

    p = &serialdevices[TrapDevice & 0x0f];

    /* first fill up buffers */
    if (!p->lastok[secadr]) {
	p->lastok[secadr] = p->nextok[secadr];
	p->lastbyte[secadr] = p->nextbyte[secadr];
	p->lastst[secadr] = p->nextst[secadr];
	p->nextok[secadr] = 0;
	if (!p->lastok[secadr]) {
	    p->lastst[secadr] =
		(*(p->getf)) (p->info, &(p->lastbyte[secadr]), secadr);
	    p->lastok[secadr] = 1;
	}
    }
    if ((!p->nextok[secadr]) && (!p->lastst[secadr])) {
	p->nextst[secadr] =
	    (*(p->getf)) (p->info, &(p->nextbyte[secadr]), secadr);
	p->nextok[secadr] = 1;
    }
    *data = p->lastbyte[secadr];
    if (!fake)
	p->lastok[secadr] = 0;
    st = p->nextok[secadr] ? p->nextst[secadr] :
	(p->lastok[secadr] ? p->lastst[secadr] : 2);
    st += TrapDevice << 8;

    if (parallel_debug)
	log_message(serial_log,
                    "receive: sa=%02x lastb = %02x,  ok=%s, st=%04x, nextb = %02x, "
                    "ok=%s, st=%04x.", secadr,
                    p->lastbyte[secadr], p->lastok[secadr] ? "ok" : "no",
                    p->lastst[secadr],
                    p->nextbyte[secadr], p->nextok[secadr] ? "ok" : "no",
                    p->nextst[secadr]);

    if ((!fake) && p->nextok[secadr] && p->nextst[secadr])
	p->nextok[secadr] = 0;

    if ((st & 0x40) && eof_callback_func != NULL)
        eof_callback_func();
    return st;
}

/* ------------------------------------------------------------------------- */

int serial_init(const trap_t * trap_list)
{
    int i;

    if (serial_log == LOG_ERR)
        serial_log = log_open("Serial");

    /* Remove installed traps, if any.  */
    serial_remove_traps();

    /* Install specified traps.  */
    serial_traps = trap_list;
    serial_install_traps();

    /*
     * Clear serial device functions
     */
    for (i = 0; i < MAXDEVICES; i++) {
	serial_t *p;

	p = &serialdevices[i];

	p->inuse = 0;
	p->getf = (int (*)(void *, BYTE *, int)) fn;
	p->putf = (int (*)(void *, BYTE, int)) fn;
	p->openf = (int (*)(void *, const char *, int, int)) fn;
	p->closef = (int (*)(void *, int)) fn;
	p->flushf = (void (*)(void *, int)) NULL;
    }

#ifdef HAVE_PRINTER
    prdevice_late_init();
#endif

    return 0;
}

int serial_install_traps(void)
{
    if (!traps_installed && serial_traps != NULL) {
	const trap_t *p;

	for (p = serial_traps; p->func != NULL; p++)
	    traps_add(p);
	traps_installed = 1;
    }
    return 0;
}

int serial_remove_traps(void)
{
    if (traps_installed && serial_traps != NULL) {
	const trap_t *p;

	for (p = serial_traps; p->func != NULL; p++)
	    traps_remove(p);
	traps_installed = 0;
    }
    return 0;
}

int serial_attach_device(int device, char *var, const char *name,
			 int (*getf) (void *, BYTE *, int),
			 int (*putf) (void *, BYTE, int),
			 int (*openf) (void *, const char *, int, int),
			 int (*closef) (void *, int),
			 void (*flushf) (void *, int))
{
    serial_t *p;
    int i;

    if (device < 0 || device >= MAXDEVICES)
	return 1;

    p = &serialdevices[device];

    if (p->inuse) {
        log_warning(serial_log,
                    "Serial device %d (%s) in use.",
                    device, name);
        return 1;
    }
    p->getf = getf;
    p->putf = putf;
    p->openf = openf;
    p->closef = closef;
    p->flushf = flushf;

    p->inuse = 1;
    p->info = var;		/* run-time data storage */

    if (p->name != NULL)
        free(p->name);
    p->name = stralloc(name);

    for (i = 0; i < 16; i++) {
        p->nextok[i] = 0;
        p->isopen[i] = 0;
    }

    return 0;
}

/* Attach a file for use to specified peripheral.  The device numbed is
   searched in filename if device # == -1.  Note that tape and RS-232 are
   handled here as well.  */
int serial_select_file(int type, int number, const char *file)
{
    serial_t *p;

    if (number < 0 && type) {
	if (type & DT_PRINTER)
	    number = 4;
        else
	if (type & DT_DISK)
	    number = 8;
    }
    if (number < 0 || number >= MAXDEVICES) {
	log_error(serial_log, "Illegal device number %d.", number);
	return -1;
    }
    p = &serialdevices[number];

    /* WARNING: In older versions of this code, we initialized the devices
       which had not been initialized yet, but this is not done anymore.  All
       the devices must have been explicitly initialized before we get here.
       EP 98-04-24.  */
    if (!p || !(p->inuse)) {
	log_error(serial_log, "No device for #%d.", number);
	return -1;
    }

    /* should be based on p -> info.type */

    switch (number) {
#ifdef HAVE_PRINTER
      case 4:
      case 5:
	  return attach_prdevice((PRINTER *) p->info, file, 0);
#endif

      case 8:
      case 9:
      case 10:
      case 11:
#if 0
	  return attach_floppy_image((DRIVE *) p->info, file, 0);
#endif
      default:
	  return -1;
    }

    return 0;
}


/* Detach files from serial devices.  Detach all (shutdown) if dev# == -1.  */
int serial_remove_file(int number)
{
    serial_t *p;
    int i;

    if (number < 0) {
	for (i = 0; i < MAXDEVICES; i++) {

	    p = &serialdevices[i];

	    if (p && p->inuse) {
		if (number == -2) {	/* QUERY mode */
		    log_message(serial_log, "    Unit #%d: %s.", i, p->name);	/* add file */
		} else {
		    serial_remove_file(i);
		}
	    }
	}
    }
    /* number < 0 */
    else {
	if (number < 0 || number >= MAXDEVICES) {
	    log_error(serial_log, "Illegal device number %d.", number);
	    return -1;
	}
	p = &serialdevices[number];

	if (!p || !(p->inuse)) {
	    log_error(serial_log, "Attempting to remove empty device #%d.", number);
	} else
	    switch (number) {	/* should be based on actual type ... */
#ifdef HAVE_PRINTER
          case 4:
	      case 5:
		  detach_prdevice((PRINTER *) p->info);
		  break;
#endif
	      case 8:
	      case 9:
	      case 10:
	      case 11:
		  detach_floppy_image((DRIVE *) p->info);
		  break;

	      default:
		  log_error(serial_log, "Orphan device #%d.", number);
		  return -1;
	    }

    }				/* else */

    return (0);
}

/* Detach and kill serial devices.  Detach all (shutdown) if device# == -1.  */
int serial_detach_device(int number)
{
    serial_t *p;

    if (number < 0 || number >= MAXDEVICES) {
        log_error(serial_log, "Illegal device number %d.", number);
        return -1;
    }
    p = &serialdevices[number];

    if (!p || !(p->inuse)) {
        log_error(serial_log, "Attempting to remove empty device #%d.", number);
    } else {
	p->inuse = 0;
    }

    return (0);
}

serial_t *serial_get_device(int device)
{
    return &serialdevices[device];
}

/* Return the name of the image attached at unit `number', NULL if none.  */
char *serial_get_file_name(int number)
{
    char *p;

    if (!serialdevices[number].inuse)
	return NULL;

    switch (number) {
      case 8:
      case 9:
      case 10:
	  p = ((DRIVE *) serialdevices[number].info)->ActiveName;
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
    int i, j;
    serial_t *p;

    for (i = 0; i < MAXDEVICES; i++) {
	if (serialdevices[i].inuse) {
	    p = &serialdevices[i];
	    for (j = 0; j < 16; j++) {
		if (p->isopen[j]) {
		    p->isopen[j] = 0;
		    (*(p->closef)) (p->info, j);
		}
	    }
	}
    }
}

/* ------------------------------------------------------------------------- */

/* These are just kludges for the autostart code (see `autostart.c').  */

/* Specify a function to call when EOF happens in `serialreceivebyte()'.  */
void serial_set_eof_callback(void (*func)(void))
{
    eof_callback_func = func;
}

/* Specify a function to call when the `serialattention()' trap is called.  */
void serial_set_attention_callback(void (*func)(void))
{
    attention_callback_func = func;
}
