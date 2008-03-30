/*
 * output-file.c - Output file interface.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "output-file.h"
#include "resources.h"
#include "utils.h"
#include "types.h"


static char *PrinterDev[3] = { NULL, NULL, NULL };
static unsigned int printer_device[3];
static FILE *output_fd[3] = { NULL, NULL, NULL };

static int set_printer_device_name(resource_value_t v, void *param)
{
    util_string_set(&PrinterDev[(int)param], (const char *)v);
    return 0;
}

static int set_printer_device(resource_value_t v, void *param)
{
    unsigned int prn_dev;

    prn_dev = (unsigned int)v;

    if (prn_dev > 3)
        return -1;

    printer_device[(int)param] = (unsigned int)v;
    return 0;
}

static resource_t resources[] = {
    {"PrinterDevice1", RES_STRING, (resource_value_t)PRINTER_DEFAULT_DEV1,
      (resource_value_t *)&PrinterDev[0], set_printer_device_name, (void *)0 },
    {"PrinterDevice2", RES_STRING, (resource_value_t)PRINTER_DEFAULT_DEV2,
      (resource_value_t *)&PrinterDev[1], set_printer_device_name, (void *)1 },
    {"PrinterDevice3", RES_STRING, (resource_value_t)PRINTER_DEFAULT_DEV3,
      (resource_value_t *)&PrinterDev[2], set_printer_device_name, (void *)2 },
    { "Printer4Device", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&printer_device[0], set_printer_device, (void *)0 },
    { "Printer5Device", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&printer_device[1], set_printer_device, (void *)1 },
    { "PrinterUserportDevice", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&printer_device[2], set_printer_device, (void *)2 },
    {NULL}
};

int output_file_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] =
{
    { "-prdev1", SET_RESOURCE, 1, NULL, NULL, "PrinterDevice1", NULL,
     "<name>", N_("Specify name of printer device or dump file") },
    { "-prdev2", SET_RESOURCE, 1, NULL, NULL, "PrinterDevice2", NULL,
     "<name>", N_("Specify name of printer device or dump file") },
    { "-prdev3", SET_RESOURCE, 1, NULL, NULL, "PrinterDevice3", NULL,
     "<name>", N_("Specify name of printer device or dump file") },
    { "-pr4dev", SET_RESOURCE, 1, NULL, NULL, "Printer4Device",
      (resource_value_t)0,
      "<0-2>", "Specify printer output device for IEC printer #4" },
    { "-pr5dev", SET_RESOURCE, 1, NULL, NULL, "Printer5Device",
      (resource_value_t)0,
      "<0-2>", "Specify printer output device for IEC printer #5" },
    { "-pruserdev", SET_RESOURCE, 1, NULL, NULL, "PrinterUserportDevice",
      (resource_value_t) 0,
      "<0-2>", "Specify printer output device for userport printer" },
    { NULL }
};

int output_file_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void output_file_init(void)
{
}

void output_file_reset(void)
{
}

int output_file_open(unsigned int prnr)
{
    switch (printer_device[prnr]) {
      case 0:
      case 1:
      case 2:
        if (PrinterDev[printer_device[prnr]] == NULL)
            return -1;

        if (output_fd[printer_device[prnr]] == NULL) {
            FILE *fd;

            fd = fopen(PrinterDev[printer_device[prnr]], MODE_APPEND);
            if (fd == NULL)
                return -1;
            output_fd[printer_device[prnr]] = fd;
        }
        return 0;
      default:
        return -1;
    }
}

void output_file_close(unsigned int prnr)
{
    if (output_fd[printer_device[prnr]] != NULL)
        fclose(output_fd[printer_device[prnr]]);
    output_fd[printer_device[prnr]] = NULL;
}

int output_file_putc(unsigned int prnr, BYTE b)
{
    if (output_fd[printer_device[prnr]] == NULL)
        return -1;
    fputc(b, output_fd[printer_device[prnr]]);

    return 0;
}

int output_file_getc(unsigned int prnr, BYTE *b)
{
    if (output_fd[printer_device[prnr]] == NULL)
        return -1;
    *b = fgetc(output_fd[printer_device[prnr]]);
    return 0;
}

int output_file_flush(unsigned int prnr)
{
    if (output_fd[printer_device[prnr]] == NULL)
        return -1;
    fflush(output_fd[printer_device[prnr]]);

    return 0;
}

