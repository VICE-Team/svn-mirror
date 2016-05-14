/*
 * catweaselmkiii-drv.c - Unix specific cw3 driver.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Dirk Jadgmann <doj@cubic.org>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifdef HAVE_CATWEASELMKIII

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "catweaselmkiii.h"
#include "log.h"
#include "types.h"

#include <cwsid.h>

typedef void (*voidfunc_t)(void);

#define MAXSID 2

/* file handle for unix device */
static int sidfh = -1;

/* set all CatWeasels frequency to global variable ntsc */
static void setfreq()
{
    if (sidfh >= 0) {
        ioctl(sidfh, ntsc ? CWSID_IOCTL_NTSC : CWSID_IOCTL_PAL);
    }
}

/* open unix device */
int catweaselmkiii_open(void)
{
    int i;
    static int atexitinitialized = 0;

    /* if no device is currently opened */
    if (sidfh < 0) {
        sidfh = open("/dev/sid", O_RDWR);

        if (sidfh < 0) {
            sidfh = open("/dev/misc/sid", O_RDWR);
        }

        /* could not open at standard locations: error */
        if (sidfh < 0) {
            log_error(LOG_DEFAULT, "could not open sid device /dev/sid or /dev/misc/sid");
            return -1;
        }
    }

    /* mute all sids */
    lseek(sidfh, 0, SEEK_SET);
    for (i = 0; i < 32; ++i) {
        write(sidfh, 0, 1);
    }

    setfreq();

    log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: opened");

    /* install exit handler, so device is closed on exit */
    if (!atexitinitialized) {
        atexitinitialized = 1;
        atexit((voidfunc_t)catweaselmkiii_close);
    }

    return 0;
}

/* close unix device */
int catweaselmkiii_close(void)
{
    int i;

    /* if there is a device opened */
    if (sidfh >= 0) {
        /* mute */
        lseek(sidfh, 0, SEEK_SET);
        for (i = 0; i < 32; ++i) {
            write(sidfh, 0, 1);
        }

        close(sidfh);
        sidfh = -1;

        log_message(LOG_DEFAULT, "CatWeasel MK3 PCI SID: closed");
    }

    return 0;
}

/* read value from SIDs */
int catweaselmkiii_read(WORD addr, int chipno)
{
    BYTE retval;

    /* check if chipno and addr is valid */
    if (chipno < MAXSID && addr < 0x20) {
        /* if addr is from read-only register, perform a read read */
        if (addr >= 0x19 && addr <= 0x1C && sidfh >= 0) {
            addr += chipno*0x20;
            lseek(sidfh, addr, SEEK_SET);
            read(sidfh, &retval, 1);
            return retval;
        }
    }

    return 0;
}

/* write value into SID */
void catweaselmkiii_store(WORD addr, BYTE val, int chipno)
{
    /* check if chipno and addr is valid */
    if (chipno < MAXSID && addr <= 0x18) {
        /* correct addr, so it becomes an index into sidbuf[] and the unix device */
        addr += chipno * 0x20;

        /* if the device is opened, write to device */
        if (sidfh >= 0) {
            lseek(sidfh, addr, SEEK_SET);
            write(sidfh, &val, 1);
        }
    }
}

/* set current main clock frequency, which gives us the possibilty to
   choose between pal and ntsc frequencies */
void catweaselmkiii_set_machine_parameter(long cycles_per_sec)
{
    setfreq();
}
#endif
