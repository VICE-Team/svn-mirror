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

/* The handle to be returned by print_open() is architecture independent
   and a simple int. We save the real file descriptor here */
static FILE *fd[3] = { NULL, NULL, NULL };

static int set_printer_device(resource_value_t v, void *param)
{
    util_string_set(&PrinterDev[(int)param], (const char*)v);
    return 0;
}

resource_t resources[] = {
    {"PrinterDevice1", RES_STRING, (resource_value_t)PRINTER_DEFAULT_DEV1,
      (resource_value_t *)&PrinterDev[0], set_printer_device, (void *)0 },
    {"PrinterDevice2", RES_STRING, (resource_value_t)PRINTER_DEFAULT_DEV2,
      (resource_value_t *)&PrinterDev[1], set_printer_device, (void *)1 },
    {"PrinterDevice3", RES_STRING, (resource_value_t)PRINTER_DEFAULT_DEV3,
      (resource_value_t *)&PrinterDev[2], set_printer_device, (void *)2 },
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
    { NULL }
};

int output_file_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

void output_file_init(void)
{
}


void output_file_reset(void)
{
}


int output_file_open(int device)
{
    switch (device) {
      case 0:
      case 1:
      case 2:
        if (PrinterDev[device] == NULL)
            return -1;
        if (fd[device] == NULL)
            fd[device] = fopen(PrinterDev[device], MODE_APPEND);
        return 0;
      default:
        return -1;
    }
}


void output_file_close(int fi)
{
    if (fd[fi] != NULL)
        fclose(fd[fi]);
    fd[fi] = NULL;
}


int output_file_putc(int fi, BYTE b)
{
    if (fd[fi] == NULL)
        return -1;
    fputc(b, fd[fi]);

#if defined(__MSDOS__) || defined(WIN32) || defined(OS2) || defined(__BEOS__)
    if (b == 13)
        fputc(10, fd[fi]);
#endif

    return 0;
}


int output_file_getc(int fi, BYTE *b)
{
    if (fd[fi] == NULL)
        return -1;
    *b = fgetc(fd[fi]);
    return 0;
}


int output_file_flush(int fi)
{
    if (fd[fi] == NULL)
        return -1;
    fflush(fd[fi]);
    return 0;
}

