/*
 * cw_device.c
 *
 * Written by
 *  Ian Gledhill <ian.gledhill@btinternet.com>
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

static int cwmkiii_found = 1;

#define __USE_INLINE__

#include <stdlib.h>
#include <string.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>

#include <exec/exec.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/devices.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <devices/trackdisk.h>
#include <proto/exec.h>


#include "cw.h"
#include "log.h"
#include "types.h"

static unsigned char read_sid(unsigned char reg, int chipno); // Read a SID register
static void write_sid(unsigned char reg, unsigned char data, int chipno); // Write a SID register

typedef void (*voidfunc_t)(void);

#define MAXSID 2

static int gSIDs = 0;

/* buffer containing current register state of SIDs */
static BYTE sidbuf[0x20 * MAXSID];

static int sidfh = 0;

/* read value from SIDs */
int cw_device_read(WORD addr, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < gSIDs && addr < 0x20) {
        /* if addr is from read-only register, perform a read read */
        if (addr >= 0x19 && addr <= 0x1C && sidfh >= 0) {
            addr += chipno * 0x20;
            sidbuf[addr] = read_sid(addr, chipno);
        } else {
          addr += chipno * 0x20;
        }

        /* take value from sidbuf[] */
        return sidbuf[addr];
    }

    return 0;
}

/* write value into SID */
void cw_device_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < gSIDs && addr <= 0x18) {
        /* correct addr, so it becomes an index into sidbuf[] and the unix device */
        addr += chipno * 0x20;
        /* write into sidbuf[] */
        sidbuf[addr] = val;
        /* if the device is opened, write to device */
        if (sidfh >= 0) {
            write_sid(addr, val, chipno);
        }
    }
}

#undef BYTE
#undef WORD
#include <exec/types.h>
#include <proto/expansion.h>
#include <proto/exec.h>

// Set as appropriate
static int sid_NTSC = FALSE; // TRUE for 60Hz oscillator, FALSE for 50

static struct MsgPort *gDiskPort[2] = {NULL, NULL};
static struct IOExtTD *gCatweaselReq[2] = {NULL, NULL};

static BOOL gSwapSIDs = FALSE;

int cw_device_open(void)
{
    static int atexitinitialized = 0;
    unsigned int i;

    if (atexitinitialized) {
        cw_device_close();
    }

    gSIDs = 0;
    gSwapSIDs = FALSE;

    for (i = 0; i < 2; i++) {
        if ((gDiskPort[i] = CreatePort(NULL, 0)) != NULL) {
            if (gCatweaselReq[i] = (struct IOExtTD *)CreateExtIO(gDiskPort[i], sizeof(struct IOExtTD))) {
                if (OpenDevice("catweaselsid.device", i, (struct IORequest *)gCatweaselReq[i], 0)) {
                    DeleteExtIO((struct IORequest *)gCatweaselReq[i]);
                    DeletePort(gDiskPort[i]);
                    gCatweaselReq[i] = NULL;
                    gDiskPort[i] = NULL;
                } else {
                    gSIDs++;
                }
            }
        }
    }

    if (gSIDs == 0) {
        return -1;
    }

    if (gCatweaselReq[1] && gCatweaselReq[0] == NULL) {
        gSwapSIDs = TRUE;
    }
	
    /* install exit handler, so device is closed on exit */
    if (!atexitinitialized) {
        atexitinitialized = 1;
        atexit((voidfunc_t)cw_device_close);
    }

    sidfh = 1; /* ok */

    return 1;
}

int cw_device_close(void)
{
    unsigned int i;

    /* mute all sids */
    memset(sidbuf, 0, sizeof(sidbuf));
    for (i = 0; i < sizeof(sidbuf); i++) {
        write_sid(i, 0, i / 0x20);
    }

    for (i = 0; i < 2; i++) {
        if (gCatweaselReq[i]) {
            CloseDevice((struct IORequest *)gCatweaselReq[i]);
            if (gCatweaselReq[i] != NULL) {
                DeleteExtIO((struct IORequest *)gCatweaselReq[i]);
            }
            if (gDiskPort[i] != NULL) {
                DeletePort(gDiskPort[i]);
            }
            gCatweaselReq[i] = NULL;
            gDiskPort[i] = NULL;
        }
    }
	
    log_message(LOG_DEFAULT, "CatWeasel Device: closed");

    return 0;
}

static unsigned char read_sid(unsigned char reg, int chipno)
{
    unsigned char tData[2];

    if (gSIDs == 0) {
        return 0;
    }

    if (gSwapSIDs) {
        chipno = 1 - chipno;
    }

    if (gCatweaselReq[chipno] == NULL) {
        return 0;
    }

    tData[0] = reg;
    gCatweaselReq[chipno]->iotd_Req.io_Length = 2;
    gCatweaselReq[chipno]->iotd_Req.io_Command = CMD_READ;
    gCatweaselReq[chipno]->iotd_Req.io_Data = tData;
    gCatweaselReq[chipno]->iotd_Req.io_Offset = 0;

    DoIO((struct IORequest *)gCatweaselReq[chipno]);

    return ((unsigned char*)(gCatweaselReq[chipno]->iotd_Req.io_Data))[1];
}

static void write_sid(unsigned char reg, unsigned char data, int chipno)
{
    unsigned char tData[2];

    if (gSIDs == 0) {
        return;
    }

    if (gSwapSIDs) {
        chipno = 1 - chipno;
    }

    if (gCatweaselReq[chipno] == NULL) {
        return;
    }

    tData[0] = reg;
    tData[1] = data;
    gCatweaselReq[chipno]->iotd_Req.io_Length = 2;
    gCatweaselReq[chipno]->iotd_Req.io_Command = CMD_WRITE;
    gCatweaselReq[chipno]->iotd_Req.io_Data = tData;
    gCatweaselReq[chipno]->iotd_Req.io_Offset = 0;

    DoIO((struct IORequest *)gCatweaselReq[chipno]);
}

/* set current main clock frequency, which gives us the possibilty to
   choose between pal and ntsc frequencies */
void cw_device_set_machine_parameter(long cycles_per_sec)
{
    int i;

    for (i = 0; i < 2; i++) {
        if (gCatweaselReq[i] != NULL) {
            gCatweaselReq[i]->iotd_Req.io_Length = 1;
            gCatweaselReq[i]->iotd_Req.io_Command = TD_FORMAT;
            gCatweaselReq[i]->iotd_Req.io_Offset = 0;
            if (cycles_per_sec <= 1000000) {
                gCatweaselReq[i]->iotd_Req.io_Data = (void *)0;  // PAL
            } else {
                gCatweaselReq[i]->iotd_Req.io_Data = (void *)1;  // NTSC
            }
            DoIO((struct IORequest *)gCatweaselReq[i]);
        }
    }
    sid_NTSC = (cycles_per_sec <= 1000000) ? FALSE : TRUE;
}
