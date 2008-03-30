/*
 * serial.c - IEEE and serial device implementation.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <viceteam@t-online.de>
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
 */

#include "vice.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "attach.h"
#include "drive.h"
#include "lib.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mos6510.h"
#include "printer.h"
#include "realdevice.h"
#include "serial-trap.h"
#include "serial.h"
#include "traps.h"
#include "types.h"
#include "vdrive.h"


extern BYTE SerialBuffer[SERIAL_NAMELENGTH + 1];
extern int SerialPtr;

int serial_truedrive;

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

int serial_init(const trap_t *trap_list)
{
    unsigned int i;

    serial_log = log_open("Serial");

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

void serial_shutdown(void)
{
    unsigned int unit;

    for (unit = 0; unit < SERIAL_MAXDEVICES; unit++)
        serial_detach_device(unit);
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

    if (p->inuse != 0)
        serial_detach_device(unit);

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

    if (unit >= SERIAL_MAXDEVICES) {
        log_error(serial_log, "Illegal device number %d.", unit);
        return -1;
    }

    p = serial_device_get(unit);

    if (p != NULL && p->inuse != 0) {
        p->inuse = 0;
        lib_free(p->name);
        p->name = NULL;
    }

    return 0;
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

