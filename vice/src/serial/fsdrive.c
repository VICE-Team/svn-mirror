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
#include "serial.h"
#include "types.h"
#include "vdrive.h"


static log_t fsdrive_log = LOG_ERR;

BYTE SerialBuffer[SERIAL_NAMELENGTH + 1];
int SerialPtr;


/* Handle Serial Bus Commands under Attention.  */
static BYTE serialcommand(unsigned int device, BYTE secondary)
{
    serial_t *p;
    void *vdrive;
    int channel;
    int i;
    BYTE st = 0;

    /*
     * which device ?
     */
    p = serial_get_device(device & 0x0f);
    channel = secondary & 0x0f;

    if ((device & 0x0f) >= 8)
        vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);
    else
        vdrive = NULL;

    /* if command on a channel, reset output buffer... */
    if ((secondary & 0xf0) != 0x60) {
        p->nextok[channel] = 0;
    }
    switch (secondary & 0xf0) {
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
        log_error(fsdrive_log, "Unknown command %02X.", secondary & 0xff);
    }

    return st;
}

/* ------------------------------------------------------------------------- */

void fsdrive_open(unsigned int device, BYTE secondary)
{
    serial_t *p;
    void *vdrive;

    p = serial_get_device(device & 0x0f);
    if (p->isopen[secondary & 0x0f] == 2) {
        if ((device & 0x0f) >= 8)
            vdrive = file_system_get_vdrive(device & 0x0f);
        else
            vdrive = NULL;
        (*(p->closef))(vdrive, secondary & 0x0f);
    }
    p->isopen[secondary & 0x0f] = 1;
}

void fsdrive_close(unsigned int device, BYTE secondary)
{
    BYTE st;

    st = serialcommand(device, secondary);
    serial_set_st(st);
}

void fsdrive_listentalk(unsigned int device, BYTE secondary)
{
    BYTE st;

    st = serialcommand(device, secondary);
    serial_set_st(st);
}

void fsdrive_unlisten(unsigned int device, BYTE secondary)
{
    BYTE st;
    serial_t *p;

    if ((secondary & 0xf0) == 0xf0
        || (secondary & 0x0f) == 0x0f) {
        st = serialcommand(device, secondary);
        serial_set_st(st);
        /* Flush serial read ahead buffer too.  */
        p = serial_get_device(device & 0x0f);
        p->nextok[secondary & 0x0f] = 0;
    }
}

void fsdrive_untalk(unsigned int device, BYTE secondary)
{

}

void fsdrive_write(unsigned int device, BYTE secondary, BYTE data)
{
    BYTE st;
    serial_t *p;
    void *vdrive;

    p = serial_get_device(device & 0x0f);
    if ((device & 0x0f) >= 8)
        vdrive = file_system_get_vdrive(device & 0x0f);
    else
        vdrive = NULL;

    if (p->inuse) {
        if (p->isopen[secondary & 0x0f] == 1) {
            /* Store name here */
            if (SerialPtr < SERIAL_NAMELENGTH)
                SerialBuffer[SerialPtr++] = data;
        } else {
            /* Send to device */
            st = (*(p->putf))(vdrive, data, (int)(secondary & 0x0f));
            serial_set_st(st);
        }
    } else {                    /* Not present */
        serial_set_st(0x83);
    }
}

BYTE fsdrive_read(unsigned int device, BYTE secondary)
{
    int st = 0, secadr = secondary & 0x0f;
    BYTE data;
    serial_t *p;
    void *vdrive;

    p = serial_get_device(device & 0x0f);
    if ((device & 0x0f) >= 8)
        vdrive = file_system_get_vdrive(device & 0x0f);
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
        serial_set_st(st);

    return data;
}

void fsdrive_reset(void)
{
    unsigned int i, j;
    serial_t *p;
    void *vdrive;

    for (i = 0; i < SERIAL_MAXDEVICES; i++) {
        p = serial_get_device(i);
        if (p->inuse) {
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

