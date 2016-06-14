/*
 * ps-file.c - file device based PARallel port SID support for UNIX.
 *
 * Written by
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

#ifdef HAVE_PORTSID

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_LINUX_PARPORT_HEADERS
#include <linux/ppdev.h>
#include <linux/parport.h>
#endif

#include "alarm.h"
#include "log.h"
#include "parsid.h"
#include "ps.h"
#include "sid-resources.h"
#include "types.h"

#define MAXSID 3

#ifdef HAVE_LINUX_PARPORT_HEADERS
static char *parport_name[MAXSID] = { "/dev/parport0", "/dev/parport1", "/dev/parport2" };
#define PARPORT_TYPE int
#define PARPORT_NULL -1
#endif

static int sids_found = -1;
static PARPORT_TYPE pssids[MAXSID] = { PARPORT_NULL, PARPORT_NULL, PARPORT_NULL };

#ifdef HAVE_LINUX_PARPORT_HEADERS
BYTE ps_file_in_data(int chipno)
{
    BYTE retval = 0;

    if (chipno < MAXSID && pssids[chipno] != PARPORT_NULL) {
        ioctl(pssids[chipno], PPRDATA, &retval);
    }
    return retval;
}

void ps_file_out_data(BYTE outval, int chipno)
{
    if (chipno < MAXSID && pssids[chipno] != PARPORT_NULL) {
        ioctl(pssids[chipno], PPWDATA, &outval);
    }
}

void ps_file_out_ctr(WORD parsid_ctrport, int chipno)
{
    BYTE ctl = 0;
    int datadir = 0;

    if (chipno < MAXSID && pssids[chipno] != PARPORT_NULL) {
        if (parsid_ctrport & parsid_STROBE) {
            ctl |= PARPORT_CONTROL_STROBE;
        }
        if (parsid_ctrport & parsid_AUTOFEED) {
            ctl |= PARPORT_CONTROL_AUTOFD;
        }
        if (parsid_ctrport & parsid_nINIT) {
            ctl |= PARPORT_CONTROL_INIT;
        }
        if (parsid_ctrport & parsid_SELECTIN) {
            ctl |= PARPORT_CONTROL_SELECT;
        }
        datadir = (parsid_ctrport & parsid_PCD) ? 1 : 0;
        ioctl(pssids[chipno], PPWCONTROL, &ctl);
        ioctl(pssids[chipno], PPDATADIR, &datadir);
    }
}

BYTE ps_file_in_ctr(int chipno)
{
    BYTE retval = 0;
    BYTE ctl;

    if (chipno < MAXSID && pssids[chipno] != PARPORT_NULL) {
        ioctl(pssids[chipno], PPRCONTROL, &ctl);
        if (ctl & PARPORT_CONTROL_STROBE) {
            retval |= parsid_STROBE;
        }
        if (ctl & PARPORT_CONTROL_AUTOFD) {
            retval |= parsid_AUTOFEED;
        }
        if (ctl & PARPORT_CONTROL_INIT) {
            retval |= parsid_nINIT;
        }
        if (ctl & PARPORT_CONTROL_SELECT) {
            retval |= parsid_SELECTIN;
        }
    }

    return retval;
}

static BYTE detect_sid_read(WORD addr, int chipno)
{
    BYTE value = 0;
    BYTE ctl = ps_file_in_ctr(chipno);

    ps_file_out_data(addr & 0x1f, chipno);

    ctl &= ~parsid_AUTOFEED;
    ps_file_out_ctr(ctl, chipno);

    ctl |= parsid_AUTOFEED;
    ps_file_out_ctr(ctl, chipno);

    ctl |= parsid_PCD;
    ps_file_out_ctr(ctl, chipno);

    ctl |= parsid_nINIT;
    ps_file_out_ctr(ctl, chipno);

    ctl |= parsid_STROBE;
    ps_file_out_ctr(ctl, chipno);

    value = ps_file_in_data(chipno);

    ctl &= ~parsid_STROBE;
    ps_file_out_ctr(ctl, chipno);

    return value;
}

static void detect_sid_store(WORD addr, BYTE outval, int chipno)
{
    BYTE ctl = ps_file_in_ctr(chipno);

    ps_file_out_data(addr & 0x1f, chipno);

    ctl &= ~parsid_AUTOFEED;
    ps_file_out_ctr(ctl, chipno);

    ctl |= parsid_AUTOFEED;
    ps_file_out_ctr(ctl, chipno);

    ps_file_out_data(outval, chipno);

    ctl |= parsid_STROBE;
    ps_file_out_ctr(ctl, chipno);

    ctl &= ~parsid_STROBE;
    ps_file_out_ctr(ctl, chipno);
}

static int detect_sid(int chipno)
{
    int i;

    for (i = 0x18; i >= 0; --i) {
        detect_sid_store(i, 0, chipno);
    }

    detect_sid_store(0x12, 0xff, chipno);

    for (i = 0; i < 100; ++i) {
        if (detect_sid_read(0x1b, chipno)) {
            return 0;
        }
    }

    detect_sid_store(0x0e, 0xff, chipno);
    detect_sid_store(0x0f, 0xff, chipno);
    detect_sid_store(0x12, 0x20, chipno);

    for (i = 0; i < 100; ++i) {
        if (detect_sid_read(0x1b, chipno)) {
            return 1;
        }
    }
    return 0;
}

int ps_file_open(void)
{
    int i;
    int mode = IEEE1284_MODE_COMPAT;

    if (!sids_found) {
        return -1;
    }

    if (sids_found > 0) {
        return 0;
    }

    sids_found = 0;

    for (i = 0; i < MAXSID; ++i) {
        pssids[sids_found] = open(parport_name[i], O_RDWR);
        if (pssids[sids_found] != -1) {
            if (!ioctl(pssids[sids_found], PPCLAIM)) {
                if (!ioctl(pssids[sids_found], PPNEGOT, &mode)) {
                    if (detect_sid(sids_found)) {
                        sids_found++;
                    } else {
                        close(pssids[sids_found]);
                        pssids[sids_found] = -1;
                    }
                } else {
                    close(pssids[sids_found]);
                    pssids[sids_found] = -1;
                }
            } else {
                close(pssids[sids_found]);
                pssids[sids_found] = -1;
            }
        }
    }

    if (!sids_found) {
        return -1;
    }

    return 0;
}

int ps_file_close(void)
{
    int i;

    for (i = 0; i < MAXSID; ++i) {
        if (pssids[i] != -1) {
            close(pssids[i]);
            pssids[i] = -1;
        }
    }
    return 0;
}
#endif

int ps_file_available(void)
{
    return sids_found;
}
#endif
