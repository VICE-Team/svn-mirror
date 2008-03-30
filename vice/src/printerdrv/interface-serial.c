/*
 * interface-serial.c - Serial printer interface.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdline.h"
#include "driver-select.h"
#include "interface-serial.h"
#include "log.h"
#include "printer.h"
#include "resources.h"
#include "serial.h"
#include "types.h"
#include "utils.h"


static int interface_serial_attach(unsigned int prnr);
static int interface_serial_detach(unsigned int prnr);

static log_t interface_serial_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

static int printer_enabled[2];

static int set_printer_enabled(resource_value_t v, void *param)
{
    int flag;
    unsigned int prnr;

    flag = (int)v;

    if (flag != PRINTER_DEVICE_NONE
        && flag != PRINTER_DEVICE_FS
#if HAVE_OPENCBM
        && flag != PRINTER_DEVICE_REAL
#endif    
        )
        return -1;

    prnr = (unsigned int)param;

    if (printer_enabled[prnr] == PRINTER_DEVICE_FS
        && flag != PRINTER_DEVICE_FS) {
        if (interface_serial_detach(prnr) < 0)
            return -1;
    }
    if (flag == PRINTER_DEVICE_FS
        && printer_enabled[prnr] != PRINTER_DEVICE_FS) {
        if (interface_serial_attach(prnr) < 0)
            return -1;
    }

    if (printer_enabled[prnr] == PRINTER_DEVICE_REAL
        && flag != PRINTER_DEVICE_REAL) {
    }

    if (flag == PRINTER_DEVICE_REAL
        && printer_enabled[prnr] != PRINTER_DEVICE_REAL) {
    }

    printer_enabled[prnr] = flag;

    return 0;
}

static const resource_t resources[] = {
    { "Printer4", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&printer_enabled[0], set_printer_enabled, (void *)0 },
    { "Printer5", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&printer_enabled[1], set_printer_enabled, (void *)1 },
    { NULL }
};

int interface_serial_init_resources(void)
{
    return resources_register(resources);
}

static const cmdline_option_t cmdline_options[] = {
    { "-device4", SET_RESOURCE, 1, NULL, NULL, "Printer4",
      (void *)PRINTER_DEVICE_FS, "<type>",
      "Set device type for device #4 (0: NONE, 1: FS, 2: REAL)" },
    { "-device5", SET_RESOURCE, 1, NULL, NULL, "Printer5",
      (void *)PRINTER_DEVICE_FS, "<type>",
      "Set device type for device #4 (0: NONE, 1: FS, 2: REAL)" },
    { NULL }
};

int interface_serial_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static unsigned int inuse[2];

static int open_pr(unsigned int prnr, const char *name, int length,
                   unsigned int secondary)
{
    if (inuse[prnr]) {
        log_error(interface_serial_log,
                  "Open printer #%i while still open - ignoring.", prnr + 4);
        return 0;
    }

    if (driver_select_open(prnr, secondary) < 0) {
        log_error(interface_serial_log,
                  "Couldn't open device #%i.", prnr + 4);
        return -1;
    }

    inuse[prnr] = 1;

    return 0;
}

static int read_pr(unsigned int prnr, BYTE *byte, unsigned int secondary)
{
    return 0x80;
}

static int write_pr(unsigned int prnr, BYTE byte, unsigned int secondary)
{
    int err;

    if (!inuse[prnr]) {
        /* oh, well, we just assume an implicit open - "OPEN 1,4"
           just does not leave any trace on the serial bus */
        log_message(interface_serial_log,
                    "Auto-opening printer #%i.", prnr + 4);

        err = open_pr(prnr, NULL, 0, secondary);

        if (err < 0)
            return err;
    }

    return driver_select_putc(prnr, secondary, (BYTE)byte);
}

static int close_pr(unsigned int prnr, unsigned int secondary)
{
    if (!inuse[prnr]) {
        log_error(interface_serial_log,
                  "Close printer #%i while being closed - ignoring.",
                  prnr + 4);
        return 0;
    }

    driver_select_close(prnr, secondary);
    inuse[prnr] = 0;

    return 0;
}


static void flush_pr(unsigned int prnr, unsigned int secondary)
{
    if (!inuse[prnr]) {
        log_error(interface_serial_log,
                  "Flush printer #%i while being closed - ignoring.",
                  prnr + 4);
        return;
    }

    driver_select_flush(prnr, secondary);
}

/* ------------------------------------------------------------------------- */

static int open_pr4(struct vdrive_s *var, const char *name, int length,
                    unsigned int secondary)
{
    return open_pr(0, name, length, secondary);
}

static int read_pr4(struct vdrive_s *var, BYTE *byte, unsigned int secondary)
{
    return read_pr(0, byte, secondary);
}

static int write_pr4(struct vdrive_s *var, BYTE byte, unsigned int secondary)
{
    return write_pr(0, byte, secondary);
}

static int close_pr4(struct vdrive_s *var, unsigned int secondary)
{
    return close_pr(0, secondary);
}

static void flush_pr4(struct vdrive_s *var, unsigned int secondary)
{
    flush_pr(0, secondary);
}

static int open_pr5(struct vdrive_s *var, const char *name, int length,
                    unsigned int secondary)
{
    return open_pr(1, name, length, secondary);
}

static int read_pr5(struct vdrive_s *var, BYTE *byte, unsigned int secondary)
{
    return read_pr(1, byte, secondary);
}

static int write_pr5(struct vdrive_s *var, BYTE byte, unsigned int secondary)
{
    return write_pr(1, byte, secondary);
}

static int close_pr5(struct vdrive_s *var, unsigned int secondary)
{
    return close_pr(1, secondary);
}

static void flush_pr5(struct vdrive_s *var, unsigned int secondary)
{
    flush_pr(1, secondary);
}

/* ------------------------------------------------------------------------- */

int printer_interface_serial_close(unsigned int unit)
{
    if (unit == 4) {
        close_pr(0, -1);
    }
    if (unit == 5) {
        close_pr(1, -1);
    }
    return 0;
}

int printer_interface_serial_late_init(void)
{
    if (printer_enabled[0]) {
        if (interface_serial_attach(0) < 0)
            return -1;
    }
    if (printer_enabled[1]) {
        if (interface_serial_attach(1) < 0)
            return -1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */

static int interface_serial_attach(unsigned int prnr)
{
    int err;

    inuse[prnr] = 0;

    switch (prnr) {
      case 0:
        err = serial_attach_device(4, "Printer #4 device", read_pr4,
                                   write_pr4, open_pr4, close_pr4, flush_pr4);
        break;
      case 1:
        err = serial_attach_device(5, "Printer #5 device", read_pr5,
                                   write_pr5, open_pr5, close_pr5, flush_pr5);
        break;
      default:
        err = -1;
    }

    if (err) {
        log_error(interface_serial_log,
                  "Cannot attach serial printer #%i.", prnr + 4);
        return -1;
    }

    return 0;
}

static int interface_serial_detach(unsigned int prnr)
{
    if (inuse[prnr]) {
        flush_pr(prnr, -1);
        close_pr(prnr, -1);
    }

    serial_detach_device(prnr + 4);

    return 0;
}

/* ------------------------------------------------------------------------- */

void interface_serial_init(void)
{
    interface_serial_log = log_open("Serial Interface");
}

