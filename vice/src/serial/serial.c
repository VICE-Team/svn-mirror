/*
 * serial.c - IEEE and serial device implementation.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Patches by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "attach.h"
#include "drive.h"
#include "iecbus.h"
#include "lib.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mos6510.h"
#include "parallel.h"
#include "printer.h"
#include "realdevice.h"
#include "serial-trap.h"
#include "serial.h"
#include "traps.h"
#include "types.h"
#include "vdrive.h"


extern BYTE SerialBuffer[SERIAL_NAMELENGTH + 1];
extern int SerialPtr;
extern int serial_truedrive;

/* Flag: Have traps been installed?  */
static int traps_installed = 0;

/* Pointer to list of traps we are using.  */
static const trap_t *serial_traps;

/* Logging goes here.  */
static log_t serial_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

/* This is just a kludge for the autostart code (see `autostart.c').  */

/* Function to call when EOF happens in `serialreceivebyte()'.  */
void (*eof_callback_func)(void);

/* Function to call when the `serialattention()' trap is called.  */
void (*attention_callback_func)(void);

/* ------------------------------------------------------------------------- */

/* Call this if device is not attached: -128 == device not present.  */
static int fn(void)
{
    return 0x80;
}

void serial_set_st(BYTE st)
{
    mem_store((WORD)0x90, (BYTE)(mem_read((WORD)0x90) | st));
}

BYTE serial_get_st(void)
{
    return mem_read((WORD)0x90);
}

/* ------------------------------------------------------------------------- */

/* These routines work for IEEE488 emulation on both C64 and PET.  */
static int parallelcommand(void)
{
    serial_t *p;
    BYTE b;
    int channel;
    int i, st = 0;
    void *vdrive;

    if (((TrapDevice & 0x0f) == 8 && drive[0].enable)
        || ((TrapDevice & 0x0f) == 9 && drive[1].enable)) {
        return 0x83;    /* device not present */
    }

    /* which device ? */
    p = serial_device_get(TrapDevice & 0x0f);
    vdrive = (void *)file_system_get_vdrive(TrapDevice & 0x0f);
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
            st = (*(p->openf))(vdrive, NULL, 0, channel);
            for (i = 0; i < SerialPtr; i++)
                (*(p->putf))(vdrive, SerialBuffer[i], channel);
            SerialPtr = 0;
        }
        if (p->flushf)
            (*(p->flushf))(vdrive, channel);

        if ((!st) && ((TrapDevice & 0xf0) == 0x40)) {
            /* any error, except eof */
            st = parallelreceivebyte(&b, 1) & 0xbf;
        }
        break;
      case 0xE0:
        /* Close File */
        p->isopen[channel] = 0;
        st = (*(p->closef))(vdrive, channel);
        break;
      case 0xF0:
        /* Open File */
        if (p->isopen[channel]) {
            if (p->isopen[channel] == 2) {
                log_warning(serial_log, "Bogus close?");
                (*(p->closef))(vdrive, channel);
            }
            p->isopen[channel] = 2;
            SerialBuffer[SerialPtr] = 0;
            st = (*(p->openf))(vdrive, (char *) SerialBuffer, SerialPtr,
                               channel);
            SerialPtr = 0;

            if (st) {
               p->isopen[channel] = 0;
               (*(p->closef))(vdrive, channel);
               log_error(serial_log, "Cannot open file. Status $%02x.", st);
            }
        }
        if (p->flushf)
            (*(p->flushf))(vdrive, channel);
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
    void *vdrive;

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

          case 0x60:            /* secondary address */
          case 0xe0:            /* close a file */
            TrapSecondary = b;
            st |= parallelcommand();
            break;

          case 0xf0:            /* Open File needs the filename first */
            TrapSecondary = b;
            p = serial_device_get(TrapDevice & 0x0f);
            vdrive = (void *)file_system_get_vdrive(TrapDevice & 0x0f);
            if (p->isopen[b & 0x0f] == 2) {
               (*(p->closef))(vdrive, b & 0x0f);
            }
            p->isopen[b & 0x0f] = 1;
            break;
        }
    }

    p = serial_device_get(TrapDevice & 0x0f);
    if (!(p->inuse))
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

int parallelsendbyte(BYTE data)
{
    int st = 0;
    serial_t *p;
    void *vdrive;

    if (((TrapDevice & 0x0f) == 8 && drive[0].enable)
        || ((TrapDevice & 0x0f) == 9 && drive[1].enable)) {
        return 0x83;    /* device not present */
    }

    p = serial_device_get(TrapDevice & 0x0f);
    vdrive = (void *)file_system_get_vdrive(TrapDevice & 0x0f);

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
            st = (*(p->putf))(vdrive, data, TrapSecondary & 0x0f);
        }
    } else {                    /* Not present */
        st = 0x83;
    }

    return st + (TrapDevice << 8);
}

