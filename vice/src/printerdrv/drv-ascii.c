/*
 * drv-ascii.c - ASCII printer driver.
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

#include "driver-select.h"
#include "drv-ascii.h"
#include "log.h"
#include "output-select.h"
#include "output.h"
#include "types.h"


/* #define DEBUG_PRINTER */

static log_t drv_ascii_log = LOG_ERR;

static int drv_ascii_open(unsigned int prnr, unsigned int secondary)
{
    output_parameter_t output_parameter;

    output_parameter.maxcol = 480;
    output_parameter.maxrow = 66 * 9;

    return output_select_open(prnr, &output_parameter);
}

static void drv_ascii_close(unsigned int prnr, unsigned int secondary)
{
    output_select_close(prnr);
}

static int drv_ascii_putc(unsigned int prnr, unsigned int secondary, BYTE b)
{
#ifdef DEBUG_PRINTER
    log_message(drv_ascii_log, "Print device #%i secondary %i data %02x.",
                prnr + 4, secondary, b);
#endif

    if (output_select_putc(prnr, b) < 0)
        return -1;

#if defined(__MSDOS__) || defined(WIN32) || defined(__OS2__) || defined(__BEOS__)
    if (b == 13)
        return output_select_putc(prnr, 10);
#endif
    return 0;
}

static int drv_ascii_getc(unsigned int prnr, unsigned int secondary, BYTE *b)
{
    return output_select_getc(prnr, b);
}

static int drv_ascii_flush(unsigned int prnr, unsigned int secondary)
{
    return output_select_flush(prnr);
}

static int drv_ascii_formfeed(unsigned int prnr)
{
    return 0;
}

int drv_ascii_init_resources(void)
{
    driver_select_t driver_select;

    driver_select.drv_name = "ascii";
    driver_select.drv_open = drv_ascii_open;
    driver_select.drv_close = drv_ascii_close;
    driver_select.drv_putc = drv_ascii_putc;
    driver_select.drv_getc = drv_ascii_getc;
    driver_select.drv_flush = drv_ascii_flush;
    driver_select.drv_formfeed = drv_ascii_formfeed;

    driver_select_register(&driver_select);

    return 0;
}

void drv_ascii_init(void)
{
    drv_ascii_log = log_open("Drv-Ascii");
}

