/*
 * output-file.c - Output file interface.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "output-select.h"
#include "output-text.h"
#include "output.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "util.h"


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

static const resource_t resources[] = {
    { "PrinterTextDevice1", RES_STRING,
      (resource_value_t)ARCHDEP_PRINTER_DEFAULT_DEV1,
      (void *)&PrinterDev[0], set_printer_device_name, (void *)0 },
    { "PrinterTextDevice2", RES_STRING,
      (resource_value_t)ARCHDEP_PRINTER_DEFAULT_DEV2,
      (void *)&PrinterDev[1], set_printer_device_name, (void *)1 },
    { "PrinterTextDevice3", RES_STRING,
      (resource_value_t)ARCHDEP_PRINTER_DEFAULT_DEV3,
      (void *)&PrinterDev[2], set_printer_device_name, (void *)2 },
    { "Printer4TextDevice", RES_INTEGER, (resource_value_t)0,
      (void *)&printer_device[0], set_printer_device, (void *)0 },
    { "Printer5TextDevice", RES_INTEGER, (resource_value_t)0,
      (void *)&printer_device[1], set_printer_device, (void *)1 },
    { "PrinterUserportTextDevice", RES_INTEGER, (resource_value_t)0,
      (void *)&printer_device[2], set_printer_device, (void *)2 },
    { NULL }
};

#ifdef HAS_TRANSLATION
static const cmdline_option_trans_t cmdline_options[] =
{
    { "-prtxtdev1", SET_RESOURCE, 1, NULL, NULL, "PrinterTextDevice1", NULL,
     IDCLS_P_NAME, IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME },
    { "-prtxtdev2", SET_RESOURCE, 1, NULL, NULL, "PrinterTextDevice2", NULL,
     IDCLS_P_NAME, IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME },
    { "-prtxtdev3", SET_RESOURCE, 1, NULL, NULL, "PrinterTextDevice3", NULL,
     IDCLS_P_NAME, IDCLS_SPECIFY_TEXT_DEVICE_DUMP_NAME },
    { "-pr4txtdev", SET_RESOURCE, 1, NULL, NULL, "Printer4TextDevice",
      (resource_value_t)0,
      IDCLS_P_0_2, IDCLS_SPECIFY_TEXT_DEVICE_4 },
    { "-pr5txtdev", SET_RESOURCE, 1, NULL, NULL, "Printer5TextDevice",
      (resource_value_t)0,
      IDCLS_P_0_2, IDCLS_SPECIFY_TEXT_DEVICE_5 },
    { "-prusertxtdev", SET_RESOURCE, 1, NULL, NULL, "PrinterUserportTextDevice",
      (resource_value_t)0,
      IDCLS_P_0_2, IDCLS_SPECIFY_TEXT_USERPORT },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-prtxtdev1", SET_RESOURCE, 1, NULL, NULL, "PrinterTextDevice1", NULL,
     "<name>", N_("Specify name of printer text device or dump file") },
    { "-prtxtdev2", SET_RESOURCE, 1, NULL, NULL, "PrinterTextDevice2", NULL,
     "<name>", N_("Specify name of printer text device or dump file") },
    { "-prtxtdev3", SET_RESOURCE, 1, NULL, NULL, "PrinterTextDevice3", NULL,
     "<name>", N_("Specify name of printer text device or dump file") },
    { "-pr4txtdev", SET_RESOURCE, 1, NULL, NULL, "Printer4TextDevice",
      (resource_value_t)0,
      "<0-2>", "Specify printer text output device for IEC printer #4" },
    { "-pr5txtdev", SET_RESOURCE, 1, NULL, NULL, "Printer5TextDevice",
      (resource_value_t)0,
      "<0-2>", "Specify printer text output device for IEC printer #5" },
    { "-prusertxtdev", SET_RESOURCE, 1, NULL, NULL, "PrinterUserportTextDevice",
      (resource_value_t)0,
      "<0-2>", "Specify printer text output device for userport printer" },
    { NULL }
};
#endif

int output_text_init_cmdline_options(void)
{
#ifdef HAS_TRANSLATION
    return cmdline_register_options_trans(cmdline_options);
#else
    return cmdline_register_options(cmdline_options);
#endif
}

/* ------------------------------------------------------------------------- */

static int output_text_open(unsigned int prnr,
                            output_parameter_t *output_parameter)
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

static void output_text_close(unsigned int prnr)
{
    if (output_fd[printer_device[prnr]] != NULL)
        fclose(output_fd[printer_device[prnr]]);
    output_fd[printer_device[prnr]] = NULL;
}

static int output_text_putc(unsigned int prnr, BYTE b)
{
    if (output_fd[printer_device[prnr]] == NULL)
        return -1;
    fputc(b, output_fd[printer_device[prnr]]);

    return 0;
}

static int output_text_getc(unsigned int prnr, BYTE *b)
{
    if (output_fd[printer_device[prnr]] == NULL)
        return -1;
    *b = fgetc(output_fd[printer_device[prnr]]);
    return 0;
}

static int output_text_flush(unsigned int prnr)
{
    if (output_fd[printer_device[prnr]] == NULL)
        return -1;
    fflush(output_fd[printer_device[prnr]]);

    return 0;
}

/* ------------------------------------------------------------------------- */

void output_text_init(void)
{
}

void output_text_reset(void)
{
}

int output_text_init_resources(void)
{
    output_select_t output_select;

    output_select.output_name = "text";
    output_select.output_open = output_text_open;
    output_select.output_close = output_text_close;
    output_select.output_putc = output_text_putc;
    output_select.output_getc = output_text_getc;
    output_select.output_flush = output_text_flush;

    output_select_register(&output_select);

    return resources_register(resources);
}

void output_text_shutdown_resources(void)
{
    lib_free(PrinterDev[0]);
    lib_free(PrinterDev[1]);
    lib_free(PrinterDev[2]);
}

