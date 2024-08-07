/*
 * drv-ascii.c - ASCII printer driver.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  groepaz <groepaz@gmx.net>
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
#include <stdbool.h>

#include "archdep.h"
#include "charset.h"
#include "driver-select.h"
#include "drv-ascii.h"
#include "log.h"
#include "userport.h"
#include "output-select.h"
#include "output.h"
#include "palette.h"
#include "printer.h"
#include "types.h"

/* #define DEBUG_PRINTER */

#define CHARSPERLINE    74

struct ascii_s {
    int pos;
    int mode;
};
typedef struct ascii_s ascii_t;

static ascii_t drv_ascii[NUM_OUTPUT_SELECT];

static log_t drv_ascii_log = LOG_DEFAULT;

#ifdef DEBUG_PRINTER
#define DBG(x) log_printf  x
#else
#define DBG(x)
#endif

/*
* a unix line ending is "LF", ie: 0x0a / "\n"
* a win/dos line ending is "CRLF", ie: 0x0d, 0x0a / "\r\n"
*/
static int print_lineend(ascii_t *ascii, unsigned int prnr)
{
    ascii->pos = 0;
#ifdef ARCHDEP_PRINTER_RETURN_BEFORE_NEWLINE
    if (output_select_putc(prnr, '\r') < 0) {
        return -1;
    }
#endif
    if (output_select_putc(prnr, '\n') < 0) {
        return -1;
    }
    return 0;
}

static int print_char(ascii_t *ascii, unsigned int prnr, uint8_t c)
{
    uint8_t asc;

    switch (c) {
        case 8: /* bitmap mode */
            return 0;
        case 10: /* linefeed */
            break;
        case 13: /* return */
            /* ascii->mode = 0; */ /* ? */
            break;
        case 14:  /* EN on*/
        case 15:  /* EN off*/
        case 16:  /* POS*/
            return 0;
        case 17: /* lowercase */
            ascii->mode = 1;
            return 0;
        case 18: /* revers on */
            return 0;
        case 145: /* uppercase */
            ascii->mode = 0;
            return 0;
        case 146: /* revers off */
            return 0;
    }

    /* fix duplicated chrout codes */
    if ((c >= 0x60) && (c <= 0x7f)) {
        /* uppercase */
        c = ((c - 0x60) + 0xc0);
    }

    if (ascii->mode == 0) {
        /* uppercase / graphics mode */
        if ((c >= 0x41) && (c <= 0x5a)) {
            /* lowercase (petscii 0x41 -) */
            c += 0x80; /* convert to uppercase */
        } else if ((c >= 0xc1) && (c <= 0xda)) {
            /* uppercase (petscii 0xc1 -) */
            c = '.'; /* can't convert gfx characters */
        }
    }

    asc = charset_p_toascii(c, CONVERT_WITHOUT_CTRLCODES);

    if (asc == '\n') {
        if (print_lineend(ascii, prnr) < 0) {
            return -1;
        }
    } else {
        if (output_select_putc(prnr, asc) < 0) {
            return -1;
        }
        ascii->pos++;
    }

    if (ascii->pos == CHARSPERLINE) {
        if (print_lineend(ascii, prnr) < 0) {
            return -1;
        }
    }

    return 0;
}

static int drv_ascii_open(unsigned int prnr, unsigned int secondary)
{
    if (secondary == 7) {
        DBG(("drv_ascii_open(prnr:%u secondary:%u) device:%u", prnr, secondary, 4 + prnr));
        print_char(&drv_ascii[prnr], prnr, 17);
    } else if (secondary == DRIVER_FIRST_OPEN) {
        output_parameter_t output_parameter;

        DBG(("drv_ascii_open(prnr:%u secondary: DRIVER_FIRST_OPEN) device:%u", prnr, 4 + prnr));

        /* these are unused for non gfx output */
        output_parameter.maxcol = 480;
        output_parameter.maxrow = 66 * 9;
        output_parameter.dpi_x = 100;
        output_parameter.dpi_y = 100;

        return output_select_open(prnr, &output_parameter);
    }

    return 0;
}

static void drv_ascii_close(unsigned int prnr, unsigned int secondary)
{
    DBG(("drv_ascii_close(prnr:%u secondary:%u) device:%u", prnr, secondary, 4 + prnr));
    if (secondary == DRIVER_LAST_CLOSE) {
        output_select_close(prnr);
    }
}

static int drv_ascii_putc(unsigned int prnr, unsigned int secondary, uint8_t b)
{
#ifdef DEBUG_PRINTER
    log_message(drv_ascii_log, "drv_ascii_putc(prnr:%u secondary:%u data %02x) device:%u",
                prnr, secondary, b, prnr + 4);
#endif

    if (print_char(&drv_ascii[prnr], prnr, b) < 0) {
        return -1;
    }

    return 0;
}

static int drv_ascii_getc(unsigned int prnr, unsigned int secondary, uint8_t *b)
{
    return output_select_getc(prnr, b);
}

static int drv_ascii_flush(unsigned int prnr, unsigned int secondary)
{
#ifdef DEBUG_PRINTER
    log_message(drv_ascii_log, "drv_ascii_flush(prnr:%u secondary:%u) device #%u", prnr, secondary, prnr + 4);
#endif
    return output_select_flush(prnr);
}

static int drv_ascii_formfeed(unsigned int prnr)
{
#ifdef DEBUG_PRINTER
    log_message(drv_ascii_log, "drv_ascii_formfeed(prnr:%u) device #%u", prnr, prnr + 4);
#endif
    return output_select_formfeed(prnr);
}

static int drv_ascii_select(unsigned int prnr)
{
    DBG(("drv_ascii_select(prnr:%u) device:%u", prnr, 4 + prnr));
    if ((prnr == PRINTER_USERPORT) && (userport_get_device() == USERPORT_DEVICE_PRINTER)) {
        return drv_ascii_open(prnr, DRIVER_FIRST_OPEN);
    }
    return 0;
}

int drv_ascii_init_resources(void)
{
    driver_select_t driver_select = {
        .drv_name     = "ascii",
        .ui_name      = "ASCII",
        .drv_open     = drv_ascii_open,

        .drv_close    = drv_ascii_close,
        .drv_putc     = drv_ascii_putc,
        .drv_getc     = drv_ascii_getc,
        .drv_flush    = drv_ascii_flush,
        .drv_formfeed = drv_ascii_formfeed,
        .drv_select   = drv_ascii_select,

        .printer      = true,
        .plotter      = false,
        .iec          = true,
        .ieee488      = true,
        .userport     = true,
        .text         = true,
        .graphics     = false
    };
    driver_select_register(&driver_select);

    return 0;
}

int drv_ascii_init(void)
{
    drv_ascii_log = log_open("Drv-Ascii");
    return 0;
}
