/*
 * fsdrive.c - Filesystem based serial emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "attach.h"
#include "fsdrive.h"
#include "log.h"
#include "maincpu.h"
#include "mem.h"
#include "mos6510.h"
#include "serial.h"
#include "types.h"
#include "vdrive.h"


static log_t fsdrive_log = LOG_ERR;

/* Initialized serial devices.  */
serial_t serialdevices[SERIAL_MAXDEVICES];

BYTE SerialBuffer[SERIAL_NAMELENGTH + 1];
int SerialPtr;

/* On which channel did listen happen to?  */
BYTE TrapDevice;
BYTE TrapSecondary;


/* Handle Serial Bus Commands under Attention.  */
static BYTE serialcommand(void)
{
    serial_t *p;
    void *vdrive;
    int channel;
    int i;
    BYTE st = 0;

    /*
     * which device ?
     */
    p = &serialdevices[TrapDevice & 0x0f];
    channel = TrapSecondary & 0x0f;

    if ((TrapDevice & 0x0f) >= 8)
        vdrive = (vdrive_t *)file_system_get_vdrive(TrapDevice & 0x0f);
    else
        vdrive = NULL;

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
            st = (BYTE)((*(p->openf))(vdrive, NULL, 0, channel));
            for (i = 0; i < SerialPtr; i++)
                (*(p->putf))(vdrive, ((BYTE)(SerialBuffer[i])), channel);
            SerialPtr = 0;
        }
        if (p->flushf)
            (*(p->flushf))(vdrive, channel);
        break;

        /*
         * Close File
         */
      case 0xE0:
        p->isopen[channel] = 0;
        st = (BYTE)((*(p->closef))(vdrive, channel));
        break;

        /*
         * Open File
         */
      case 0xF0:
        if (p->isopen[channel]) {
            if(p->isopen[channel] == 2) {
                log_warning(fsdrive_log, "Bogus close?");
                (*(p->closef))(vdrive, channel);
            }
            p->isopen[channel] = 2;
            SerialBuffer[SerialPtr] = 0;
            st = (BYTE)((*(p->openf))(vdrive, (char *)SerialBuffer,
                 SerialPtr, channel));
            SerialPtr = 0;

            if (st) {
                p->isopen[channel] = 0;
                (*(p->closef))(vdrive, channel);

                log_error(fsdrive_log, "Cannot open file. Status $%02x.", st);
            }
        }
        if (p->flushf)
            (*(p->flushf))(vdrive, channel);
        break;

      default:
        log_error(fsdrive_log, "Unknown command %02X.", TrapSecondary & 0xff);
    }

    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
    return st;
}

/* ------------------------------------------------------------------------- */

void fsdrive_open(BYTE b)
{
    serial_t *p;
    void *vdrive;

    TrapSecondary = b;
    p = &(serialdevices[TrapDevice & 0x0f]);
    if (p->isopen[b & 0x0f] == 2) {
        if ((TrapDevice & 0x0f) >= 8)
            vdrive = file_system_get_vdrive(TrapDevice & 0x0f);
        else
            vdrive = NULL;
        (*(p->closef))(vdrive, b & 0x0f);
    }
    p->isopen[b & 0x0f] = 1;
}

void fsdrive_close(BYTE b)
{
    BYTE st;

    TrapSecondary = b;
    st = serialcommand();
    SERIAL_SET_ST(st);
}

void fsdrive_listentalk(BYTE b)
{
    BYTE st;

    TrapSecondary = b;
    st = serialcommand();
    SERIAL_SET_ST(st);
}

void fsdrive_unlisten(void)
{
    BYTE st;
    serial_t *p;

    if ((TrapSecondary & 0xf0) == 0xf0
        || (TrapSecondary & 0x0f) == 0x0f) {
        st = serialcommand();
        SERIAL_SET_ST(st);
        /* Flush serial read ahead buffer too.  */
        p = &(serialdevices[TrapDevice & 0x0f]);
        p->nextok[TrapSecondary & 0x0f] = 0;
    }
}

void fsdrive_untalk(void)
{

}

void fsdrive_write(BYTE data)
{
    BYTE st;
    serial_t *p;
    void *vdrive;

    p = &serialdevices[TrapDevice & 0x0f];
    if ((TrapDevice & 0x0f) >= 8)
        vdrive = file_system_get_vdrive(TrapDevice & 0x0f);
    else
        vdrive = NULL;

    if (p->inuse) {
        if (p->isopen[TrapSecondary & 0x0f] == 1) {
            /* Store name here */
            if (SerialPtr < SERIAL_NAMELENGTH)
                SerialBuffer[SerialPtr++] = data;
        } else {
            /* Send to device */
            st = (*(p->putf))(vdrive, data, (int)(TrapSecondary & 0x0f));
            SERIAL_SET_ST(st);
        }
    } else {                    /* Not present */
        SERIAL_SET_ST(0x83);
    }
}

BYTE fsdrive_read(void)
{
    int st = 0, secadr = TrapSecondary & 0x0f;
    BYTE data;
    serial_t *p;
    void *vdrive;

    p = &serialdevices[TrapDevice & 0x0f];
    if ((TrapDevice & 0x0f) >= 8)
        vdrive = file_system_get_vdrive(TrapDevice & 0x0f);
    else
        vdrive = NULL;

    /* Get next byte if necessary.  */
    if (!(p->nextok[secadr]))
        st = (*(p->getf))(vdrive, &(p->nextbyte[secadr]), secadr);

    /* Move byte from buffer to output.  */
    data = p->nextbyte[secadr];
    p->nextok[secadr] = 0;
    /* Fill buffer again.  */
    if (!st) {
        st = (*(p->getf))(vdrive, &(p->nextbyte[secadr]), secadr);
        if (!st)
            p->nextok[secadr] = 1;
    }

    /* Set up serial success / data.  */
    if (st)
        SERIAL_SET_ST(st);

    return data;
}

void fsdrive_reset(void)
{
    unsigned int i, j;
    serial_t *p;
    void *vdrive;

    for (i = 0; i < SERIAL_MAXDEVICES; i++) {
        if (serialdevices[i].inuse) {
            p = &serialdevices[i];
            for (j = 0; j < 16; j++) {
                if (p->isopen[j]) {
                    vdrive = file_system_get_vdrive(i);
                    p->isopen[j] = 0;
                    (*(p->closef))(vdrive, j);
                }
            }
        }
    }
}

void fsdrive_init(void)
{
    fsdrive_log = log_open("FSDrive");
}