int parallelreceivebyte(BYTE * data, int fake)
{
    int st = 0, secadr = TrapSecondary & 0x0f;
    serial_t *p;
    void *vdrive;

    if ( ((TrapDevice & 0x0f) == 8 && drive[0].enable)
        || ((TrapDevice & 0x0f) == 9 && drive[1].enable) ) {
        return 0x83;    /* device not present */
    }

    p = serial_device_get(TrapDevice & 0x0f);
    vdrive = (void *)file_system_get_vdrive(TrapDevice & 0x0f);

    /* first fill up buffers */
    if (!p->lastok[secadr]) {
        p->lastok[secadr] = p->nextok[secadr];
        p->lastbyte[secadr] = p->nextbyte[secadr];
        p->lastst[secadr] = p->nextst[secadr];
        p->nextok[secadr] = 0;
        if (!p->lastok[secadr]) {
            p->lastst[secadr] =
                (*(p->getf))(vdrive, &(p->lastbyte[secadr]), secadr);
            p->lastok[secadr] = 1;
        }
    }
    if ((!p->nextok[secadr]) && (!p->lastst[secadr])) {
        p->nextst[secadr] =
            (*(p->getf))(vdrive, &(p->nextbyte[secadr]), secadr);
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
                    "receive: sa=%02x lastb = %02x (data=%02x), "
                    "ok=%s, st=%04x, nextb = %02x, "
                    "ok=%s, st=%04x.", secadr,
                    p->lastbyte[secadr], (int)*data,
                    p->lastok[secadr] ? "ok" : "no",
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

int serial_init(const trap_t *trap_list, WORD tmpin)
{
    unsigned int i;

    serial_log = log_open("Serial");
    iecbus_init();

    serial_trap_init(tmpin);

    /* Remove installed traps, if any.  */
    serial_remove_traps();

    /* Install specified traps.  */
    serial_traps = trap_list;
    serial_install_traps();
    serial_truedrive = 0;

    /*
     * Clear serial device functions
     */
    for (i = 0; i < SERIAL_MAXDEVICES; i++) {
        serial_t *p;

        p = serial_device_get(i);

        p->inuse = 0;
        p->getf = (int (*)(vdrive_t *, BYTE *, unsigned int))fn;
        p->putf = (int (*)(vdrive_t *, BYTE, unsigned int))fn;
        p->openf = (int (*)(vdrive_t *, const char *, int, unsigned int))fn;
        p->closef = (int (*)(vdrive_t *, unsigned int))fn;
        p->flushf = (void (*)(vdrive_t *, unsigned int))NULL;
    }

    if (printer_interface_serial_late_init() < 0)
        return -1;

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

void serial_set_truedrive(int flag)
{
    serial_truedrive = flag;
}

int serial_attach_device(unsigned int unit, const char *name,
                         int (*getf) (vdrive_t *, BYTE *, unsigned int),
                         int (*putf) (vdrive_t *, BYTE, unsigned int),
                         int (*openf) (vdrive_t *, const char *,
                         int, unsigned int),
                         int (*closef) (vdrive_t *, unsigned int),
                         void (*flushf) (vdrive_t *, unsigned int))
{
    serial_t *p;
    int i;

    if (unit >= SERIAL_MAXDEVICES)
        return 1;

    p = serial_device_get(unit);

    p->getf = getf;
    p->putf = putf;
    p->openf = openf;
    p->closef = closef;
    p->flushf = flushf;

    p->inuse = 1;

    if (p->name != NULL)
        lib_free(p->name);

    p->name = lib_stralloc(name);

    for (i = 0; i < 16; i++) {
        p->nextok[i] = 0;
        p->isopen[i] = 0;
    }

    return 0;
}

/* Detach and kill serial devices.  */
int serial_detach_device(unsigned int unit)
{
    serial_t *p;

    if (unit < 0 || unit >= SERIAL_MAXDEVICES) {
        log_error(serial_log, "Illegal device number %d.", unit);
        return -1;
    }
    p = serial_device_get(unit);

    if (!p || !(p->inuse)) {
        log_error(serial_log, "Attempting to remove empty device #%d.", unit);
    } else {
        p->inuse = 0;
    }

    return 0;
}

/* Close all files.  */
void serial_reset(void)
{
    iecbus_reset();
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

